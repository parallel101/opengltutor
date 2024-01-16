#version 330 core
in vec3 position;
out vec3 vertPosition;
uniform vec2 uniMouse;
uniform mat4 uniView;
void main() {
    vertPosition = position;
    gl_Position = vec4(vertPosition, 1);
}
