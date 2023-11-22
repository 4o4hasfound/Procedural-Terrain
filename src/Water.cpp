#include "Water.h"

float Water::height = 1000.0f;

Water::Water()
{
}

Water::Water(float size): Size(size) {
    shader = new Shader();
    shader->AttachShader("shaders/water.vert", VERT)
          ->AttachShader("shaders/water.frag", FRAG)
          ->LinkProgram();
    //dudv = Texture("textures/water/Dudv.png");
    //normal = Texture("textures/water/Normal.png");
	vertices = genNonSymPlaneUniform(glm::vec3(-Size/2.0, 0.0, -Size/2.0), glm::vec3(-Size/2.0, 0.0, Size/2), glm::vec3(Size/2, 0.0, Size/2.0), glm::vec3(Size/2, 0.0, -Size/2), 1);
	indices = genPlaneInd(1);
    setupBuffer();
}

Water::~Water() {
	
}

void Water::Draw(unsigned reflect, unsigned refract, unsigned refractDepth, float time) {
    glEnable(GL_CULL_FACE);
    moveFactor += MOVE_SPEED * time;

    shader->use();
    shader->setFloat("height", height);
    shader->setInt("reflectionTexture", 0);
    shader->setInt("refractionTexture", 1);
    shader->setInt("dudvMap", 2);
    shader->setInt("normalMap", 3);
    shader->setInt("depthMap", 4);
    shader->setFloat("moveFactor", moveFactor);
    shader->setFloat("Size", Size);
    shader->setFloat("tiling", 640.0);
    shader->setFloat("waterTileCount", waterTileCount);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, reflect);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, refract);
    glActiveTexture(GL_TEXTURE2);
    dudv.Bind();
    glActiveTexture(GL_TEXTURE3);
    normal.Bind();
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, refractDepth);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

Shader* Water::getShad() {
    return shader;
}

void Water::setupBuffer() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
}

