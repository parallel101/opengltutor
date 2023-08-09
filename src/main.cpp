#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "check_gl.hpp" // includes glad/glad.h
#include <GLFW/glfw3.h> // must be placed behind glad/glad.h
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <cstdlib>

constexpr float pi = 3.1415926535897f;
glm::vec3 rotate(glm::vec3 p, glm::vec3 c, int angle) {
    glm::vec3 afterPoint;
    float sita = float(angle) / 360.0f * 2.0f * pi;
    afterPoint.x = cosf(sita) * (p.x - c.x) - sinf(sita) * (p.y - c.y) + c.x;
    afterPoint.y = sinf(sita) * (p.x - c.x) + cosf(sita) * (p.y - c.y) + c.y;
    afterPoint.z = 0.0f;
    return afterPoint;
}
bool dotUpdateLock = false;
bool lockLock = false;

int x[] = { int(210 / 2) - 1,int(90 / 2) - 1,int(330 / 2) - 1 };

void donut(float o_x, float o_y, int startAngle, int endAngle, int num) { //o_x代表一个圆的中心x坐标，下面的点加上o_x为移动后的坐标
    //////……这里加上int startAngle,int endAngle
    glBegin(GL_TRIANGLES);
    constexpr int n = 180;
    float radius = 0.45f;
    float inner_radius = 0.225f;

    o_x = o_x / 2.0f + 0.5f;
    o_y = (-1.0f) * o_y / 2.0f + 0.5f;//要把圆心的坐标 转化为 坐标系原点在左上角的坐标

    o_x = 3.0f * o_x / 4.0f;
    o_y = 3.0f * o_y / 4.0f;//计算完缩小多少倍后
    radius = 3.0f * radius / 4.0f;
    inner_radius = 3.0f * inner_radius / 4.0f; //R(外圈圆半径)和r（内圈圆半径）要乘以缩小的倍数

    o_x = (o_x - 0.5f) * 2.0f;
    o_y = (o_y - 0.5f) * (-2.0f);//再转化为坐标系原点屏幕中心的坐标。

    if (!((x[num] % 180 > startAngle) && (x[num] > 180))) //因为要让它超过startAngle且跑过一圈就不++了
        x[num]++;
    else {
        if (lockLock == false) {
            dotUpdateLock = true;
            lockLock = true;
        }
    }


    if (num == 0) { //加上颜色
        glColor3f(1.0f, 0.0f, 0.0f);
    }
    else if (num == 1) {
        glColor3f(0.0f, 1.0f, 0.0f);
    }
    else {
        glColor3f(0.0f, 0.0f, 1.0f);
    }
    //std::cout << "DI" << num << "GE" << " : x=" << o_x << " ;y=" << o_y << std::endl;
    for (int i = endAngle; i < x[num]; i++) {
        //if (o_x - 0.5f <= 0.00001f) std::cout << "i: " << i << std::endl;
        if (i % 180 < startAngle || i % 180 >= endAngle) {
            float angle = i / (float)n * pi * 2;
            float angle_next = (i + 1) / (float)n * pi * 2;
            glVertex3f(radius * sinf(angle) + o_x, radius * cosf(angle) + o_y, 0.0f);
            glVertex3f(radius * sinf(angle_next) + o_x, radius * cosf(angle_next) + o_y, 0.0f);
            glVertex3f(inner_radius * sinf(angle) + o_x, inner_radius * cosf(angle) + o_y, 0.0f);

            glVertex3f(inner_radius * sinf(angle_next) + o_x, inner_radius * cosf(angle_next) + o_y, 0.0f);
            glVertex3f(inner_radius * sinf(angle) + o_x, inner_radius * cosf(angle) + o_y, 0.0f);
            glVertex3f(radius * sinf(angle_next) + o_x, radius * cosf(angle_next) + o_y, 0.0f);
        }
    }
    CHECK_GL(glEnd());
}

void donut_two(float o_x, float o_y, int startAngle, int endAngle, int num, int blueRotateAngle) { //o_x代表一个圆的中心x坐标，下面的点加上o_x为移动后的坐标
    //////……这里加上int startAngle,int endAngle
    glBegin(GL_TRIANGLES);
    constexpr int n = 180;//////……这里变成360，因为方便计算角度
    constexpr float pi = 3.1415926535897f;
    float radius = 0.45f;
    float inner_radius = 0.225f;

    o_x = o_x / 2.0f + 0.5f;
    o_y = (-1.0f) * o_y / 2.0f + 0.5f;//要把圆心的坐标 转化为 坐标系原点在左上角的坐标

    o_x = 3.0f * o_x / 4.0f;
    o_y = 3.0f * o_y / 4.0f;//计算完缩小多少倍后
    radius = 3.0f * radius / 4.0f;
    inner_radius = 3.0f * inner_radius / 4.0f; //R(外圈圆半径)和r（内圈圆半径）要乘以缩小的倍数

    o_x = (o_x - 0.5f) * 2.0f;
    o_y = (o_y - 0.5f) * (-2.0f);//再转化为坐标系原点屏幕中心的坐标。

    if (num == 0) { //加上颜色
        glColor3f(1.0f, 0.0f, 0.0f);
    }
    else if (num == 1) {
        glColor3f(0.0f, 1.0f, 0.0f);
    }
    else {
        glColor3f(0.0f, 0.0f, 1.0f);
    }

    for (int i = -15 + blueRotateAngle; i < n; i++) {
        if (i >= 0 && (i < startAngle || i >= endAngle)) {//////……这里变成这样，仔细想一下上图紫色的线
            float angle = i / (float)n * pi * 2;
            float angle_next = (i + 1) / (float)n * pi * 2;
            glVertex3f(radius * sinf(angle) + o_x, radius * cosf(angle) + o_y, 0.0f);
            glVertex3f(radius * sinf(angle_next) + o_x, radius * cosf(angle_next) + o_y, 0.0f);
            glVertex3f(inner_radius * sinf(angle) + o_x, inner_radius * cosf(angle) + o_y, 0.0f);

            glVertex3f(inner_radius * sinf(angle_next) + o_x, inner_radius * cosf(angle_next) + o_y, 0.0f);
            glVertex3f(inner_radius * sinf(angle) + o_x, inner_radius * cosf(angle) + o_y, 0.0f);
            glVertex3f(radius * sinf(angle_next) + o_x, radius * cosf(angle_next) + o_y, 0.0f);
        }
    }
    CHECK_GL(glEnd());
}


void fangfang(glm::vec3 vertices[]) {
    glBegin(GL_POLYGON);
    glColor3f(16.0f / 255.0f, 53.0f / 255.0f, 86.0f / 255.0f);
    glVertex3f(vertices[0].x, vertices[0].y, vertices[0].z);
    glColor3f(16.0f / 255.0f, 53.0f / 255.0f, 86.0f / 255.0f);
    glVertex3f(vertices[1].x, vertices[1].y, vertices[1].z);
    glColor3f(160.0f / 255.0f, 185.0f / 255.0f, 208.0f / 255.0f);
    glVertex3f(vertices[2].x, vertices[2].y, vertices[2].z);
    glColor3f(160.0f / 255.0f, 185.0f / 255.0f, 208.0f / 255.0f);
    glVertex3f(vertices[3].x, vertices[3].y, vertices[3].z);
    CHECK_GL(glEnd());
}



glm::vec3 O = glm::vec3(-0.25f, 0.625f, 0.0f);
float length = 0.04f;
glm::vec3 a = glm::vec3(O.x - length, O.y, 0.0f);
glm::vec3 b = glm::vec3(O.x - length, O.y - 2 * length, 0.0f);
glm::vec3 c = glm::vec3(O.x, O.y - 2 * length, 0.0f);
glm::vec3 d = glm::vec3(O.x + length, O.y - 2 * length, 0.0f);
glm::vec3 e = glm::vec3(O.x + length, O.y, 0.0f);

glm::vec3 f = glm::vec3(O.x - length, O.y - 3 * length, 0.0f);
glm::vec3 g = glm::vec3(O.x + length, O.y - 3 * length, 0.0f);


void dotUpdate(int rotateAngle) { //让其旋转多少度
    a = rotate(a, O, rotateAngle);
    b = rotate(b, O, rotateAngle);
    c = rotate(c, O, rotateAngle);
    d = rotate(d, O, rotateAngle);
    e = rotate(e, O, rotateAngle);
    f = rotate(f, O, rotateAngle);
    g = rotate(g, O, rotateAngle);
}

void dotHuiQu() { //和O.x的新位置保持更新
    a = glm::vec3(O.x - length, O.y, 0.0f);
    b = glm::vec3(O.x - length, O.y - 2 * length, 0.0f);
    c = glm::vec3(O.x, O.y - 2 * length, 0.0f);
    d = glm::vec3(O.x + length, O.y - 2 * length, 0.0f);
    e = glm::vec3(O.x + length, O.y, 0.0f);

    f = glm::vec3(O.x - length, O.y - 3 * length, 0.0f);
    g = glm::vec3(O.x + length, O.y - 3 * length, 0.0f);
}

void bresenham(int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2;

    for (int i = 0; i < 4; i++) { //速度快一点
        if (x0 != x1 || y0 != y1) {
            int e2 = err;
            if (e2 > -dx) { err -= dy; x0 += sx; }
            if (e2 < dy) { err += dx; y0 += sy; }
        }
        else {
            dotUpdateLock = true;
        }
    }
    O.x = float(x0) / 640.0f; O.y = float(y0) / 640.0f;
    //std::cout <<" : x=" << O.x << " ;y=" << O.y << std::endl;
}

void fangfangUnity() {
    glm::vec3 vertices1[] = {
        a,b,c,O
    };
    fangfang(vertices1);

    glm::vec3 vertices2[] = {
        d,e,O,c
    };
    fangfang(vertices2);
}

void sanjiao(glm::vec3 vertices[]) {
    glBegin(GL_TRIANGLES);
    glColor3f(16.0f / 255.0f, 53.0f / 255.0f, 86.0f / 255.0f);
    glVertex3f(vertices[0].x, vertices[0].y, vertices[0].z);
    glColor3f(16.0f / 255.0f, 53.0f / 255.0f, 86.0f / 255.0f);
    glVertex3f(vertices[1].x, vertices[1].y, vertices[1].z);
    glColor3f(160.0f / 255.0f, 185.0f / 255.0f, 208.0f / 255.0f);
    glVertex3f(vertices[2].x, vertices[2].y, vertices[2].z);
    glEnd();
}

void sanjiaoUnity() {
    glm::vec3 vertices1[] = {
    b,f,c
    };
    sanjiao(vertices1);

    glm::vec3 vertices2[] = {
    d,g,c
    };
    sanjiao(vertices2);
}

void banyuanUnity(int rotateAngle) {
    constexpr int n = 360;
    constexpr float pi = 3.1415926535897f;
    for (int i = 0; i < n; i++) {
        if (i < 90 || i >= 270) {
            float angle = i / (float)n * pi * 2;
            float angle_next = (i + 1) / (float)n * pi * 2;

            glm::vec3 k = glm::vec3(length * sinf(angle) + O.x, length * cosf(angle) + O.y, 0.0f);
            glm::vec3 l = glm::vec3(length * sinf(angle_next) + O.x, length * cosf(angle_next) + O.y, 0.0f);

            glm::vec3 vertices1[] = {
                rotate(k,O,rotateAngle),rotate(l,O,rotateAngle),O
            };
            sanjiao(vertices1);
        }
    }
}

glm::vec2 sangediandian[] = { //三个点的位置
    glm::vec2(-0.625f,-0.0244999f),//第二个点
    glm::vec2(0.125f,-0.0244999f),//第三个点
    glm::vec2(-0.25f,0.625f)//第一个短
};

int jiaodu[] = { 150/*第一个该转的角度*/,270,390 }; //三个点的角度

int zhuangtai = -1; //向第几个点进发

bool blueRotateLock = false;
int blueRotateAngle = 0;

static void render() {


    constexpr float R = 0.5f; //R = 2 / 4。 Opengl宽2，宽4R
    donut(0, R, int(150 / 2), int(210 / 2), 0);
    donut(-R, -(R - 0.268f * R), int(30 / 2), int(90 / 2), 1);
    if (blueRotateLock == false) {
        donut(R, -(R - 0.268f * R), int(270 / 2), int(330 / 2), 2);
    }
    else {
        if (blueRotateAngle <= 60)
            blueRotateAngle++;
        donut_two(R, -(R - 0.268f * R), int((270 + blueRotateAngle) / 2), int((330 + blueRotateAngle) / 2), 2, int(blueRotateAngle / 2));
    }

    if (dotUpdateLock == true) {
        if (zhuangtai < 2) {
            zhuangtai++;
            dotUpdateLock = false;
        }
    }

    if ((zhuangtai != -1) && (!((zhuangtai == 2) && dotUpdateLock == true))) { //当状态开始后且没有到头的时候画图
        bresenham(int(O.x * 640.0f), int(O.y * 640.0f), int(sangediandian[zhuangtai].x * 640.0f), int(sangediandian[zhuangtai].y * 640.0f));//必须放在前面，不然Ox在dotHuiQu();里更新就是下一次
        dotHuiQu();
        dotUpdate(jiaodu[zhuangtai]);

        fangfangUnity();
        sanjiaoUnity();
        banyuanUnity(jiaodu[zhuangtai]);
    }
    else if (zhuangtai == 2) {
        blueRotateLock = true;
    }
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
