#include "check_gl.hpp"
#include <iostream>
#include "Game.hpp"
#ifdef _WIN32
#include <windows.h>
#include <cstdlib>
#include <clocale>
#endif

int main() {
#ifdef _WIN32
    try {
        // this is to support Unicode in the console
        static UINT oldCCP = GetConsoleOutputCP();
        if (!SetConsoleOutputCP(CP_UTF8)) {
            std::cerr << "warning: failed to chcp 65001 for utf-8 output\n";
        } else {
            std::atexit(+[] { SetConsoleOutputCP(oldCCP); });
        }
        static UINT oldCP = GetConsoleCP();
        if (!SetConsoleCP(CP_UTF8)) {
            std::cerr << "warning: failed to chcp 65001 for utf-8 input\n";
        } else {
            std::atexit(+[] { SetConsoleCP(oldCP); });
        }
        // this is to support ANSI control characters (e.g. \033[0m)
        static HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
        static HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hIn != INVALID_HANDLE_VALUE && hOut != INVALID_HANDLE_VALUE) {
            static DWORD oldOutMode = 0;
            static DWORD oldInMode = 0;
            GetConsoleMode(hOut, &oldOutMode);
            GetConsoleMode(hIn, &oldInMode);
            if (SetConsoleMode(hOut, oldOutMode | 0x000C)) {
                std::atexit(+[] { SetConsoleMode(hOut, oldOutMode); });
                if (SetConsoleMode(hIn, oldInMode | 0x0200)) {
                    std::atexit(+[] { SetConsoleMode(hIn, oldInMode); });
                }
            }
        }
        // this is to support Unicode in path name (make Windows API regard char * as UTF-8 instead of GBK)
        // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/setlocale-wsetlocale
        // Windows 10 1803 or above required
        std::setlocale(LC_ALL, ".UTF-8");
    } catch (...) {
        std::cerr << "warning: failed to set utf-8 locale\n";
    }
#endif

    // Initalize GLFW library
    if (!glfwInit()) {
        const char *errmsg = nullptr;
        glfwGetError(&errmsg);
        if (!errmsg) errmsg = "(no error)";
        std::cerr << "failed to initialize GLFW: " << errmsg << '\n';
        return -1;
    }

    // Hint the version required: OpenGL 2.0
    constexpr int version = 20;
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, version / 10);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, version % 10);
#ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#else
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_FALSE);
#endif
    if (version >= 33) {
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    }

    // enable 4x MSAA
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Enable transparent framebuffer
    constexpr bool transparent = false;
    if (transparent) {
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
        glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_FALSE);
        glfwWindowHint(GLFW_FOCUSED, GLFW_FALSE);
    }

    // Create main window
    constexpr char title[] = "Example";
    GLFWwindow *window = glfwCreateWindow(1024, 768, title, NULL, NULL);

    // Test if window creation succeed
    if (!window) {
        check_gl::opengl_show_glfw_error_diagnose();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Switch to fullscreen mode
    constexpr bool fullscreen = false;
    if (fullscreen) {
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        if (monitor) {
            const GLFWvidmode *mode = glfwGetVideoMode(monitor);
            if (mode) {
                glfwSetWindowSize(window, mode->width, mode->height);
                glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
                std::cerr << "Entered fullscreen mode: " << mode->width << 'x' << mode->height
                    << " at " << mode->refreshRate << " Hz\n";
            }
        }
    } else {
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        if (monitor) {
            const GLFWvidmode *mode = glfwGetVideoMode(monitor);
            if (mode) {
                int width, height;
                glfwGetWindowSize(window, &width, &height);
                glfwSetWindowPos(window, (mode->width - width) / 2, (mode->height - height) / 2);
            }
        }
    }

    // Load glXXX function pointers (only after this you may use OpenGL functions)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        std::cerr << "GLAD failed to load GL functions\n";
        return -1;
    }
    check_gl::opengl_try_enable_debug_message();

    // Print diagnostic information
    std::cerr << "OpenGL version: " << (const char *)glGetString(GL_VERSION) << '\n';

    // Enable V-Sync
    glfwSwapInterval(1);

    // Create game instance
    auto game = std::make_unique<Game>();
    game->set_window(window);

    // Initialize data structures
    game->initialize();
    // Start main game loop
    while (!glfwWindowShouldClose(window)) {
        // Render graphics
        game->render();
        // Update screen
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    game.reset();
    glfwTerminate();
    return 0;
}
