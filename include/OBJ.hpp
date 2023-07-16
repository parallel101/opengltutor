#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>
#include <string>

struct OBJ {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    std::vector<glm::umat3x3> faces;

    void load_obj(std::string path);
    void draw_obj();
};
