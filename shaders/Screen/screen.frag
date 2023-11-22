#version 330 core

in vec2 TexCoords;
uniform sampler2D text;
uniform float gamma;
uniform float exposure;

float linearize_depth(float d,float zNear,float zFar)
{
    float z_n = 2.0 * d - 1.0;
    return 2.0 * zNear * zFar / (zFar + zNear - z_n * (zFar - zNear));
}

void main(){
//	vec4 fragment = texture(text, TexCoords);
//	gl_FragColor.rgb = pow(fragment.rgb, vec3(1.0/gamma));
	vec3 fragment = texture(text, TexCoords).rgb;
	vec3 mapped = vec3(1.0) - exp(-fragment * exposure);
	gl_FragColor = vec4(mapped, 1.0);
	//gl_FragColor = vec4(vec3(max(fragment.r-0.999, 0.0))*1000.0, 1.0);
}
