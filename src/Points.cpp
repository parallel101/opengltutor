#include "check_gl.hpp"
#include "gl_attribs.hpp"
#include "Points.hpp"
#include <algorithm>
#include <glm/ext.hpp>
#include <random>

static glm::uint hash_uvec3(glm::uvec3 ipos) {
    return (ipos.x * 73856093) ^ (ipos.y * 19349663) ^ (ipos.z * 83492791);
}

static glm::uvec3 pos_to_uvec3(glm::vec3 pos, float cell_scale) {
    return glm::uvec3(glm::ivec3(glm::floor(pos * cell_scale)));
}

void Points::evolve(float dt) {
    if (vertices.empty()) return;
    if (grids.empty()) return;
    if (average_distance == 0.0f) return;

    float const half_dt = dt * 0.5f;
    float const max_interaction_ratio = 5.0f;
    float const min_interaction_ratio = 0.3f;
    float const max_interaction_ratio2 = max_interaction_ratio * max_interaction_ratio;
    float const min_interaction_ratio2 = min_interaction_ratio * min_interaction_ratio;
    float const inv_average_distance_2 = 1.0f / (average_distance * average_distance);
    float const cell_scale = 1.0f / (max_interaction_ratio * average_distance);

#if !__clangd__
    #pragma omp parallel for
#endif
    for (size_t i = 0; i < vertices.size(); ++i) {
        auto &atom = vertices[i];
        atom.velocity += atom.acceleration * half_dt;
        for (size_t axis = 0; axis < 3; ++axis) {
            if (atom.position[axis] > bound_max[axis] || atom.position[axis] < bound_min[axis]) {
                atom.velocity[axis] = -atom.velocity[axis];
            }
        }
        atom.position += atom.velocity * dt;
        atom.position = glm::clamp(atom.position, bound_min, bound_max);
        atom.velocity *= 0.999f;
    }

    for (size_t grid_id = 0; grid_id < grids.size(); ++grid_id) {
        grids[grid_id].clear();
    }
    for (size_t i = 0; i < vertices.size(); ++i) {
        auto &atom = vertices[i];
        glm::uint grid_id = hash_uvec3(pos_to_uvec3(atom.position, cell_scale)) % grids.size();
        grids[grid_id].push_back(glm::uint(i));
    }

    size_t count = 0;
#if !__clangd__
    #pragma omp parallel for reduction(+:count)
#endif
    for (size_t grid_id = 0; grid_id < grids.size(); ++grid_id) {
        if (grids[grid_id].empty())
            continue;

        auto pos = vertices[grids[grid_id].front()].position;
        auto this_grid_ipos = pos_to_uvec3(pos, cell_scale);
        glm::uint neigh_grid_ids[3 * 3 * 3];
        for (glm::uint dz = 0; dz != 3; ++dz) {
            for (glm::uint dy = 0; dy != 3; ++dy) {
                for (glm::uint dx = 0; dx != 3; ++dx) {
                    neigh_grid_ids[dz * 9 + dy * 3 + dx] = hash_uvec3(this_grid_ipos + (glm::uvec3(dx, dy, dz) - 1u)) % grids.size();
                }
            }
        }
        for (size_t this_vertex_id: grids[grid_id]) {
            auto &this_atom = vertices[this_vertex_id];
            this_atom.acceleration = {};
            auto this_dispersion_factor = this_atom.dispersion / this_atom.mass;
            for (size_t neigh_id = 0; neigh_id != 3 * 3 * 3; ++neigh_id) {
                for (size_t that_vertex_id: grids[neigh_grid_ids[neigh_id]]) {
                    ++count;
                    auto &that_atom = vertices[that_vertex_id];
                    auto distance_vector = this_atom.position - that_atom.position;
                    auto distance_2 = glm::dot(distance_vector, distance_vector);
                    distance_vector *= 1.0f / distance_2;
                    auto ratio_2 = inv_average_distance_2 * distance_2;
                    if (ratio_2 < min_interaction_ratio2 || ratio_2 > max_interaction_ratio2) {
                        continue;
                    }
                    ratio_2 = 1.0f / ratio_2;
                    auto ratio_6 = ratio_2 * ratio_2 * ratio_2;
                    auto ratio_12 = ratio_6 * ratio_6;
                    auto dispersion_strength = that_atom.dispersion * (ratio_12 - ratio_6);
                    this_atom.acceleration += this_dispersion_factor * dispersion_strength * distance_vector;
                }
            }
        }
    }
    printf("%f\n", (float)count / (vertices.size() * vertices.size()));

#if !__clangd__
    #pragma omp parallel for
#endif
    for (size_t i = 0; i < vertices.size(); ++i) {
        auto &atom = vertices[i];
        atom.velocity += atom.acceleration * half_dt;
    }
}

static glm::vec3 random_sphere(std::mt19937 &rng) {
    // generate a random vector on unit sphere
    // 1. generate z for -1 to 1
    // 2. generate phi for -M_PI to M_PI
    // 3. calculate x and y
    float z = std::uniform_real_distribution<float>(-1, 1)(rng);
    float t = std::uniform_real_distribution<float>(-M_PI, M_PI)(rng);
    float r = glm::sqrt(1.0f - z * z);
    return glm::vec3(r * glm::cos(t), r * glm::sin(t), z);
}

static glm::vec3 random_velocity(std::mt19937 &rng) {
    return random_sphere(rng) * std::gamma_distribution<float>(1)(rng);
}

void Points::init_cube(glm::vec3 min, glm::vec3 max, glm::uvec3 size, float radius) {
    std::mt19937 rng;
    vertices.clear();
    vertices.reserve(size.x * size.y * size.z);
    glm::vec3 scale = (max - min) * (1.0f / glm::vec3(size));
    glm::vec3 base = min + 0.5f * scale;
    for (glm::uint z = 0; z < size.z; ++z) {
        for (glm::uint y = 0; y < size.y; ++y) {
            for (glm::uint x = 0; x < size.x; ++x) {
                Vertex vertex;
                vertex.position = base + scale * glm::vec3(x, y, z);
                vertex.velocity = random_velocity(rng);
                vertex.acceleration = glm::vec3();
                vertex.mass = 1.0f;
                vertex.charge = 0.0f;
                vertex.dispersion = 1.0f;
                vertex.radius = radius;
                vertex.color = glm::vec3(0.9, 0.7, 0.4);
                vertices.push_back(vertex);
            }
        }
    }
    average_distance = radius * 2.0f;
    float volume = 8.0f / (scale.x * scale.y * scale.z);
    grids.resize(volume);
    bound_max = max;
    bound_min = min;
}

void DrawablePoints::draw() {
    auto vboBind = vbo.bind(GL_ARRAY_BUFFER);
    auto vaoBind = vao.bind();
    CHECK_GL(glDrawArrays(/*mode=*/GL_POINTS, /*first=*/0, /*count=*/numElements));
}

void Points::draw_points(DrawablePoints &drawable, bool dynamic) {
    // 把一个 CPU 内存中的 OBJ 对象，转化为可以直接绘制的，GPU 显存中的 DrawableOBJ 对象

    auto vboBind = drawable.vbo.make().bind(GL_ARRAY_BUFFER);
    CHECK_GL(glBufferData(
        /*target=*/GL_ARRAY_BUFFER, /*size=*/sizeof(Vertex) * vertices.size(),
        /*data=*/vertices.data(),
        /*usage=*/dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));

    auto vaoBind = drawable.vao.make().bind();

    gl_vertex_attrib(0, &Vertex::position);
    gl_vertex_attrib(1, &Vertex::velocity);
    gl_vertex_attrib(2, &Vertex::acceleration);
    gl_vertex_attrib(3, &Vertex::mass);
    gl_vertex_attrib(4, &Vertex::charge);
    gl_vertex_attrib(5, &Vertex::radius);
    gl_vertex_attrib(6, &Vertex::color);

    drawable.numElements = vertices.size();
}
