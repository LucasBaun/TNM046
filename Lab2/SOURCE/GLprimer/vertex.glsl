#version 330 core

layout(location = 0) in vec3 Position;

layout(location = 1) in vec3 Color;

out vec3 interpolatedColor;

uniform float time;
uniform mat4 T;
uniform mat4 R;


void main(){
    interpolatedColor = Color;
    //gl_Position = vec4(Position, 1);

    gl_Position = T * vec4(Position, 1.0);
}