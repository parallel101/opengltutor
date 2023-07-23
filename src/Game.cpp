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
    //m_private->monkey.load_obj(OPENGLTUTOR_HOME "assets/opencvpart.obj");
    //m_private->monkey.load_obj(OPENGLTUTOR_HOME "assets/monkey.obj");
    //m_private->monkey.load_obj(OPENGLTUTOR_HOME "assets/cube.obj");
    m_private->monkey = create_opencv_obj(20, 0.3f, 1.0f, 0.2f);
    CHECK_GL(glEnable(GL_DEPTH_TEST));
    CHECK_GL(glDisable(GL_MULTISAMPLE));
    CHECK_GL(glEnable(GL_BLEND));
    CHECK_GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    CHECK_GL(glEnable(GL_LIGHTING));
    CHECK_GL(glEnable(GL_LIGHT0));
    CHECK_GL(glEnable(GL_COLOR_MATERIAL));
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

    constexpr float colors[3][3] = {
        {1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f},
    };
    constexpr glm::vec3 translates[3] = {
        {+0.0f, +0.7f, 0.0f},
        {+0.7f, -0.7f, 0.0f},
        {-0.7f, -0.7f, 0.0f},
    };
    constexpr float offset_angle[3] = {-60.0f, 120.0f, 60.0f};

    static float angle = 0.0f;
    for (int i = 0; i < 3; ++i) {
        glm::mat4x4 model(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.12f * glm::sin(glm::radians(angle) * 2.718f), 0.0f));
        model = glm::translate(model, translates[i]);
        model = glm::rotate(model, glm::radians(60.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(angle + offset_angle[i]), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
        
        CHECK_GL(glMatrixMode(GL_MODELVIEW));
        CHECK_GL(glLoadMatrixf(glm::value_ptr(view * model)));

        glColor3fv(colors[i]);
        m_private->monkey.draw_obj();
    }
    angle += 0.5f;
}
