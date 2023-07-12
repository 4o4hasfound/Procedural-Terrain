#pragma once
#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>
#include <GLM/common.hpp>

#include "Shader.h"
#include "ComputeShader.h"
#include "ScreenShader.h"
#include "Camera.h"
#include "Plane.h"
#include "Texture.h"
#include "Model.h"

class ScreenShader;

class Chunk {
public:
	Chunk();
	Chunk(float x, float y, unsigned size, unsigned int texture);
	~Chunk();
	unsigned int heightmap;
	float x, y;
	unsigned size;
	std::vector<std::vector<unsigned char> > data;
};

class ChunkPairCMP {
public:
	bool operator()(const std::shared_ptr<Chunk> j, const std::pair<int, int>& i) {
		return i < std::make_pair((int)j->x, (int)j->y);
	}
};

class ChunkCMP {
public:
	bool operator()(const std::shared_ptr<Chunk> i, const std::shared_ptr<Chunk> j) {
		return std::make_pair((int)i->x, (int)i->y) > std::make_pair((int)j->x, (int)j->y);
	}
};

class SortByDis {
public:
	SortByDis(glm::vec2 position): position(position) {

	}
	bool operator()(const glm::vec2& a, const glm::vec2& b) {
		return (position.x - a.x) * (position.x - a.x) + (position.y - a.y) * (position.y - a.y)
			> (position.x - b.x) * (position.x - b.x) + (position.y - b.y) * (position.y - b.y);
	}
	glm::vec2 position;
};

class Terrain {
public:
	Terrain();
	Terrain(int terrainSize, int chunksize);
	~Terrain();
	void SetupTextures();
	void Draw();
	Shader* getShad();
	Shader* getModelShad();
	void Update();
	void RenderShadow();
	int getTerrainSize();
	int getChunkSize();
	static Camera* camera;
	glm::vec3 lastCameraPos;
private:
	unsigned VAO, VBO, EBO;
	unsigned texture;
	int terrainSize, chunkSize;
	Shader* shader;
	ComputeShader* cmpShader;
	ComputeShader* noiseMapShad;
	ScreenShader* srcShader;
	Shader* modelShad;
	Model model;

	std::vector<std::shared_ptr<Chunk> > chunks;
	std::vector<glm::vec2> toGenerate;

	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;

	Texture grass, grassNor, rock, rockNor, rockDis;

	void setupBuffer();
	void GenHeightmap(float x, float y);
	void GenChunks(float x, float y);
	void UpdateChunks();
	void GenerateChunks(unsigned int numGenerate = 1);
};

#endif