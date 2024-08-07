#include "Game.hpp"
#include "check_gl.hpp"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "fileutils.hpp"
#include "Points.hpp"

struct Game::Private { // P-IMPL 模式
    glm::mat4x4 viewMat;
    glm::mat4x4 projMat;

    Points points;
    DrawablePoints drawablePoints;
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
    CHECK_GL(glEnable(GL_PROGRAM_POINT_SIZE));
    CHECK_GL(glEnable(GL_POINT_SPRITE));
    CHECK_GL(glCullFace(GL_BACK));
    CHECK_GL(glFrontFace(GL_CCW));

    auto program = GLProgram().make();

    auto vertShader = GLShader().make(GL_VERTEX_SHADER);
    check_gl::opengl_shader_source(vertShader, get_file_content(OPENGLTUTOR_HOME "assets/points.vert"));
    CHECK_GL(glAttachShader(program, vertShader));

    auto fragShader = GLShader().make(GL_FRAGMENT_SHADER);
    check_gl::opengl_shader_source(fragShader, get_file_content(OPENGLTUTOR_HOME "assets/points.frag"));
    CHECK_GL(glAttachShader(program, fragShader));

    CHECK_GL(glLinkProgram(program));
    m_private->program = std::move(program);

    m_private->points.init_cube(glm::vec3(-1, -1, -0.25f), glm::vec3(1, 1, 0.25f), glm::uvec3(32, 32, 8), 1.0f / 40);
}

void Game::render() {
    int width, height;
    glfwGetWindowSize(m_window, &width, &height);
    CHECK_GL(glViewport(0, 0, width, height));

    CHECK_GL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
    CHECK_GL(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));

    auto camera = m_inputCtl.get_camera_params();
    glm::mat4x4 model(1.0f);

    CHECK_GL(glUseProgram(m_private->program));
    CHECK_GL(glUniformMatrix4fv(glGetUniformLocation(m_private->program, "uniModel"), 1, GL_FALSE, glm::value_ptr(model)));
    CHECK_GL(glUniformMatrix4fv(glGetUniformLocation(m_private->program, "uniView"), 1, GL_FALSE, glm::value_ptr(camera.view)));
    CHECK_GL(glUniformMatrix4fv(glGetUniformLocation(m_private->program, "uniProjection"), 1, GL_FALSE, glm::value_ptr(camera.projection)));
    CHECK_GL(glUniform1f(glGetUniformLocation(m_private->program, "uniScreenHeight"), camera.height));
    CHECK_GL(glUniform1f(glGetUniformLocation(m_private->program, "uniFovFactor"), camera.fov_factor));

    glm::vec2 mousePos = m_inputCtl.get_cursor_pos();
    glm::vec3 lightDir = glm::normalize(glm::vec3(mousePos.x, mousePos.y, 1));
    CHECK_GL(glUniform3fv(glGetUniformLocation(m_private->program, "uniLightDir"), 1, glm::value_ptr(lightDir)));

    for (size_t substep = 0; substep < 1; ++substep) {
        m_private->points.evolve(0.0001f);
    }
    m_private->points.draw_points(m_private->drawablePoints, /*dynamic=*/true);
    m_private->drawablePoints.draw();
}
