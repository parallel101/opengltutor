#pragma once

#include "check_gl.hpp"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <memory>

struct Game {
    struct Private;
    std::unique_ptr<Private> m_private;
    GLFWwindow *m_window;

    Game(GLFWwindow *window);
    ~Game();

    Game(Game &&) = delete;

    void initialize();
    void render();
    void mouse_button_callback(int button, int action, int mods);
};
