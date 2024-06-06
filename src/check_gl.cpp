#include "check_gl.hpp"
#include <iomanip>
#include <map>
#include <stdexcept>
#include <iostream>
#include <sstream>
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
    // initialized once on first entering opengl_has_extension:
    static auto extension_set = [] {
        GLint n = 0;
        glGetIntegerv(GL_NUM_EXTENSIONS, &n);
        std::set<std::string> res;
        for (GLint i = 0; i < n; i++) {
            const GLubyte *ext = glGetStringi(GL_EXTENSIONS, i);
            res.emplace(reinterpret_cast<const char *>(ext));
        }
        return res;
    }();
    return extension_set.find(extension) != extension_set.end();
}

static void APIENTRY opengl_debug_message_callback(
    GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length,
    const GLchar *msg, const void *data)
{
    (void)length;
    (void)data;

    // if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;

    // ignore some non-important warning ids:
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    const char *_source = "???";
    const char *_type = "???";
    const char *_severity = "???";

    switch (source) {
#define PER_GL_DEBUG_SOURCE(x) case GL_DEBUG_SOURCE_##x: _source = #x; break;
    PER_GL_DEBUG_SOURCE(API)
    PER_GL_DEBUG_SOURCE(WINDOW_SYSTEM)
    PER_GL_DEBUG_SOURCE(SHADER_COMPILER)
    PER_GL_DEBUG_SOURCE(THIRD_PARTY)
    PER_GL_DEBUG_SOURCE(APPLICATION)
    PER_GL_DEBUG_SOURCE(OTHER)
#undef PER_GL_DEBUG_SOURCE
    };

    switch (type) {
#define PER_GL_DEBUG_TYPE(x) case GL_DEBUG_TYPE_##x: _type = #x; break;
    PER_GL_DEBUG_TYPE(ERROR)
    PER_GL_DEBUG_TYPE(UNDEFINED_BEHAVIOR)
    PER_GL_DEBUG_TYPE(PORTABILITY)
    PER_GL_DEBUG_TYPE(PERFORMANCE)
    PER_GL_DEBUG_TYPE(OTHER)
    PER_GL_DEBUG_TYPE(MARKER)
#undef PER_GL_DEBUG_TYPE
    }

    switch (severity) {
#define PER_GL_DEBUG_SEVERITY(x) case GL_DEBUG_SEVERITY_##x: _severity = #x; break;
    PER_GL_DEBUG_SEVERITY(HIGH)
    PER_GL_DEBUG_SEVERITY(MEDIUM)
    PER_GL_DEBUG_SEVERITY(LOW)
    PER_GL_DEBUG_SEVERITY(NOTIFICATION)
#undef PER_GL_DEBUG_SEVERITY
    }

    fprintf(stderr, "OpenGL message [%d]: %s of %s severity, raised from %s: %s\n",
            id, _type, _severity, _source, msg);
    /* #ifndef NDEBUG */
    /* #if defined(_MSC_VER) */
    /*         __debugbreak(); */
    /* #elif defined(__GNUC__) */
    /*         __builtin_trap(); */
    /* #endif */
    /* #endif */
}

void check_gl::opengl_try_enable_debug_message() {
    int flags = 0;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        if (opengl_has_extension("GL_ARB_debug_output")) {
            auto ptr_glDebugMessageCallbackARB = (PFNGLDEBUGMESSAGECALLBACKPROC)glfwGetProcAddress("glDebugMessageCallbackARB");
            if (ptr_glDebugMessageCallbackARB) {
                ptr_glDebugMessageCallbackARB(opengl_debug_message_callback, nullptr);
                glEnable(GL_DEBUG_OUTPUT);
            }
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

void check_gl::opengl_link_program(unsigned int program) {
    CHECK_GL(glLinkProgram(program));
    int status = GL_TRUE;
    CHECK_GL(glGetProgramiv(program, GL_LINK_STATUS, &status));
    if (status != GL_TRUE) [[unlikely]] {
        GLsizei logLength;
        CHECK_GL(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength));
        std::string log;
        log.resize(logLength);
        CHECK_GL(glGetProgramInfoLog(program, logLength, &logLength, log.data()));
        log.resize(logLength);
        std::cerr << "OpenGL ERROR linking program:\n" << log << '\n';
        std::terminate();
    }
}

void check_gl::opengl_shader_source(unsigned int shader, std::string const &src) {
    const char *srcList[1] = {src.c_str()};
    int srcLenList[1] = {(int)src.size()};
    CHECK_GL(glShaderSource(shader, 1, srcList, srcLenList));
    CHECK_GL(glCompileShader(shader));
    int status = GL_TRUE;
    CHECK_GL(glGetShaderiv(shader, GL_COMPILE_STATUS, &status));
    if (status != GL_TRUE) [[unlikely]] {
        GLsizei logLength;
        CHECK_GL(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength));
        std::string log;
        log.resize(logLength);
        CHECK_GL(glGetShaderInfoLog(shader, logLength, &logLength, log.data()));
        log.resize(logLength);
        std::map<int, std::string> lines;
        for (size_t i = 0; i < log.size(); i++) {
            if (i >= 1 && log[i] == '(' && log[i - 1] == '0' && (i == 1 || log[i - 2] == '\n')) {
                size_t j = log.find_first_of(")\n", i + 1);
                if (j != log.npos) [[likely]] {
                    size_t k = log.find_first_not_of(") :", j);
                    auto &s = lines[std::atoi(log.substr(i + 1, j - i - 1).c_str())];
                    size_t l = log.find('\n', k);
                    s.append(">>>>> ");
                    s.append(log.substr(k, l == log.npos ? log.npos : l - k));
                    s.push_back('\n');
                }
            }
        }
        int line = 2;
        std::ostringstream oss;
        auto flg = oss.flags();
        std::string lastErr;
        auto it = lines.find(1);
        if (it != lines.end()) {
            oss << "\033[33;1m  1 |\033[0m ";
            lastErr = it->second;
        } else {
            oss << "\033[37m  1 |\033[0m ";
        }
        for (auto const &c: src) {
            oss << c;
            if (c == '\n') {
                if (!lastErr.empty()) {
                    oss << "\033[33m" << lastErr << "\033[0m";
                    lastErr.clear();
                }
                auto it = lines.find(line);
                if (it != lines.end()) {
                    oss << "\033[33;1m";
                    lastErr = it->second;
                } else {
                    oss << "\033[37m";
                }
                oss << std::setw(3) << std::right << line;
                oss << " |\033[0m ";
                oss.flags(flg);
                ++line;
            }
        }
        if (!lastErr.empty()) {
            oss << "\033[33m" << lastErr << "\033[0m";
        }
        std::cerr << "OpenGL ERROR compiling shader:\n" << oss.str() << '\n';
        if (lines.empty())
            std::cerr << "Log messages:\n" << log << '\n';
        std::terminate();
    }
}
