#version 330 core

out vec4 finalcolor;
in vec3 interpolatedColor;

uniform float time;
uniform mat4 T;
uniform mat4 R;


void main() {
    finalcolor = vec4(0.5, 1.0, 1.0, 0.7);
    finalcolor = vec4(interpolatedColor, 1.0);
}