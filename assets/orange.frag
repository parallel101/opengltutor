#version 330 core
in vec3 vertPosition;
in vec3 vertNormal;
out vec4 fragColor;
uniform vec3 uniLightDir;
uniform mat4 uniModel;
uniform mat4 uniView;
uniform mat4 uniProjection;
void main() {
    vec3 normal = normalize(vertNormal);
    vec3 viewPos = vec3(inverse(uniView) * vec4(0, 0, 0, 1));
    vec3 viewDir = normalize(viewPos - vertPosition);
    vec3 halfDir = normalize(viewDir + uniLightDir);
    float highlight = pow(max(0, dot(halfDir, normal)), 40);
    float intensity = max(0, dot(uniLightDir, normal));
    intensity = intensity * 0.4 + 0.1 + highlight * 0.5;
    vec3 finalColor = vec3(intensity);
    fragColor = vec4(finalColor, 1);
}
