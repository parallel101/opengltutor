#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "check_gl.hpp" // includes glad/glad.h
#include <GLFW/glfw3.h> // must be placed behind glad/glad.h
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <cstdlib>

static void render() {
    ////render triangle
    //glBegin(GL_TRIANGLES);
    //glColor3f(1.0f, 0.0f, 0.0f);
    //glVertex3f(0.0f, 0.5f, 0.0f);
    //glColor3f(0.0f, 1.0f, 0.0f);
    //glVertex3f(-0.5f, -0.5f, 0.0f);
    //glColor3f(0.0f, 0.0f, 1.0f);
    //glVertex3f(0.5f, -0.5f, 0.0f);
    //CHECK_GL(glEnd());

	//// render concentric circle
    //glBegin(GL_TRIANGLES);
    //constexpr int n = 5000;
    //constexpr float pi = 3.1415926535897f;
    //float radius = 0.5f;
    //float inner_radius = 0.25f;
    //static int x = 0;
    //x++;
    //if (x > n)
    //    x -= n;
    //for (int i = 0; i < x; i++) {
    //    float angle = i / (float)n * pi * 2;
    //    float angle_next = (i + 1) / (float)n * pi * 2;
    //    //glVertex3f(0.0f, 0.0f, 0.0f);
    //    glVertex3f(radius * sinf(angle), radius * cosf(angle), 0.0f);
    //    glVertex3f(radius * sinf(angle_next), radius * cosf(angle_next), 0.0f);
    //    glVertex3f(inner_radius * sinf(angle), inner_radius * cosf(angle), 0.0f);

    //    glVertex3f(inner_radius * sinf(angle_next), inner_radius * cosf(angle_next), 0.0f);
    //    glVertex3f(inner_radius * sinf(angle), inner_radius * cosf(angle), 0.0f);
    //    glVertex3f(radius * sinf(angle_next), radius * cosf(angle_next), 0.0f);

    //}
    //CHECK_GL(glEnd());

	// render OpenCV icon
    glBegin(GL_TRIANGLES);
    constexpr int n = 5000;
    constexpr float pi = 3.1415926535897f;
    float radius = 0.3f;
    float inner_radius = 0.12f;
    float x = 0, y = 0;
    //int clear_start = 0, clear_end = 0;
    //glm::vec2 start = glm::vec2(0, n / 2);
    //glm::vec2 end = glm::vec2(n / 2, n);
    //float start[2] = { 0, n / 2 }, end[2] = { n / 2, n };
    float start = 0, end = 0;

    static float bound = 0;
    bound++;
    if (bound > n)
        bound -= n;

    for (int count = 0; count < 3; count++) {
        /*switch (count) {
            case 0: x =  0.0f                                , y =  0.4f                                , clear_start = 150.0f / 360.0f * n, clear_end = 210.0f / 360.0f * n; break;
            case 1: x = -0.4f * cosf(30.0f / 360.0f * 2 * pi), y = -0.4f * sinf(30.0f / 360.0f * 2 * pi), clear_start = 30.0f / 360.0f * n , clear_end = 90.0f / 360.0f * n; break;
            case 2: x =  0.4f * cosf(30.0f / 360.0f * 2 * pi), y = -0.4f * sinf(30.0f / 360.0f * 2 * pi), clear_start = 330.0f / 360.0f * n, clear_end = 30.0f / 360.0f * n; break;
        }*/
        /*switch (count) {
            case 0: x =  0.0f                                , y =  0.4f                                , end.x = 150.0f / 360.0f * n, end.y = 360.0f / 360.0f * n, start.x = 0.0f / 360.0f * n , start.y = 210.0f / 360.0f * n; break;
            case 1: x = -0.4f * cosf(30.0f / 360.0f * 2 * pi), y = -0.4f * sinf(30.0f / 360.0f * 2 * pi), end.x = 30.0f / 360.0f * n , end.y = 360.0f / 360.0f * n, start.x = 0.0f / 360.0f * n , start.y = 90.0f / 360.0f * n; break;
            case 2: x =  0.4f * cosf(30.0f / 360.0f * 2 * pi), y = -0.4f * sinf(30.0f / 360.0f * 2 * pi), end.x = 180.0f / 360.0f * n, end.y = 330.0f / 360.0f * n, start.x = 30.0f / 360.0f * n, start.y = 180.0f / 360.0f * n; break;
        }*/
        switch (count) {
            case 0: x =  0.0f                                , y =  0.4f                                , end = 150.0f / 360.0f * n, start = 210.0f / 360.0f * n; break;
            case 1: x = -0.4f * cosf(30.0f / 360.0f * 2 * pi), y = -0.4f * sinf(30.0f / 360.0f * 2 * pi), end = 30.0f / 360.0f * n, start = 90.0f / 360.0f * n; break;
            case 2: x =  0.4f * cosf(30.0f / 360.0f * 2 * pi), y = -0.4f * sinf(30.0f / 360.0f * 2 * pi), end = 330.0f / 360.0f * n, start = 30.0f / 360.0f * n; break;
        }

        for (float i = start; i < bound; i++) {
            float angle = i / (float)n * pi * 2;
            float angle_next = (i + 1) / (float)n * pi * 2;

            switch (count) {
                case 0: glColor3f(1.0f, 0.0f, 0.0f); break;
                case 1: glColor3f(0.0f, 1.0f, 0.0f); break;
                case 2: glColor3f(0.0f, 0.0f, 1.0f); break;
            }

            /*if ((start <= end) ? (i > start && i < end) : (i > clear_start || i < clear_end))
                glColor3f(0.0f, 0.0f, 0.0f);*/
            if ((start <= end) ? (i < start || i > end) : (i < start && i > end))
                glColor3f(0.0f, 0.0f, 0.0f);

            glVertex3f(radius * sinf(angle) + x, radius * cosf(angle) + y, 0.0f);
            glVertex3f(radius * sinf(angle_next) + x, radius * cosf(angle_next) + y, 0.0f);
            glVertex3f(inner_radius * sinf(angle) + x, inner_radius * cosf(angle) + y, 0.0f);

            glVertex3f(inner_radius * sinf(angle_next) + x, inner_radius * cosf(angle_next) + y, 0.0f);
            glVertex3f(inner_radius * sinf(angle) + x, inner_radius * cosf(angle) + y, 0.0f);
            glVertex3f(radius * sinf(angle_next) + x, radius * cosf(angle_next) + y, 0.0f);
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
