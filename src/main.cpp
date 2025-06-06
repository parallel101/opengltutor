#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "check_gl.hpp" // includes glad/glad.h
#include <GLFW/glfw3.h> // must be placed behind glad/glad.h
#include <iostream>
#include <cstring>
#include <cstdlib>

constexpr int Fineness = 120;
constexpr float pi = 3.1415926535897f;

/**
 * @brief 通过度数范围计算为对应百分比
 * @param du [0, 360]
 * @return constexpr int 
 */
constexpr int getShowRoundPercentage(float du) {
    return static_cast<int>(Fineness * (du / 360.f));
}

/**
 * @brief 画一个圆, 位于 (x, y), 半径是 r, 环半径是 r - dr
 * @tparam IsClockwise 是否是顺时针
 * @param x [-1, 1]
 * @param y [-1, 1]
 * @param r [-1, 1]
 * @param dr [-1, 1]
 * @param step 渲染步数
 * @param begin 渲染起始位置
 * @param end 渲染结束位置 [0, Fineness]
 */
 template <bool IsClockwise>
void showRound(
    float x, float y,
    float r, float dr,
    int step,
    int begin, int end
) {
    constexpr auto mod = Fineness + 1;
    for (int i = begin; step && i != end; i = (i + (2 * IsClockwise - 1) + mod) % mod, --step) {
        float angle = static_cast<float>(i) / Fineness * pi * 2;
        float angleNext = static_cast<float>(i + 1) / Fineness * pi * 2;

        glVertex3f(dr * std::sin(angle) + x, dr * std::cos(angle) + y, 0.f);
        glVertex3f(r * std::sin(angle) + x, r * std::cos(angle) + y, 0.f);
        glVertex3f(r * std::sin(angleNext) + x, r * std::cos(angleNext) + y, 0.f);

        glVertex3f(dr * std::sin(angle) + x, dr * std::cos(angle) + y, 0.f);
        glVertex3f(dr * std::sin(angleNext) + x, dr * std::cos(angleNext) + y, 0.f);
        glVertex3f(r * std::sin(angleNext) + x, r * std::cos(angleNext) + y, 0.f);
    }
}

static void render() {
    glBegin(GL_TRIANGLES);
    constexpr float r = 0.3f, dr = 0.15f;
    float xDown = std::sqrt(3.f) * 0.25f;

    static int cntR = 0, cntG = 0, cntB = 0, tmp = 0; // 控制动画和间隔时间

    glColor3f(1.f, 0.f, 0.f);
    showRound<false>(0.f, 0.5f, r, dr, cntR,
        getShowRoundPercentage(150), getShowRoundPercentage(210));

    glColor3f(0.f, 1.f, 0.f);
    showRound<false>(-xDown, -0.25f, r, dr, cntG,
        getShowRoundPercentage(30), getShowRoundPercentage(90));

    glColor3f(0.f, 0.f, 1.f);
    showRound<false>(xDown, -0.25f, r, dr, cntB,
        getShowRoundPercentage(330), getShowRoundPercentage(30));

    constexpr int MaxStep = static_cast<int>(Fineness / 360.f * (360 - 60));

    if (++cntR > MaxStep
        && ++cntG > MaxStep
        && ++cntB > MaxStep
        && ++tmp > MaxStep
    ) {
        cntR = cntG = cntB = tmp = 0;
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
