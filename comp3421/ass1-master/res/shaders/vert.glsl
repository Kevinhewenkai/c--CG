#version 330 core

uniform mat4 m_transform;

layout (location = 0) in vec4 pos;
layout (location = 1) in vec2 tc_in;


out vec2 tc;

void main() {
    tc = tc_in;

    gl_Position = pos * m_transform;
}

