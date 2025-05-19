#version 330 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 UVin;
layout (location = 2) in float charIn;

uniform mat4 model;
uniform vec4 screen;
uniform float time;

out vec2 UV;
out float charNum;

void main()
{
    vec4 worldSpace = model * vec4(pos,0,1);
    vec4 scaleCoords = worldSpace / screen;
    vec4 mapToScreen = scaleCoords*2 + vec4(-1,-1,0,0);
    mapToScreen.y = -mapToScreen.y;

    gl_Position = vec4(mapToScreen.xy,0,1);

    UV = UVin;
    charNum = charIn;
}