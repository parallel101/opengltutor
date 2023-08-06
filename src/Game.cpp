#include "Game.hpp"
#include "check_gl.hpp"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "OBJ.hpp"

struct Game::Private { // P-IMPL pattern
    glm::mat4x4 viewMat;
    glm::mat4x4 projMat;

    OBJ monkey;
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
    /* m_private->monkey.load_obj(OPENGLTUTOR_HOME "assets/opencvpart.obj"); */
    m_private->monkey.load_obj(OPENGLTUTOR_HOME "assets/monkey.obj");
    /* m_private->monkey.load_obj(OPENGLTUTOR_HOME "assets/cube.obj"); */
    CHECK_GL(glEnable(GL_DEPTH_TEST));
    CHECK_GL(glDisable(GL_MULTISAMPLE));
    CHECK_GL(glEnable(GL_BLEND));
    CHECK_GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    CHECK_GL(glEnable(GL_LIGHTING));
    CHECK_GL(glEnable(GL_LIGHT0));
    CHECK_GL(glEnable(GL_COLOR_MATERIAL));
    CHECK_GL(glEnable(GL_NORMALIZE));
    CHECK_GL(glEnable(GL_CULL_FACE));
    CHECK_GL(glCullFace(GL_BACK));
    CHECK_GL(glFrontFace(GL_CCW));
}

void Game::render() {
    int width, height;
    glfwGetWindowSize(m_window, &width, &height);
    CHECK_GL(glViewport(0, 0, width, height));

    CHECK_GL(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));

    auto projection = m_inputCtl.get_projection_matrix();

    CHECK_GL(glMatrixMode(GL_PROJECTION));
    CHECK_GL(glLoadMatrixf(glm::value_ptr(projection)));
    
    auto view = m_inputCtl.get_view_matrix();
    
    glm::mat4x4 model(1.0f);

    glColor3f(0.9f, 0.6f, 0.1f);

    /* CHECK_GL(glMatrixMode(GL_MODELVIEW)); */
    /* model = glm::translate(glm::mat4x4(1), glm::vec3(0.8f, 0, 0)) * model; */
    /* CHECK_GL(glLoadMatrixf(glm::value_ptr(view * model))); */
    /* m_private->monkey.draw_obj(true); */
    
    CHECK_GL(glMatrixMode(GL_MODELVIEW));
    /* model = glm::translate(glm::mat4x4(1), -2.0f * glm::vec3(0.8f, 0, 0)) * model; */
    CHECK_GL(glLoadMatrixf(glm::value_ptr(view * model)));
    m_private->monkey.draw_obj(true);
}
