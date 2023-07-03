#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "check_gl.hpp" // includes glad/glad.h
#include <GLFW/glfw3.h> // must be placed behind glad/glad.h
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <vector>

void plotCircle(std::vector<float> center, float radius, std::vector<float> color, float depth)
{
    constexpr int iter = 300;
    constexpr float pi = 3.1415926535897f;
    glColor3f(color.at(0), color.at(1), color.at(2));
    for (int i = 0; i < iter; i++) {
        float angle = i / (float)iter * pi * 2;
        float angle_next = (i + 1) / (float)iter * pi * 2;
        glVertex3f(center.at(0), center.at(1), center.at(2));
        glVertex3f(center.at(0) + radius * sinf(angle), center.at(1) + radius * cosf(angle), depth);
        glVertex3f(center.at(0) + radius * sinf(angle_next), center.at(1) + radius * cosf(angle_next), depth);
    }
}

static void render() {
    glBegin(GL_TRIANGLES);
    constexpr int n = 300;
    constexpr float pi = 3.1415926535897f;
    float radius = 0.3f;
    float inner_radius = 0.15f;
    static int x = 0;
    std::vector<float> green_color{.0f, 1.0f, .0f};
    std::vector<float> red_color{1.0f, .0f, .0f};
    std::vector<float> blue_color{ .0f, .0f, 1.0f };
    std::vector<float> black_color{ .0f, .0f, 0.0f };
    std::vector<float> green_circle_center{-0.4f, -0.4f, 1.0f};
    std::vector<float> red_circle_center{0.0f, 0.4f, 1.0f};
    std::vector<float> blue_circle_center{0.4f, -0.4f, 1.0f};
    //Circles
    plotCircle(green_circle_center, radius, green_color, 0.9f);
    plotCircle(red_circle_center, radius, red_color, 0.9f);
    for (int i = 0; i < n; i++) {
        float angle = i / (float)n * pi * 2;
        float angle_next = (i + 1) / (float)n * pi * 2;
        if (angle > pi / 6 && angle < pi * 11 / 6)
        {
            glColor3f(0.0f, 0.0f, 1.0f);
            glVertex3f(blue_circle_center.at(0), blue_circle_center.at(1), 0.5f);
            glVertex3f(blue_circle_center.at(0) + radius * sinf(angle), blue_circle_center.at(1) + radius * cosf(angle), 0.5f);
            glVertex3f(blue_circle_center.at(0) + radius * sinf(angle_next), blue_circle_center.at(1) + radius * cosf(angle_next), 0.5f);
        }
        else
        {
            continue;
        }
    }
    plotCircle(green_circle_center, inner_radius, black_color, -0.1f);
    plotCircle(red_circle_center, inner_radius, black_color, -0.1f);
    plotCircle(blue_circle_center, inner_radius, black_color, -0.1f);
    //Black Triangle
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex3f(green_circle_center.at(0), green_circle_center.at(1), 0.6f);
    glVertex3f(red_circle_center.at(0), red_circle_center.at(1), 0.6f);
    glVertex3f(blue_circle_center.at(0), blue_circle_center.at(1), 0.6f);

    CHECK_GL(glEnd());
}

int main() {
    if (!glfwInit()) {
        const char* errmsg;
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
    GLFWwindow* window = glfwCreateWindow(640, 640, "Example", NULL, NULL);
    if (!window) {
        const char* errmsg;
        glfwGetError(&errmsg);
        if (!errmsg) errmsg = "(no error)";
        std::cerr << "GLFW failed to create window: " << errmsg << '\n';
        std::cerr << "==============================================\n";
        if (!strcmp(errmsg, "X11: The DISPLAY environment variable is missing")) {
            std::cerr << "You seems not running with graphic display\n";
        }
        else if (!strcmp(errmsg, "WGL: The driver does not appear to support OpenGL")) {
            std::cerr << "Please consider install an OpenGL driver, or use the mesa driver\n";
        }
        else if (!strcmp(errmsg, "WGL: Failed to create OpenGL context")) {
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
    CHECK_GL(glEnable(GL_DEPTH_TEST));
    CHECK_GL(glDepthMask(GL_TRUE));

    // start main game loop
    while (!glfwWindowShouldClose(window)) {
        // render graphics
        CHECK_GL(glClear(GL_COLOR_BUFFER_BIT));
        CHECK_GL(glClearDepth(1.0f));
        CHECK_GL(glClear(GL_DEPTH_BUFFER_BIT));
        render();
        // refresh screen
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
