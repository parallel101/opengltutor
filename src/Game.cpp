#include "Game.hpp"
#include "check_gl.hpp"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "OBJ.hpp"

#include <vector>
#include <iostream>

struct Game::Private { // P-IMPL pattern
    glm::mat4x4 viewMat;
    glm::mat4x4 projMat;

    std::vector<OBJ> ocvLogo;
    OBJ monkey;
};

Game::Game() : m_private(std::make_unique<Private>()), m_window(nullptr) {}

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
    OBJ ocvpart;
    ocvpart.load_obj(OPENGLTUTOR_HOME "assets/opencvpart_grp.obj", true);

    for (size_t i = 0; i < 3; ++i) {
        m_private->ocvLogo.push_back(ocvpart);
    }
    constexpr float short_side = 0.65f;
    glm::vec3 tri_sides = {short_side, 2 * short_side, short_side * sqrtf(3)};

    auto C0 = glm::vec3(0.0f, tri_sides.y, 0.0f);
    auto C1 = glm::vec3(-tri_sides.z, -tri_sides.x, 0.0f);
    auto C2 = glm::vec3(tri_sides.z, -tri_sides.x, 0.0f);
    
    std::vector<glm::mat4x4> cvLogoModel;

    auto model = glm::mat4x4(1.0f);
    model = glm::translate(model, C0);
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(150.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    cvLogoModel.push_back(model);

    model = glm::mat4x4(1.0f);
    model = glm::translate(model, C1);
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    cvLogoModel.push_back(model);

    model = glm::mat4x4(1.0f);
    model = glm::translate(model, C2);
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    cvLogoModel.push_back(model);

    for (size_t i = 0; i < m_private->ocvLogo.size(); ++i) {
        auto& ocvPart = m_private->ocvLogo.at(i);
        auto& ocvPartModel = cvLogoModel.at(i);
        for (auto& vert : ocvPart.vertices) {
            vert = ocvPartModel * glm::vec4(vert, 1.0f);
        }
    }

    //m_private->monkey.load_obj(OPENGLTUTOR_HOME "assets/monkey.obj");
    //m_private->monkey.load_obj(OPENGLTUTOR_HOME "assets/cube.obj"); 

    CHECK_GL(glEnable(GL_DEPTH_TEST));
    CHECK_GL(glEnable(GL_MULTISAMPLE));
    CHECK_GL(glEnable(GL_BLEND));
    CHECK_GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    CHECK_GL(glEnable(GL_LIGHTING));
    CHECK_GL(glEnable(GL_LIGHT0));
    CHECK_GL(glEnable(GL_COLOR_MATERIAL));
    CHECK_GL(glEnable(GL_CULL_FACE));
    CHECK_GL(glCullFace(GL_BACK));
    CHECK_GL(glFrontFace(GL_CCW));
    CHECK_GL(glShadeModel(GL_SMOOTH));
}

void Game::render() {
    int width, height;
    glfwGetWindowSize(m_window, &width, &height);
    CHECK_GL(glViewport(0, 0, width, height));

    CHECK_GL(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    auto projection = m_inputCtl.get_projection_matrix();

    CHECK_GL(glMatrixMode(GL_PROJECTION));
    CHECK_GL(glLoadMatrixf(glm::value_ptr(projection)));

    auto view = m_inputCtl.get_view_matrix();

    glm::mat4x4 model(1.0f);
   
    static float motionAngle = 0.0f;
    static constexpr glm::vec3 motionAxis = { 0.0f, 1.0f, 0.0f };
    static constexpr float motionDelta = 1.2f;

    CHECK_GL(glMatrixMode(GL_MODELVIEW));

    model = glm::rotate(glm::mat4(1.0f), glm::radians(motionAngle), motionAxis);
    CHECK_GL(glLoadMatrixf(glm::value_ptr(view * model)));
    glColor3f(1.0f, 0.0f, 0.0f);
    m_private->ocvLogo.at(0).draw_obj_smooth();

    model = glm::rotate(glm::mat4(1.0f), glm::radians(motionAngle), motionAxis);
    CHECK_GL(glLoadMatrixf(glm::value_ptr(view * model)));
    glColor3f(0.0f, 1.0f, 0.0f);
    m_private->ocvLogo.at(1).draw_obj_smooth();

    model = glm::rotate(glm::mat4(1.0f), glm::radians(motionAngle), motionAxis);
    CHECK_GL(glLoadMatrixf(glm::value_ptr(view * model)));
    glColor3f(0.0f, 0.0f, 1.0f);
    m_private->ocvLogo.at(2).draw_obj_smooth();

    motionAngle += motionDelta;
}
