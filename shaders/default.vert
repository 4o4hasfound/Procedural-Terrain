#version 330 core
layout ( location = 0 ) in vec3 position;
layout ( location = 1 ) in vec3 normal;
layout ( location = 2 ) in vec2 texCoords;

out vec2 TexCoords;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec2 Pos;
uniform int size;
uniform float height;
uniform sampler2D heightmap;

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


float getHeight(vec2 pos){
	return noise(pos.x, pos.y, 12);
}

vec3 computeNormal(vec2 Pos, out mat3 TBN){
	float st = 1.0;
	float dhdu = ((getHeight(vec2((Pos.x + (st / (size + 2.0))), Pos.y))*height - getHeight(vec2((Pos.x -  (st / (size + 2.0))), Pos.y))*height)/(2.0*st));
	float dhdv = ((getHeight(vec2(Pos.x, (Pos.y +  (st / (size + 2.0)))))*height - getHeight(vec2(Pos.x, (Pos.y -  (st / (size + 2.0)))))*height)/(2.0*st));

	vec3 X = vec3(1.0, dhdu, 1.0);
	vec3 Z = vec3(0.0, dhdv, 1.0);

	vec3 n = normalize(cross(Z,X));
	
	TBN = mat3(normalize(X), normalize(Z), n);
	//vec3 norm = mix(n, Normal, 0.5); 
	//norm = normalize(norm);
	return n;
}


void main( )
{
	vec2 pos = vec2((Pos.x+int(size/2.0)+1.0)/(size+2.0), (Pos.y+int(size/2.0)+1.0)/(size+2.0));
    bool show=true;
    for(int i = -1; i <= 1; i++){
        for(int j = -1; j <= 1; j++){
			mat3 TBN;
	        float cosV = max(dot(computeNormal(Pos+vec2(i, j), TBN), vec3(0.0, 1.0, 0.0)), 0.0);
            if(cosV<0.1){
				//gl_ClipDistance[1] = -1;
			}
        }
    }
    gl_Position = projection * view * model * vec4(position+vec3(Pos.x, texture(heightmap, pos).r*height, Pos.y), 1.0f );
    TexCoords = texCoords;
    Normal = normal;
}