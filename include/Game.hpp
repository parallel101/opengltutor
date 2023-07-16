#pragma once

#include "check_gl.hpp"
#include <memory>

struct Game {
    struct Private;
    std::unique_ptr<Private> const m_private;
    GLFWwindow *const m_window;

    Game(GLFWwindow *window);
    ~Game();

    Game(Game &&) = delete;

    void initialize();
    void render();
    void cursor_pos_callback(double xpos, double ypos);
    void mouse_button_callback(int button, int action, int mods);
    void scroll_callback(double xoffset, double yoffset);
    void key_callback(int key, int scancode, int action, int mods);
};
