#version 410 core

out vec4 outColor;

void main(){
	outColor = vec4(10.0, 10.0, 10.0, 1.0);
	gl_FragDepth = 0.99999;
}