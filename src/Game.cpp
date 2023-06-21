#include "Game.hpp"
#include "ECS.hpp"
#include "Components.hpp"

Game::Game()
    : self(std::make_unique<Self>())
{}

Game::~Game() = default;

struct Game::Self {
    Scene scene;

    Self &operator=(Self &&) = delete;

    Self() {
        scene.addComp<KinematicComponent>();
        scene.addComp<GravityComponent>();
        scene.addComp<RenderComponent>();
        scene.addComp<BounceInBoxComponent>();

        auto obj = scene.addObj();
        scene.objAddComp<KinematicComponent>(obj);
        scene.objAddComp<RenderComponent>(obj);
        scene.objAddComp<GravityComponent>(obj);
        scene.objAddComp<BounceInBoxComponent>(obj);

        scene.objGetComp<RenderComponent>(obj).strategy = std::make_unique<PointRenderStrategy>();
        scene.objGetComp<KinematicComponent>(obj).vel = {0.2f, 0, 0};
    }

    void tick(double dt) {
        scene.foreachComp<RenderComponent>([&] (auto &comp, auto objId) {
            comp.render(scene, objId);
        });
        scene.foreachComp<GravityComponent>([&] (auto &comp, auto objId) {
            comp.advance(scene, objId, (float)dt);
        });
        scene.foreachComp<BounceInBoxComponent>([&] (auto &comp, auto objId) {
            comp.advance(scene, objId, (float)dt);
        });
    }
};

void Game::tick(double dt) {
    self->tick(dt);
}
