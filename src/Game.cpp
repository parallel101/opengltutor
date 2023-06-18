#include "Game.hpp"
#include "SparseVec.hpp"
#include <vector>
#include <GL/gl.h>
#include "check_gl.hpp"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <stdexcept>
#include <set>
#include <map>

Game::Game()
    : self(std::make_unique<Self>())
{}

Game::~Game() = default;

using ObjectId = std::uint32_t;
using ComponentId = std::uint32_t;

struct Component;

struct Object {
    std::map<ComponentId, std::unique_ptr<Component>> components;

    Object &operator=(Object &&) = delete;
};

struct Event {
};

struct Scene {
    std::map<ComponentId, std::unique_ptr<Component>(*)()> componentFactories;
    std::map<ObjectId, std::unique_ptr<Object>> objects;
    std::map<ComponentId, std::set<ObjectId>> compHasObjs;

    ObjectId objIdTop = 0;
    ComponentId compIdTop = 0;

    template <class Func>
    void foreachComp(ComponentId compId, Func &&func) {
        for (auto objId: compHasObjs.at(compId)) {
            auto comp = objGetComp(objId, compId);
            func(comp, objId);
        }
    }

    template <class CompType, class Func>
    void foreachComp(Func &&func) {
        for (auto objId: compHasObjs.at(CompType::kId)) {
            auto &comp = objGetComp<CompType>(objId, CompType::kId);
            func(comp, objId);
        }
    }

    ObjectId addObj() {
        auto objId = objIdTop++;
        auto obj = std::make_unique<Object>();
        if (!objects.try_emplace(objId, std::move(obj)).second) [[unlikely]]
            throw std::invalid_argument("object id conflict");
        return objId;
    }

    void addPresetComp(ComponentId compId, std::unique_ptr<Component> (*factory)()) {
        compIdTop = std::max(compIdTop, compId + 1);
        auto comp = std::make_unique<Object>();
        if (!componentFactories.try_emplace(compId, factory).second) [[unlikely]]
            throw std::invalid_argument("component id conflict");
        if (!compHasObjs.try_emplace(compId).second) [[unlikely]]
            throw std::invalid_argument("component id conflict in lut");
    }

    ComponentId addComp(std::unique_ptr<Component> (*factory)()) {
        auto compId = compIdTop++;
        auto comp = std::make_unique<Object>();
        if (!componentFactories.try_emplace(compId, factory).second) [[unlikely]]
            throw std::invalid_argument("component id conflict");
        if (!compHasObjs.try_emplace(compId).second) [[unlikely]]
            throw std::invalid_argument("component id conflict in lut");
        return compId;
    }

    template <class T>
    static std::unique_ptr<Component> _componentFactoryFor() {
        return std::make_unique<T>();
    }

    template <class T>
    void addPresetComp() {
        addPresetComp(T::kId, _componentFactoryFor<T>);
    }

    template <class T>
    ComponentId addComp() {
        return addComp(_componentFactoryFor<T>);
    }

    Component *objAddComp(ObjectId objId, ComponentId compId) {
        auto comp = componentFactories.at(compId)();
        auto compPtr = comp.get();
        auto &obj = objects.at(objId);
        if (!obj->components.try_emplace(compId, std::move(comp)).second) [[unlikely]]
            throw std::invalid_argument("object component already exist");
        compHasObjs.at(compId).insert(objId);
        return compPtr;
    }

    template <class T>
    T &objAddComp(ObjectId objId) {
        auto comp = componentFactories.at(T::kId)();
        auto compPtr = comp.get();
        auto &obj = objects.at(objId);
        if (!obj->components.try_emplace(T::kId, std::move(comp)).second) [[unlikely]]
            throw std::invalid_argument("object component already exist");
        compHasObjs.at(T::kId).insert(objId);
        return *static_cast<T *>(compPtr);
    }

    void objRemoveComp(ObjectId objId, ComponentId compId) {
        auto &obj = objects.at(objId);
        obj->components.erase(compId);
        compHasObjs.erase(compId);
    }

    Component *objGetComp(ObjectId objId, ComponentId compId) {
        auto &obj = objects.at(objId);
        auto comp = obj->components.at(compId).get();
        return comp;
    }

    template <class T>
    T &objGetComp(ObjectId objId, ComponentId compId) {
        return *static_cast<T *>(objGetComp(objId, compId));
    }
};

struct Component {
    virtual ~Component() = default;

    Component &operator=(Component &&) = delete;
};

struct KinematicComponent : Component {
    glm::vec3 pos{};
    glm::vec3 vel{};

    static constexpr ComponentId kId = __LINE__;
};

struct GravityComponent : Component {
    void advance(Scene &scene, ObjectId objId, float dt) {
        auto &store = scene.objGetComp<KinematicComponent>(objId, KinematicComponent::kId);
        store.pos += store.vel * dt;
        store.vel += gravity * dt;
    }

    glm::vec3 gravity{0.0f, -9.8f, 0.0f};

    static constexpr ComponentId kId = __LINE__;
};

struct RenderParticleComponent : Component {
    void render(Scene &scene, ObjectId objId) {
        auto &store = scene.objGetComp<KinematicComponent>(objId, KinematicComponent::kId);
        glVertex3fv(glm::value_ptr(store.pos));
    }

    static constexpr ComponentId kId = __LINE__;
};

struct Game::Self {
    Scene scene;

    Self &operator=(Self &&) = delete;

    Self() {
        scene.addPresetComp<KinematicComponent>();
        scene.addPresetComp<GravityComponent>();
        scene.addPresetComp<RenderParticleComponent>();
        auto obj = scene.addObj();
        scene.objAddComp<KinematicComponent>(obj);
        scene.objAddComp<RenderParticleComponent>(obj);
        scene.objAddComp<GravityComponent>(obj);
    }

    void tick(double dt) {
        glBegin(GL_POINTS);
        scene.foreachComp<RenderParticleComponent>([&] (auto &comp, auto objId) {
            comp.render(scene, objId);
        });
        scene.foreachComp<GravityComponent>([&] (auto &comp, auto objId) {
            comp.advance(scene, objId, (float)dt);
        });
        CHECK_GL(glEnd());
    }
};

void Game::tick(double dt) {
    self->tick(dt);
}
