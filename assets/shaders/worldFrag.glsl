#version 330 core

uniform float time;
uniform mat4 model;
uniform sampler2D texture1;

in vec2 texCoord;
in vec3 normal;
in float textureID;


void main ()
{
	vec3 sun = vec3(-0.25,1,-0.5);
	

	float dotProd = (dot(normalize(normal),normalize(sun))+1)/2;
	gl_FragColor = (dotProd+.5)*texture(texture1, texCoord);
}