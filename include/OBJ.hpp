#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>
#include <string>

struct OBJ {
    std::vector<glm::vec3> vertices;
    std::vector<glm::uvec3> faces;

    void load_obj(std::string path, const glm::mat4& model);
    void draw_obj(bool isFlat, const glm::vec3& color);
};
