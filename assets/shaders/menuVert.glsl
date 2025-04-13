#version 330 core
layout (location = 0) in vec2 aPos; // Change aPos to vec2
layout (location = 1) in vec2 texIn;

uniform mat4 projection;

out vec2 UV;

void main()
{
    gl_Position = vec4(aPos, 0.0, 1.0); // Adjust gl_Position to use vec2
    UV = texIn;
}