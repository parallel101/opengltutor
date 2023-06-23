#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "check_gl.hpp"

struct GLRAII {
    GLuint i;

    GLRAII() {
        glGenVertexArrays(1, &i);
    }

    ~GLRAII() {
        glGenVertexArrays(1, &i);
    }
};
