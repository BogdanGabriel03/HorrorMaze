#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexCoord;		// texture coordinates

out vec4 ourColor2;
out vec2 texCoord;
out vec3 Normal;
out vec3 FragPos;

uniform float xOffset;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
   gl_Position = projection * view * model * vec4(aPos.x + xOffset, aPos.y, aPos.z, 1.0);
   ourColor2 = vec4(1.0,1.0,0.0,1.0);
   texCoord = aTexCoord;
   FragPos = vec3(model * vec4(aPos, 1.0));
   Normal = mat3(transpose(inverse(model))) * aNormal;
}