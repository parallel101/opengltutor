#include "check_gl.hpp"
#include "OBJ.hpp"
#include "Game.hpp"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>

void OBJ::load_obj(std::string path) {
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

        } else if (line.substr(0, 2) == "f ") {
            std::istringstream s(line.substr(2));
            std::string splitted;
            std::vector<unsigned int> indices;
            while (std::getline(s, splitted, ' ')) {
                unsigned int index = 1;
                std::istringstream(splitted) >> index;
                indices.push_back(index - 1);
            }
            for (size_t i = 2; i < indices.size(); i++) {
                glm::uvec3 face = glm::uvec3(indices[0], indices[i - 1], indices[i]);
                faces.push_back(face);
            }
       }
    }

    file.close();
    std::cout << "Loaded " << vertices.size() << " vertices, " << faces.size() << " faces.\n";
}

static glm::vec3 perspective_divide(glm::vec4 pos) {
    return glm::vec3(pos.x / pos.w, pos.y / pos.w, pos.z / pos.w);
}

static glm::vec3 compute_normal(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
    // jisuan sanjiaoxin faxian
    glm::vec3 ab = b - a;
    glm::vec3 ac = c - a;
    return glm::normalize(glm::cross(ab, ac));
}

void OBJ::draw_obj() {
    glBegin(GL_TRIANGLES);

    for (auto face : faces) {
        auto const &a = vertices.at(face[0]);
        auto const &b = vertices.at(face[1]);
        auto const &c = vertices.at(face[2]);

        glm::vec3 norm = compute_normal(a, b, c);
        glNormal3fv(glm::value_ptr(norm));

        glVertex3fv(glm::value_ptr(a));
        glVertex3fv(glm::value_ptr(b));
        glVertex3fv(glm::value_ptr(c));
    }

    CHECK_GL(glEnd());
}

OBJ create_opencv_obj(glm::uint division, float inner_radius, float outer_radius, float height) {
    OBJ obj{};

    constexpr float PI = glm::pi<float>();
    constexpr float max_theta = PI * 5.0f / 3.0f;

    const float delta_theta = max_theta / division;

    const float y = height / 2.0f;

    for (glm::uint i = 0; i <= division; ++i) {
        const float theta = i * delta_theta;
        const float cos_t = glm::cos(theta);
        const float sin_t = glm::sin(theta);

        obj.vertices.emplace_back(inner_radius * sin_t, +y, inner_radius * cos_t);
        obj.vertices.emplace_back(inner_radius * sin_t, -y, inner_radius * cos_t);
        obj.vertices.emplace_back(outer_radius * sin_t, -y, outer_radius * cos_t);
        obj.vertices.emplace_back(outer_radius * sin_t, +y, outer_radius * cos_t);
    }

    for (glm::uint i = 0; i < division; ++i) {
        const glm::uint offset = i * 4;

        obj.faces.push_back(glm::uvec3{offset + 0, offset + 3, offset + 7});
        obj.faces.push_back(glm::uvec3{offset + 0, offset + 7, offset + 4});
        obj.faces.push_back(glm::uvec3{offset + 0, offset + 4, offset + 5});
        obj.faces.push_back(glm::uvec3{offset + 0, offset + 5, offset + 1});
        obj.faces.push_back(glm::uvec3{offset + 1, offset + 5, offset + 6});
        obj.faces.push_back(glm::uvec3{offset + 1, offset + 6, offset + 2});
        obj.faces.push_back(glm::uvec3{offset + 2, offset + 6, offset + 7});
        obj.faces.push_back(glm::uvec3{offset + 2, offset + 7, offset + 3});
    }

    obj.faces.push_back(glm::uvec3{0, 1, 2});
    obj.faces.push_back(glm::uvec3{0, 2, 3});
    obj.faces.push_back(glm::uvec3{4 * division, 4 * division + 3, 4 * division + 2});
    obj.faces.push_back(glm::uvec3{4 * division, 4 * division + 2, 4 * division + 1});

    return obj;
}
