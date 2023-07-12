#pragma once
#ifndef _WATER_H_
#define _WATER_H_
#include <vector>
#include <glad/glad.h>
#include "Plane.h"
#include "Shader.h"
#include "Texture.h"

class Water {
public:
	Water();
	Water(float size);
	~Water();
	void Draw(unsigned reflect, unsigned refract, unsigned refractDepth, float time);
	Shader* getShad();
	static float height;
private:
	Shader* shader;
	Texture dudv, normal;
	unsigned VAO, VBO, EBO;
	const float MOVE_SPEED = 0.0025f;
	float waterTileCount = 0;
	float moveFactor = 0.0;
	float Size;
	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;

	void setupBuffer();
};

#endif