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

        } else if (line.substr(0, 3) == "vt ") {
            std::istringstream s(line.substr(3));
            glm::vec2 uv;
            s >> uv.x >> uv.y;
            uvs.push_back(uv);

        } else if (line.substr(0, 3) == "vn ") {
            std::istringstream s(line.substr(3));
            glm::vec3 normal;
            s >> normal.x >> normal.y >> normal.z;
            normals.push_back(glm::normalize(normal));

        } else if (line.substr(0, 2) == "f ") {
            std::istringstream s(line.substr(2));
            std::string splitted;
            std::vector<glm::uvec3> indices;
            while (std::getline(s, splitted, ' ')) {
                glm::uvec3 index(1);
                std::istringstream ss(splitted);
                std::string slashsplitted;
                size_t indexsubs = 0;
                while (std::getline(ss, slashsplitted, '/') && indexsubs < 3) {
                    std::istringstream(slashsplitted) >> index[indexsubs++];
                }
                indices.push_back(index - 1u);
            }
            for (size_t i = 2; i < indices.size(); i++) {
                glm::umat3x3 face = glm::umat3x3(indices[0], indices[i - 1], indices[i]);
                faces.push_back(face);
            }
       }
    }

    if (!normals.size()) {
        normals.push_back(glm::vec3(0, 0, 1));
    }
    if (!uvs.size()) {
        uvs.push_back(glm::vec3(0));
    }

    file.close();
    std::cout << "Loaded " << vertices.size() << " vertices, " << faces.size() << " faces.\n";
}

static glm::vec3 perspective_divide(glm::vec4 pos) {
    return glm::vec3(pos.x / pos.w, pos.y / pos.w, pos.z / pos.w);
}

void OBJ::draw_obj() {
    glBegin(GL_TRIANGLES);
    glColor3f(0.9f, 0.6f, 0.1f);

    for (auto face : faces) {
        auto const &a_pos = vertices.at(face[0][0]);
        auto const &b_pos = vertices.at(face[1][0]);
        auto const &c_pos = vertices.at(face[2][0]);
        auto const &a_uv = uvs.at(face[0][1]);
        auto const &b_uv = uvs.at(face[1][1]);
        auto const &c_uv = uvs.at(face[2][1]);
        auto const &a_norm = normals.at(face[0][2]);
        auto const &b_norm = normals.at(face[1][2]);
        auto const &c_norm = normals.at(face[2][2]);

        glNormal3fv(glm::value_ptr(a_norm));

        glVertex3fv(glm::value_ptr(a_pos));
        glTexCoord2fv(glm::value_ptr(b_uv));

        glVertex3fv(glm::value_ptr(b_pos));
        glTexCoord2fv(glm::value_ptr(b_uv));

        glVertex3fv(glm::value_ptr(c_pos));
        glTexCoord2fv(glm::value_ptr(c_uv));
    }

    CHECK_GL(glEnd());
}
