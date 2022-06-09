#version 330 core

uniform sampler2D tex;

in vec2 tc;

out vec4 fs_color;

void main() {
    fs_color = texture(tex, tc);
}
