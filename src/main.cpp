#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "check_gl.hpp" // includes glad/glad.h
#include <GLFW/glfw3.h> // must be placed behind glad/glad.h
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <vector>

constexpr float PI = 3.1415926535897f;

float DigToPI(float dig)
{
    return dig / 360.f * 2 * PI;
}

struct Point
{
    float x;
    float y;
    float z;
};

struct Color
{
    float r;
    float g;
    float b;
};

struct Triangle
{
    Point vertexes[3];
    Color color;
};


static std::vector<Triangle> SpawnTaiji()
{
    std::vector<Triangle> res;
    // µ×É«
    float radius = 0.75f;
    for (int i = 0; i < 360; i++)
    {
        int ni = i + 1;
        float angle = DigToPI(i);
        float angleNext = DigToPI(ni);

        if (i >= 0 && i < 90)
        {
            // È«°×
            Triangle t;
            t.color = {1.0f, 1.0f, 1.0f};
            t.vertexes[0] = { 0.0f, 0.0f, 0.0f };
            t.vertexes[1] = { radius * sinf(angle), radius * cosf(angle), 0.0f };
            t.vertexes[2] = { radius * sinf(angleNext), radius * cosf(angleNext), 0.0f };
            res.push_back(t);
        }

        if (i >= 90 && i < 180)
        {
            float angleSemi = DigToPI(180.f - 2 * (180.f - i));
            float angleSemiN = DigToPI(180.f - 2 * (180.f - ni));
            float radiusSemi = radius / 2;

            // °×µ×
            {
                Triangle t;
                t.color = { 1.0f, 1.0f, 1.0f };
                t.vertexes[0] = { 0.0f, 0.0f, 0.0f };
                t.vertexes[1] = { radius * sinf(angle), radius * cosf(angle), 0.0f };
                t.vertexes[2] = { radius * sinf(angleNext), radius * cosf(angleNext), 0.0f };
                res.push_back(t);
            }
            // ºì°ëÔ²
            {
                Triangle t;
                t.color = { 1.0f, 0.0f, 0.0f };
                t.vertexes[0] = { 0.0f, 0.0f, 0.0f };
                t.vertexes[1] = { radiusSemi * sinf(angleSemi), radiusSemi * cosf(angleSemi) - radiusSemi, 0.0f };
                t.vertexes[2] = { radiusSemi * sinf(angleSemiN), radiusSemi * cosf(angleSemiN) - radiusSemi, 0.0f };
                res.push_back(t);
            }
        }

        if (i >= 180 && i < 270)
        {
            // È«ºì
            {
                Triangle t;
                t.color = { 1.0f, 0.0f, 0.0f };
                t.vertexes[0] = { 0.0f, 0.0f, 0.0f };
                t.vertexes[1] = { radius * sinf(angle), radius * cosf(angle), 0.0f };
                t.vertexes[2] = { radius * sinf(angleNext), radius * cosf(angleNext), 0.0f };
                res.push_back(t);
            }
        }

        if (i >= 270 && i < 360)
        {
            float angleSemi = DigToPI(180.f - 2 * (360 - i) + 180.f);
            float angleSemiN = DigToPI(180.f - 2 * (360 - ni) + 180.f);
            float radiusSemi = radius / 2;

            // ºìµ×
            {
                Triangle t;
                t.color = { 1.0f, 0.0f, 0.0f };
                t.vertexes[0] = { 0.0f, 0.0f, 0.0f };
                t.vertexes[1] = { radius * sinf(angle), radius * cosf(angle), 0.0f };
                t.vertexes[2] = { radius * sinf(angleNext), radius * cosf(angleNext), 0.0f };
                res.push_back(t);
            }

            // °×°ëÔ²
            {
                Triangle t;
                t.color = { 1.0f, 1.0f, 1.0f };
                t.vertexes[0] = { 0.0f, 0.0f, 0.0f };
                t.vertexes[1] = { radiusSemi * sinf(angleSemi), radiusSemi * cosf(angleSemi) + radiusSemi, 0.0f };
                t.vertexes[2] = { radiusSemi * sinf(angleSemiN), radiusSemi * cosf(angleSemiN) + radiusSemi, 0.0f };
                res.push_back(t);
            }
        }
    }

    // Á½¸öÔ²
    float radiusSmall = radius / 4;
    for (int i = 0; i < 360; i++)
    {
        int ni = i + 1;
        float angle = DigToPI(i);
        float angleNext = DigToPI(ni);

        // È«ºì
        {
            Triangle t;
            t.color = { 1.0f, 0.0f, 0.0f };
            t.vertexes[0] = { 0.0f, radius / 2, 0.0f };
            t.vertexes[1] = { radiusSmall * sinf(angle), radiusSmall * cosf(angle) + radius / 2, 0.0f };
            t.vertexes[2] = { radiusSmall * sinf(angleNext), radiusSmall * cosf(angleNext) + radius / 2, 0.0f };
            res.push_back(t);
        }

        // È«°×
        {
            Triangle t;
            t.color = { 1.0f, 1.0f, 1.0f };
            t.vertexes[0] = { 0.0f, -radius / 2, 0.0f };
            t.vertexes[1] = { radiusSmall * sinf(angle), radiusSmall * cosf(angle) - radius / 2, 0.0f };
            t.vertexes[2] = { radiusSmall * sinf(angleNext), radiusSmall * cosf(angleNext) - radius / 2, 0.0f };
            res.push_back(t);
        }
    }

    return res;
}

static std::vector<Triangle> taiji = SpawnTaiji();

Point Rotate(const Point& in, Point o, float d)
{
    float dPi = DigToPI(d);
    Point r;
    r.x = in.x * cosf(dPi) - in.y * sinf(dPi);
    r.y = in.x * sinf(dPi) + in.y * cosf(dPi);
    r.z = in.z;

    return r;
}

Triangle Rotate(const Triangle& in, Point o, float d)
{
    Triangle r;

    r.color = in.color;

    r.vertexes[0] = Rotate(in.vertexes[0], o, d);
    r.vertexes[1] = Rotate(in.vertexes[1], o, d);
    r.vertexes[2] = Rotate(in.vertexes[2], o, d);

    return r;
}

int rot = 0;

static void render() {
    glBegin(GL_TRIANGLES);

    if (rot > 360)
    {
        rot = 0;
    }
    rot++;

    for (const auto& triangle : taiji)
    {
        auto newTriangle = Rotate(triangle, Point{ 0, 0, 0 }, rot);
        glColor3f(newTriangle.color.r, newTriangle.color.g, newTriangle.color.b);
        glVertex3f(newTriangle.vertexes[0].x, newTriangle.vertexes[0].y, newTriangle.vertexes[0].z);
        glVertex3f(newTriangle.vertexes[1].x, newTriangle.vertexes[1].y, newTriangle.vertexes[1].z);
        glVertex3f(newTriangle.vertexes[2].x, newTriangle.vertexes[2].y, newTriangle.vertexes[2].z);
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
