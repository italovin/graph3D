#version 330
layout (location = 0) in vec2 aPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float time;

void main(){
float x = aPos.x;
float y = aPos.y;
float z = cos(x - time)*sin(y - time);
gl_Position = projection*view*model*vec4(x, z, y, 1.0);
}