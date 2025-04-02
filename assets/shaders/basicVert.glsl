#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 texIn;

uniform float time;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 texCoord;


void main()
{
	// gl_Position = vec4(aPos.x + sin(time*aPos.x*1.5)/2, aPos.y-sin(time*aPos.y)/2*(aPos.x+.1), aPos.z, 1.0);
	gl_Position = projection * view * model * vec4(aPos, 1.0);

	texCoord = texIn;
}