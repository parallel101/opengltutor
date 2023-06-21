#include "Image.hpp"
#include <cstring>
#include <string>
#include <vector>
#include <stdexcept>
#include <glm/glm.hpp>

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_STATIC
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

template <>
Image<glm::u8vec4>::Image(std::string const &path) {
    int nx = 0, ny = 0, comp = 4;
    unsigned char *p = stbi_load(path.c_str(), &nx, &ny, &comp, comp);
    if (comp != 4) throw std::invalid_argument("invalid component return from stbi_load");
    m_flatimg.resize(nx * ny);
    std::memcpy(m_flatimg.data(), p, m_flatimg.size() * sizeof(m_flatimg[0]));
    stbi_image_free(p);
    m_nx = nx;
    m_ny = ny;
}

template <>
Image<glm::u8vec3>::Image(std::string const &path) {
    int nx = 0, ny = 0, comp = 3;
    unsigned char *p = stbi_load(path.c_str(), &nx, &ny, &comp, comp);
    if (comp != 3) throw std::invalid_argument("invalid component return from stbi_load");
    m_flatimg.resize(nx * ny);
    std::memcpy(m_flatimg.data(), p, m_flatimg.size() * sizeof(m_flatimg[0]));
    stbi_image_free(p);
    m_nx = nx;
    m_ny = ny;
}

template <>
Image<std::uint8_t>::Image(std::string const &path) {
    int nx = 0, ny = 0, comp = 1;
    unsigned char *p = stbi_load(path.c_str(), &nx, &ny, &comp, comp);
    if (comp != 1) throw std::invalid_argument("invalid component return from stbi_load");
    m_flatimg.resize(nx * ny);
    std::memcpy(m_flatimg.data(), p, m_flatimg.size() * sizeof(m_flatimg[0]));
    stbi_image_free(p);
    m_nx = nx;
    m_ny = ny;
}

template <>
Image<glm::vec4>::Image(std::string const &path) {
    int nx = 0, ny = 0, comp = 4;
    float *p = stbi_loadf(path.c_str(), &nx, &ny, &comp, comp);
    if (comp != 4) throw std::invalid_argument("invalid component return from stbi_loadf");
    m_flatimg.resize(nx * ny);
    std::memcpy(m_flatimg.data(), p, m_flatimg.size() * sizeof(m_flatimg[0]));
    stbi_image_free(p);
    m_nx = nx;
    m_ny = ny;
}

template <>
Image<glm::vec3>::Image(std::string const &path) {
    int nx = 0, ny = 0, comp = 3;
    float *p = stbi_loadf(path.c_str(), &nx, &ny, &comp, comp);
    if (comp != 3) throw std::invalid_argument("invalid component return from stbi_loadf");
    m_flatimg.resize(nx * ny);
    std::memcpy(m_flatimg.data(), p, m_flatimg.size() * sizeof(m_flatimg[0]));
    stbi_image_free(p);
    m_nx = nx;
    m_ny = ny;
}

template <>
Image<float>::Image(std::string const &path) {
    int nx = 0, ny = 0, comp = 1;
    float *p = stbi_loadf(path.c_str(), &nx, &ny, &comp, comp);
    if (comp != 1) throw std::invalid_argument("invalid component return from stbi_loadf");
    m_flatimg.resize(nx * ny);
    std::memcpy(m_flatimg.data(), p, m_flatimg.size() * sizeof(m_flatimg[0]));
    stbi_image_free(p);
    m_nx = nx;
    m_ny = ny;
}
