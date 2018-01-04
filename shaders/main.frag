#version 330 core
out vec4 frag_colour;

in vec4 colour;
in vec2 passTexCoord;

uniform sampler2D tex;

void main() {
    frag_colour = texture(tex, passTexCoord) * colour;
}
