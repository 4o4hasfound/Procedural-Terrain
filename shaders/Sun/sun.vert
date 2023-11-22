#version 410 core

layout(location = 0) in vec2 pos;

uniform vec3 cameraPos;
uniform vec3 lightDir;
uniform mat4 proj;
uniform mat4 view;

void main(){
	gl_Position = proj * view * vec4(cameraPos+lightDir, 1.0);
}