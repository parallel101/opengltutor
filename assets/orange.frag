#version 330 core
out vec4 fragColor;
in vec3 vertPosition;
void main() {
    fragColor = vec4(vertPosition * 0.5 + 0.5, 1);
}
