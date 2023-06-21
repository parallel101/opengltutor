#pragma once

#include <iostream>
#include <glad/glad.h>

static const char *opengl_errno_name(int err) {
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

static void check_gl_error(const char *filename, int lineno, const char *expr) {
    int err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << filename << ":" << lineno << ": " << expr << " failed: " << opengl_errno_name(err) << '\n';
        std::terminate();
    }
}

#define CHECK_GL(x) do { \
    (x); \
    check_gl_error(__FILE__, __LINE__, #x); \
} while (0)
