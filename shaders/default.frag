#version 330 core

in vec2 TexCoords;

out vec4 color;
in vec3 Normal;

uniform sampler2D Texture;
uniform vec3 lightDir;
uniform vec3 lightColor;
	
void main()
{
	float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;
	float diff = max(dot(Normal, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
    vec3 result = (ambient + diffuse*0.8) * texture(Texture, TexCoords).rgb;
    color = vec4(result, 1.0);
}