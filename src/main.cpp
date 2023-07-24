#include "check_gl.hpp"
#include <iostream>
#include <cmath>


static void paintCircularRing(bool flag, int st, int ed, float cx, float cy, float r, float g, float b)
{
    constexpr int n = 360;
    constexpr float pi = 3.141592653589793f;
    float radius = 0.3f;
    float inner_radius = 0.15f;

    glColor3f(r, g, b);

    for (int i = 0; i < n; i++)
    {
        if (flag)
        {
            if (i <= st || i >= ed)
            {
                continue;
            }
        }
        else
        {
            if (i >= st && i <= ed)
            {
                continue;
            }
        }


        float angle = i / (float)n * pi * 2;
        float angle_next = (i + 1) / (float)n * pi * 2;

        glVertex3f(radius * std::sinf(angle) + cx, radius * std::cosf(angle) + cy, 0.0f);
        glVertex3f(radius * std::sinf(angle_next) + cx, radius * std::cosf(angle_next) + cy, 0.0f);
        glVertex3f(inner_radius * std::sinf(angle) + cx, inner_radius * std::cosf(angle) + cy, 0.0f);

        glVertex3f(inner_radius * std::sinf(angle_next) + cx, inner_radius * std::cosf(angle_next) + cy, 0.0f);
        glVertex3f(inner_radius * std::sinf(angle) + cx, inner_radius * std::cosf(angle) + cy, 0.0f);
        glVertex3f(radius * std::sinf(angle_next) + cx, radius * std::cosf(angle_next) + cy, 0.0f);

    }
}

static void render()
{
    glBegin(GL_TRIANGLES);

    paintCircularRing(false, 150, 210, 0.0f, 0.35f, 1.0f, 0.0f, 0.0f);

    paintCircularRing(false, 30, 89, -0.35f, -0.35f, 0.0f, 1.0f, 0.0f);

    paintCircularRing(true, 30, 330, 0.35f, -0.35f, 0.0f, 0.0f, 1.0f);

    CHECK_GL(glEnd());
}

int main()
{
    if (!glfwInit())
    {
        throw std::runtime_error("failed to initialize GLFW");
    }

    auto window = glfwCreateWindow(640, 640, "Example", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        throw std::runtime_error("GLFW failed to create window");
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGL())
    {
        glfwTerminate();
        throw std::runtime_error("GLAD failed to load GL functions");
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    CHECK_GL(glEnable(GL_POINT_SMOOTH));
    CHECK_GL(glEnable(GL_BLEND));
    CHECK_GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    while (!glfwWindowShouldClose(window))
    {
        render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
