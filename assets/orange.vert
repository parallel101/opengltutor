#version 330 core
in vec3 position;
out vec3 vertPosition;
void main() {
    vertPosition = position;
    gl_Position = vec4(vertPosition, 1);
}
