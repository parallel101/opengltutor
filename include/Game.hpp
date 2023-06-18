#pragma once

#include <memory>

struct Game {
    Game();
    ~Game();

    void tick(double dt);

    Game(Game &&) = delete;

    struct Self;

private:
    std::unique_ptr<Self> self;
};
