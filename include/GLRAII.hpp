#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "check_gl.hpp"
#include <utility>

template <class Trait>
class GLRAII;

#define DEF_GLRAII_TRAIT(_trait) \
namespace _glraii_details { \
struct _trait; \
} \
using GL##_trait = GLRAII<_glraii_details::_trait>; \
struct _glraii_details::_trait

DEF_GLRAII_TRAIT(VAO) {
    static void allocate(GLuint &i) {
        CHECK_GL(glGenVertexArrays(1, &i));
    }

    static void deallocate(GLuint const &i) {
        CHECK_GL(glDeleteVertexArrays(1, &i));
    }

    static void bind(GLuint i) {
        CHECK_GL(glBindVertexArray(i));
    }

    static void unbind(GLuint i) {
        CHECK_GL(glBindVertexArray(0));
    }
};

DEF_GLRAII_TRAIT(VBO) {
    static void allocate(GLuint &i) {
        CHECK_GL(glGenBuffers(1, &i));
    }

    static void deallocate(GLuint const &i) {
        CHECK_GL(glDeleteBuffers(1, &i));
    }

    static void bind(GLuint i, GLuint target) {
        CHECK_GL(glBindBuffer(target, i));
    }

    static void unbind(GLuint i, GLuint target) {
        CHECK_GL(glBindBuffer(target, 0));
    }
};

DEF_GLRAII_TRAIT(FBO) {
    static void allocate(GLuint &i) {
        CHECK_GL(glGenFramebuffers(1, &i));
    }

    static void deallocate(GLuint const &i) {
        CHECK_GL(glDeleteFramebuffers(1, &i));
    }

    static void bind(GLuint i, GLuint target) {
        CHECK_GL(glBindFramebuffer(target, i));
    }

    static void unbind(GLuint i, GLuint target) {
        CHECK_GL(glBindFramebuffer(target, 0));
    }
};

DEF_GLRAII_TRAIT(RBO) {
    static void allocate(GLuint &i) {
        CHECK_GL(glGenRenderbuffers(1, &i));
    }

    static void deallocate(GLuint const &i) {
        CHECK_GL(glDeleteRenderbuffers(1, &i));
    }

    static void bind(GLuint i, GLuint target) {
        CHECK_GL(glBindRenderbuffer(target, i));
    }

    static void unbind(GLuint i, GLuint target) {
        CHECK_GL(glBindRenderbuffer(target, 0));
    }
};

DEF_GLRAII_TRAIT(Texture) {
    static void allocate(GLuint &i) {
        CHECK_GL(glGenTextures(1, &i));
    }

    static void deallocate(GLuint const &i) {
        CHECK_GL(glDeleteTextures(1, &i));
    }

    static void bind(GLuint i, GLuint target) {
        CHECK_GL(glBindTexture(target, i));
    }

    static void unbind(GLuint i, GLuint target) {
        CHECK_GL(glBindTexture(target, 0));
    }
};

DEF_GLRAII_TRAIT(Program) {
    static void allocate(GLuint &i) {
        CHECK_GL(i = glCreateProgram());
    }

    static void deallocate(GLuint const &i) {
        CHECK_GL(glDeleteProgram(i));
    }
};

DEF_GLRAII_TRAIT(Shader) {
    static void allocate(GLuint &i, GLuint type) {
        CHECK_GL(i = glCreateShader(type));
    }

    static void deallocate(GLuint const &i) {
        CHECK_GL(glDeleteShader(i));
    }
};

#undef DEF_GLRAII_TRAIT


template <class Trait>
class GLRAII {
    GLuint m_i;

    struct _PrivateTagFromExisting {
        explicit _PrivateTagFromExisting() = default;
    };

    struct _PrivateTagCreate {
        explicit _PrivateTagCreate() = default;
    };

    explicit GLRAII(_PrivateTagFromExisting, GLuint i) noexcept : m_i(i) {
    }

    template <class ...Args>
    explicit GLRAII(_PrivateTagCreate, Args &&...args) noexcept(Trait::allocate(&m_i, std::forward<Args>(args)...)) {
        Trait::allocate(&m_i, std::forward<Args>(args)...);
    }

public:
    // create an empty GLRAII object with handle being zero (null state)
    explicit GLRAII() noexcept : m_i(0) {
    }

    // move construct from another GLRAII object, no allocation, will empty that GLRAII object to null state
    GLRAII(GLRAII &&that) noexcept : m_i(std::exchange(that.m_i, 0)) {
    }

    // move from another GLRAII object, no allocation, will empty that object to null and free current holding handle if any
    GLRAII &operator=(GLRAII &&that) noexcept {
        if (this != &that)
            reset(std::exchange(that.m_i, 0));
        return *this;
    }

    // swap two GLRAII objects without any allocation
    GLRAII &swap(GLRAII &&that) noexcept {
        if (this != &that)
            std::swap(that.m_i, m_i);
        return *this;
    }

    // bind the handle in GLRAII object, invokes glBindXXXX
    template <class ...Args>
    GLRAII &bind(Args &&...args) noexcept(Trait::bind(m_i, std::forward<Args>(args)...)) {
        Trait::bind(m_i, std::forward<Args>(args)...);
    }

    // destroy the current GLRAII object, and release it
    ~GLRAII() noexcept {
        reset();
    }

    // reset the current GLRAII object to another handle, if current object already holds one, delete it
    // if called with no parameter, it will delete the current handle, and set the GLRAII object to null
    GLRAII &reset(GLuint i = 0) noexcept {
        if (m_i != 0) {
            Trait::deallocate(&m_i);
        }
        m_i = i;
        return *this;
    }

    // get the handle integer using explicit cast, if used in boolean context this would test if object is null
    explicit operator GLuint() const noexcept {
        return m_i;
    }

    // get the handle integer, simillar to .get() of unique_ptr
    GLuint handle() const noexcept {
        return m_i;
    }

    // leak handle out, set handle in current GLRAII object to 0, without deleting it, similiar to .release() of unique_ptr
    GLuint leak_handle() noexcept {
        return std::exchange(m_i, 0);
    }

    // static method, create GLRAII object from existing generated handle integer, usually combined with leak_handle
    static GLRAII from_existing(GLuint i) noexcept {
        return GLRAII(_PrivateTagFromExisting(), i);
    }

    // static method, allocate a new handle, return it in GLRAII object, invokes glGenXXX
    template <class ...Args>
    static GLRAII create(Args &&...args) noexcept(GLRAII(_PrivateTagCreate(), std::forward<Args>(args)...)) {
        GLRAII(_PrivateTagCreate(), std::forward<Args>(args)...);
    }
};
