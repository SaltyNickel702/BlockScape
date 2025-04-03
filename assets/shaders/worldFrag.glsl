#version 330 core

uniform float time;
uniform mat4 model;
uniform sampler2D texture1;
uniform int totalTextures;

in vec2 UV;
in vec3 normal;
in float textureID;

out vec4 FragColor;

void main ()
{
	vec3 sun = vec3(-0.25,1,-0.5);
	

	float dotProd = (dot(normalize(normal),normalize(sun))+1)/2;

	vec2 newUV = UV * vec2(1.0 / totalTextures, 1.0) + vec2(textureID / totalTextures, 0.0);
	FragColor = (dotProd+0.1)*texture(texture1, newUV);
}