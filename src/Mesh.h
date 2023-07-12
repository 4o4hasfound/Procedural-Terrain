#pragma once
#ifndef _MESH_H
#define _MESH_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Texture.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
};

class Mesh {
public:
    std::vector<Vertex>       vertices;
    std::vector<GLuint> indices;
    Texture texture;
    unsigned int VAO;
    float scale = 10;
    glm::vec3 Position;

    Mesh() = default;

    Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, Texture texure);

    void Draw(Shader* shader);

private:
    unsigned int VBO, EBO;

    void setupMesh();
};
#endif