#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
out vec3 vertPosition;
out vec3 vertNormal;
uniform vec3 uniLightDir;
uniform mat4 uniModel;
uniform mat4 uniView;
uniform mat4 uniProjection;
void main() {
    vertPosition = vec3(uniView * uniModel * vec4(position, 1.0));
    vertNormal = normalize(transpose(inverse(mat3(uniView * uniModel))) * normal);
    gl_Position = uniProjection * uniView * uniModel * vec4(position, 1.0);
}
