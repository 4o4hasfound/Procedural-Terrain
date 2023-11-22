#version 410 core

// Quads
layout (quads, equal_spacing , ccw) in;

in vec2 uvsCoord[];
out vec2 uvs;
out vec2 pos;

uniform mat4 proj;
uniform mat4 view;
uniform sampler2D heightMap;
uniform vec3 cameraPos;
uniform vec2 chunkPos;
uniform vec4 plane;
uniform float chunkSize;
out vec3 WorldPos;

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

uniform float size;
uniform float height;

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec2 uv0 = uvsCoord[0];
    vec2 uv1 = uvsCoord[1];
    vec2 uv2 = uvsCoord[2];
    vec2 uv3 = uvsCoord[3];

    vec2 leftUV = uv0 + v * (uv3 - uv0);
    vec2 rightUV = uv1 + v * (uv2 - uv1);
    vec2 texCoord = leftUV + u * (rightUV - leftUV);

    vec4 pos0 = gl_in[0].gl_Position;
    vec4 pos1 = gl_in[1].gl_Position;
    vec4 pos2 = gl_in[2].gl_Position;
    vec4 pos3 = gl_in[3].gl_Position;

    vec4 leftPos = pos0 + v * (pos3 - pos0);
    vec4 rightPos = pos1 + v * (pos2 - pos1);
    vec4 Pos = leftPos + u * (rightPos - leftPos);

	// Relative position to chunk
	pos = vec2((Pos.x+int(size/2.0)+1.0)/(size+2.0), (Pos.z+int(size/2.0)+1.0)/(size+2.0));

	// Absolute position in world
	WorldPos = vec3((Pos.x+chunkPos.x), texture(heightMap, pos).r*height, (Pos.z+chunkPos.y));

	gl_ClipDistance[0] = WorldPos.y + plane.w;
    gl_Position = proj * view * vec4(WorldPos, 1.0);
    uvs = texCoord;
}