#version 330 core

layout (location = 0) in vec4 aPos;
layout (location = 2) in vec2 aTexCoord;

out vec2 vTexCoord;

void main() {
    vTexCoord = aTexCoord;
    gl_Position = aPos;
}
