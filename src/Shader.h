#ifndef _SHADER_H
#define _SHADER_H

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

enum ShaderType{
    VERT,
    FRAG,
    GEOM,
    TCS,
    TES,
    PROGRAM,
};

class Shader {
public:
    Shader();
    ~Shader();

    Shader* AttachShader(std::string filename, ShaderType type);
    void LinkProgram();

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
    unsigned int ID;
    bool linked = false;

    std::vector<unsigned int> shaders;
    mutable std::unordered_map<std::string, GLuint> attributes;

    GLuint getLocation(const std::string& name) const;
    std::string getShaderCode(const char* filePath, ShaderType type);
    bool checkCompileErrors(GLuint shader, ShaderType type);
};
#endif