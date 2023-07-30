#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace check_gl {
const char *opengl_errno_name(GLenum err);
void opengl_check_error(const char *filename, int lineno, const char *expr);
bool opengl_has_extension(const char *extension);
void opengl_try_enable_debug_message();
void opengl_show_glfw_error_diagnose();
}

#define CHECK_GL(x) do { \
    (x); \
    ::check_gl::opengl_check_error(__FILE__, __LINE__, #x); \
} while (0)
