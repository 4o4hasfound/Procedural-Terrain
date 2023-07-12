#include "Noise.h"

unsigned int hash(unsigned int x) {
	x += (x << 10u);
	x ^= (x >> 6u);
	x += (x << 3u);
	x ^= (x >> 11u);
	x += (x << 15u);
	return x;
}

unsigned int hash(glm::vec2 v) { return hash((unsigned int)v.x ^ hash(v.y)); }

float floatConstruct(unsigned int m) {
	const unsigned int ieeeMantissa = 0x007FFFFFu; // binary32 mantissa bitmask
	const unsigned int ieeeOne = 0x3F800000u; // 1.0 in IEEE binary32

	m &= ieeeMantissa;                     // Keep only mantissa bits (fractional part)
	m |= ieeeOne;                          // Add fractional part to 1.0

	float  f = glm::uintBitsToFloat(m);       // Range [1:2]
	return f - 1.0;                        // Range [0:1]
}
float Noise(float x, float y) { return floatConstruct(hash(glm::floatBitsToUint(glm::vec2(x,y)))); }

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
	int integer_X = x;
	float fractional_X = x - integer_X;
	int integer_Y = y;
	float fractional_Y = y - integer_Y;

	float v1 = SmoothedNoise(integer_X, integer_Y),
		v2 = SmoothedNoise(integer_X + 1, integer_Y),
		v3 = SmoothedNoise(integer_X, integer_Y + 1),
		v4 = SmoothedNoise(integer_X + 1, integer_Y + 1),
		i1 = Interpolate(v1, v2, fractional_X),
		i2 = Interpolate(v3, v4, fractional_X);
	return Interpolate(i1, i2, fractional_Y);
}

float noise(float x, float y, int octaves) {
	x += 100000;
	y += 100000;
	float total = 0,
		frequency = pow(2, octaves),
		amplitude = 1;
	for (int i = 0; i < octaves; ++i) {
		frequency /= 2;
		amplitude *= persistence;
		total += InterpolatedNoise(x / frequency, y / frequency) * amplitude;
	}
	return total / frequency;
}