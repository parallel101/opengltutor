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

void OBJ::draw_obj_smooth() {
    std::vector<glm::vec3> faceNormalBuffer;
    std::vector<glm::vec3> vertexNormalBuffer(vertices.size());
    
    for (auto& face : faces) {
        auto const& a = vertices.at(face[0]);
        auto const& b = vertices.at(face[1]);
        auto const& c = vertices.at(face[2]);

        glm::vec3 fn = compute_normal(a, b, c);
        faceNormalBuffer.push_back(fn);
        vertexNormalBuffer[face[0]] += fn;
        vertexNormalBuffer[face[1]] += fn;
        vertexNormalBuffer[face[2]] += fn;
    }
    for (auto& vn : vertexNormalBuffer) {
        vn = glm::normalize(vn);
    }

    glBegin(GL_TRIANGLES);
    for (auto const& face : faces) {
        auto const& a = vertices.at(face[0]);
        auto const& b = vertices.at(face[1]);
        auto const& c = vertices.at(face[2]);
        
        auto const& n1 = vertexNormalBuffer.at(face[0]);
        auto const& n2 = vertexNormalBuffer.at(face[1]);
        auto const& n3 = vertexNormalBuffer.at(face[2]);

        glNormal3fv(glm::value_ptr(n1));
        glVertex3fv(glm::value_ptr(a));
        glNormal3fv(glm::value_ptr(n2));
        glVertex3fv(glm::value_ptr(b));
        glNormal3fv(glm::value_ptr(n3));
        glVertex3fv(glm::value_ptr(c));
    }
    CHECK_GL(glEnd());
}