#version 330 core
in vec3 position;
out vec3 vertPosition;
uniform vec2 uniMouse;
void main() {
    vertPosition = position + vec3(uniMouse.x, uniMouse.y, 0);
    gl_Position = vec4(vertPosition, 1);
}
