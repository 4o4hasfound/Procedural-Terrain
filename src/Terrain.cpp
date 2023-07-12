#include "Terrain.h"
Camera* Terrain::camera = nullptr;

Terrain::Terrain() {

}
Terrain::Terrain(int terrainsize, int chunksize) : terrainSize(terrainsize), chunkSize(chunksize) {
	shader = new Shader();
	shader->AttachShader("shaders/Tess.vert", VERT)
		->AttachShader("shaders/Tess.tesc", TCS)
		->AttachShader("shaders/Tess.tese", TES)
		->AttachShader("shaders/Tess.frag", FRAG)
		->LinkProgram();
	modelShad = new Shader();
	modelShad->AttachShader("shaders/default.vert", VERT)
		->AttachShader("shaders/default.frag", FRAG)
		->LinkProgram();
	cmpShader = new ComputeShader("shaders/HeightMap.comp");
	Update();
	lastCameraPos = camera->Position;
	int divisions = 2;
	vertices = genNonSymPlaneUniform(glm::vec3(chunkSize / 2, 0.0f, chunkSize / 2), glm::vec3(chunkSize / 2, 0.0f, -chunkSize / 2), glm::vec3(-chunkSize / 2, 0.0f, -chunkSize / 2), glm::vec3(-chunkSize / 2, 0.0f, chunkSize / 2), divisions);
	indices = genPlaneIndTes(divisions);
	setupBuffer();
	shader->use();
	shader->setInt("grass", 0);
	shader->setInt("grassNor", 1);
	shader->setInt("rock", 2);
	shader->setInt("rockNor", 3);
	shader->setInt("rockdis", 4);
}
Terrain::~Terrain() {
	//delete shader;
	//delete cmpShader;
}

void Terrain::SetupTextures() {
	grass = Texture("textures/grass/Albedo.jpg");
	grassNor = Texture("textures/grass/Normal.jpg");
	rock = Texture("textures/soil/Albedo.jpg");
	rockNor = Texture("textures/soil/Normal.jpg");
}

void Terrain::Draw() {
	glEnable(GL_DEPTH_TEST);
	shader->use();
	shader->setInt("grass", 0);
	shader->setInt("grassNor", 1);
	shader->setInt("rock", 2);
	shader->setInt("rockNor", 3);
	shader->setInt("rockDis", 4);
	shader->setInt("heightMap", 5);
	shader->setFloat("size", chunkSize+1.0);
	shader->setFloat("chunkSize", chunkSize + 1.0);
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
	//modelShad->use();
	//modelShad->setFloat("size", chunkSize + 1.0);
	for (int i = 0; i < chunks.size(); i++) {
		glEnable(GL_CLIP_DISTANCE0);
		shader->use();
		shader->setVec2("chunkPos", chunks[i]->x, chunks[i]->y);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, chunks[i]->heightmap);
		glBindVertexArray(VAO);
		glDrawElements(GL_PATCHES, indices.size(), GL_UNSIGNED_INT, 0);
		//glDisable(GL_CULL_FACE);
		//glEnable(GL_CLIP_DISTANCE1);
		//for (int j = chunks[i]->x; j < chunks[i]->x + chunkSize; j += chunkSize) {
		//	for (int k = chunks[i]->y; k < chunks[i]->y + chunkSize; k += chunkSize) {
		//		modelShad->use();
		//		modelShad->setVec2("Pos", j, k);
		//		modelShad->setInt("heightmap", 5);
		//		model.Draw(modelShad);
		//	}
		//}
		//glEnable(GL_CULL_FACE);
	}
}

void Terrain::setupBuffer() {
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

Shader* Terrain::getShad() {
	return shader;
}

Shader* Terrain::getModelShad() {
	return modelShad;
}

void Terrain::Update() {
	if (!chunks.size() || glm::abs(camera->Position.x - lastCameraPos.x) > chunkSize || glm::abs(camera->Position.z - lastCameraPos.z) > chunkSize) {
		UpdateChunks();
	}
	GenerateChunks(4);
}

void Terrain::RenderShadow() {
	for (int i = 0; i < chunks.size(); i++) {

	}
}

int Terrain::getTerrainSize() {
	return terrainSize;
}

int Terrain::getChunkSize()
{
	return chunkSize;
}

void Terrain::UpdateChunks() {
	std::vector<std::pair<int, int> >exist;
	exist.reserve(chunks.size());
	//std::unordered_map<std::pair<int, int>, bool, pairhash> exist;
	std::vector<std::shared_ptr<Chunk> > newChunks;
	newChunks.reserve(chunks.size());
	glm::vec2 position(glm::floor(camera->Position.x / chunkSize) * chunkSize,
		glm::floor(camera->Position.z / chunkSize) * chunkSize);
	double dis = terrainSize * terrainSize * chunkSize * chunkSize;
	toGenerate.clear();
	std::sort(chunks.begin(), chunks.end(), ChunkCMP());
#pragma omp parallel for
	for (int row = -terrainSize; row <= terrainSize; row++) {
#pragma omp parallel for
		for (int col = -terrainSize; col <= terrainSize; col++) {
			if (col * col + row * row <= terrainSize * terrainSize) {
				std::pair<int, int> p = { position.x + col * chunkSize, position.y + row * chunkSize };
				auto itr = std::lower_bound(chunks.begin(), chunks.end(), p, ChunkPairCMP());
				if (itr != chunks.end() && !(p > std::pair<int, int>((*itr)->x, (*itr)->y))) {
					newChunks.emplace_back((*itr));
				}
				else{
					toGenerate.emplace_back(position + glm::vec2(col, row) * (float)chunkSize);
				}
			}
		}
	}

	std::sort(toGenerate.begin(), toGenerate.end(), SortByDis(position));
	chunks = newChunks;
	lastCameraPos = camera->Position;
}

void Terrain::GenerateChunks(unsigned int numGenerate) {
	for (int i = 0; i < numGenerate; i++) {
		if (toGenerate.size()) {
			glm::vec2 pos = toGenerate.back();
			toGenerate.pop_back();
			unsigned int texture = cmpShader->genTexture(chunkSize + 3, chunkSize + 3);
			GenHeightmap(pos.x, pos.y);
			chunks.emplace_back(std::make_shared<Chunk>(pos.x, pos.y, chunkSize, texture));
		}
		else break;
	}
}

void Terrain::GenHeightmap(float x, float y) {
	cmpShader->use();
	cmpShader->setVec2("pos", x-1, y-1);
	cmpShader->Compute(chunkSize+3, chunkSize+3);
}

void Terrain::GenChunks(float x, float y) {

}

Chunk::Chunk() {

}

Chunk::Chunk(float x, float y, unsigned size, unsigned int texture):x(x), y(y), size(size), heightmap(texture) {
	glBindTexture(GL_TEXTURE_2D, heightmap);
	data.resize(size);
	for (int i = 0; i < size; i++) data[i].resize(size);

	//glGetTexImage(GL_TEXTURE_2D, 0, GL_R16F, GL_UNSIGNED_BYTE, data[0].data());
	//for (int i = 0; i < size; i++) {
	//	for (int j = 0; j < size; j++) {
	//		std::cout << data[i][j] << " ";
	//	}
	//	std::cout << "\n";
	//}
}

Chunk::~Chunk() {
	glDeleteTextures(1, &heightmap);
}