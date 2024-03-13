#version 330
in float graphZ;
out vec4 FragColor;
uniform float red;

void main(){
    FragColor = vec4(graphZ/1.5, graphZ/1.5, 1.0, 1.0);
}
