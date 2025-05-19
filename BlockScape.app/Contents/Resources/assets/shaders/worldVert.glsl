#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 normIn;
layout (location = 2) in vec2 texIn;
layout (location = 3) in float textureIDin;
layout (location = 4) in float blockID;

uniform float time;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 pos;
out vec2 UV;
out vec3 normal;
out float textureID;


void main()
{
	vec4 viewPos = view * model * vec4(aPos, 1.0);
	if (blockID == 5) {
		viewPos += vec4(sin(time + viewPos.x),cos(2*time + viewPos.y),sin(time*3 +  + viewPos.z), 0) * 0.03;
	}

	gl_Position = projection * viewPos;

	pos = aPos;
	UV = texIn;
	normal = normIn;
	textureID = textureIDin;
}