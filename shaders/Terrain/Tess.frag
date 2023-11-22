#version 410 core

out vec4 FragColor;
in vec2 uvs;
in vec2 pos;
in vec3 WorldPos;

uniform sampler2D screen;
uniform sampler2D heightMap;
uniform float height;
uniform float size;
uniform vec3 cameraPos;
uniform sampler2D grass;
uniform sampler2D grassNor;
uniform sampler2D rock;
uniform sampler2D rockNor;
uniform sampler2D rockDis;
uniform vec3 lightDir;
uniform float waterHeight;
uniform float gamma;
uniform bool usePar;
uniform float height_scale;

uniform vec3 lightColor;
vec3 objectColor = {1.0, 1.0, 1.0};

vec3 computeNormal(vec2 Pos, out mat3 TBN){
	float st = 1.0;
	float dhdu = ((texture(heightMap, vec2((Pos.x + (st / (size + 2.0))), Pos.y)).r*height - texture(heightMap, vec2((Pos.x -  (st / (size + 2.0))), Pos.y)).r*height)/(2.0*st));
	float dhdv = ((texture(heightMap, vec2(Pos.x, (Pos.y +  (st / (size + 2.0))))).r*height - texture(heightMap, vec2(Pos.x, (Pos.y -  (st / (size + 2.0))))).r*height)/(2.0*st));

	vec3 X = vec3(1.0, dhdu, 1.0);
	vec3 Z = vec3(0.0, dhdv, 1.0);

	vec3 n = normalize(cross(Z,X));
	
	TBN = mat3(normalize(X), normalize(Z), n);
	return n;
}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir, sampler2D depthMap)
{ 
    float height =  texture(depthMap, texCoords).r;    
    vec2 p = viewDir.xy / viewDir.z * (height * height_scale);
    return texCoords - p;    
} 


void main()
{
	float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

	mat3 TBN;
	vec3 normal = computeNormal(pos, TBN);
	vec2 UV = vec2(WorldPos.xz/25.0);

	vec3 GrassNormal = normalize(TBN*(texture(grassNor, UV).rgb*2.0-1.0));
	vec3 RockNormal = normalize(TBN*(texture(rockNor, UV).rgb*2.0-1.0));

	// Decide the texture of the current point
	float cosV = max(dot(normal, vec3(0.0, 1.0, 0.0)), 0.0);
	if (cosV < 0.45) {
		// all grass
		objectColor = pow(texture(rock, UV).rgb, vec3(gamma));
		normal = RockNormal;
	}
	else if (cosV > 0.8) {
		// all rock
		objectColor = pow(texture(grass, UV/2.0).rgb, vec3(gamma)) * 0.9;
		normal = GrassNormal;
	}
	else{
		// mix rock and grass based on cosV
		objectColor = mix(pow(texture(rock, UV).rgb, vec3(gamma)), pow(texture(grass, UV/2.0).rgb, vec3(gamma)), (cosV-0.45)/0.35);
		normal = GrassNormal;
	}

	// Diffuse lighting
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	// Combined
    vec3 result = (ambient + diffuse*0.8) * objectColor;

	// Calculate fog
	float fog_maxdist = 30000.0;
	float fog_mindist = 6000.0;
	vec4 fog_colour = vec4(0.4, 0.4, 0.4, 1.0);

	float dist = length(cameraPos-WorldPos);
	float fog_factor = (fog_maxdist - dist) /
				  (fog_maxdist - fog_mindist);
	fog_factor = clamp(fog_factor, 0.0, 1.0);

	FragColor = vec4(pow(mix(fog_colour, vec4(result, 1.0), fog_factor).rgb, vec3(1.0/gamma)), WorldPos.y/waterHeight);
}