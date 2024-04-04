#version 330 core

layout(location = 0) in vec3 Position;

layout(location = 1) in vec3 Color;

out vec3 interpolatedColor;

void main(){
    interpolatedColor = Color;
    gl_Position = vec4(Position, 1.5);
}