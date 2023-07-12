#pragma once
#ifndef _MODEL_H_
#define _MODEL_H_

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Mesh.h"

class Model {
public:
	Model() {}
	Model(std::string path, std::string texturePath);

	void Draw(Shader* shader);

private:
	std::vector<Mesh> meshes;
	std::string directory;
	Texture texture;

	void loadModel(std::string path);

	void processNode(aiNode* node, const aiScene* scene);

	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, Texture::Type typeName);
};


#endif