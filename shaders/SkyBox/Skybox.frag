#version 430 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform vec3 lightColor;
uniform float gamma;

void main()
{    
    vec3 fragment = pow(texture(skybox, TexCoords).rgb, vec3(gamma));
    FragColor.rgb = pow(fragment*lightColor, vec3(1.0/gamma));
}