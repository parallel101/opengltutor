#include "Game.hpp"
#include "check_gl.hpp"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "OBJ.hpp"

struct Game::Private { // P-IMPL pattern
    glm::mat4x4 viewMat;
    glm::mat4x4 projMat;

   //OBJ monkey;
    OBJ redopencv;
    OBJ greenopencv;
    OBJ blueopencv;
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
    m_private->redopencv.load_obj(OPENGLTUTOR_HOME "assets/opencvpart.obj"); 
    m_private->greenopencv.load_obj(OPENGLTUTOR_HOME "assets/opencvpart.obj");
    m_private->blueopencv.load_obj(OPENGLTUTOR_HOME "assets/opencvpart.obj");
    /*m_private->monkey.load_obj(OPENGLTUTOR_HOME "assets/monkey.obj");*/
    /* m_private->monkey.load_obj(OPENGLTUTOR_HOME "assets/cube.obj"); */
    CHECK_GL(glEnable(GL_DEPTH_TEST));
    CHECK_GL(glEnable(GL_MULTISAMPLE));
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

    CHECK_GL(glClearColor(0.5f, 0.5f, 0.5f, 0.0f));
    CHECK_GL(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));

    auto projection = m_inputCtl.get_projection_matrix();

    CHECK_GL(glMatrixMode(GL_PROJECTION));
    CHECK_GL(glLoadMatrixf(glm::value_ptr(projection)));

    auto view = m_inputCtl.get_view_matrix();

    glm::mat4x4 model(1.0f);

    glColor3f(1.0f,0,0);

    //red
    CHECK_GL(glMatrixMode(GL_MODELVIEW));
    model = glm::translate(glm::mat4x4(1), glm::vec3(0, 0.5f, 0)) * glm::rotate(glm::mat4x4(1),glm::radians(150.0f),glm::vec3(0,0,1.0f)) * 
        glm::rotate(glm::mat4x4(1), glm::radians(90.0f), glm::vec3(1.0f, 0, 0)) * 
        glm::scale(glm::mat4x4(1), glm::vec3(0.5f, 0.5f, 0.5f)) * model;
    CHECK_GL(glLoadMatrixf(glm::value_ptr(view * model)));
    m_private->redopencv.draw_obj(true);

    model = glm::mat4(1.0f);
    glColor3f(0, 0,1.0f);
    //green
    CHECK_GL(glMatrixMode(GL_MODELVIEW));
    model = glm::translate(glm::mat4x4(1), glm::vec3(0.65f, -0.5f, 0)) * glm::rotate(glm::mat4x4(1), glm::radians(-30.0f), glm::vec3(0, 0, 1.0f)) *
        glm::rotate(glm::mat4x4(1), glm::radians(90.0f), glm::vec3(1.0f,0,0)) *
        glm::scale(glm::mat4x4(1), glm::vec3(0.5f, 0.5f, 0.5f))*model;
    CHECK_GL(glLoadMatrixf(glm::value_ptr(view * model)));
    m_private->greenopencv.draw_obj(true);

    model = glm::mat4(1.0f);
    glColor3f(0, 1.0f, 0);
    //blue
    CHECK_GL(glMatrixMode(GL_MODELVIEW));
    model = glm::translate(glm::mat4x4(1), glm::vec3(-0.65f, -0.5f, 0)) * glm::rotate(glm::mat4x4(1), glm::radians(-90.0f), glm::vec3(0, 0, 1.0f)) *
        glm::rotate(glm::mat4x4(1), glm::radians(90.0f), glm::vec3(1.0f, 0, 0)) *
        glm::scale(glm::mat4x4(1), glm::vec3(0.5f, 0.5f, 0.5f)) * model;
    CHECK_GL(glLoadMatrixf(glm::value_ptr(view * model)));
    m_private->blueopencv.draw_obj(true);
   
}
