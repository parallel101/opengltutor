#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "check_gl.hpp" // includes glad/glad.h
#include <GLFW/glfw3.h> // must be placed behind glad/glad.h
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>

static std::vector<glm::vec3> vertices;
static std::vector<glm::uvec3> faces;

static void load_obj(std::string path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path << '\n';
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.substr(0, 2) == "v ") {
            std::istringstream s(line.substr(2));
            glm::vec3 vertex;
            s >> vertex.x >> vertex.y >> vertex.z;
            vertices.push_back(vertex);
        } else if (line.substr(0, 2) == "f ") {
           std::istringstream s(line.substr(2));
           std::string splitted;
           std::vector<unsigned int> indices;
           while (std::getline(s, splitted, ' ')) {
               unsigned int index;
               std::istringstream(splitted) >> index;
               indices.push_back(index - 1);
           }
           for (size_t i = 2; i < indices.size(); i++) {
               glm::uvec3 face = {indices[0], indices[i - 1], indices[i]};
               faces.push_back(face);
           }
       }
    }

    file.close();
    std::cout << "Loaded " << vertices.size() << " vertices, " << faces.size() << " faces.\n";
}

static void draw_obj() {
    glBegin(GL_TRIANGLES);

    for (auto const &face : faces) {
        auto const &a = vertices.at(face.x);
        auto const &b = vertices.at(face.y);
        auto const &c = vertices.at(face.z);

        glVertex3fv(glm::value_ptr(a));
        glVertex3fv(glm::value_ptr(b));
        glVertex3fv(glm::value_ptr(c));
    }

    CHECK_GL(glEnd());
}

static void render() {
    draw_obj();
}

static void mouse_button_callback
( GLFWwindow *window
, int button
, int action
, int mods
) {
    if ( button == GLFW_MOUSE_BUTTON_LEFT
      && action == GLFW_PRESS
    ) { // when left mouse button is pressed:
        double xpos, ypos;
        int width, height;
        glfwGetCursorPos(window, &xpos, &ypos);
        glfwGetWindowSize(window, &width, &height);

        float x = (float)(2 * xpos / width - 1);
        float y = (float)(2 * (height - ypos) / height - 1);

        /* camera_on_lmb(x, y); */
    }
}

static void initialize() {
    load_obj("/home/bate/Codes/zeno_assets/assets/monkey.obj");
    CHECK_GL(glEnable(GL_DEPTH_TEST));
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

    glfwSetMouseButtonCallback(window, mouse_button_callback);

    initialize();
    // start main game loop
    while (!glfwWindowShouldClose(window)) {
        // render graphics
        CHECK_GL(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));
        render();
        // refresh screen
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
