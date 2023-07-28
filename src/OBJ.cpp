#include "check_gl.hpp"
#include "OBJ.hpp"
#include "Game.hpp"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>

void OBJ::load_obj(std::string path, bool enable_group) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path << '\n';
        return;
    }

    std::string line;
    int current_group = -1, grouped_face = 0;

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

                if (!enable_group && current_group >= 0) continue;
                face_groups[current_group].push_back(faces.size() - 1);
                grouped_face++;
            }
        } else if (line.substr(0, 7) == "#group ") {
            std::istringstream s(line.substr(7));
            unsigned int group;
            s >> group;
            current_group = group;
        }
    }

    file.close();
    std::cout << "Loaded " << vertices.size() << " vertices, " << faces.size() << " faces.\n";
    std::cout << grouped_face << " faces grouped.\n";
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

static glm::vec3 compute_contrib_normal(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, uint32_t vert) {
    std::vector<glm::vec3> face = {v0, v1, v2};
    auto const& a = face.at(vert);
    auto const& b = face.at((vert + 1) % 3);
    auto const& c = face.at((vert + 2) % 3);

    glm::vec3 ab = b - a;
    glm::vec3 ac = c - a;
    glm::vec3 norm = glm::cross(ab, ac);
    float contrib_w = asinf(static_cast<float>(norm.length()) / (ab.length() * ac.length()));

    return glm::normalize(norm) * contrib_w;
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
    std::vector<glm::vec3> vertexNormalBuffer(vertices.size());
    
    // NormalComputePass
    for (auto& face : faces) {
        auto const& a = vertices.at(face[0]);
        auto const& b = vertices.at(face[1]);
        auto const& c = vertices.at(face[2]);

        vertexNormalBuffer[face[0]] += compute_contrib_normal(a, b, c, 0);
        vertexNormalBuffer[face[1]] += compute_contrib_normal(a, b, c, 1);
        vertexNormalBuffer[face[2]] += compute_contrib_normal(a, b, c, 2);
    }
    for (auto& vn : vertexNormalBuffer) {
        vn = glm::normalize(vn);
    }

    // ObjectDrawPass
    glBegin(GL_TRIANGLES);
    for (auto const& face : faces) {
        auto const& a = vertices.at(face[0]);
        auto const& b = vertices.at(face[1]);
        auto const& c = vertices.at(face[2]);
        
        auto const& n0 = vertexNormalBuffer.at(face[0]);
        auto const& n1 = vertexNormalBuffer.at(face[1]);
        auto const& n2 = vertexNormalBuffer.at(face[2]);

        glNormal3fv(glm::value_ptr(n0));
        glVertex3fv(glm::value_ptr(a));
        glNormal3fv(glm::value_ptr(n1));
        glVertex3fv(glm::value_ptr(b));
        glNormal3fv(glm::value_ptr(n2));
        glVertex3fv(glm::value_ptr(c));
    }
    CHECK_GL(glEnd());
}

void OBJ::draw_obj_group_smooth() {
    // TODO
}
