#include "check_gl.hpp"
#include "OBJ.hpp"
#include "Game.hpp"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>

void OBJ::load_obj(std::string path, bool grouped) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path << '\n';
        return;
    }

    std::string line;
    unsigned int current_group = 0;

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

                smoothing_groups[current_group].push_back(faces.size() - 1);
            }
        } else if (line.substr(0, 7) == "#group ") {
            std::istringstream s(line.substr(7));
            unsigned int group;
            s >> group;
            if (grouped) current_group = group;
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

static void object_draw_flat_pass(OBJ const& obj) {
    glBegin(GL_TRIANGLES);
    for (auto face : obj.faces) {
        auto const& a = obj.vertices.at(face[0]);
        auto const& b = obj.vertices.at(face[1]);
        auto const& c = obj.vertices.at(face[2]);

        glm::vec3 norm = compute_normal(a, b, c);
        glNormal3fv(glm::value_ptr(norm));

        glVertex3fv(glm::value_ptr(a));
        glVertex3fv(glm::value_ptr(b));
        glVertex3fv(glm::value_ptr(c));
    }
    CHECK_GL(glEnd());
}

static void normal_compute_pass(OBJ const& obj, std::vector<unsigned int> const& face_indices, std::vector<glm::vec3>& vertex_normal_buffer) {
    for (auto const& index : face_indices) {
        auto const& face = obj.faces.at(index);
        auto const& a = obj.vertices.at(face[0]);
        auto const& b = obj.vertices.at(face[1]);
        auto const& c = obj.vertices.at(face[2]);

        vertex_normal_buffer[face[0]] += compute_contrib_normal(a, b, c, 0);
        vertex_normal_buffer[face[1]] += compute_contrib_normal(a, b, c, 1);
        vertex_normal_buffer[face[2]] += compute_contrib_normal(a, b, c, 2);
    }
    for (auto& vn : vertex_normal_buffer) {
        vn = glm::normalize(vn);
    }
}

static void object_draw_smooth_pass(OBJ const& obj, std::vector<unsigned int> const& face_indices, std::vector<glm::vec3> const& vertex_normal_buffer) {
    glBegin(GL_TRIANGLES);
    for (auto const& index : face_indices) {
        auto const& face = obj.faces.at(index);
        auto const& a = obj.vertices.at(face[0]);
        auto const& b = obj.vertices.at(face[1]);
        auto const& c = obj.vertices.at(face[2]);
    
        auto const& n0 = vertex_normal_buffer.at(face[0]);
        auto const& n1 = vertex_normal_buffer.at(face[1]);
        auto const& n2 = vertex_normal_buffer.at(face[2]);
    
        glNormal3fv(glm::value_ptr(n0));
        glVertex3fv(glm::value_ptr(a));
        glNormal3fv(glm::value_ptr(n1));
        glVertex3fv(glm::value_ptr(b));
        glNormal3fv(glm::value_ptr(n2));
        glVertex3fv(glm::value_ptr(c));
    }
    CHECK_GL(glEnd());
}

void OBJ::draw_obj() {
    object_draw_flat_pass(*this);
}

void OBJ::draw_obj_smooth() {
    for (auto const& [i, group] : smoothing_groups) {
        std::vector<glm::vec3> vert_norm_buffer(vertices.size(), glm::vec3(0.0f));
        normal_compute_pass(*this, group, vert_norm_buffer);
        object_draw_smooth_pass(*this, group, vert_norm_buffer);
    }
}
