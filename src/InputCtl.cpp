#include "Game.hpp"
#include "InputCtl.hpp"
#include "check_gl.hpp"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
/* #include <cstdio> */

namespace {

template <class, class ...Ts>
static void (*_impl_glfw_input_callback(void (InputCtl::*pFn)(Ts...)))(GLFWwindow *, Ts...) {
    static void (InputCtl::*gpFn)(Ts...);
    gpFn = pFn;
    return [] (GLFWwindow *window, Ts ...args) -> void {
        auto game = (Game *)glfwGetWindowUserPointer(window);
        if (game) [[likely]] {
            (game->m_inputCtl.*gpFn)(args...);
        }
    };
}

template <class FpFn>
static auto glfw_input_callback(FpFn fpFn) {
    return _impl_glfw_input_callback<FpFn>(fpFn());
}

struct CameraState {
    glm::vec3 eye = {0, 0, 5};
    glm::vec3 lookat = {0, 0, 0};
    glm::vec3 up_vector = {0, 1, 0};
    glm::vec3 keep_up_axis = {0, 1, 0};
    float focal_len = 40.0f;
    float film_height = 24.0f;
    float film_width = 32.0f;
    int width = 1920;
    int height = 1080;

    void pan(InputCtl::InputPreference const &pref, glm::vec2 delta) {
        delta *= -pref.pan_speed;

        auto front_vector = glm::normalize(lookat - eye);
        auto right_vector = glm::normalize(glm::cross(front_vector, up_vector));
        auto fixed_up_vector = glm::normalize(glm::cross(right_vector, front_vector));

        auto delta3d = delta.x * right_vector + delta.y * fixed_up_vector;

        eye += delta3d;
        lookat += delta3d;
    }

    void orbit(InputCtl::InputPreference const &pref, glm::vec2 delta, bool isDrift) {
        if (isDrift) {
            delta *= -pref.drift_speed;
            delta *= std::atan(film_height / (2 * focal_len));
        } else {
            delta *= pref.orbit_speed;
        }

        auto angle_X_inc = delta.x;
        auto angle_Y_inc = delta.y;

        // pivot choose: drift mode rotates around eye center, orbit mode rotates around target object
        auto rotation_pivot = isDrift ? eye : lookat;

        auto front_vector = glm::normalize(lookat - eye);

        // new right vector (orthogonal to front, up)
        auto right_vector = glm::normalize(glm::cross(front_vector, up_vector));

        // new up vector (orthogonal to right, front)
        up_vector = glm::normalize(glm::cross(right_vector, front_vector));

        // rotation 1: based on the mouse horizontal axis
        glm::mat4x4 rotation_matrixX = glm::rotate(glm::mat4x4(1), -angle_X_inc, up_vector);

        // rotation 2: based on the mouse vertical axis
        glm::mat4x4 rotation_matrixY = glm::rotate(glm::mat4x4(1), angle_Y_inc, right_vector);

        // translate back to the origin, rotate and translate back to the pivot location
        auto transformation = glm::translate(glm::mat4x4(1), rotation_pivot)
            * rotation_matrixY * rotation_matrixX
            * glm::translate(glm::mat4x4(1), -rotation_pivot);

        // update eye and lookat coordinates
        eye = glm::vec3(transformation * glm::vec4(eye, 1));
        lookat = glm::vec3(transformation * glm::vec4(lookat, 1));

        // try to keep the camera horizontal line correct (eval right axis error)
        float right_o_up = glm::dot(right_vector, keep_up_axis);
        float right_handness = glm::dot(glm::cross(keep_up_axis, right_vector), front_vector);
        float angle_Z_err = glm::asin(right_o_up);
        angle_Z_err *= glm::atan(right_handness);
        // rotation for up: cancel out the camera horizontal line drift
        glm::mat4x4 rotation_matrixZ = glm::rotate(glm::mat4x4(1), angle_Z_err, front_vector);
        up_vector = glm::mat3x3(rotation_matrixZ) * up_vector;
    }

    void zoom(InputCtl::InputPreference const &pref, float delta, bool isHitchcock) {
        float inv_zoom_factor = glm::exp(-pref.zoom_speed * delta);
        eye = (eye - lookat) * inv_zoom_factor + lookat;
        if (isHitchcock) {
            focal_len *= inv_zoom_factor;
            /* printf("focalLen: %.1f mm, FoVx: %.1f deg, FoVy: %.1f deg, Distance: %.1f m\n", focal_len, */
            /*        glm::degrees(2 * glm::atan(film_width / (2 * focal_len))), */
            /*        glm::degrees(2 * glm::atan(film_height / (2 * focal_len))), */
            /*        glm::length(eye - lookat)); */
        }
    }

    glm::mat4x4 view_matrix() const {
        return glm::lookAt(eye, lookat, up_vector);
    }

    glm::mat4x4 projection_matrix() {
        auto fov = 2 * std::atan(film_height / (2 * focal_len));
        auto aspect = (float)width / height;
        return glm::perspective(fov, aspect, 0.01f, 100.0f);
    }
};

}

struct InputCtl::Private { // P-IMPL pattern
    glm::vec2 lastpos;
    bool moving = false;
    CameraState camState;
    InputPreference camCtlPref;
    GLFWwindow *window = nullptr;
};

InputCtl::InputCtl() : m_private(std::make_unique<Private>()) {}

InputCtl::~InputCtl() = default;

glm::vec2 InputCtl::get_cursor_pos() {
    double xpos, ypos;
    glfwGetCursorPos(m_private->window, &xpos, &ypos);
    int width, height;
    glfwGetWindowSize(m_private->window, &width, &height);

    float x = (float)(2 * xpos / width - 1);
    float y = (float)(2 * (height - ypos) / height - 1);
    return glm::vec2(x, y);
}

glm::mat4x4 InputCtl::get_view_matrix() {
    return m_private->camState.view_matrix();
}

glm::mat4x4 InputCtl::get_projection_matrix() {
    return m_private->camState.projection_matrix();
}

void InputCtl::register_callbacks(GLFWwindow *window) {
    m_private->window = window;
    glfwSetCursorPosCallback(window, glfw_input_callback([] { return &InputCtl::cursor_pos_callback; }));
    glfwSetMouseButtonCallback(window, glfw_input_callback([] { return &InputCtl::mouse_button_callback; }));
    glfwSetScrollCallback(window, glfw_input_callback([] { return &InputCtl::scroll_callback; }));
    glfwSetKeyCallback(window, glfw_input_callback([] { return &InputCtl::key_callback; }));
    glfwSetFramebufferSizeCallback(window, glfw_input_callback([] { return &InputCtl::framebuffer_size_callback; }));

    int width, height;
    glfwGetWindowSize(m_private->window, &width, &height);
    framebuffer_size_callback(width, height);
}

void InputCtl::cursor_pos_callback(double xpos, double ypos) {
    int width, height;
    glfwGetWindowSize(m_private->window, &width, &height);

    float x = (float)(2 * xpos / width - 1);
    float y = (float)(2 * (height - ypos) / height - 1);
    glm::vec2 pos(x, y);

    m_private->moving = true;
    auto delta = glm::fract((pos - m_private->lastpos) * 0.5f + 0.5f) * 2.0f - 1.0f;
    if (m_inputPref.orbit_binding.check_is_pressed(m_private->window)) {
        m_private->camState.orbit(m_inputPref, delta, false);
    } else if (m_inputPref.drift_binding.check_is_pressed(m_private->window)) {
        m_private->camState.orbit(m_inputPref, delta, true);
    } else if (m_inputPref.pan_binding.check_is_pressed(m_private->window)) {
        m_private->camState.pan(m_inputPref, delta);
    } else if (m_inputPref.zoom_binding.check_is_pressed(m_private->window)) {
        m_private->camState.zoom(m_inputPref, delta[m_inputPref.zoom_axis], false);
    } else if (m_inputPref.hitchcock_binding.check_is_pressed(m_private->window)) {
        m_private->camState.zoom(m_inputPref, delta[m_inputPref.zoom_axis], true);
    } else {
        m_private->moving = false;
    }
    m_private->lastpos = pos;

    if (m_private->moving && m_inputPref.clamp_cursor && (xpos >= width - 1 || ypos >= height - 1 || xpos <= 1 || ypos <= 1)) {
        // clamp mouse cursor inside the window (ZHI JING Blender)
        xpos = std::fmod(xpos + width - 3, width - 2) + 1;
        ypos = std::fmod(ypos + height - 3, height - 2) + 1;
        glfwSetCursorPos(m_private->window, xpos, ypos);
    }
}

void InputCtl::mouse_button_callback(int button, int action, int mods) {
    (void)button;
    (void)action;
    (void)mods;

    auto pos = get_cursor_pos();
    m_private->lastpos = pos;

    m_private->moving = m_inputPref.orbit_binding.check_is_pressed(m_private->window)
        || m_inputPref.drift_binding.check_is_pressed(m_private->window)
        || m_inputPref.pan_binding.check_is_pressed(m_private->window)
        || m_inputPref.zoom_binding.check_is_pressed(m_private->window)
        || m_inputPref.hitchcock_binding.check_is_pressed(m_private->window);

    GLFWcursor *cursor = glfwCreateStandardCursor(m_private->moving ? GLFW_CROSSHAIR_CURSOR : GLFW_ARROW_CURSOR);
    glfwSetCursor(m_private->window, cursor);
}

void InputCtl::scroll_callback(double xoffset, double yoffset) {
    float deltax = xoffset < 0 ? -1 : xoffset > 0 ? 1 : 0;
    float deltay = yoffset < 0 ? -1 : yoffset > 0 ? 1 : 0;
    glm::vec2 delta(deltax, deltay);

    if (m_inputPref.orbit_binding.check_is_scrolled(m_private->window)) {
        m_private->camState.orbit(m_inputPref, delta, false);
    } else if (m_inputPref.drift_binding.check_is_scrolled(m_private->window)) {
        m_private->camState.orbit(m_inputPref, delta, true);
    } else if (m_inputPref.pan_binding.check_is_scrolled(m_private->window)) {
        m_private->camState.pan(m_inputPref, delta);
    } else if (m_inputPref.zoom_binding.check_is_scrolled(m_private->window)) {
        m_private->camState.zoom(m_inputPref, delta[m_inputPref.zoom_axis], false);
    } else if (m_inputPref.hitchcock_binding.check_is_scrolled(m_private->window)) {
        m_private->camState.zoom(m_inputPref, delta[m_inputPref.zoom_axis], true);
    }
}

void InputCtl::key_callback(int key, int scancode, int action, int mods) {
    (void)key;
    (void)scancode;
    (void)action;
    (void)mods;
}

void InputCtl::framebuffer_size_callback(int width, int height) {
    m_private->camState.width = width;
    m_private->camState.height = height;
}
