#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>
#include <string>
#include <unordered_map>

struct OBJ {
    std::vector<glm::vec3> vertices;
    std::vector<glm::uvec3> faces;
    std::unordered_map<unsigned int, std::vector<unsigned int>> face_groups;

    void load_obj(std::string path, bool enable_group = false);
    void draw_obj();
    void draw_obj_smooth();
    void draw_obj_group_smooth();   // TODO
};
