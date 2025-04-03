#version 330 core

uniform float time;
uniform sampler2D texture1;

in vec2 texCoord;
in vec3 normal;
in mat4 modelFrag;

out vec4 FragColor;

void main ()
{
	vec3 sun = vec3(2,3,1);
	vec3 nFace;

	vec3 absNormal = abs(normalize(normal));
    if (absNormal.x > absNormal.y && absNormal.x > absNormal.z) {
        // X-axis is dominant
        if (normal.x > 0.0) {
            // Right face
			nFace = vec3(1,0,0);
        } else {
            // Left face
			nFace = vec3(-1,0,0);
        }
    } else if (absNormal.y > absNormal.x && absNormal.y > absNormal.z) {
        // Y-axis is dominant
        if (normal.y > 0.0) {
            // Top face
			nFace = vec3(0,1,0);
        } else {
            // Bottom face
			nFace = vec3(0,-1,0);
        }
    } else {
        // Z-axis is dominant
        if (normal.z > 0.0) {
            // Front face
			nFace = vec3(0,0,1);
        } else {
            // Back face
			nFace = vec3(0,0,-1);
        }
    }

	float dotProd = (dot(normalize(nFace),normalize(sun))+1)/2;
	FragColor = (dotProd + .2) * texture(texture1, texCoord);
}