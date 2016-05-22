#version 330 core

in vec3 fragmentPosition;
in vec3 fragmentColor;
in vec3 fragmentNormal;

// Ouput data
out vec3 color;

uniform vec3 uLight;

void main(){
	
	//TODO: Assign fragmentColor as a final fragment color
		
	//TODO:Assign fragmentNormal as a final fragment color
	vec3 normal = normalize(fragmentNormal);
	color = normal;		
	//TODO: Phong reflection model	
}
