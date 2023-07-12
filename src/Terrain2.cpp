#include "Terrain2.hpp"
Camera* Terrain2::camera = nullptr;

Terrain2::Terrain2() {

}

Terrain2::Terrain2(int terrainWidth, int terrainHeight, int division)
	: m_terrainWidth(terrainWidth), m_terrainHeight(terrainHeight) {

	shader = new Shader();
	shader->AttachShader("shaders/Tess2.vert", VERT)
		->AttachShader("shaders/Tess2.tesc", TCS)
		->AttachShader("shaders/Tess2.tese", TES)
		->AttachShader("shaders/Tess2.frag", FRAG)
		->LinkProgram();
	modelShad = new Shader();
	modelShad->AttachShader("shaders/default.vert", VERT)
		->AttachShader("shaders/default.frag", FRAG)
		->LinkProgram();
	cmpShader = new ComputeShader("shaders/HeightMap.comp");
	Update();
	lastCameraPos = camera->Position;

	vertices = genNonSymPlaneUniform(
		glm::vec3(terrainWidth / 2, 0.0f, terrainHeight / 2),
		glm::vec3(terrainWidth / 2, 0.0f, -terrainHeight / 2),
		glm::vec3(-terrainWidth / 2, 0.0f, -terrainHeight / 2), 
		glm::vec3(-terrainWidth / 2, 0.0f, terrainHeight / 2),
		division
	);
	indices = genPlaneIndTes(division);
	setupBuffer();
	shader->use();
	shader->setInt("grass", 0);
	shader->setInt("grassNor", 1);
	shader->setInt("rock", 2);
	shader->setInt("rockNor", 3);
	shader->setInt("rockdis", 4);
}

Terrain2::~Terrain2()
{
}

void Terrain2::SetupTextures() {
	grass = Texture("textures/moss_forest_vfylbge/Thumbs/1k/vfylbge_1K_Albedo.jpg");
	grassNor = Texture("textures/moss_forest_vfylbge/Thumbs/1k/vfylbge_1K_Normal.jpg");
	rock = Texture("textures/rock1/1k/Albedo.jpg");
	rockNor = Texture("textures/rock1/1k/Normal.jpg");
	rockDis = Texture("textures/rock1/1k/Displacement.jpg");
	model = Model("models/plants/Var15/Var15_LOD3.fbx", "models/plants/Textures/Atlas/wk1ncbxja_2K_Albedo.jpg");
}

void Terrain2::Draw()
{
	glEnable(GL_DEPTH_TEST);
	shader->use();
	shader->setInt("grass", 0);
	shader->setInt("grassNor", 1);
	shader->setInt("rock", 2);
	shader->setInt("rockNor", 3);
	shader->setInt("rockDis", 4);
	shader->setInt("heightMap", 5);
	glActiveTexture(GL_TEXTURE0);
	grass.Bind();
	glActiveTexture(GL_TEXTURE1);
	grassNor.Bind();
	glActiveTexture(GL_TEXTURE2);
	rock.Bind();
	glActiveTexture(GL_TEXTURE3);
	rockNor.Bind();
	glActiveTexture(GL_TEXTURE4);
	rockDis.Bind();
	glEnable(GL_CLIP_DISTANCE0);
	glActiveTexture(GL_TEXTURE5);
	shader->use();
	glBindVertexArray(VAO);
	glDrawElements(GL_PATCHES, indices.size(), GL_UNSIGNED_INT, 0);
}

Shader* Terrain2::getShad()
{
	return shader;
}

Shader* Terrain2::getModelShad()
{
	return modelShad;
}

void Terrain2::setupBuffer() {
	glGenVertexArrays(1, &VAO);
	//glCreateVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	//glCreateBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	//glCreateBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), indices.data(), GL_STATIC_DRAW);
	//glNamedBufferData(VBO, vertices.size() * sizeof(vertices[0]), vertices.data(), GL_STATIC_DRAW);
	//glNamedBufferData(EBO, indices.size() * sizeof(indices[0]), indices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glPatchParameteri(GL_PATCH_VERTICES, 4);
}
