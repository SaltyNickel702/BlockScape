#version 330 core

uniform float time;
uniform mat4 model;
uniform mat4 view;
uniform int renderDistance;
uniform int totalTextures;
uniform sampler2D tex0;

in vec3 pos;
in vec2 UV;
in vec3 normal;
in float textureID;

out vec4 FragColor;

void main ()
{
	//Get Texture
	vec2 newUV = UV * vec2(1.0 / totalTextures, 1.0) + vec2(textureID / totalTextures, 0.0);	
	vec4 textureColor = texture(tex0, newUV);

	//Grass Whiteness
	textureColor = mix(textureColor,vec4(1),.9*clamp((pos.y-50)/(90-50),0,1));
	
	//Sun Shading
	vec3 sun = vec3(-0.25,1,-0.5);
	float dotProd = (dot(normalize(normal),normalize(sun))+1)/2;
	textureColor = (dotProd+0.1)*textureColor;

	//Fog
	float distance = length(view * model * vec4(pos, 1.0));
	float fogDistance = renderDistance - 16;
	float cutoff = .3*fogDistance;
	float fogFactor = (distance - (fogDistance - cutoff))/(.8*cutoff);
	textureColor = mix(textureColor, vec4(.5,.7,.8,1), clamp(fogFactor,0,1));

	FragColor = textureColor;
}