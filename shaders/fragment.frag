#version 330 core

struct LightSource {
	vec3 position;
	vec3 color;
	float intensity;

	float constant;
	float linear;
	float quadratic;

	vec3 direction;
	float cutOff;
	float outerCutOff;
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
uniform LightSource flashlight;
uniform bool flashlightOn;
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
    diffuse  *= attenuation * attenuation;
    specular *= attenuation * attenuation;

	return (ambient + diffuse + specular);
}

vec3 CalculateFlashlightImpact(LightSource light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	vec3 lightDir = normalize(light.position - fragPos);
	float theta = dot(lightDir, normalize(-light.direction));

		float epsilon = (light.cutOff - light.outerCutOff);
		float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

		vec3 ambient = light.intensity * light.color;

		float diff = max(dot(normal, lightDir), 0.0);
		vec3 diffuse = diff * light.color * light.intensity;

		vec3 reflectDir = reflect(-lightDir, normal);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
		vec3 specular = 0.3 * spec * light.color;

		ambient *=intensity;
		diffuse *= intensity;
        specular *= intensity;

		float distance = length(light.position - fragPos);
		float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance)); 

		ambient *= attenuation;
		diffuse *= attenuation;
        //specular *= attenuation;

		return ambient + diffuse + specular;
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
	if(flashlightOn) result += CalculateFlashlightImpact(flashlight, norm, FragPos, viewDir);
	
	if(result == vec3(0.0f)) result = vec3(0.024f, 0.002f, 0.002f);

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