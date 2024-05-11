#pragma once

#include "check_gl.hpp"
#include <memory>
#include "InputCtl.hpp"

struct Game {
    struct Private;
    std::unique_ptr<Private> const m_private;

    InputCtl m_inputCtl;
    GLFWwindow *m_window;

    Game();
    ~Game();

    Game(Game &&) = delete;

    void set_window(GLFWwindow *window);
    void initialize();
    void render();
};
