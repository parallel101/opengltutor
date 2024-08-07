#version 330 core
in vec3 vertPosition;
in vec3 vertColor;
out vec4 fragColor;
uniform vec3 uniLightDir;
uniform mat4 uniModel;
uniform mat4 uniView;
uniform mat4 uniProjection;

vec3 unreal(vec3 x) {
  return x / (x + 0.155) * 1.019;
}

vec3 calcColor(vec3 position, vec3 normal, vec3 baseColor, float shineness, float ambient) {
    vec3 viewPos = -vec3(uniView * vec4(0, 0, 0, 1));
    vec3 viewDir = normalize(viewPos - position);
    vec3 halfDir = normalize(viewDir + uniLightDir);

    vec3 specColor = mix(baseColor, vec3(1, 1, 1), 0.1) * 1.5;
    vec3 diffColor = baseColor;

    float specular = pow(max(0, dot(halfDir, normal)), shineness);
    float diffuse = max(0, dot(uniLightDir, normal));
    vec3 finalColor = (diffuse + ambient) * diffColor + specular * specColor;

    return unreal(finalColor);
}

void main() {
    vec3 normal;
    normal.xy = gl_PointCoord * vec2(2.0, -2.0) + vec2(-1.0, 1.0); // glEnable(GL_POINT_SPRITE)
    float mag = dot(normal.xy, normal.xy);
    if (mag >= 1.0) {
        discard;
    }
    normal.z = sqrt(1.0 - mag);

    vec3 finalColor = calcColor(vertPosition, normal, vertColor, 40, 0.05);

    fragColor = vec4(finalColor, 1);
}
