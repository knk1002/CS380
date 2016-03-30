#version 330 core

// Ouput color
uniform vec3 colorIn;
out vec3 colorOut;

void main(){
	colorOut = vec3(colorIn.x,colorIn.y,colorIn.z);
}