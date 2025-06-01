#version 330 core

struct LightSource {
	vec3 position;
	vec3 color;
	float intensity;

	float constant;
	float linear;
	float quadratic;
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
uniform LightSource lights[4];
uniform bool hitByLight[4];
uniform vec4 objColor;
uniform vec3 viewPos;

vec3 CalculatePointLight(LightSource light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	
	// diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

	//attenuation
	float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance)); 

	vec3 ambient = light.intensity * light.color;
	vec3 diffuse = diff * light.color * light.intensity;
	vec3 specular = 0.3 * spec * light.color;

	ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

	return (ambient + diffuse + specular);
}

void main()
{
	
	vec3 result = vec3(0.0f);
	vec3 ambient = vec3(0.0f);
	vec3 diffuse = vec3(0.0f);
	vec3 norm = normalize(Normal);
	float specularStrength = 0.3;
	vec3 specular = vec3(0.0f);
	vec3 materialColor;
	vec3 viewDir = normalize(viewPos - FragPos);

	for ( int i=0;i<4;++i) {
		if(hitByLight[i] == true) {
			result += CalculatePointLight(lights[i], norm, FragPos, viewDir);
		}
	}
	//vec3 lighting = ambient + diffuse + specular;
	if(result == vec3(0.0f)) result = vec3(0.024f, 0.002f, 0.002f);
	//lighting.x = max(lighting.x,0.096f);
	//lighting.y = max(lighting.y,0.008f);
	//lighting.z = max(lighting.z,0.008f);

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
		materialColor = texture(doorTexture, texCoord).rgb * vec3(0.6);
	}

	FragColor = vec4(materialColor * result, 1.0);
	//FragColor = vec4(normalize(Normal)*0.5 + 0.5, 1.0);
};