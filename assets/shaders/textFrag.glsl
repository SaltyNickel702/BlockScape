#version 330 core

in vec2 UV;

uniform sampler2D tex0;
unifrom int charInd;
uniform int fontChars;

out vec4 FragColor;

void main()
{
    UV.x = 1/(float)fontChars*(charInd + UV.x);
    FragColor = texture(tex0, UV);
}