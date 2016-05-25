#version 330 core

in vec3 fragmentPosition;
in vec3 fragmentColor;
flat in vec3 fragmentNormal;
in int fragmentLightState;

// Ouput data
out vec3 color;

uniform vec3 uLight;
uniform vec3 uLightPos;
uniform int lightState;

void main(){
	
	if(lightState == 0)
	{
		vec3 diffuseColor = vec3(1.0, 1.0, 1.0);
		vec3 tolight = -normalize(uLight);
		vec3 normal = normalize(fragmentNormal);

		float intensity = clamp(dot(normal, tolight), 0.0, 1.0);

		color = clamp(diffuseColor * intensity, 0.1, 1.0);

		color = fragmentColor * color;
	}
}
