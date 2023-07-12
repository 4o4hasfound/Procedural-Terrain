#ifndef TERRAIN2_INCLUDED
#define TERRAIN2_INCLUDED

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

class Terrain2 {
public:
	Terrain2();
	Terrain2(int terrainWidth, int terrainHeight, int division);
	~Terrain2();
	void SetupTextures();
	void Draw();
	Shader* getShad();
	Shader* getModelShad();
	void Update() {}
	void RenderShadow();
	int getTerrainSize() { return 0; }
	int getChunkSize() { return 0; }
	static Camera* camera;
	glm::vec3 lastCameraPos;
private:
	unsigned VAO, VBO, EBO;
	unsigned texture;
	int m_terrainWidth, m_terrainHeight;
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