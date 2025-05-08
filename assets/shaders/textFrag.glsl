#version 330 core

uniform sampler2D tex0;
uniform float len;

in vec2 UV;
in float charNum;

out vec4 FragColor;

void main()
{
    if (charNum == -1) {
        FragColor = vec4(1);
    } else {
        vec2 newUV = vec2((charNum + UV.x)/len,UV.y);
        FragColor = texture(tex0, newUV);
    }
    if (FragColor.w == 0) discard;
}