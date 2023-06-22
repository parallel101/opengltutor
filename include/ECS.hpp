#pragma once

#include <vector>
#include <stdexcept>
#include <set>
#include <map>
#include <memory>

enum class ObjectId : std::uint32_t {};
enum class ComponentId : std::uint32_t {};

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

    ObjectId objIdTop = ObjectId(0);
    ComponentId compIdTop = ComponentId(0);

    template <class Func>
    void foreachComp(ComponentId compId, Func &&func) {
        for (auto objId: compHasObjs.at(compId)) {
            auto comp = objGetComp(objId, compId);
            func(comp, objId);
        }
    }

    template <class CompType, class Func>
    void foreachComp(Func &&func) {
        for (auto objId: compHasObjs.at(CompType::id())) {
            auto &comp = objGetComp<CompType>(objId);
            func(comp, objId);
        }
    }

    ObjectId addObj() {
        auto objId = objIdTop;
        objIdTop = ObjectId(std::uint32_t(objIdTop) + 1);
        auto obj = std::make_unique<Object>();
        if (!objects.emplace(objId, std::move(obj)).second) [[unlikely]]
            throw std::invalid_argument("object id conflict");
        return objId;
    }

    void addComp(ComponentId compId, std::unique_ptr<Component> (*factory)()) {
        compIdTop = ComponentId(std::max(std::uint32_t(compIdTop), std::uint32_t(compId) + 1));
        auto comp = std::make_unique<Object>();
        if (!componentFactories.emplace(compId, factory).second) [[unlikely]]
            throw std::invalid_argument("component id conflict");
        if (!compHasObjs.emplace(compId, std::set<ObjectId>()).second) [[unlikely]]
            throw std::invalid_argument("component id conflict in lut");
    }

    ComponentId addComp(std::unique_ptr<Component> (*factory)()) {
        auto compId = compIdTop;
        compIdTop = ComponentId(std::uint32_t(compIdTop) + 1);
        auto comp = std::make_unique<Object>();
        if (!componentFactories.emplace(compId, factory).second) [[unlikely]]
            throw std::invalid_argument("component id conflict");
        if (!compHasObjs.emplace(compId, std::set<ObjectId>()).second) [[unlikely]]
            throw std::invalid_argument("component id conflict in lut");
        return compId;
    }

    template <class T>
    static std::unique_ptr<Component> _componentFactoryFor() {
        return std::make_unique<T>();
    }

    template <class T>
    void addComp() {
        addComp(T::id(), _componentFactoryFor<T>);
    }

    Component *objAddComp(ObjectId objId, ComponentId compId) {
        auto comp = componentFactories.at(compId)();
        auto compPtr = comp.get();
        auto &obj = objects.at(objId);
        if (!obj->components.emplace(compId, std::move(comp)).second) [[unlikely]]
            throw std::invalid_argument("object component already exist");
        compHasObjs.at(compId).insert(objId);
        return compPtr;
    }

    template <class T>
    T &objAddComp(ObjectId objId) {
        auto comp = componentFactories.at(T::id())();
        auto compPtr = comp.get();
        auto &obj = objects.at(objId);
        if (!obj->components.emplace(T::id(), std::move(comp)).second) [[unlikely]]
            throw std::invalid_argument("object component already exist");
        compHasObjs.at(T::id()).insert(objId);
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
    T &objGetComp(ObjectId objId) {
        return *static_cast<T *>(objGetComp(objId, T::id()));
    }
};

struct Component {
    virtual ~Component() = default;

    Component &operator=(Component &&) = delete;
};
