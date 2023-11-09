#include "Game.hpp"
#include "check_gl.hpp"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "OBJ.hpp"

struct Game::Private { // P-IMPL pattern
    glm::mat4x4 viewMat;
    glm::mat4x4 projMat;

    OBJ obj;
};

Game::Game() : m_private(std::make_unique<Private>()) {}

Game::~Game() = default;

Game &Game::get() {
    static Game game; // singleton
    return game;
}

void Game::set_window(GLFWwindow *window) {
    m_window = window;
    glfwSetWindowUserPointer(window, this);
    m_inputCtl.register_callbacks(window);
}

#ifndef OPENGLTUTOR_HOME // this should be defined by CMakeLists.txt, your project root directory
#define OPENGLTUTOR_HOME
#endif

void Game::initialize() {
    m_private->obj.load_obj(OPENGLTUTOR_HOME "assets/sphere.obj");
    CHECK_GL(glEnable(GL_DEPTH_TEST));
    CHECK_GL(glEnable(GL_MULTISAMPLE));
    CHECK_GL(glEnable(GL_BLEND));
    CHECK_GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    CHECK_GL(glEnable(GL_CULL_FACE));
    CHECK_GL(glCullFace(GL_BACK));
    CHECK_GL(glFrontFace(GL_CCW));

    // glm::vec4 lightPos(-1, 1, 1, 0); // 等会用更现代的方式指定光源方向

    unsigned int program = glCreateProgram();
    unsigned int shader = glCreateShader(GL_FRAGMENT_SHADER);
    const char *src = R"(#version 330 core
void main() {
    gl_FragColor = vec4(1, 0.5, 0, 1);
}
)";
    const char *srcList[1] = {src};
    int srcLenList[1] = {(int)strlen(src)};
    glShaderSource(shader, 1, srcList, srcLenList);
    glAttachShader(program, shader);
    glLinkProgram(program);
    glUseProgram(program);
}

void Game::render() {
    int width, height;
    glfwGetWindowSize(m_window, &width, &height);
    CHECK_GL(glViewport(0, 0, width, height));

    CHECK_GL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
    CHECK_GL(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));

    // auto projection = m_inputCtl.get_projection_matrix();
    // auto view = m_inputCtl.get_view_matrix();
    // glm::mat4x4 model(1.0f); // 等会用更现代的方式指定这些矩阵

    m_private->obj.draw_obj(/*isFlat=*/false);
}
