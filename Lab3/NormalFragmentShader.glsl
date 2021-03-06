#version 330 core

in vec3 fragmentPosition;
in vec3 fragmentColor;
in vec3 fragmentNormal;
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
	else if(lightState == 1)
	{
		vec3 tolight = normalize(fragmentPosition - uLightPos);
		vec3 toV = -normalize(vec3(fragmentPosition));
		vec3 h = normalize(toV + tolight);
		vec3 normal = normalize(fragmentNormal);

		float specular = pow(max(0.0, dot(h, normal)), 64.0);
		float diffuse = max(0.0, dot(normal, -tolight));

		vec3 mixColor =  fragmentColor * diffuse + vec3(0.6, 0.6, 0.6) * specular;

		color = pow(mixColor, vec3(1.0 / 2.2));
	}
	
}
