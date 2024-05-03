#version 440 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec4 clipSpace;
out vec3 toCameraVector;
out vec2 textureCoords;
out vec4 worldPosition;

uniform mat4 projection;
uniform mat4 view;
uniform float height;
uniform vec3 cameraPos;
uniform float Size;
uniform float tiling;

void main()
{
    worldPosition = vec4(aPos.x + cameraPos.x, height, aPos.z + cameraPos.z, 1.0);
    clipSpace = projection * view * worldPosition;
    gl_Position= clipSpace; 
    toCameraVector = cameraPos - worldPosition.xyz;
    textureCoords = (aPos.xz + cameraPos.xz + Size/2.0) / Size * tiling;
}  