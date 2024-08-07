#pragma once

#include "check_gl.hpp"
#include <glm/glm.hpp>
#include <vector>

struct DrawablePoints {
    GLVertexArray vao;
    GLBuffer vbo;
    std::size_t numElements;

    void draw();
};

struct Points {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 velocity;
        glm::vec3 acceleration;
        float mass;
        float charge;
        float dispersion;
        float radius;
        glm::vec3 color;
    };
    std::vector<Vertex> vertices;
    std::vector<std::vector<glm::uint>> grids;
    float average_distance = 0.0f;
    glm::vec3 bound_min = glm::vec3(-2.0f);
    glm::vec3 bound_max = glm::vec3(2.0f);

    void init_cube(glm::vec3 min, glm::vec3 max, glm::uvec3 size, float radius);
    void draw_points(DrawablePoints &drawable, bool dynamic);
    void evolve(float dt);
};
