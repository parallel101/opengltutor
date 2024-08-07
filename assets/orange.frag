#version 330 core
in vec3 vertPosition;
in vec3 vertNormal;
out vec4 fragColor;
uniform vec3 uniLightDir;
uniform mat4 uniModel;
uniform mat4 uniView;
uniform mat4 uniProjection;

// vec3 filmic(vec3 x) {
//     vec3 X = max(vec3(0.0), x - 0.004);
//     vec3 result = (X * (6.2 * X + 0.5)) / (X * (6.2 * X + 1.7) + 0.06);
//     return pow(result, vec3(2.2));
// }
//
// vec3 aces(vec3 x) {
//     const float a = 2.51;
//     const float b = 0.03;
//     const float c = 2.43;
//     const float d = 0.59;
//     const float e = 0.14;
//     return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
// }
//
// vec3 gammaCorrect(vec3 x) {
//     return pow(x, vec3(1/2.2));
// }

vec3 unreal(vec3 x) {
  return x / (x + 0.155) * 1.019;
}

void main() {
    vec3 normal = normalize(vertNormal);
    vec3 viewPos = -vec3(uniView * vec4(0, 0, 0, 1));
    vec3 viewDir = normalize(viewPos - vertPosition);
    vec3 halfDir = normalize(viewDir + uniLightDir);

    vec3 copper = pow(vec3(0xb6 / 255.0, 0x71 / 255.0, 0x30 / 255.0), vec3(2.2));
    vec3 specColor = mix(copper, vec3(1, 1, 1), 0.1) * 1.5;
    vec3 diffColor = copper;
    float shineness = 40;

    float specular = pow(max(0, dot(halfDir, normal)), shineness);
    float diffuse = max(0, dot(uniLightDir, normal));
    float ambient = 0.05;
    vec3 finalColor = (diffuse + ambient) * diffColor + specular * specColor;

    finalColor = unreal(finalColor);
    fragColor = vec4(finalColor, 1);
}
