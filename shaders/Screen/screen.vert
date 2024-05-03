#version 430 core

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 tc;


out vec2 TexCoords;

void main(){
	gl_Position = vec4(pos.xy, 0.0, 1.0);
	TexCoords = tc;
}
