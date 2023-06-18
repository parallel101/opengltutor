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
using StorageId = std::uint32_t;
using EventId = std::uint32_t;

struct Component;

struct Object {
    std::map<ComponentId, std::unique_ptr<Component>> components;
};

struct Scene {
    std::map<ComponentId, std::unique_ptr<Component>(*)()> componentFactories;
    std::map<ObjectId, std::unique_ptr<Object>> objects;
    std::map<ComponentId, std::set<ObjectId>> compHasObjs;

    ObjectId objIdTop = 0;
    ComponentId compIdTop = 0;

    ObjectId addObj() {
        auto objId = objIdTop++;
        auto obj = std::make_unique<Object>();
        if (!objects.try_emplace(objId, obj).second) [[unlikely]]
            throw std::invalid_argument("object id conflict");
        return objId;
    }

    ComponentId addComp(std::unique_ptr<Component> (*factory)()) {
        auto compId = compIdTop++;
        auto comp = std::make_unique<Object>();
        if (!componentFactories.try_emplace(compId, factory).second) [[unlikely]]
            throw std::invalid_argument("component id conflict");
        return objId;
    }

    void objAddComp(ObjectId objId, ComponentId compId) {
        auto comp = componentFactories.at(compId);
        auto &obj = objects.at(objId);
        if (!obj->components.try_emplace(compId, std::move(comp)).second) [[unlikely]]
            throw std::invalid_argument("object component already exist");
        compHasObjs.try_emplace(compId).first->second.insert(objId);
    }
};

struct Component {
    virtual ~Component() = default;
};

struct Object {
    std::map<ComponentId, StorageId> compId2StoId;
};

struct Component {
    SparseVec<StorageId, ObjectId, std::uint32_t> objectIds;
    std::set<EventId> eventIds;
    BlobVec<StorageId> storage;
    void (*callback)(Game::Self &self, Component &comp, EventId eventId);
};

struct Event {
    std::set<ComponentId> compIds;
};

struct Game::Self {
    SparseVec<Object, ObjectId, std::uint32_t> objects;
    SparseVec<Component, ComponentId, std::uint32_t> components;
    SparseVec<Event, EventId, std::uint32_t> events;

    ComponentId cGravity;
    ComponentId cRenderParticle;
    ComponentId cKinematic;

    struct GravityStorage {
        void operator()(Self &self, ObjectId objId, EventId eventId) {
            auto &store = self.objGetComp<KinematicStorage>(objId, self.cKinematic);
            store.pos += store.vel;
        }
    };

    struct RenderParticleStorage {
        void operator()(Self &self, ObjectId objId, EventId eventId) {
            auto &store = self.objGetComp<KinematicStorage>(objId, self.cKinematic);
            glVertex3fv(glm::value_ptr(store.pos));
        }
    };

    struct KinematicStorage {
        glm::vec3 pos{};
        glm::vec3 vel{};

        void operator()(Self &self, ObjectId objId, EventId eventId) {
        }
    };

    Self(Self &&) = delete;

    Self() {
        cGravity = addComp<GravityStorage>();
        cRenderParticle = addComp<RenderParticleStorage>();
        cKinematic = addComp<KinematicStorage>();

        auto obj0 = addObj();
        objAddComp(obj0, cRenderParticle);
        objAddComp(obj0, cKinematic);
        /* objAddComp(obj0, cGravity); */
    }

    template <class Storage>
    ComponentId addComp() {
        auto [compId, comp] = components.emplace_back();
        comp->storage.initialize_type<Storage>();
        comp->callback = [] (Self &self, Component &comp, EventId eventId) {
            comp.objectIds.foreach([&] (StorageId stoId, ObjectId objId) {
                comp.storage.get<Storage>(stoId)(self, objId, eventId);
            });
        };
        return compId;
    }

    ObjectId addObj() {
        auto objId = objects.emplace_back().first;
        return objId;
    }

    void objAddComp(ObjectId objId, ComponentId compId) {
        auto &comp = components[compId];
        auto &obj = objects[objId];
        auto stoId = comp.objectIds.emplace_back(objId).first;
        comp.storage.emplace_back();
        obj.compId2StoId.try_emplace(objId, stoId);
    }

    template <class Storage>
    Storage &objGetComp(ObjectId objId, ComponentId compId) {
        auto &comp = components[compId];
        auto &obj = objects[objId];
        auto it = obj.compId2StoId.find(compId);
        assert(it != obj.compId2StoId.end());
        auto stoId = it->second;
        return comp.storage.get<Storage>(stoId);
    }

    void objRemoveComp(ObjectId objId, ComponentId compId) {
        auto &comp = components[compId];
        auto &obj = objects[objId];
        auto it = obj.compId2StoId.find(compId);
        assert(it != obj.compId2StoId.end());
        auto stoId = it->second;
        comp.objectIds.erase(stoId);
        obj.compId2StoId.erase(it);
    }

    void callComponent(ComponentId compId, EventId eventId) {
        auto &comp = components[compId];
        comp.callback(*this, comp, eventId);
    }

    void tick(double dt) {
        callComponent(cGravity, 0);
        glBegin(GL_POINTS);
        callComponent(cRenderParticle, 0);
        CHECK_GL(glEnd());
        /* components.foreach([&] (ComponentId compId, Component &comp) { */
        /* }); */
    }
};

void Game::tick(double dt) {
    self->tick(dt);
}
