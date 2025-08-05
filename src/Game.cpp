#include "Game.hpp"
#include "check_gl.hpp"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "fileutils.hpp"
#include "OBJ.hpp"

struct Game::Private { // P-IMPL 模式
    glm::mat4x4 viewMat;
    glm::mat4x4 projMat;

    OBJ obj;
    DrawableOBJ drawableObj;
    GLProgram program;
};

Game::Game() : m_private(std::make_unique<Private>()) {}

Game::~Game() = default;

void Game::set_window(GLFWwindow *window) {
    m_window = window;
    glfwSetWindowUserPointer(window, this);
    m_inputCtl.register_callbacks(window);
}

#ifndef OPENGLTUTOR_HOME // this should be defined by CMakeLists.txt, your project root directory
#define OPENGLTUTOR_HOME
#endif

void Game::initialize() {
    CHECK_GL(glEnable(GL_DEPTH_TEST));
    CHECK_GL(glEnable(GL_MULTISAMPLE));
    CHECK_GL(glEnable(GL_BLEND));
    CHECK_GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    CHECK_GL(glEnable(GL_CULL_FACE));
    CHECK_GL(glCullFace(GL_BACK));
    CHECK_GL(glFrontFace(GL_CCW));

    CHECK_GL(glEnable(GL_LIGHTING));         // 启用固定管线光照 (古代特性)
    CHECK_GL(glEnable(GL_LIGHT0));           // 启用 0 号光源 (古代特性)
    CHECK_GL(glEnable(GL_COLOR_MATERIAL));   // 启用材质颜色追踪 (古代特性)
    CHECK_GL(glEnable(GL_NORMALIZE));        // 法线归一化

    // glm::vec4 lightPos(-1, 1, 1, 0); // 等会用更现代的方式指定光源方向

    auto program = GLProgram().make();

    auto vertShader = GLShader().make(GL_VERTEX_SHADER);
    check_gl::opengl_shader_source(vertShader, get_file_content(OPENGLTUTOR_HOME "assets/orange.vert"));
    CHECK_GL(glAttachShader(program, vertShader));

    auto fragShader = GLShader().make(GL_FRAGMENT_SHADER);
    check_gl::opengl_shader_source(fragShader, get_file_content(OPENGLTUTOR_HOME "assets/orange.frag"));
    CHECK_GL(glAttachShader(program, fragShader));

    CHECK_GL(glLinkProgram(program));
    m_private->program = std::move(program);

    m_private->obj.load_obj(OPENGLTUTOR_HOME "assets/opencvpart.obj");
    m_private->obj.draw_obj(m_private->drawableObj, /*dynamic=*/false);
}

// 手动计算法线 asin 系数
static glm::vec3 compute_normal_biased(glm::vec3 a, glm::vec3 b, glm::vec3 c) noexcept {
    auto ab = b - a;
    auto ac = c - a;
    auto n = glm::cross(ab, ac);
    auto nLen = glm::length(n);
    if (nLen != 0) {
        n *= glm::asin(nLen / (glm::length(ab) * glm::length(ac))) / nLen;
    }
    return n;
}

void Game::render() {
    CHECK_GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)); // 清空画布

    // 存储各个角度
    static auto models = [] {
        float k = 0.618f;
        float xDown = k * std::sqrt(3.f);
        float hOpencv = k * 1.f;
        std::vector<glm::mat4x4> res;
        glm::mat4x4 model{1.f};
        model = glm::translate(model, glm::vec3{hOpencv * 2, 0, 0});
        model = glm::rotate(model, glm::radians(60.f), glm::vec3{0, 1, 0});
        res.push_back(model);

        model = glm::mat4x4{1.f};
        model = glm::translate(model, glm::vec3{-hOpencv, 0, -xDown});
        model = glm::rotate(model, glm::radians(180.f), glm::vec3{0, 1, 0});
        res.push_back(model);

        model = glm::mat4x4{1.f};
        model = glm::translate(model, glm::vec3{-hOpencv, 0, xDown});
        model = glm::rotate(model, glm::radians(240.f), glm::vec3{0, 1, 0});
        res.push_back(model);
        return res;
    }();

    auto& vertices = m_private->obj.vertices;
    auto& faces = m_private->obj.faces;

    // 计算平滑渲染需要的法线
    static auto normals = [&] {
        std::vector<glm::vec3> res;
        res.resize(faces.size());
        for (auto const& v : faces) {
            auto a = vertices[v[0]].position,
                 b = vertices[v[1]].position,
                 c = vertices[v[2]].position;
            for (std::size_t i = 0; i < 3; ++i)
                res[v[i]] += compute_normal_biased(a, b, c);
        }
        for (auto& it : res)
            it = glm::normalize(it);
        return res;
    }();

    std::size_t i = 1;
    auto projection = m_inputCtl.get_projection_matrix();
    auto view = m_inputCtl.get_view_matrix();
    for (auto model : models) {
        glm::mat4x4 viewModel = view * model; // ModelView

        // 加载投影矩阵
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(glm::value_ptr(projection));

        // 加载模型视图矩阵
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(glm::value_ptr(viewModel));

        glColor3f(i & 1, (i & 2) >> 1, (i & 4) >> 2); // RGB, 对应 opencv 颜色
        glBegin(GL_TRIANGLES);
        for (auto const& v : faces) {
            auto a = vertices[v.x].position,
                 b = vertices[v.y].position,
                 c = vertices[v.z].position;

            glNormal3fv(glm::value_ptr(normals[v[0]]));
            glVertex3fv(glm::value_ptr(a));
            glNormal3fv(glm::value_ptr(normals[v[1]]));
            glVertex3fv(glm::value_ptr(b));
            glNormal3fv(glm::value_ptr(normals[v[2]]));
            glVertex3fv(glm::value_ptr(c));
        }
        CHECK_GL(glEnd());
        i <<= 1;
    }
}
