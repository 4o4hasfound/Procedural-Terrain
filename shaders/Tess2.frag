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

uint hash(uint x) {
	x += (x << 10u);
	x ^= (x >> 6u);
	x += (x << 3u);
	x ^= (x >> 11u);
	x += (x << 15u);
	return x;
}

uint hash( uvec2 v ) { return hash( v.x ^ hash(v.y)                         ); }

float floatConstruct( uint m ) {
    const uint ieeeMantissa = 0x007FFFFFu; // binary32 mantissa bitmask
    const uint ieeeOne      = 0x3F800000u; // 1.0 in IEEE binary32

    m &= ieeeMantissa;                     // Keep only mantissa bits (fractional part)
    m |= ieeeOne;                          // Add fractional part to 1.0

    float  f = uintBitsToFloat( m );       // Range [1:2]
    return f - 1.0;                        // Range [0:1]
}
float Noise(float x, float y) { return floatConstruct(hash(floatBitsToUint(vec2(x,y)))); }
float SmoothedNoise(int x, int y) {
	float corners = (Noise(x - 1, y - 1) + Noise(x + 1, y - 1) +
		Noise(x - 1, y + 1) + Noise(x + 1, y + 1)) / 16,
		sides = (Noise(x - 1, y) + Noise(x + 1, y) + Noise(x, y - 1) +
			Noise(x, y + 1)) / 8,
		center = Noise(x, y) / 4;
	return corners + sides + center;
}

float Interpolate(float a, float b, float x) {
	float ft = x * 3.1415927,
		f = (1 - cos(ft)) * 0.5;
	return  a * (1 - f) + b * f;
}

float InterpolatedNoise(float x, float y) {
	int integer_X = int(floor(x));
	float fractional_X = x - integer_X;
	int integer_Y = int(floor(y));
	float fractional_Y = y - integer_Y;

	float v1 = SmoothedNoise(integer_X, integer_Y),
		v2 = SmoothedNoise(integer_X + 1, integer_Y),
		v3 = SmoothedNoise(integer_X, integer_Y + 1),
		v4 = SmoothedNoise(integer_X + 1, integer_Y + 1),
		i1 = Interpolate(v1, v2, fractional_X),
		i2 = Interpolate(v3, v4, fractional_X);
	return Interpolate(i1, i2, fractional_Y);
}

float persistence = 0.5;

float noise(float x, float y, int octwaves) {
	float total = 0,
		frequency = pow(2, octwaves),
		amplitude = 1;
	for (int i = 0; i < octwaves; ++i) {
		frequency /= 2;
		amplitude *= persistence;
		total += InterpolatedNoise(x / frequency, y / frequency) * amplitude;
	}
	return total / frequency;
}

float getHeight(float x, float y){
	return pow(noise(x+cameraPos.x, y+cameraPos.z, 12), 5.0);
}


vec3 computeNormal(vec2 Pos, out mat3 TBN){
	float st = 20.0;
	float dhdu = (getHeight(Pos.x + st, Pos.y) - getHeight(Pos.x - st, Pos.y)/(2.0*st));
	float dhdv = (getHeight(Pos.x, Pos.y + st) - getHeight(Pos.x, Pos.y - st)/(2.0*st));

	vec3 X = vec3(1.0, dhdu, 1.0);
	vec3 Z = vec3(0.0, dhdv, 1.0);

	vec3 n = normalize(cross(Z,X));
	
	TBN = mat3(normalize(X), normalize(Z), n);
	//vec3 norm = mix(n, Normal, 0.5); 
	//norm = normalize(norm);
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
	vec3 normal1 = computeNormal(pos, TBN);
	vec2 UV = vec2(WorldPos.xz/25.0);
	if(usePar) {
		UV = ParallaxMapping(UV, lightDir, rockDis);
	}
	vec3 GrassNormal = normalize(TBN*(texture(grassNor, UV).rgb*2.0-1.0));
	vec3 RockNormal = normalize(TBN*(texture(rockNor, UV).rgb*2.0-1.0));
	float cosV = max(dot(normal, vec3(0.0, 1.0, 0.0)), 0.0);
	if (cosV < 0.45) {
		objectColor = pow(texture(rock, UV).rgb, vec3(gamma));
		normal = RockNormal;
	}
	else if (cosV > 0.8) {
		objectColor = pow(texture(grass, UV/2.0).rgb, vec3(gamma));
		normal = GrassNormal;
	}
	else{
		objectColor = mix(pow(texture(rock, UV).rgb, vec3(gamma)), pow(texture(grass, UV/2.0).rgb, vec3(gamma)), (cosV-0.45)/0.35);
		normal = mix(RockNormal, GrassNormal, cosV);
	}
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
    vec3 result = (ambient + diffuse*0.8) * objectColor;
	float fog_maxdist = 20000.0;
	float fog_mindist = 6000.0;
	vec4 fog_colour = vec4(0.4, 0.4, 0.4, 1.0);

	// Calculate fog
	float dist = length(cameraPos-WorldPos);
	float fog_factor = (fog_maxdist - dist) /
				  (fog_maxdist - fog_mindist);
	fog_factor = clamp(fog_factor, 0.0, 1.0);
	FragColor = vec4(result, 1.0);
//	FragColor = vec4(pow(mix(fog_colour, vec4(result, 1.0), fog_factor).rgb, vec3(1.0/gamma)), WorldPos.y/waterHeight);
}