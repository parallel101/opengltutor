#pragma once

#include "check_gl.hpp"
#include <cmath>

enum class Modifier {
    kNone,
    kCtrl,
    kShift,
    kAlt,
};

enum class MouseButton {
    kLMB,
    kRMB,
    kMMB,
    kWheel,
    kNone,
};

struct MouseBinding {
    Modifier modifier = Modifier::kNone;
    MouseButton mouseBtn = MouseButton::kLMB;

    bool check_is_scrolled(GLFWwindow *window) {
        bool modPressed = true;
        bool shift = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
        bool ctrl = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS;
        bool alt = glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS;
        auto mods = (shift ? GLFW_MOD_SHIFT : 0) | (ctrl ? GLFW_MOD_CONTROL : 0) | (alt ? GLFW_MOD_ALT : 0);
        switch (modifier) {
        case Modifier::kShift: modPressed = mods == GLFW_MOD_SHIFT; break;
        case Modifier::kCtrl: modPressed = mods == GLFW_MOD_CONTROL; break;
        case Modifier::kAlt: modPressed = mods == GLFW_MOD_ALT; break;
        default: modPressed = mods == 0; break;
        }
        bool wheelScrolled = false;
        switch (mouseBtn) {
        case MouseButton::kWheel: wheelScrolled = true; break;
        default: break;
        }
        return wheelScrolled && modPressed;
    }

    /* bool check_is_pressed(GLFWwindow *window, int button, int action, int mods) { */
    /*     bool modPressed = true; */
    /*     mods &= GLFW_MOD_SHIFT | GLFW_MOD_CONTROL | GLFW_MOD_ALT; */
    /*     switch (modifier) { */
    /*     case Modifier::kShift: modPressed = mods == GLFW_MOD_SHIFT; break; */
    /*     case Modifier::kCtrl: modPressed = mods == GLFW_MOD_CONTROL; break; */
    /*     case Modifier::kAlt: modPressed = mods == GLFW_MOD_ALT; break; */
    /*     default: modPressed = mods == 0; break; */
    /*     } */
    /*     bool btnPressed = false; */
    /*     switch (mouseBtn) { */
    /*     case MouseButton::kNone: btnPressed = false; break; */
    /*     case MouseButton::kLMB: btnPressed = button == GLFW_MOUSE_BUTTON_LEFT; break; */
    /*     case MouseButton::kMMB: btnPressed = button == GLFW_MOUSE_BUTTON_MIDDLE; break; */
    /*     case MouseButton::kRMB: btnPressed = button == GLFW_MOUSE_BUTTON_RIGHT; break; */
    /*     default: break; */
    /*     } */
    /*     return btnPressed && modPressed && action == GLFW_PRESS; */
    /* } */

    bool check_is_pressed(GLFWwindow *window) {
        bool modPressed = true;
        bool shift = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
        bool ctrl = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS;
        bool alt = glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS;
        auto mods = (shift ? GLFW_MOD_SHIFT : 0) | (ctrl ? GLFW_MOD_CONTROL : 0) | (alt ? GLFW_MOD_ALT : 0);
        switch (modifier) {
        case Modifier::kShift: modPressed = mods == GLFW_MOD_SHIFT; break;
        case Modifier::kCtrl: modPressed = mods == GLFW_MOD_CONTROL; break;
        case Modifier::kAlt: modPressed = mods == GLFW_MOD_ALT; break;
        default: modPressed = mods == 0; break;
        }
        int button = GLFW_MOUSE_BUTTON_LEFT;
        switch (mouseBtn) {
        case MouseButton::kLMB: button = GLFW_MOUSE_BUTTON_LEFT; break;
        case MouseButton::kMMB: button = GLFW_MOUSE_BUTTON_MIDDLE; break;
        case MouseButton::kRMB: button = GLFW_MOUSE_BUTTON_RIGHT; break;
        case MouseButton::kNone: return modPressed
                && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS
                && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) != GLFW_PRESS
                && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != GLFW_PRESS
                ;
        default: return false;
        }
        return modPressed && glfwGetMouseButton(window, button) == GLFW_PRESS;
    }
};
