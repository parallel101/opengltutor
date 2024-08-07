#pragma once

#include <glm/glm.hpp>
#include "check_gl.hpp"

template <class T>
struct glm_traits {
    inline static constexpr size_t components = 1;
    using type = T;
};

template <glm::length_t L, class T, glm::qualifier Q>
struct glm_traits<glm::vec<L, T, Q>> {
    inline static constexpr size_t components = L;
    using type = T;
};

template <class T>
struct gl_type_enum;

template <>
struct gl_type_enum<float> {
    inline static constexpr GLenum value = GL_FLOAT;
};

template <>
struct gl_type_enum<double> {
    inline static constexpr GLenum value = GL_DOUBLE;
};

template <>
struct gl_type_enum<int> {
    inline static constexpr GLenum value = GL_INT;
};

template <>
struct gl_type_enum<short> {
    inline static constexpr GLenum value = GL_SHORT;
};

template <>
struct gl_type_enum<char> {
    inline static constexpr GLenum value = GL_BYTE;
};

template <>
struct gl_type_enum<unsigned int> {
    inline static constexpr GLenum value = GL_UNSIGNED_INT;
};

template <>
struct gl_type_enum<unsigned short> {
    inline static constexpr GLenum value = GL_UNSIGNED_SHORT;
};

template <>
struct gl_type_enum<unsigned char> {
    inline static constexpr GLenum value = GL_UNSIGNED_BYTE;
};

template <class Type, class Class>
void gl_vertex_attrib(int location, Type Class::*member) {
    CHECK_GL(glEnableVertexAttribArray(/*index=*/location));
    Class const volatile *const object = reinterpret_cast<Class *>(1);
    size_t const offset = reinterpret_cast<char const volatile *>(&(object->*member))
        - reinterpret_cast<char const volatile *>(object);
    CHECK_GL(glVertexAttribPointer(
        /*index=*/location,
        /*size=*/glm_traits<Type>::components,
        /*type=*/gl_type_enum<typename glm_traits<Type>::type>::value,
        /*normalized=*/GL_FALSE,
        /*stride=*/sizeof(Class),
        /*pointer=*/(void *)offset));
}
