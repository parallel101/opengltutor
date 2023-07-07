#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "check_gl.hpp" // includes glad/glad.h
#include <GLFW/glfw3.h> // must be placed behind glad/glad.h
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <cstdlib>

using vec3 = glm::vec3;
using vec2 = glm::vec2;

void draw_annulas(vec3 center, float outer_radius, float inner_radius, vec3 color, float scale = 1.0f, int subdiv = 100, vec2 gap_range = {0.0f, 0.0f}, bool gap_mask = false) {
    // if :gap_mask is true, gap in the :gap_range, else draw in the :gap_range

    constexpr float pi = 3.1415926535897f;
    float r_o = scale * outer_radius;
    float r_i = scale * inner_radius;
    const vec3& C = center;
    const int& n = subdiv;
    
    int gap_begin = gap_range.x * n;
    int gap_end = gap_range.y * n;

    glBegin(GL_TRIANGLES);   
    glColor3f(color.x, color.y, color.z);
    for (int i = 0; i < n; i++) {
        float angle = i / (float)n * pi * 2;
        float angle_next = (i + 1) / (float)n * pi * 2;

        if (gap_mask && gap_begin <= i && i < gap_end) {
          continue;
        } else if (!gap_mask && !(gap_begin <= i && i < gap_end)) {
          continue;
        }
        glVertex3f(C.x + r_o * sinf(angle), C.y + r_o * cosf(angle), 0.0f);
        glVertex3f(C.x + r_o * sinf(angle_next), C.y + r_o * cosf(angle_next), 0.0f);
        glVertex3f(C.x + r_i * sinf(angle), C.y + r_i * cosf(angle), 0.0f);
        
        glVertex3f(C.x + r_i * sinf(angle_next), C.y + r_i * cosf(angle_next), 0.0f);
        glVertex3f(C.x + r_i * sinf(angle), C.y + r_i * cosf(angle), 0.0f);
        glVertex3f(C.x + r_o * sinf(angle_next), C.y + r_o * cosf(angle_next), 0.0f);
    }
    CHECK_GL(glEnd());
}

static void render() {
    constexpr float scale_factor = 0.76f;
    constexpr int subdiv_number = 120;
    constexpr float inner_radius = 0.5f, outer_radius = 0.2f;
    constexpr float _short = 0.25f;
    vec3 tri_sides = {_short, 2 * _short, _short * sqrtf(3)};

    draw_annulas(vec3(0.0f, tri_sides.y, 0.0f), inner_radius, outer_radius, vec3(1.0f, 0.0f, 0.0f), scale_factor, subdiv_number, vec2(0.5f-1.0f/12.0f, 0.5f+1.0f/12.0f), true);
    draw_annulas(vec3(-tri_sides.z, -tri_sides.x, 0.0f), inner_radius, outer_radius, vec3(0.0f, 1.0f, 0.0f), scale_factor, subdiv_number, vec2(1.0f/12.0f, 1.0f/4.0f), true);
    draw_annulas(vec3(tri_sides.z, -tri_sides.x, 0.0f), inner_radius, outer_radius, vec3(0.0f, 0.0f, 1.0f), scale_factor, subdiv_number, vec2(1.0/12.0f, 1.0f-1.0f/12.0f), false);
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
    GLFWwindow *window = glfwCreateWindow(640, 640, "Homework1: OpenCV Logo", nullptr, nullptr);
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
