#version 430 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoords;

out vec2 uvs;
uniform mat4 model;


void main()
{
	gl_Position = model * vec4(pos.xyz, 1.0);
	uvs = texCoords;
}