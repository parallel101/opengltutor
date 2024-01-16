#version 330 core
in vec3 vertPosition;
out vec4 fragColor;
uniform vec2 uniMouse;
void main() {
    float alpha = 1.0 - length(vertPosition.xy - uniMouse);
    fragColor = vec4(vertPosition * 0.5 + 0.5, alpha);
}
