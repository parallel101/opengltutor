#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "check_gl.hpp" // includes glad/glad.h
#include <GLFW/glfw3.h> // must be placed behind glad/glad.h
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <cstdlib>

static void render() {
    glBegin(GL_TRIANGLES);
    constexpr int n = 1000;
    constexpr float pi = 3.1415926535897f;
    float radius = 0.3f;
    float inner_radius = 0.15f;
    float top_x = 0.0f;
    float top_y = 1.0f / sqrt(3.0f);
    float left_x = 0.5f;
    float left_y = -0.5f * top_y;
    float right_x = -left_x;
    float right_y = left_y;

    for (int i = 0; i < 5 * n / 6; i++) {
        float angle1 = i / (float)n * pi * 2 - pi / 3;
        float angle1_next = (i + 1) / (float)n * pi * 2 - pi / 3;
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(top_x + radius * cosf(angle1), top_y + radius * sinf(angle1), 0.0f);
        glVertex3f(top_x + radius * cosf(angle1_next), top_y + radius * sinf(angle1_next), 0.0f);
        glVertex3f(top_x + inner_radius * cosf(angle1), top_y + inner_radius * sinf(angle1), 0.0f);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(top_x + inner_radius * cosf(angle1_next), top_y + inner_radius * sinf(angle1_next), 0.0f);
        glVertex3f(top_x + radius * cosf(angle1_next), top_y + radius * sinf(angle1_next), 0.0f);
        glVertex3f(top_x + inner_radius * cosf(angle1), top_y + inner_radius * sinf(angle1), 0.0f);

        float angle2 = i / (float)n * pi * 2 + pi / 3;
        float angle2_next = (i + 1) / (float)n * pi * 2 + pi / 3;
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(right_x + radius * cosf(angle2), right_y + radius * sinf(angle2), 0.0f);
        glVertex3f(right_x + radius * cosf(angle2_next), right_y + radius * sinf(angle2_next), 0.0f);
        glVertex3f(right_x + inner_radius * cosf(angle2), right_y + inner_radius * sinf(angle2), 0.0f);
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(right_x + inner_radius * cosf(angle2_next), right_y + inner_radius * sinf(angle2_next), 0.0f);
        glVertex3f(right_x + radius * cosf(angle2_next), right_y + radius * sinf(angle2_next), 0.0f);
        glVertex3f(right_x + inner_radius * cosf(angle2), right_y + inner_radius * sinf(angle2), 0.0f);

        float angle3 = i / (float)n * pi * 2 + 2 * pi / 3;
        float angle3_next = (i + 1) / (float)n * pi * 2 + 2 * pi / 3;
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(left_x + radius * cosf(angle3), left_y + radius * sinf(angle3), 0.0f);
        glVertex3f(left_x + radius * cosf(angle3_next), left_y + radius * sinf(angle3_next), 0.0f);
        glVertex3f(left_x + inner_radius * cosf(angle3), left_y + inner_radius * sinf(angle3), 0.0f);
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(left_x + inner_radius * cosf(angle3_next), left_y + inner_radius * sinf(angle3_next), 0.0f);
        glVertex3f(left_x + radius * cosf(angle3_next), left_y + radius * sinf(angle3_next), 0.0f);
        glVertex3f(left_x + inner_radius * cosf(angle3), left_y + inner_radius * sinf(angle3), 0.0f);

    }
    CHECK_GL(glEnd());

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
        render();
        // refresh screen
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
