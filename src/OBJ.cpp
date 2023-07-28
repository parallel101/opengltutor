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

static glm::vec3 compute_normal_biased(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
    // jisuan sanjiaoxin faxian, with asin factor
    glm::vec3 ab = b - a;
    glm::vec3 ac = c - a;
    glm::vec3 n = glm::cross(ab, ac);
    auto nlen = glm::length(n);
    if (nlen) {
        n *= glm::asin(nlen / (glm::length(ab) * glm::length(ac))) / nlen;
    }
    return n;
}

void OBJ::draw_obj(bool isFlat) {
    CHECK_GL(glShadeModel(isFlat ? GL_FLAT : GL_SMOOTH));

    std::vector<glm::vec3> normals;
    if (!isFlat) {
        normals.resize(vertices.size());
        for (auto const &face: faces) {
            auto const &a = vertices.at(face[0]);
            auto const &b = vertices.at(face[1]);
            auto const &c = vertices.at(face[2]);
            for (size_t i = 0; i < 3; i++) {
                normals[face[i]] += compute_normal_biased(a, b, c);
            }
        }
        for (auto &normal: normals) {
            normal = glm::normalize(normal);
        }
    }

    glBegin(GL_TRIANGLES);
    for (auto const &face: faces) {
        auto const &a = vertices[face[0]];
        auto const &b = vertices[face[1]];
        auto const &c = vertices[face[2]];
        if (isFlat) {
            glm::vec3 norm = compute_normal(a, b, c);
            glNormal3fv(glm::value_ptr(norm));
            glVertex3fv(glm::value_ptr(a));
            glVertex3fv(glm::value_ptr(b));
            glVertex3fv(glm::value_ptr(c));
        } else {
            auto const &an = normals[face[0]];
            auto const &bn = normals[face[1]];
            auto const &cn = normals[face[2]];
            glNormal3fv(glm::value_ptr(an));
            glVertex3fv(glm::value_ptr(a));
            glNormal3fv(glm::value_ptr(bn));
            glVertex3fv(glm::value_ptr(b));
            glNormal3fv(glm::value_ptr(cn));
            glVertex3fv(glm::value_ptr(c));
        }
    }
    CHECK_GL(glEnd());
}
