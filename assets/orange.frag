#version 330 core
in vec3 vertPosition;
out vec4 fragColor;
uniform vec2 uniMouse;
uniform mat4 uniModel;
uniform mat4 uniView;
uniform mat4 uniProjection;
void main() {
    vec3 normal = normalize(vertPosition);
    vec3 lightDirection = normalize(vec3(0, 0, 1));
    float lightIntensity = dot(normal, lightDirection);
    vec3 lightColor = vec3(0.8, 0.8, 0.8);
    vec3 ambientColor = vec3(0.1, 0.1, 0.1);
    vec3 finalColor = (lightIntensity * lightColor) + ambientColor;
    fragColor = vec4(finalColor, 1);
}
