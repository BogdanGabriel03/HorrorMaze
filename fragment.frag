#version 330 core
in vec2 texCoord;
in vec4 ourColor2;

out vec4 FragColor;

uniform vec4 ourColor;
uniform sampler2D floorTexture;
uniform sampler2D ceilingTexture;
uniform sampler2D wallTexture;
uniform float mixPercent;

uniform int useTextures;
uniform vec4 objColor;

void main()
{
	if(useTextures==-1) {
		FragColor = objColor;
	}
	else if(useTextures==0) {
		FragColor = texture(floorTexture,texCoord) * vec4(0.3,0.3,0.3,1.0);
	}
	else if(useTextures==1) {
		FragColor = texture(ceilingTexture,texCoord) * vec4(0.2,0.2,0.2,1.0);
	}
	else if(useTextures==2) {
		FragColor = texture(wallTexture,texCoord) * vec4(0.3,0.3,0.3,1.0);
	}
};