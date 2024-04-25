#version 330 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoord;

out vec3 interpolatedNormal;
out vec2 st;

uniform float time;
uniform mat4 T;
uniform mat4 R;



void main(){
    gl_Position = T * vec4(Position, 1.0);

    //interpolatedNormal = Normal;

    st = TexCoord;

    vec3 transformedNormal = mat3(T) * Normal;
    interpolatedNormal = normalize(transformedNormal);

}