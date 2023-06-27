#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "check_gl.hpp" // includes glad/glad.h
#include <GLFW/glfw3.h> // must be placed behind glad/glad.h
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <cstdlib>

constexpr float pi = 3.1415926535897f; 

void drawRing(float tran_x, float tran_y, float angle_start, float angle_end){
    glBegin(GL_TRIANGLES); 
    constexpr int n = 100; 
    float radius = 0.3f; 
    float inner_radius = 0.15f; 
    for (int i = 0; i < n; i++) { 
        float angle = angle_start + i / (float)n * (angle_end - angle_start ); 
        float angle_next = angle_start + (i + 1) / (float)n * (angle_end - angle_start ); 
        glVertex3f(tran_x + radius * sinf(angle), tran_y + radius * cosf(angle), 0.0f); 
        glVertex3f(tran_x + radius * sinf(angle_next), tran_y + radius * cosf(angle_next), 0.0f); 
        glVertex3f(tran_x + inner_radius * sinf(angle), tran_y + inner_radius * cosf(angle), 0.0f); 

        glVertex3f(tran_x + inner_radius * sinf(angle_next), tran_y + inner_radius * cosf(angle_next), 0.0f); 
        glVertex3f(tran_x + inner_radius * sinf(angle), tran_y + inner_radius * cosf(angle), 0.0f); 
        glVertex3f(tran_x + radius * sinf(angle_next), tran_y + radius * cosf(angle_next), 0.0f); 
     } 
     CHECK_GL(glEnd()); 
}


static void render() {
    glColor3f(1.0f,0.0,0.0);
    drawRing(0,0.5,(7/ 6.f) * pi,(17/ 6.f) * pi);
    glColor3f(0.0f,1.0,0.0);
    drawRing(0.5*cosf(7/6.f*pi),0.5*sinf(7/6.f*pi),(1/ 2.f) * pi,(13/ 6.f) * pi);
    glColor3f(0.0f,0.0,1.0);
    drawRing(0.5*cosf(11/6.f*pi),0.5*sinf(11/6.f*pi),(1/ 6.f) * pi,(11/6.f) * pi);
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
    glfwWindowHint(GLFW_SAMPLES, 8);

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
    CHECK_GL(glEnable(GL_MULTISAMPLE));
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
