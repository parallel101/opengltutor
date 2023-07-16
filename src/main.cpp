#include "check_gl.hpp"
#include <iostream>
#ifdef CMAKE_FOUND_X11
#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3native.h>
#include <X11/extensions/shape.h>
#endif
#include "Game.hpp"

namespace {
static struct {
    template <class Ret, class ...Ts>
    using function_type = Ret(Ts...);

    template <class ...Ts>
    operator function_type<void, GLFWwindow *, Ts...> *() {
        return [] (GLFWwindow *window, Ts ...args) {
            auto game = (Game *)glfwGetWindowUserPointer(window);
            if (game) [[likely]] {
                game->mouse_button_callback(args...);
            }
        };
    }
} glfw_callback_helper;
}

int main() {
    // Initalize GLFW library
    if (!glfwInit()) {
        const char *errmsg;
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
    if (version >= 33) {
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    }
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Enable transparent framebuffer
    constexpr bool transparent = true;
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
        opengl_show_glfw_error_diagnose();
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

#ifdef CMAKE_FOUND_X11
    // Setup mouse click-through
    if (transparent) {
        auto dpy = glfwGetX11Display();
        int shape_event_base, shape_error_base;
        if (XShapeQueryExtension(dpy, &shape_event_base, &shape_error_base)) {
            auto win = glfwGetX11Window(window);
            Region region = XCreateRegion();
            XShapeCombineRegion(dpy, win, ShapeInput, 0, 0, region, ShapeSet);
            XDestroyRegion(region);
            XFlush(dpy);
        }
    }
#endif

    // Load glXXX function pointers (only after this you may use OpenGL functions)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        std::cerr << "GLAD failed to load GL functions\n";
        return -1;
    }
    opengl_try_enable_debug_message();

    // Print diagnostic information
    std::cerr << "OpenGL version: " << (const char *)glGetString(GL_VERSION) << '\n';

    // Create game instance
    Game game(window);

    // Register window callbacks
    glfwSetMouseButtonCallback(window, glfw_callback_helper);

    // Initialize data structures
    game.initialize();
    // Start main game loop
    while (!glfwWindowShouldClose(window)) {
        // Render graphics
        game.render();
        // Update screen
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
