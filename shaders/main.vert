#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;

uniform mat4 modelview;

out vec4 colour;

void main() {
    gl_Position = modelview * vec4(position, 1.0);
    colour = vec4(position.xz / 4, 0.3, 1.0);
}
