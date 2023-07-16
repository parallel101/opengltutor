#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

const char *opengl_errno_name(GLenum err);
void check_gl_error(const char *filename, int lineno, const char *expr);


#define CHECK_GL(x) do { \
    (x); \
    check_gl_error(__FILE__, __LINE__, #x); \
} while (0)

bool opengl_has_extension(const char *extension);

void opengl_try_enable_debug_message();

void opengl_show_glfw_error_diagnose();
