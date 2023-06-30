#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "check_gl.hpp" // includes glad/glad.h
#include <GLFW/glfw3.h> // must be placed behind glad/glad.h
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <cstdlib>


constexpr float pi = 3.14159265358979f;


void drawRing(float center_x, float center_y, float outer_radius, float inner_radius = 0, float begin_angle = 0, float end_angle = 1, int n = 100)
{
    glBegin(GL_TRIANGLES);
    
    for (int i(0); i < n; ++i) {
        float radian_interval = (end_angle - begin_angle) / n * 2 * pi;
        float begin_radian = begin_angle * 2 * pi;

        glVertex3f(center_x + outer_radius * cosf(begin_radian + radian_interval * i), center_y + outer_radius * sinf(begin_radian + radian_interval * i), 0.0f);
        glVertex3f(center_x + inner_radius * cosf(begin_radian + radian_interval * i), center_y + inner_radius * sinf(begin_radian + radian_interval * i), 0.0f);
        glVertex3f(center_x + outer_radius * cosf(begin_radian + radian_interval * (i + 1)), center_y + outer_radius * sinf(begin_radian + radian_interval * (i + 1)), 0.0f);

        glVertex3f(center_x + inner_radius * cosf(begin_radian + radian_interval * i), center_y + inner_radius * sinf(begin_radian + radian_interval * i), 0.0f);
        glVertex3f(center_x + outer_radius * cosf(begin_radian + radian_interval * (i + 1)), center_y + outer_radius * sinf(begin_radian + radian_interval * (i + 1)), 0.0f);
        glVertex3f(center_x + inner_radius * cosf(begin_radian + radian_interval * (i + 1)), center_y + inner_radius * sinf(begin_radian + radian_interval * (i + 1)), 0.0f);
    }
    CHECK_GL(glEnd());
}


void drawOpenCVLogo()
{
    float distance = 0.5f;          // The distance from the center to the origin
    float inner_radius = 0.15f;
    float outer_radius = 0.4f;

    // Draw the red ring
    glColor3f(1.0f, 0.0f, 0.0f);
    drawRing(distance * cosf(3 / 12.0f * 2 * pi), distance * sinf(3 / 12.0f * 2 * pi), outer_radius, inner_radius, 5 / 6.0f, 10 / 6.0f);

    // Draw the green ring
    glColor3f(0.0f, 1.0f, 0.0f);
    drawRing(distance * cosf(7 / 12.0f * 2 * pi), distance * sinf(7 / 12.0f * 2 * pi), outer_radius, inner_radius, 1 / 6.0f, 6 / 6.0f);

    // Draw the blue ring
    glColor3f(0.0f, 0.0f, 1.0f);
    drawRing(distance * cosf(11 / 12.0f * 2 * pi), distance * sinf(11 / 12.0f * 2 * pi), outer_radius, inner_radius, 2 / 6.0f, 7 / 6.0f);
}


void drawTaiChi()
{
    float radius = 1.0f;

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);
    glVertex2f(0.0f, 1.0f);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(0.0f, -1.0f);
    CHECK_GL(glEnd());

    drawRing(0.0f, 0.0f, radius, 0.0f, 0.25f, 0.75f);
    glColor3f(0.0f, 0.0f, 0.0f);
    drawRing(0.0f, 0.0f, radius, 0.0f, 0.75f, 1.25f);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawRing(0.0f, 0.5f * radius, 0.5f * radius);
    glColor3f(0.0f, 0.0f, 0.0f);
    drawRing(0.0f, -0.5f * radius, 0.5f * radius);
    drawRing(0.0f, 0.5f * radius, 0.2f * radius);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawRing(0.0f, -0.5f * radius, 0.2f * radius);
}


static void render() {
    // option 1: draw the OpenCV Logo
    // 
    // option 2: draw the TaiChi Logo

    switch (1)
    {
    case 1:
        drawOpenCVLogo();
        break;
    case 2:
        drawTaiChi();
        break;
    default:
        break;
    }
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
