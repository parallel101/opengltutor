#include "Game.hpp"
#include "check_gl.hpp"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "OBJ.hpp"

struct Game::Private {
    OBJ monkey;
};

Game::Game(GLFWwindow *window) : m_private(new Private), m_window(window) {
    glfwSetWindowUserPointer(window, this);
}

Game::~Game() = default;

void Game::initialize() {
    m_private->monkey.load_obj("/home/bate/Codes/opengltutor/assets/monkey.obj");
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
}

void Game::render() {
    int width, height;
    glfwGetWindowSize(m_window, &width, &height);
    CHECK_GL(glViewport(0, 0, width, height));

    CHECK_GL(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));

    glm::mat4x4 projection = glm::perspective(glm::radians(40.0f), (float)width / height, 0.01f, 100.0f);
    /* projection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 0.0f, 100.0f); */
    /* projection = glm::frustum(-0.005f, 0.005f, -0.005f, 0.005f, 0.01f, 100.0f); */
    CHECK_GL(glMatrixMode(GL_PROJECTION));
    CHECK_GL(glLoadMatrixf(glm::value_ptr(projection)));

    glm::vec3 eye(0, 0, 5);
    glm::vec3 center(0, 0, 0);
    glm::vec3 up(0, 1, 0);
    glm::mat4x4 view = glm::lookAt(eye, center, up);

    static float angle = 0.0f;
    glm::mat4x4 model(1.0f);
    model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    angle += 0.5f;

    CHECK_GL(glMatrixMode(GL_MODELVIEW));
    CHECK_GL(glLoadMatrixf(glm::value_ptr(view * model)));

    m_private->monkey.draw_obj();
}

void Game::cursor_pos_callback(double xpos, double ypos) {
    int width, height;
    glfwGetWindowSize(m_window, &width, &height);

    float x = (float)(2 * xpos / width - 1);
    float y = (float)(2 * (height - ypos) / height - 1);

    if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        // TODO: when left mouse button is dragged
    }
}

void Game::mouse_button_callback(int button, int action, int mods) {
    double xpos, ypos;
    glfwGetCursorPos(m_window, &xpos, &ypos);
    int width, height;
    glfwGetWindowSize(m_window, &width, &height);

    float x = (float)(2 * xpos / width - 1);
    float y = (float)(2 * (height - ypos) / height - 1);

    if ( button == GLFW_MOUSE_BUTTON_LEFT
      && action == GLFW_PRESS
    ) {
        // TODO: when left mouse button is pressed
    }
}

void Game::scroll_callback(double xoffset, double yoffset) {
}

void Game::key_callback(int key, int scancode, int action, int mods) {
}
