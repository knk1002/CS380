#version 330 core

uniform vec3 colorIn;
// Ouput color
out vec3 colorOut;

void main(){
	// Output color = color specified in the vertex shader
	colorOut = vec3(colorIn.x,colorIn.y,1.0f);
	//colorOut = vec3(1.0f,1.0f,1.0f);
}