#version 330 core
in vec3 position;
out vec3 vertPosition;
uniform vec2 uniMouse;
uniform mat4 uniModel;
uniform mat4 uniView;
uniform mat4 uniProjection;
void main() {
    vertPosition = position;
    gl_Position = uniProjection * uniView * uniModel * vec4(position, 1.0);
}
