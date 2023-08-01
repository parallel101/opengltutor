#include "check_gl.hpp"
#include <iostream>
#include "Game.hpp"

static void render() {
    glBegin(GL_TRIANGLES);
    constexpr int n = 200;
    constexpr float pi = 3.141592653589f;
    float radius = 0.3f;
    float inner_radius = 0.12f;
    glColor3f(1.0f, 0.0f, 0.0f);
    float shiftY = 1 / std::sqrtf(3.0f) * 0.7;
    for (int i = 0; i < n; i++) {
        if (i<n * 5 / 12 || i>n * 7 / 12)
        {
            float angle = i / (float)n * pi *
                2;
            float angle_next = (i + 1) /
                (float)n * pi * 2;
            glVertex3f(inner_radius *
                sinf(angle), inner_radius * cosf(angle) + shiftY, 0.0f);
            glVertex3f(radius * sinf(angle),
                radius * cosf(angle) + shiftY, 0.0f);
            glVertex3f(radius *
                sinf(angle_next), radius * cosf(angle_next) + shiftY, 0.0f);
            glVertex3f(inner_radius *
                sinf(angle), inner_radius * cosf(angle) + shiftY, 0.0f);
            glVertex3f(inner_radius *
                sinf(angle_next), inner_radius * cosf(angle_next) + shiftY, 0.0f);
            glVertex3f(radius *
                sinf(angle_next), radius * cosf(angle_next) + shiftY, 0.0f);
        }
    }
    glColor3f(0.0f, 1.0f, 0.0f);
    shiftY = -0.5f / std::sqrtf(3.0f) * 0.7;
    float shiftX = -0.5 * 0.7;
    for (int i = 0; i < n; i++) {
        if (i<n * 1 / 12 || i>n * 3 / 12)
        {
            float angle = i / (float)n * pi *
                2;
            float angle_next = (i + 1) /
                (float)n * pi * 2;
            glVertex3f(inner_radius *
                sinf(angle) + shiftX, inner_radius * cosf(angle) + shiftY, 0.0f);
            glVertex3f(radius * sinf(angle) +
                shiftX, radius * cosf(angle) + shiftY, 0.0f);
            glVertex3f(radius *
                sinf(angle_next) + shiftX, radius * cosf(angle_next) + shiftY, 0.0f);
            glVertex3f(inner_radius *
                sinf(angle) + shiftX, inner_radius * cosf(angle) + shiftY, 0.0f);
            glVertex3f(inner_radius *
                sinf(angle_next) + shiftX, inner_radius * cosf(angle_next) + shiftY, 0.0f);
            glVertex3f(radius *
                sinf(angle_next) + shiftX, radius * cosf(angle_next) + shiftY, 0.0f);
        }
    }
    glColor3f(0.0f, 0.0f, 1.0f);
    shiftY = -0.5f / std::sqrtf(3.0f) * 0.7;
    shiftX = 0.5 * 0.7;
    for (int i = 0; i < n; i++) {
        if (i > n * 1 / 12 && i < n
            * 11 / 12) {
            float angle = i / (float)n * pi *
                2;
            float angle_next = (i + 1) /
                (float)n * pi * 2;
            glVertex3f(inner_radius *
                sinf(angle) + shiftX, inner_radius * cosf(angle) + shiftY, 0.0f);
            glVertex3f(radius * sinf(angle) +
                shiftX, radius * cosf(angle) + shiftY, 0.0f);
            glVertex3f(radius *
                sinf(angle_next) + shiftX, radius * cosf(angle_next) + shiftY, 0.0f);
            glVertex3f(inner_radius *
                sinf(angle) + shiftX, inner_radius * cosf(angle) + shiftY, 0.0f);
            glVertex3f(inner_radius *
                sinf(angle_next) + shiftX, inner_radius * cosf(angle_next) + shiftY, 0.0f);
            glVertex3f(radius *
                sinf(angle_next) + shiftX, radius * cosf(angle_next) + shiftY, 0.0f);
        }
    }
    CHECK_GL(glEnd());
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
    GLFWwindow *window = glfwCreateWindow(640, 640, title, NULL, NULL);
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

    //// Create game instance
    //auto &game = Game::get();
    //game.set_window(window);

    //// Initialize data structures
    //game.initialize();
    // Start main game loop
    while (!glfwWindowShouldClose(window)) {
        CHECK_GL(glClear(GL_COLOR_BUFFER_BIT));
        render();
        // Render graphics
        //game.render();
        // Update screen
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
