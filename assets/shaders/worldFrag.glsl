#version 330 core

uniform float time;
uniform mat4 model;
uniform mat4 view;
uniform int renderDistance;
uniform int totalTextures;
uniform sampler2D tex0;
uniform vec3 cameraPos;
uniform vec3 cameraRot;

in vec3 pos;
in vec2 UV;
in vec3 normal;
in float textureID;

out vec4 FragColor;

void main ()
{
	float distance = length(view * model * vec4(pos, 1.0));
	vec3 wPos = (model * vec4(pos,1)).xyz;

	//Get Texture
	vec2 newUV = UV * vec2(1.0 / totalTextures, 1.0) + vec2(textureID / totalTextures, 0.0);	
	vec4 textureColor = texture(tex0, newUV);

	//Grass Whiteness
	textureColor = mix(textureColor,vec4(1),.9*clamp((pos.y-50)/(90-50),0,1));
	
	//Sun Shading
	vec3 sun = vec3(-0.25,1,-0.5);
	float dotProd = (dot(normalize(normal),normalize(sun))+1)/2;
	textureColor = (dotProd+0.1)*textureColor;

	//Ocean Shader
	vec4 waterColor = vec4(0,.2,.6,1);
	float waterLevel = 29+14.0/16.0;
	if ((cameraPos.y > waterLevel && pos.y < waterLevel) || (cameraPos.y < waterLevel && pos.y > waterLevel)) {
		float surfaceFactor = 1-abs(dot(normalize(cameraPos-wPos),vec3(0,-1,0)));
		textureColor = mix(textureColor,waterColor,clamp(.7*pow(surfaceFactor,.75)+.3,0,1));
	}
	if (pos.y < waterLevel) {
		float density = 0.06;
		float fogFactor = 1.0 - exp(-length(vec3(cameraPos.x-wPos.x,30-wPos.y,cameraPos.z-wPos.z)) * density);
		textureColor = mix(textureColor,waterColor,clamp(fogFactor, 0.0, .95));
	}

	//Fog
	float fogDistance = renderDistance - 16;
	float cutoff = .3*fogDistance;
	float fogFactor = (distance - (fogDistance - cutoff))/(.8*cutoff);
	textureColor = mix(textureColor, vec4(.5,.7,.8,1), clamp(fogFactor,0,1));

	if (textureColor.w < 0.01) discard;

	FragColor = textureColor;
}