#include "check_gl.hpp"
#include "OBJ.hpp"
#include "Game.hpp"
#include "fileutils.hpp"
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
            vertices.push_back({vertex, {}});

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
    std::cout << path << ": Loaded " << vertices.size() << " vertices, " << faces.size() << " faces.\n";

    auto_normal();
}

/* static glm::vec3 perspective_divide(glm::vec4 pos) { */
/*     return glm::vec3(pos.x / pos.w, pos.y / pos.w, pos.z / pos.w); */
/* } */

[[maybe_unused]] static glm::vec3 compute_normal(glm::vec3 const &a, glm::vec3 const &b, glm::vec3 const &c) {
    // 计算三角形法线
    glm::vec3 ab = b - a;
    glm::vec3 ac = c - a;
    return glm::normalize(glm::cross(ab, ac));
}

static glm::vec3 compute_normal_biased(glm::vec3 const &a, glm::vec3 const &b, glm::vec3 const &c) {
    // 计算三角形法线，带asin项加权的版本
    glm::vec3 ab = b - a;
    glm::vec3 ac = c - a;
    glm::vec3 n = glm::cross(ab, ac);
    auto nlen = glm::length(n);
    if (nlen != 0) [[likely]] {
        auto labc = glm::length(ab) * glm::length(ac);
        if (labc >= nlen) [[likely]] {
            n *= glm::asin(nlen / labc) / nlen;
        } else {
            n *= 1.0f / nlen;
        }
    }
    return n;
}

void OBJ::auto_normal() {
    for (auto &v: vertices) {
        v.normal = glm::vec3();
    }
    for (auto const &face: faces) {
        auto &a = vertices[face[0]];
        auto &b = vertices[face[1]];
        auto &c = vertices[face[2]];
        // 感谢 @gaoxinge 在 #46 中指出问题
        a.normal += compute_normal_biased(a.position, b.position, c.position);
        b.normal += compute_normal_biased(b.position, c.position, a.position);
        c.normal += compute_normal_biased(c.position, a.position, b.position);
    }
    for (auto &v: vertices) {
        v.normal = glm::normalize(v.normal);
    }
}


void OBJ::draw_obj() {
    unsigned int vao = 0;
    unsigned int vbo = 0;
    unsigned int ebo = 0;
    CHECK_GL(glGenVertexArrays(1, &vao));
    CHECK_GL(glGenBuffers(1, &vbo));
    CHECK_GL(glGenBuffers(1, &ebo));
    CHECK_GL(glBindVertexArray(vao));
    CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    CHECK_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW));
    CHECK_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
    CHECK_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::uvec3) * faces.size(), faces.data(), GL_STATIC_DRAW));
    CHECK_GL(glEnableVertexAttribArray(0));
    CHECK_GL(glEnableVertexAttribArray(1));
    CHECK_GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position)));
    CHECK_GL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal)));
    CHECK_GL(glDrawElements(GL_TRIANGLES, faces.size() * 3, GL_UNSIGNED_INT, (void *)0));
    CHECK_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    CHECK_GL(glBindVertexArray(0));
    CHECK_GL(glDeleteBuffers(1, &vbo));
    CHECK_GL(glDeleteBuffers(1, &ebo));
    CHECK_GL(glDeleteVertexArrays(1, &vao));

    /* int positionLocation = glGetAttribLocation(program, "position"); */
    /* int normalLocation = glGetAttribLocation(program, "normal"); */
    /* printf("position: %d; normal: %d\n", positionLocation, normalLocation); */
    /* CHECK_GL(glBindAttribLocation(program, 0, "position")); */
    /* CHECK_GL(glBindAttribLocation(program, 1, "normal")); */
    /* glBegin(GL_TRIANGLES); */
    /* for (auto const &face: faces) { */
    /*     auto const &a = vertices[face[0]]; */
    /*     auto const &b = vertices[face[1]]; */
    /*     auto const &c = vertices[face[2]]; */
    /*     glVertexAttrib3fv(1, glm::value_ptr(a.normal)); */
    /*     glVertexAttrib3fv(0, glm::value_ptr(a.position)); */
    /*     glVertexAttrib3fv(1, glm::value_ptr(b.normal)); */
    /*     glVertexAttrib3fv(0, glm::value_ptr(b.position)); */
    /*     glVertexAttrib3fv(1, glm::value_ptr(c.normal)); */
    /*     glVertexAttrib3fv(0, glm::value_ptr(c.position)); */
    /* } */
    /* CHECK_GL(glEnd()); */
}
