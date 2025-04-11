#version 330 core

uniform float time;
uniform mat4 model;
uniform mat4 view;
uniform int renderDistance;
uniform int totalTextures;
uniform sampler2D texture1;

in vec3 pos;
in vec2 UV;
in vec3 normal;
in float textureID;

out vec4 FragColor;

void main ()
{
	//UV Textures
	vec2 newUV = UV * vec2(1.0 / totalTextures, 1.0) + vec2(textureID / totalTextures, 0.0);

	//Sun Shading
	vec3 sun = vec3(-0.25,1,-0.5);
	float dotProd = (dot(normalize(normal),normalize(sun))+1)/2;
	
	//Fog
	float distance = length(view * model * vec4(pos, 1.0));
	float fogDistance = renderDistance - 16;
	float cutoff = .3*fogDistance;
	float fogFactor = (distance - (fogDistance - cutoff))/(.8*cutoff);


	FragColor = mix((dotProd+0.1)*texture(texture1, newUV), vec4(.5,.7,.8,1), clamp(fogFactor,0,1));
	// FragColor = vec4(fogFactor);
}