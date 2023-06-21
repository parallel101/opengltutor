#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "check_gl.hpp"
#include <GLFW/glfw3.h>
#include "Game.hpp"
#include <stdexcept>
#include <string>
#include <cstdio>

int main() {
    if (!glfwInit()) {
        throw std::runtime_error("failed to initialize GLFW");
    }
#ifdef FULLSCRN
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    GLFWwindow *window = glfwCreateWindow(mode->width, mode->height, "Example", NULL, NULL);
    glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
#else
    GLFWwindow *window = glfwCreateWindow(640, 640, "Example", NULL, NULL);
#endif
    if (!window) {
        glfwTerminate();
        const char *errmsg;
        glfwGetError(&errmsg);
        throw std::runtime_error("GLFW failed to create window: "
                                 + std::string(errmsg));
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        throw std::runtime_error("GLAD failed to load GL functions");
    }
    printf("OpenGL version: %s\n", glGetString(GL_VERSION));

    {
        Game game;
        double t0 = glfwGetTime();
        while (!glfwWindowShouldClose(window)) {
            /* CHECK_GL(glClear(GL_COLOR_BUFFER_BIT)); */
            double t1 = glfwGetTime();
            game.tick(t1 - t0);
            t0 = t1;
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    glfwTerminate();
    return 0;
}
