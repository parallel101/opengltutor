#include "check_gl.hpp"
#include "Points.hpp"
#include <glm/ext.hpp>

void Points::init_cube(glm::vec3 min, glm::vec3 max, glm::uvec3 size, float radius) {
    vertices.clear();
    vertices.reserve(size.x * size.y * size.z);
    glm::vec3 scale = (max - min) * (1.0f / glm::vec3(size));
    glm::vec3 base = min + 0.5f * scale;
    for (glm::uint z = 0; z < size.z; ++z) {
        for (glm::uint y = 0; y < size.y; ++y) {
            for (glm::uint x = 0; x < size.x; ++x) {
                Vertex vertex;
                vertex.position = base + scale * glm::vec3(x, y, z);
                vertex.color = glm::vec3(0.9, 0.7, 0.4);
                vertex.velocity = glm::vec3();
                vertex.radius = radius;
                vertices.push_back(vertex);
            }
        }
    }
}

void DrawablePoints::draw() {
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

    CHECK_GL(glEnableVertexAttribArray(/*index=*/0));
    CHECK_GL(glVertexAttribPointer(
        /*index=*/0, /*size=*/3, /*type=*/GL_FLOAT, /*normalized=*/GL_FALSE,
        /*stride=*/sizeof(Vertex),
        /*pointer=*/(void *)offsetof(Vertex, position)));

    CHECK_GL(glEnableVertexAttribArray(/*index=*/1));
    CHECK_GL(glVertexAttribPointer(
        /*index=*/1, /*size=*/3, /*type=*/GL_FLOAT, /*normalized=*/GL_FALSE,
        /*stride=*/sizeof(Vertex),
        /*pointer=*/(void *)offsetof(Vertex, velocity)));

    CHECK_GL(glEnableVertexAttribArray(/*index=*/2));
    CHECK_GL(glVertexAttribPointer(
        /*index=*/2, /*size=*/3, /*type=*/GL_FLOAT, /*normalized=*/GL_FALSE,
        /*stride=*/sizeof(Vertex),
        /*pointer=*/(void *)offsetof(Vertex, color)));

    CHECK_GL(glEnableVertexAttribArray(/*index=*/3));
    CHECK_GL(glVertexAttribPointer(
        /*index=*/3, /*size=*/1, /*type=*/GL_FLOAT, /*normalized=*/GL_FALSE,
        /*stride=*/sizeof(Vertex),
        /*pointer=*/(void *)offsetof(Vertex, radius)));

    drawable.numElements = vertices.size();
}
