#version 330 core

layout (location = 0) in vec4 aPos;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aNormal;

out vec2 vTexCoord;
out vec3 vNormal;
out vec3 vPosition;

uniform mat4 uViewProj;
uniform mat4 uModel;

void main() {
    vTexCoord = aTexCoord;
    vNormal = normalize(mat3(uModel) * aNormal);
    vPosition = (uModel * aPos).xyz;
    gl_Position = uViewProj * uModel * aPos;
}
