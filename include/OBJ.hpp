#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>
#include <string>

struct OBJ {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
    };
    std::vector<Vertex> vertices;
    std::vector<glm::uvec3> faces;

    void load_obj(std::string path);
    void auto_normal();
    void draw_obj();
};
