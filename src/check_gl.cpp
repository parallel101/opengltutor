#include "check_gl.hpp"
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>
#include <set>

const char *check_gl::opengl_errno_name(GLenum err) {
    switch (err) {
#define PER_GL_ERROR(x) case GL_##x: return #x;
    PER_GL_ERROR(NO_ERROR)
    PER_GL_ERROR(INVALID_ENUM)
    PER_GL_ERROR(INVALID_VALUE)
    PER_GL_ERROR(INVALID_OPERATION)
    PER_GL_ERROR(STACK_OVERFLOW)
    PER_GL_ERROR(STACK_UNDERFLOW)
    PER_GL_ERROR(OUT_OF_MEMORY)
#undef PER_GL_ERROR
    }
    return "unknown error";
}

void check_gl::opengl_check_error(const char *filename, int lineno, const char *expr) {
    GLenum err = glad_glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << filename << ":" << lineno << ": " << expr << " failed: " << opengl_errno_name(err) << '\n';
        std::terminate();
    }
}

bool check_gl::opengl_has_extension(const char *extension) {
    static auto exts = [] {
        GLint n = 0;
        glGetIntegerv(GL_NUM_EXTENSIONS, &n);
        std::set<std::string> res;
        for (GLint i = 0; i < n; i++) {
            const GLubyte *ext = glGetStringi(GL_EXTENSIONS, i);
            res.emplace(reinterpret_cast<const char *>(ext));
        }
        return res;
    }();
    return exts.find(extension) != exts.end();
}

static void APIENTRY opengl_debug_message_callback(
    GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length,
    const GLchar *msg, const void *data)
{
    const char *_source;
    const char *_type;
    const char *_severity;

    switch (source) {
    case GL_DEBUG_SOURCE_API:
        _source = "API";
        break;

    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        _source = "WINDOW SYSTEM";
        break;

    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        _source = "SHADER COMPILER";
        break;

    case GL_DEBUG_SOURCE_THIRD_PARTY:
        _source = "THIRD PARTY";
        break;

    case GL_DEBUG_SOURCE_APPLICATION:
        _source = "APPLICATION";
        break;

    case GL_DEBUG_SOURCE_OTHER:
        _source = "UNKNOWN";
        break;

    default:
        _source = "UNKNOWN";
        break;
    }

    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        _type = "ERROR";
        break;

    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        _type = "DEPRECATED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        _type = "UDEFINED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_PORTABILITY:
        _type = "PORTABILITY";
        break;

    case GL_DEBUG_TYPE_PERFORMANCE:
        _type = "PERFORMANCE";
        break;

    case GL_DEBUG_TYPE_OTHER:
        _type = "OTHER";
        break;

    case GL_DEBUG_TYPE_MARKER:
        _type = "MARKER";
        break;

    default:
        _type = "UNKNOWN";
        break;
    }

    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        _severity = "HIGH";
        break;

    case GL_DEBUG_SEVERITY_MEDIUM:
        _severity = "MEDIUM";
        break;

    case GL_DEBUG_SEVERITY_LOW:
        _severity = "LOW";
        break;

    case GL_DEBUG_SEVERITY_NOTIFICATION:
        _severity = "NOTIFICATION";
        break;

    default:
        _severity = "UNKNOWN";
        break;
    }

    if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
        fprintf(stderr, "OpenGL error [%d]: %s of %s severity, raised from %s: %s\n",
                id, _type, _severity, _source, msg);
/* #ifndef NDEBUG */
/* #if defined(_MSC_VER) */
/*         __debugbreak(); */
/* #elif defined(__GNUC__) */
/*         __builtin_trap(); */
/* #endif */
/* #endif */
    }
}

void check_gl::opengl_try_enable_debug_message() {
    if (opengl_has_extension("GL_ARB_debug_output")) {
        auto my_glDebugMessageCallbackARB = (PFNGLDEBUGMESSAGECALLBACKPROC)glfwGetProcAddress("glDebugMessageCallbackARB");
        if (my_glDebugMessageCallbackARB) {
            my_glDebugMessageCallbackARB(opengl_debug_message_callback, nullptr);
            glEnable(GL_DEBUG_OUTPUT);
        }
    }
}

void check_gl::opengl_show_glfw_error_diagnose() {
    const char *errmsg;
    glfwGetError(&errmsg);
    if (!errmsg) errmsg = "(no error)";
    std::cerr << "GLFW failed to create window: " << errmsg << '\n';
    std::cerr << "==============================================\n";
    if (!strcmp(errmsg, "X11: The DISPLAY environment variable is missing")) {
        std::cerr << "** You seems not running with graphic display\n";
    } else if (!strcmp(errmsg, "WGL: The driver does not appear to support OpenGL")) {
        std::cerr << "** Please consider install an OpenGL driver, or use the mesa driver\n";
    } else if (!strcmp(errmsg, "WGL: Failed to create OpenGL context")) {
        std::cerr << "** Your driver seems not supporting the required OpenGL version\n";
    }
    std::cerr << "- If you have a physical graphic card (e.g. NVIDIA), install it from your graphic card vendor official website: http://www.nvidia.com/Download/index.aspx\n";
    std::cerr << "- If you are using Windows, download opengl32.dll from https://pan.baidu.com/s/1TZ6nVJC7DZIuUarZrGJYow?pwd=opgl and place it into the same directory as this executable file (alternatively you may download opengl32sw.dll from Internet and rename it to opengl32.dll to place into the same directory as this executable file)\n";
    std::cerr << "- If you are using Linux server or WSL1, install the mesa driver: https://ubuntuhandbook.org/index.php/2021/07/install-latest-mesa-ubuntu-20-04-21-04/";
    std::cerr << "- If you use WSL2, install WSLg: https://learn.microsoft.com/zh-cn/windows/wsl/tutorials/gui-apps\n";
    std::cerr << "- If you are using SSH connection for remote server, try connect it using ssh -X <ip address>\n";
    std::cerr << "- If you are using MacOS, you probably want to use Windows or Linux instead for better OpenGL support\n";
    std::cerr << "- If you are using a Laptop with dual-cards, make sure you have switch to dedicated card (NVIDIA) instead of the integrated card (Intel)\n";
    std::cerr << "==============================================\n";
#ifdef _WIN32
    std::system("pause");
#endif
}
