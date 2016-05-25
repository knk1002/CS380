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
	
	//color = vec3(1.0, 1.0, 1.0);

	if(lightState == 0)
	{
	vec3 tolight = -normalize(uLight);
	vec3 toV = -normalize(vec3(fragmentPosition));
	vec3 h = normalize(toV + tolight);
	vec3 normal = normalize(fragmentNormal);
	float specular = pow(max(0.0, dot(h, normal)), 64.0);
	float diffuse = max(0.0, dot(normal, tolight));
	vec3 intensity = fragmentColor * diffuse + vec3(0.6, 0.6, 0.6)*specular;
	color = pow(intensity, vec3(1.0 / 2.2));

	//TODO: Toon Shader
	float sil = max(dot (normal, tolight), 0.0);
	if (sil < 0.3) color = vec3(0.2, 0.0 ,0.0);
	else if (sil < 0.2) color = vec3(0.5, 0.0, 0.0);
	else if (sil < 0.1) color = vec3(0.8, 0.0 ,0.0);
	else
	{
	color = vec3(0.8, 0.0, 0.0);
	float spec = pow(max(dot(normal, h), 0.0), 20.0);

	if(spec < 0.4) 
		color *= 0.6;
	else if(spec < 0.2)
		color *= 0.4;
	else 
		color = vec3(0.8, 0.0, 0.0);

	if (diffuse < 0.5) 
		color *= 0.8;
	else if(diffuse < 0.3) 
		color *= 0.2;
	}
	}
		
	
}
