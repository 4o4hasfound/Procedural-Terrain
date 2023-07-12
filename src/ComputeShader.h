#pragma once
#ifndef _COMPUTE_SHADER_H_
#define _COMPUTE_SHADER_H_

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

class ComputeShader {
public:
    unsigned int ID,texture;

    ComputeShader();
    ComputeShader(const char* computePath);
    ~ComputeShader();

    unsigned int genTexture(unsigned int Width, unsigned int Height, unsigned int index = 0);
    void Compute(unsigned int Width, unsigned int Height);

    void use() const;

    void setBool(const std::string& name, bool value) const;

    void setInt(const std::string& name, int value) const;

    void setFloat(const std::string& name, float value) const;

    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec2(const std::string& name, float x, float y) const;

    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;

    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setVec4(const std::string& name, float x, float y, float z, float w);

    void setMat2(const std::string& name, const glm::mat2& mat) const;

    void setMat3(const std::string& name, const glm::mat3& mat) const;

    void setMat4(const std::string& name, const glm::mat4& mat) const;

private:
    mutable std::unordered_map<std::string, GLuint> attributes;
    GLuint getLocation(const std::string& name) const;
    void checkCompileErrors(GLuint shader, std::string type);
};
#endif