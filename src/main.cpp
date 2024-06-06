#include "check_gl.hpp"
#include <iostream>
#include "Game.hpp"
#ifdef _WIN32
#include <windows.h>
#include <cstdlib>

constexpr float g3 = 1.7320508075688f; // 根号3
constexpr float pi = 3.1415926535897f;

void setColor(int j)
{
    if (j == 0)
        glColor3f(1.0f, 0.0f, 0.0f);
    else if (j == 1)
        glColor3f(0.0f, 1.0f, 0.0f);
    else if (j == 2)
        glColor3f(0.0f, 0.0f, 1.0f);
}

bool missingAngle(int j, float angle){
    if (j == 0)
        if (angle > (pi * 5 / 6) && angle < (pi * 7 / 6))
            return true;
        else
            return false;
    else if (j == 1)
        if (angle > (pi / 6) && angle < (pi / 2))
            return true;
        else
            return false;
    else if (j == 2)
        if (angle > (pi / 6 * 11) || angle < (pi / 6))
            return true;
        else return false;
    return false;
}

static void render()
{
    glBegin(GL_TRIANGLES); // 开始画图, 画图模式，这里是点模式还有线模式

    constexpr int n = 200;

    float radius = g3 / 4 - 0.05;
    float inner_radius = radius * 2 / 5;
    float offset[3][3] = {
        {0.0f, 0.5f, 0.0f},
        {-(g3 / 4), -0.25f, 0.0f},
        {g3 / 4, -0.25f, 0.0f},
    };
    for (int j = 0; j < 3; j++)
    {
        for (int i = 0; i < n; i++)
        {
            float angle = i / (float)n * pi * 2;
            float angle_next = (i + 1) / (float)n * pi * 2;
            if (missingAngle(j, angle))
                continue;
            setColor(j);

            glVertex3f(offset[j][0] + radius * sinf(angle), offset[j][1] + radius * cosf(angle), 0.0f);
            glVertex3f(offset[j][0] + inner_radius * sinf(angle), offset[j][1] + inner_radius * cosf(angle), 0.0f);
            glVertex3f(offset[j][0] + inner_radius * sinf(angle_next), offset[j][1] + inner_radius * cosf(angle_next), 0.0f);

            glVertex3f(offset[j][0] + radius * sinf(angle), offset[j][1] + radius * cosf(angle), 0.0f);
            glVertex3f(offset[j][0] + inner_radius * sinf(angle_next), offset[j][1] + inner_radius * cosf(angle_next), 0.0f);
            glVertex3f(offset[j][0] + radius * sinf(angle_next), offset[j][1] + radius * cosf(angle_next), 0.0f);
        }
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