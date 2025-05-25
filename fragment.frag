#version 330 core

struct LightSource {
	vec3 position;
	vec3 color;
	float intensity;
};

in vec2 texCoord;
in vec4 ourColor2;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec4 ourColor;
uniform sampler2D floorTexture;
uniform sampler2D ceilingTexture;
uniform sampler2D wallTexture;
uniform sampler2D doorTexture;
uniform float mixPercent;

uniform int useTextures;
uniform LightSource lights[2];
uniform vec4 objColor;
uniform vec3 viewPos;

void main()
{
	
	vec3 result = vec3(0.0f);
	vec3 ambient = vec3(0.0f);
	vec3 diffuse = vec3(0.0f);
	vec3 norm = normalize(Normal);
	float specularStrength = 0.2;
	vec3 specular = vec3(0.0f);
	vec3 materialColor;

	for ( int i=0;i<2;++i) {
		float ambientStrength = lights[i].intensity;
		ambient += ambientStrength * lights[i].color;

		vec3 lightDir = normalize(lights[i].position - FragPos); 
		float diff = max(dot(norm, lightDir), 0.0);
		diffuse += diff * lights[i].color * lights[i].intensity;

		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);

		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
		specular += specularStrength * spec * lights[i].color;
	}
	vec3 lighting = ambient + diffuse + specular;

	if(useTextures==-1) {
		materialColor = vec3(objColor);
	}
	else if(useTextures==0) {
		materialColor = texture(floorTexture, texCoord).rgb;
	}
	else if(useTextures==1) {
		materialColor = texture(ceilingTexture, texCoord).rgb;
	}
	else if(useTextures==2) {
		materialColor = texture(wallTexture, texCoord).rgb;
	}
	else if(useTextures==3) {
		materialColor = texture(doorTexture, texCoord).rgb;
	}

	FragColor = vec4(materialColor * lighting, 1.0);
};