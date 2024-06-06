#include "check_gl.hpp"
#include "OBJ.hpp"
#include <sstream>
#include <fstream>
#include <iostream>
#include <glm/ext.hpp>

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
                glm::uvec3 face =
                    glm::uvec3(indices[0], indices[i - 1], indices[i]);
                faces.push_back(face);
            }
        }
    }

    file.close();
    std::cout << path << ": Loaded " << vertices.size() << " vertices, "
              << faces.size() << " faces.\n";

    auto_normal();
}

/* static glm::vec3 perspective_divide(glm::vec4 pos) { */
/*     return glm::vec3(pos.x / pos.w, pos.y / pos.w, pos.z / pos.w); */
/* } */

/* static glm::vec3 */
/* compute_normal(glm::vec3 const &a, glm::vec3 const &b, glm::vec3 const &c) { */
/*     // 计算三角形法线 */
/*     glm::vec3 ab = b - a; */
/*     glm::vec3 ac = c - a; */
/*     return glm::normalize(glm::cross(ab, ac)); */
/* } */

static glm::vec3 compute_normal_biased(glm::vec3 const &a, glm::vec3 const &b,
                                       glm::vec3 const &c) {
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

void DrawableOBJ::draw() {
    auto vaoBind = vao.bind();
    auto eboBind = ebo.bind(GL_ELEMENT_ARRAY_BUFFER);
    CHECK_GL(glDrawElements(/*mode=*/GL_TRIANGLES, /*count=*/numElements,
                            /*type=*/GL_UNSIGNED_INT, /*indices=*/(void *)0));
}

void OBJ::draw_obj(DrawableOBJ &drawable, bool dynamic) {
    // 把一个 CPU 内存中的 OBJ 对象，转化为可以直接绘制的，GPU 显存中的 DrawableOBJ 对象

    auto vboBind = drawable.vbo.make().bind(GL_ARRAY_BUFFER);
    CHECK_GL(glBufferData(
        /*target=*/GL_ARRAY_BUFFER, /*size=*/sizeof(Vertex) * vertices.size(),
        /*data=*/vertices.data(),
        /*usage=*/dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));

    auto eboBind = drawable.ebo.make().bind(GL_ELEMENT_ARRAY_BUFFER);
    CHECK_GL(glBufferData(
        /*target=*/GL_ELEMENT_ARRAY_BUFFER,
        /*size=*/sizeof(glm::uvec3) * faces.size(), /*data=*/faces.data(),
        /*usage=*/dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));

    auto vaoBind = drawable.vao.make().bind();

    CHECK_GL(glEnableVertexAttribArray(/*index=*/0));
    CHECK_GL(glVertexAttribPointer(
        /*index=*/0, /*size=*/3, /*type=*/GL_FLOAT, /*normalized=*/GL_FALSE,
        /*stride=*/sizeof(Vertex),
        /*pointer=*/(void *)offsetof(Vertex, position)));

    CHECK_GL(glEnableVertexAttribArray(/*index=*/1));
    CHECK_GL(glVertexAttribPointer(
        /*index=*/1, /*size=*/3, /*type=*/GL_FLOAT, /*normalized=*/GL_FALSE,
        /*stride=*/sizeof(Vertex),
        /*pointer=*/(void *)offsetof(Vertex, normal)));

    drawable.numElements = faces.size() * 3;
}

/* void OBJ::legacy_draw_obj() { */
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
/* } */
