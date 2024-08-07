#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 velocity;
layout (location = 2) in vec3 color;
layout (location = 3) in float radius;
out vec3 vertPosition;
out vec3 vertColor;
uniform vec3 uniLightDir;
uniform mat4 uniModel;
uniform mat4 uniView;
uniform mat4 uniProjection;
uniform float uniScreenHeight;
uniform float uniFovFactor;
void main() {
    vertPosition = vec3(uniView * uniModel * vec4(position, 1.0));
    vertColor = color;
    gl_Position = uniProjection * uniView * uniModel * vec4(position, 1.0);
    float sizeScale = uniScreenHeight / uniFovFactor / gl_Position.w;
    gl_PointSize = radius * sizeScale; // glEnable(GL_PROGRAM_POINT_SIZE)
}
