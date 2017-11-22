#version 330 core
out vec4 frag_colour;
in vec4 colour;

void main() {
    frag_colour = vec4(colour);
}
