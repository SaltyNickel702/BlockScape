#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 normIn;
layout (location = 2) in vec2 texIn;
layout (location = 3) in float textureIDin;

uniform float time;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 texCoord;
out vec3 normal;
out float textureID;


void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);

	texCoord = texIn;
	normal = normIn;
	textureID = textureIDin;
}