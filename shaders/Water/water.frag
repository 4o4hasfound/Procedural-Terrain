#version 440 core
out vec4 FragColor;

in vec4 clipSpace;
in vec3 toCameraVector;
in vec2 textureCoords;
in vec4 worldPosition;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;
uniform float moveFactor;
uniform vec3 lightDir;
uniform float near;
uniform float far;
uniform float tiling;
uniform float waterTileCount;
uniform float gamma;

const float waveStrength = 0.001;
const float shineDamper = 50.0;
const float reflectivity = 0.8;

vec3 lightColor = pow(vec3(1.0, 1.0, 1.0), vec3(gamma));


// A single iteration of Bob Jenkins' One-At-A-Time hashing algorithm.
uint hash( uint x ) {
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}



// Compound versions of the hashing algorithm I whipped together.
uint hash( uvec2 v ) { return hash( v.x ^ hash(v.y)                         ); }
uint hash( uvec3 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z)             ); }
uint hash( uvec4 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z) ^ hash(v.w) ); }



// Construct a float with half-open range [0:1] using low 23 bits.
// All zeroes yields 0.0, all ones yields the next smallest representable value below 1.0.
float floatConstruct( uint m ) {
    const uint ieeeMantissa = 0x007FFFFFu; // binary32 mantissa bitmask
    const uint ieeeOne      = 0x3F800000u; // 1.0 in IEEE binary32

    m &= ieeeMantissa;                     // Keep only mantissa bits (fractional part)
    m |= ieeeOne;                          // Add fractional part to 1.0

    float  f = uintBitsToFloat( m );       // Range [1:2]
    return f - 1.0;                        // Range [0:1]
}



// Pseudo-random value in half-open range [0:1].
float random( float x ) { return floatConstruct(hash(floatBitsToUint(x))); }
float random( vec2  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec3  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec4  v ) { return floatConstruct(hash(floatBitsToUint(v))); }


float Interpolate(float a, float b, float x) {  // cosine interpolation
	float ft = x * 3.1415927f;
	float f = (1. - cos(ft)) * 0.5;
	return  a * (1. - f) + b * f;
}

float InterpolatedNoise(int ind, float x, float y, float z) {
	int integer_X = int(floor(x));
	float fractional_X = fract(x);
	int integer_Y = int(floor(y));
	float fractional_Y = fract(y);
    int integer_Z = int(floor(z));
    float fractional_Z = fract(z);
    
	vec3 randomInput = vec3(integer_X, integer_Y, integer_Z);
	float v1 = random(randomInput + vec3(0.0, 0.0, 0.0));
	float v2 = random(randomInput + vec3(1.0, 0.0, 0.0));
	float v3 = random(randomInput + vec3(0.0, 1.0, 0.0));
	float v4 = random(randomInput + vec3(1.0, 1.0, 0.0));
    
    float v5 = random(randomInput + vec3(0.0, 0.0, 1.0));
	float v6 = random(randomInput + vec3(1.0, 0.0, 1.0));
	float v7 = random(randomInput + vec3(0.0, 1.0, 1.0));
	float v8 = random(randomInput + vec3(1.0, 1.0, 1.0));
    
    
	float i1 = Interpolate(v1, v2, fractional_X);
	float i2 = Interpolate(v3, v4,  fractional_X);
    
    float i3 = Interpolate(v5, v6, fractional_X);
    float i4 = Interpolate(v7, v8, fractional_X);
    
    float y1 = Interpolate(i1, i2, fractional_Y);
    float y2 = Interpolate(i3, i4, fractional_Y);
    
    
	return Interpolate(y1, y2, fractional_Z);
}

float perlin(float x, float y, float z){
	
    int numOctaves = 3;
	float persistence = 0.5;
	float total = 0.,
		frequency = 0.025,
		amplitude = 6.0;
	for (int i = 0; i < numOctaves; ++i) {
		frequency *= 2.;
		amplitude *= persistence;
		
		x+=moveFactor*13.0;
		y+=moveFactor*7.0;

		total += InterpolatedNoise(0, x * frequency, y * frequency, z * frequency) * amplitude;
		//total += InterpolatedNoise(0, y * frequency, x * frequency, (z+0.5) * frequency) * amplitude;
	}
	return pow(total, 1.0);
}

vec3 computeNormals(vec3 WorldPos, out mat3 TBN){
	float st = 0.35;
	float dhdu = (perlin((WorldPos.x + st), WorldPos.z, ((moveFactor+waterTileCount)*tiling)/2.0) - perlin((WorldPos.x - st), WorldPos.z, ((moveFactor+waterTileCount)*tiling)/2.0))/(2.0*st);
	float dhdv = (perlin( WorldPos.x, (WorldPos.z + st), ((moveFactor+waterTileCount)*tiling)/2.0) - perlin(WorldPos.x, (WorldPos.z - st), ((moveFactor+waterTileCount)*tiling)/2.0))/(2.0*st);

	vec3 X = vec3(1.0, dhdu, 1.0);
	vec3 Z = vec3(0.0, dhdv, 1.0);

	vec3 n = normalize(cross(Z,X));
	TBN = mat3(normalize(X), normalize(Z), n);
	return n;
}

vec3 computeWaterNormals(vec2 WorldPos){
	float st = 0.35;
	float dhdu = (perlin((WorldPos.x + st), WorldPos.x + WorldPos.y, ((moveFactor+waterTileCount)*tiling)/2.0) - perlin((WorldPos.x - st), WorldPos.x + WorldPos.y, ((moveFactor+waterTileCount)*tiling)/2.0))/(2.0*st);
	float dhdv = (perlin( WorldPos.x, (WorldPos.x + WorldPos.y + st), ((moveFactor+waterTileCount)*tiling)/2.0) - perlin(WorldPos.x, (WorldPos.x + WorldPos.y - st), ((moveFactor+waterTileCount)*tiling)/2.0))/(2.0*st);

	vec3 X = vec3(1.0, dhdu, 1.0);
	vec3 Z = vec3(0.0, dhdv, 1.0);

	vec3 n = normalize(cross(Z,X));
	return n;
}

void main()
{   
    vec2 ndc = (clipSpace.xy/clipSpace.w)/2.0+0.5;
    vec2 refractTexCoords = vec2(ndc.x, ndc.y);
    vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);

    float floorY = texture(refractionTexture, ndc).a;
	float waterDepth = 1.0 - floorY;
    float waterDepthClamped = clamp(waterDepth*5.0, 0.0, 1.0);
    
    mat3 TBN;
    vec2 distortedTexCoords = (computeWaterNormals(vec2(textureCoords.x + moveFactor, textureCoords.y)).rg * 2.0 - 1.0) * waveStrength * clamp(waterDepth / 1.0, 0.0, 1.0);
    distortedTexCoords = textureCoords + vec2(distortedTexCoords.x, distortedTexCoords.y + moveFactor);
    
    vec2 totalDistortion = (computeWaterNormals(distortedTexCoords).rg * 2.0 - 1.0) * waveStrength;
    refractTexCoords += totalDistortion;
    refractTexCoords = clamp(refractTexCoords, 0.001, 0.999);

    reflectTexCoords += totalDistortion;
    reflectTexCoords.x = clamp(reflectTexCoords.x, 0.001, 0.999);
    reflectTexCoords.y = clamp(reflectTexCoords.y, -0.999, -0.001);

    vec4 reflectColor = texture(reflectionTexture, reflectTexCoords);
    vec4 refractColor = texture(refractionTexture, refractTexCoords); 
    
    vec3 normal = computeNormals(vec3(worldPosition+0.0*((moveFactor+waterTileCount)*tiling)), TBN);
    if (length(toCameraVector)>15000.0){
        normal = vec3(0.0, 1.0, 0.0);
    }
	normal = normalize(mix(normal, vec3(0.0, 1.0, 0.0), 0.1));

    vec3 viewVector = normalize(toCameraVector);
    float refractiveFactor = dot(viewVector, normal);
    refractiveFactor = pow(refractiveFactor, 0.5);
    
    vec3 halfwayDir = normalize(lightDir + viewVector);

    float specular = pow(max(dot(normal, halfwayDir), 0.0), shineDamper);
    vec3 specularHighlights = lightColor * specular * reflectivity;

    FragColor = mix(reflectColor, refractColor, clamp(refractiveFactor, 0.0, 1.0));
    FragColor = mix(FragColor, vec4(0.2,0.71,0.85, 1.0), 0.2) + vec4(specularHighlights, 0.0);
}