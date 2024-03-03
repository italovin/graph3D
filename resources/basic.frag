#version 460
out vec4 FragColor;
uniform float red;
void main(){
FragColor = vec4(red, 0.0, 0.0, 1.0);
}