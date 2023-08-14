#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "check_gl.hpp" // includes glad/glad.h
#include <GLFW/glfw3.h> // must be placed behind glad/glad.h
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cmath>

constexpr float pi = 3.1415926535897f;

static void render_bkg(float angle = 0.f) {
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex3f(2.0f * std::sin(angle), 2.f * std::cos(angle), 0.f);
    glVertex3f(2.0f * std::sin(angle + pi * 0.5f), 2.f * std::cos(angle + pi * 0.5f), 0.f);
    glVertex3f(2.0f * std::sin(angle + pi), 2.f * std::cos(angle + pi), 0.f);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex3f(2.0f * std::sin(angle + pi), 2.f * std::cos(angle + pi), 0.f);
    glVertex3f(2.0f * std::sin(angle + pi * 1.5f), 2.f * std::cos(angle + pi * 1.5f), 0.f);
    glVertex3f(2.0f * std::sin(angle), 2.f * std::cos(angle), 0.f);
    CHECK_GL(glEnd());
}

static void render_half_circle(float m_angle = 0.f) {
    constexpr size_t n = 200;
    float radius = 1.0;
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_TRIANGLES);
    glColor3f(0.f, 0.f, 0.f);
    for (size_t i = 0; i < n / 2; i++) {
        float angle = i / (float)n * 2 * pi + m_angle;
        float next_angle = (i + 1) / (float)n * 2 * pi + m_angle;
        glVertex3f(std::sin(angle), std::cos(angle), 0.f);
        glVertex3f(std::sin(next_angle), std::cos(next_angle), 0.f);
        glVertex3f(0.f, 0.f, 0.f);
    }
    glColor3f(1.0f, 1.0f, 1.0f);
    for (size_t i = n / 2; i < n; i++) {
        float angle = i / (float)n * 2 * pi + m_angle;
        float next_angle = (i + 1) / (float)n * 2 * pi + m_angle;
        glVertex3f(std::sin(angle), std::cos(angle), 0.f);
        glVertex3f(std::sin(next_angle), std::cos(next_angle), 0.f);
        glVertex3f(0.f, 0.f, 0.f);
    }
    CHECK_GL(glEnd());
}

static void render_inner_circle(float m_angle = 0.f) {
    constexpr float radius = 0.2f;
    constexpr float outer_radius = 0.5f;
    constexpr size_t n = 200;
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_TRIANGLES);
    glColor3f(1.f, 1.f, 1.f);
    for (size_t i = 0; i < n; i++) {
        float angle = i / (float)n * pi * 2;
        float next_angle = (i + 1) / (float)n * pi * 2;
        glVertex3f(outer_radius * std::sin(angle) + 0.5f * std::sin(m_angle), \
                outer_radius * std::cos(angle) + 0.5f * std::cos(m_angle), 0.f);
        glVertex3f(outer_radius * std::sin(next_angle) + 0.5f * std::sin(m_angle), \
                outer_radius * std::cos(next_angle) + 0.5f * std::cos(m_angle), 0.f);
        glVertex3f(0.5f * std::sin(m_angle), 0.5f * std::cos(m_angle), 0.f);
    }
    glColor3f(0.f, 0.f, 0.f);
    for (size_t i = 0; i < n; i++) {
        float angle = i / (float)n * pi * 2;
        float next_angle = (i + 1) / (float)n * pi * 2;
        glVertex3f(outer_radius * std::sin(angle) + 0.5f * std::sin(m_angle + pi), \
                outer_radius * std::cos(angle) + 0.5f * std::cos(m_angle + pi), 0.f);
        glVertex3f(outer_radius * std::sin(next_angle) + 0.5f * std::sin(m_angle + pi), \
                outer_radius * std::cos(next_angle) + 0.5f * std::cos(m_angle + pi), 0.f);
        glVertex3f(0.5f * std::sin(m_angle + pi), 0.5f * std::cos(m_angle + pi), 0.f);
    }
    glColor3f(0.f, 0.f, 0.f);
    for (size_t i = 0; i < n; i++) {
        float angle = i / (float)n * pi * 2;
        float next_angle = (i + 1) / (float)n * pi * 2;
        glVertex3f(radius * std::sin(angle) + 0.5f * std::sin(m_angle), \
                radius * std::cos(angle) + 0.5f * std::cos(m_angle), 0.f);
        glVertex3f(radius * std::sin(next_angle) + 0.5f * std::sin(m_angle), \
                radius * std::cos(next_angle) + 0.5f * std::cos(m_angle), 0.f);
        glVertex3f(0.5f * std::sin(m_angle), 0.5f * std::cos(m_angle), 0.f);
    }
    glColor3f(1.f, 1.f, 1.f);
    for (size_t i = 0; i < n; i++) {
        float angle = i / (float)n * pi * 2;
        float next_angle = (i + 1) / (float)n * pi * 2;
        glVertex3f(radius * std::sin(angle) + 0.5f * std::sin(m_angle + pi), \
                radius * std::cos(angle) + 0.5f * std::cos(m_angle + pi), 0.f);
        glVertex3f(radius * std::sin(next_angle) + 0.5f * std::sin(m_angle + pi), \
                radius * std::cos(next_angle) + 0.5f * std::cos(m_angle + pi), 0.f);
        glVertex3f(0.5f * std::sin(m_angle + pi), 0.5f * std::cos(m_angle + pi), 0.f);
    }
    CHECK_GL(glEnd());
}

static void rotate_render() {
    static float i = 0;
    constexpr float n = 200.0;
    i += 0.2;
    if (i > n)
        i -= n;
    float angle = i / n * 2.f * pi;
    render_bkg(angle);
    render_half_circle(angle);
    render_inner_circle(angle);
}

int main() {
    if (!glfwInit()) {
        const char *errmsg;
        glfwGetError(&errmsg);
        if (!errmsg) errmsg = "(no error)";
        std::cerr << "failed to initialize GLFW: " << errmsg << '\n';
        return -1;
    }

    // hint the version required: OpenGL 2.0
    constexpr int version = 20;
    glfwWindowHint(GLFW_OPENGL_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, version / 10);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, version % 10);
    if (version >= 33) {
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    }

    // Create window
    GLFWwindow *window = glfwCreateWindow(640, 640, "Example", NULL, NULL);
    if (!window) {
        const char *errmsg;
        glfwGetError(&errmsg);
        if (!errmsg) errmsg = "(no error)";
        std::cerr << "GLFW failed to create window: " << errmsg << '\n';
        std::cerr << "==============================================\n";
        if (!strcmp(errmsg, "X11: The DISPLAY environment variable is missing")) {
            std::cerr << "You seems not running with graphic display\n";
        } else if (!strcmp(errmsg, "WGL: The driver does not appear to support OpenGL")) {
            std::cerr << "Please consider install an OpenGL driver, or use the mesa driver\n";
        } else if (!strcmp(errmsg, "WGL: Failed to create OpenGL context")) {
            std::cerr << "Your driver seems not supporting the required OpenGL version\n";
        }
        std::cerr << "- If you have a physical graphic card (e.g. NVIDIA), install it from your graphic card vendor official website: http://www.nvidia.com/Download/index.aspx\n";
        std::cerr << "- If you are using Windows, download opengl32.dll from https://pan.baidu.com/s/1TZ6nVJC7DZIuUarZrGJYow?pwd=opgl and place it into the same directory as this executable file (alternatively you may download opengl32sw.dll from Internet and rename it to opengl32.dll to place into the same directory as this executable file)\n";
        std::cerr << "- If you are using Linux or WSL1, install the mesa driver: https://ubuntuhandbook.org/index.php/2021/07/install-latest-mesa-ubuntu-20-04-21-04/";
        std::cerr << "- If you use WSL2, install WSLg: https://learn.microsoft.com/zh-cn/windows/wsl/tutorials/gui-apps\n";
        std::cerr << "- If you are using SSH remote server, try connect it using ssh -X <ip address>\n";
        std::cerr << "- If you are using MacOS, you probably want to use Windows or Linux instead for better OpenGL support\n";
        std::cerr << "- If you are using a Laptop with dual-cards, make sure you have switch to dedicated card (NVIDIA) instead of the integrated card (Intel)\n";
        std::cerr << "==============================================\n";
#ifdef _WIN32
        std::system("pause");
#endif
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Load glXXX function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        std::cerr << "GLAD failed to load GL functions\n";
        return -1;
    }
    std::cerr << "OpenGL version: " << glGetString(GL_VERSION) << '\n';

    CHECK_GL(glEnable(GL_POINT_SMOOTH));
    CHECK_GL(glEnable(GL_BLEND));
    CHECK_GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    CHECK_GL(glPointSize(64.0f));

    // start main game loop
    while (!glfwWindowShouldClose(window)) {
        // render graphics
        CHECK_GL(glClear(GL_COLOR_BUFFER_BIT));
        rotate_render();
        // refresh screen
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
