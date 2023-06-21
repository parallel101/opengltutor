#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <cassert>
#include <glm/glm.hpp>

template <class T = glm::u8vec4>
struct Image {
    std::vector<T> m_flatimg;
    std::size_t m_nx{}, m_ny{};

    using value_type = T;

    Image() = default;

    explicit Image(std::string const &path);

    T *data() {
        return m_flatimg.data();
    }

    T const *data() const {
        return m_flatimg.data();
    }

    std::size_t sizex() const {
        return m_nx;
    }

    std::size_t sizey() const {
        return m_ny;
    }

    std::size_t size() const {
        assert(m_nx * m_ny == m_flatimg.size());
        return m_nx * m_ny;
    }
};
