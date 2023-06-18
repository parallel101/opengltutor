#include <cstdio>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "check_gl.hpp"
#include <GLFW/glfw3.h>
#include "print.hpp"
#include "Game.hpp"

int main() {
    if (!glfwInit()) {
        return -1;
    }
#ifdef FULLSCRN
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    GLFWwindow *window = glfwCreateWindow(mode->width, mode->height, "Example", NULL, NULL);
    glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    aspect = (float)mode->width / (float)mode->height;
#else
    GLFWwindow *window = glfwCreateWindow(640, 640, "Example", NULL, NULL);
#endif
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

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
