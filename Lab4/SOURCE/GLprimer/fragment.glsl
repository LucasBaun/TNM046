#version 330 core

in vec3 interpolatedNormal;
in vec2 st;
out vec4 finalcolor;
//out vec3 finalcolor;

//uniform float time;
uniform mat4 T;
uniform mat4 R;



void main() {
    vec3 lightDirection = vec3(1.0, 1.0, 1.0);
    float shading = dot(interpolatedNormal, lightDirection);
    shading = max(0.0, shading);

    vec3 L = mat3(T) * lightDirection;
    L = normalize(L);

    vec3 V = vec3(0.0f, 0.0f, 1.0f);

    vec3 N = interpolatedNormal;

    float n = 20.0f;
    
    vec3 ka = vec3(0.3f, 0.3f, 1.0f);
    vec3 Ia = vec3(0.3f, 0.0f, 0.0f);
    vec3 kd = vec3(1.0f, 0.6f, 0.0f);
    vec3 Id = vec3(1.0f, 1.0f, 1.0f);
    vec3 ks = vec3(1.0f, 1.0f, 1.0f);
    vec3 Is = vec3(1.0f, 1.0f, 1.0f);





    vec3 Rv = 2.0 * dot(N, L) * N - L;
    float dotNL = max(dot(N, L), 0.0);
    float dotRV = max(dot(Rv, V), 0.0);
    if (dotNL == 0.0){
        dotRV = 0.0;
    }
    vec3 shadedcolor = Ia * ka + Id * kd * dotNL + Is * ks * pow(dotRV, n);
    finalcolor = vec4(shadedcolor, 1.0);

}