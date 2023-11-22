#ifndef SUN_H
#define SUN_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>
#include <GLM/common.hpp>

#include "Shader.h"

class Sun {
public:
	Sun();
    Shader* getShad();
    void Draw();
private:
    float points[2] = {
        0.0, 0.0
    };

    GLuint vao, vbo;
    Shader* shader;
};

#endif