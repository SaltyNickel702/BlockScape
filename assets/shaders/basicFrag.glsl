#version 330 core

uniform float time;
uniform mat4 model;
uniform sampler2D texture1;

in vec2 texCoord;
in vec3 normal;
in float textureID

out vec4 FragColor;

void main ()
{
	vec3 sun = vec3(2,3,1);
	

	float dotProd = (dot(normalize(normal),normalize(sun))+1)/2;
	FragColor = (dotProd + .2) * texture(texture1, texCoord);
}