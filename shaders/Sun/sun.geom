#version 410 core

const int sides = 63;

layout(points) in;
layout(triangle_strip, max_vertices = 189) out;

const float PI = 3.1415926;

uniform float screenRatio;

// Angle between each side in radians
#define Ang(i) PI * 2.0 / sides * i
#define Offset(i) vec4(cos(Ang(i)) * 0.05, -sin(Ang(i)) * 0.05 * screenRatio, 0.9, 0.0)

void main(){
	for (int i = 1; i <= sides; i++) {
        // Offset from center of point
        vec4 offset = Offset(i);
        gl_Position = gl_in[0].gl_Position + Offset(0);

        EmitVertex();
        gl_Position = gl_in[0].gl_Position + Offset(i);

        EmitVertex();
        gl_Position = gl_in[0].gl_Position + Offset(mod((i+1), (sides+1)));

        EmitVertex();
    }

	EndPrimitive();
}