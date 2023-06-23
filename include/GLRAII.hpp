#pragma once

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
