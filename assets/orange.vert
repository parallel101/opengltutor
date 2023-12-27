#version 330 core
in vec3 position;
out vec3 vertPosition;
uniform vec2 uniMouse;
void main() {
    if (position.y > 0)
        vertPosition = position + vec3(uniMouse.x, uniMouse.y, 0);
    else
        vertPosition = position;
    gl_Position = vec4(vertPosition, 1);
}
