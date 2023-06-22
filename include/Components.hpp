#pragma once

#include "ECS.hpp"
#include <glad/glad.h>
#include "check_gl.hpp"
#include "GLRAII.hpp"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Image.hpp"

struct KinematicComponent : Component {
    glm::vec3 pos{};
    glm::vec3 vel{};

    static constexpr ComponentId id() { return ComponentId(__LINE__); }
};

struct BounceInBoxComponent : Component {
    void advance(Scene &scene, ObjectId objId, float dt) {
        auto &store = scene.objGetComp<KinematicComponent>(objId);
        for (std::size_t i = 0; i < 3; i++) {
            if (store.pos[i] <= -1.0f && store.vel[i] < 0.0f) {
                store.vel[i] = -store.vel[i] * (1.0f - decay_rate);
            }
            if (store.pos[i] >= 1.0f && store.vel[i] > 0.0f) {
                store.vel[i] = -store.vel[i] * (1.0f - decay_rate);
            }
        }
    }

    float decay_rate = 0.3f;

    static constexpr ComponentId id() { return ComponentId(__LINE__); }
};

struct GravityComponent : Component {
    void advance(Scene &scene, ObjectId objId, float dt) {
        auto &store = scene.objGetComp<KinematicComponent>(objId);
        store.pos += store.vel * dt;
        store.vel += gravity * dt;
    }

    glm::vec3 gravity{0.0f, -9.8f, 0.0f};

    static constexpr ComponentId id() { return ComponentId(__LINE__); }
};

struct RenderStrategy {
    virtual void render(Scene &scene, ObjectId objId) {
    }

    virtual ~RenderStrategy() = default;
};

struct RenderComponent : Component {
    void render(Scene &scene, ObjectId objId) {
        strategy->render(scene, objId);
    }

    std::unique_ptr<RenderStrategy> strategy;

    static constexpr ComponentId id() { return ComponentId(__LINE__); }
};

struct PointRenderStrategy : RenderStrategy {
    void render(Scene &scene, ObjectId objId) override {
        auto &store = scene.objGetComp<KinematicComponent>(objId);
        glBegin(GL_POINTS);
        glVertex3fv(glm::value_ptr(store.pos));
        CHECK_GL(glEnd());
    }
};

struct SpriteRenderStrategy : RenderStrategy {
    Image<> image;

    void render(Scene &scene, ObjectId objId) override {
        auto &store = scene.objGetComp<KinematicComponent>(objId);
        glBegin(GL_POINTS);
        glVertex3fv(glm::value_ptr(store.pos));
        CHECK_GL(glEnd());
    }
};
