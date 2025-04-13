#version 330 core
out vec4 FragColor;

in vec2 UV;

uniform sampler2D menuTexture;

// Modify the fragment shader to output a solid white color for debugging
void main()
{
    FragColor = texture(menuTexture, UV); // Sample the texture using UV coordinates
}