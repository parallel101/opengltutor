#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "check_gl.hpp"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#ifdef CMAKE_FOUND_X11
#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3native.h>
#include <X11/extensions/shape.h>
#endif

static std::vector<glm::vec3> vertices;
static std::vector<glm::vec2> uvs;
static std::vector<glm::vec3> normals;
static std::vector<glm::umat3x3> faces;

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

        } else if (line.substr(0, 3) == "vt ") {
            std::istringstream s(line.substr(3));
            glm::vec2 uv;
            s >> uv.x >> uv.y;
            uvs.push_back(uv);

        } else if (line.substr(0, 3) == "vn ") {
            std::istringstream s(line.substr(3));
            glm::vec3 normal;
            s >> normal.x >> normal.y >> normal.z;
            normals.push_back(glm::normalize(normal));

        } else if (line.substr(0, 2) == "f ") {
            std::istringstream s(line.substr(2));
            std::string splitted;
            std::vector<glm::uvec3> indices;
            while (std::getline(s, splitted, ' ')) {
                glm::uvec3 index(1);
                std::istringstream ss(splitted);
                std::string slashsplitted;
                size_t indexsubs = 0;
                while (std::getline(ss, slashsplitted, '/') && indexsubs < 3) {
                    std::istringstream(slashsplitted) >> index[indexsubs++];
                }
                indices.push_back(index - 1u);
            }
            for (size_t i = 2; i < indices.size(); i++) {
                glm::umat3x3 face = glm::umat3x3(indices[0], indices[i - 1], indices[i]);
                faces.push_back(face);
            }
       }
    }

    if (!normals.size()) {
        normals.push_back(glm::vec3(0, 0, 1));
    }
    if (!uvs.size()) {
        uvs.push_back(glm::vec3(0));
    }

    file.close();
    std::cout << "Loaded " << vertices.size() << " vertices, " << faces.size() << " faces.\n";
}

static void draw_obj() {
    glBegin(GL_TRIANGLES);

    glColor3f(0.9f, 0.6f, 0.1f);

    for (auto face : faces) {
        auto const &a_pos = vertices.at(face[0][0]);
        auto const &b_pos = vertices.at(face[1][0]);
        auto const &c_pos = vertices.at(face[2][0]);
        auto const &a_uv = uvs.at(face[0][1]);
        auto const &b_uv = uvs.at(face[1][1]);
        auto const &c_uv = uvs.at(face[2][1]);
        auto const &a_norm = normals.at(face[0][2]);
        auto const &b_norm = normals.at(face[1][2]);
        auto const &c_norm = normals.at(face[2][2]);

        glNormal3fv(glm::value_ptr(a_norm));

        glVertex3fv(glm::value_ptr(a_pos));
        glTexCoord2fv(glm::value_ptr(b_uv));

        glVertex3fv(glm::value_ptr(b_pos));
        glTexCoord2fv(glm::value_ptr(b_uv));

        glVertex3fv(glm::value_ptr(c_pos));
        glTexCoord2fv(glm::value_ptr(c_uv));
    }

    CHECK_GL(glEnd());
}

static void render(GLFWwindow *window) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    CHECK_GL(glViewport(0, 0, width, height));

    glm::mat4x4 projection = glm::perspective(glm::radians(40.0f), (float)width / height, 0.01f, 100.0f);
    /* projection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 0.0f, 100.0f); */
    /* projection = glm::frustum(-0.005f, 0.005f, -0.005f, 0.005f, 0.01f, 100.0f); */
    CHECK_GL(glMatrixMode(GL_PROJECTION));
    CHECK_GL(glLoadMatrixf(glm::value_ptr(projection)));

    glm::vec3 eye(0, 0, 5);
    glm::vec3 center(0, 0, 0);
    glm::vec3 up(0, 1, 0);
    glm::mat4x4 view = glm::lookAt(eye, center, up);

    static float angle = 0.0f;
    glm::mat4x4 model(1.0f);
    model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    angle += 0.5f;

    CHECK_GL(glMatrixMode(GL_MODELVIEW));
    CHECK_GL(glLoadMatrixf(glm::value_ptr(view * model)));

    draw_obj();
}

static void mouse_button_callback
( GLFWwindow *window
, int button
, int action
, int mods
) {
    double xpos, ypos;
    int width, height;
    glfwGetCursorPos(window, &xpos, &ypos);
    glfwGetWindowSize(window, &width, &height);

    float x = (float)(2 * xpos / width - 1);
    float y = (float)(2 * (height - ypos) / height - 1);

    if ( button == GLFW_MOUSE_BUTTON_LEFT
      && action == GLFW_PRESS
    ) { // when left mouse button is pressed:
    }
}

static void initialize(GLFWwindow *window) {
    load_obj("/home/bate/Codes/opengltutor/assets/monkey.obj");
    CHECK_GL(glEnable(GL_DEPTH_TEST));
    CHECK_GL(glEnable(GL_MULTISAMPLE));
    CHECK_GL(glEnable(GL_BLEND));
    CHECK_GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    CHECK_GL(glEnable(GL_LIGHTING));
    CHECK_GL(glEnable(GL_LIGHT0));
    CHECK_GL(glEnable(GL_COLOR_MATERIAL));
    CHECK_GL(glEnable(GL_CULL_FACE));
    CHECK_GL(glCullFace(GL_BACK));
    CHECK_GL(glFrontFace(GL_CCW));
}

int main() {
    // Initalize GLFW library
    if (!glfwInit()) {
        const char *errmsg;
        glfwGetError(&errmsg);
        if (!errmsg) errmsg = "(no error)";
        std::cerr << "failed to initialize GLFW: " << errmsg << '\n';
        return -1;
    }
    
    // Hint the version required: OpenGL 2.0
    constexpr int version = 20;
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, version / 10);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, version % 10);
    if (version >= 33) {
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    }
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Enable transparent framebuffer
    constexpr bool transparent = true;
    if (transparent) {
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
        glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_FALSE);
        glfwWindowHint(GLFW_FOCUSED, GLFW_FALSE);
    }

    // Create main window
    constexpr char title[] = "Example";
    GLFWwindow *window = glfwCreateWindow(1024, 768, title, NULL, NULL);

    // Test if window creation succeed
    if (!window) {
        opengl_show_glfw_error_diagnose();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Switch to fullscreen mode
    constexpr bool fullscreen = false;
    if (fullscreen) {
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        if (monitor) {
            const GLFWvidmode *mode = glfwGetVideoMode(monitor);
            if (mode) {
                glfwSetWindowSize(window, mode->width, mode->height);
                glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
                std::cerr << "Entered fullscreen mode: " << mode->width << 'x' << mode->height
                    << " at " << mode->refreshRate << " Hz\n";
            }
        }
    } else {
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        if (monitor) {
            const GLFWvidmode *mode = glfwGetVideoMode(monitor);
            if (mode) {
                int width, height;
                glfwGetWindowSize(window, &width, &height);
                glfwSetWindowPos(window, (mode->width - width) / 2, (mode->height - height) / 2);
            }
        }
    }

#ifdef CMAKE_FOUND_X11
    // Setup mouse click-through
    if (transparent) {
        auto dpy = glfwGetX11Display();
        int shape_event_base, shape_error_base;
        if (XShapeQueryExtension(dpy, &shape_event_base, &shape_error_base)) {
            auto win = glfwGetX11Window(window);
            Region region = XCreateRegion();
            XShapeCombineRegion(dpy, win, ShapeInput, 0, 0, region, ShapeSet);
            XDestroyRegion(region);
            XFlush(dpy);
        }
    }
#endif

    // Load glXXX function pointers (only after this you may use OpenGL functions)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        std::cerr << "GLAD failed to load GL functions\n";
        return -1;
    }
    opengl_try_enable_debug_message();

    // Print diagnostic information
    std::cerr << "OpenGL version: " << (const char *)glGetString(GL_VERSION) << '\n';

    // Register window callbacks
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // Initialize data structures
    initialize(window);
    // Start main game loop
    while (!glfwWindowShouldClose(window)) {
        // Render graphics
        CHECK_GL(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));
        render(window);
        // Update screen
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
