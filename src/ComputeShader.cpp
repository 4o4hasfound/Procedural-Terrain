#include "ComputeShader.h"

ComputeShader::ComputeShader(const char* computePath) {
    std::string computeCode;
    std::ifstream cShaderFile;

    cShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        cShaderFile.open(computePath);
        std::stringstream cShaderStream;

        cShaderStream << cShaderFile.rdbuf();

        cShaderFile.close();

        computeCode = cShaderStream.str();
    }
    catch (std::ifstream::failure& e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
    }
    const char* cShaderCode = computeCode.c_str();

    unsigned int compute;
    compute = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute, 1, &cShaderCode, NULL);
    glCompileShader(compute);
    checkCompileErrors(compute, "COMPUTE");

    ID = glCreateProgram();
    glAttachShader(ID, compute);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
}

ComputeShader::~ComputeShader() {
    //glDeleteProgram(ID);
}

unsigned int ComputeShader::genTexture(unsigned int Width, unsigned int Height, unsigned int index) {
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, Width, Height, 0, GL_RED,
        GL_FLOAT, NULL);

    glBindImageTexture(index, texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R16F);
    return texture;
}

void ComputeShader::Compute(unsigned int Width, unsigned int Height) {
    glDispatchCompute(Width, Height, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void ComputeShader::use() const {
    glUseProgram(ID);
}

void ComputeShader::setBool(const std::string& name, bool value) const {
    glUniform1i(getLocation(name), (int)value);
}

void ComputeShader::setInt(const std::string& name, int value) const {
    glUniform1i(getLocation(name), value);
}

void ComputeShader::setFloat(const std::string& name, float value) const {
    glUniform1f(getLocation(name), value);
}

void ComputeShader::setVec2(const std::string& name, const glm::vec2& value) const {
    glUniform2fv(getLocation(name), 1, &value[0]);
}
void ComputeShader::setVec2(const std::string& name, float x, float y) const {
    glUniform2f(getLocation(name), x, y);
}

void ComputeShader::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(getLocation(name), 1, &value[0]);
}
void ComputeShader::setVec3(const std::string& name, float x, float y, float z) const {
    glUniform3f(getLocation(name), x, y, z);
}

void ComputeShader::setVec4(const std::string& name, const glm::vec4& value) const {
    glUniform4fv(getLocation(name), 1, &value[0]);
}
void ComputeShader::setVec4(const std::string& name, float x, float y, float z, float w) {
    glUniform4f(getLocation(name), x, y, z, w);
}

void ComputeShader::setMat2(const std::string& name, const glm::mat2& mat) const {
    glUniformMatrix2fv(getLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void ComputeShader::setMat3(const std::string& name, const glm::mat3& mat) const {
    glUniformMatrix3fv(getLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void ComputeShader::setMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(getLocation(name), 1, GL_FALSE, &mat[0][0]);
}

GLuint ComputeShader::getLocation(const std::string& name) const {
    if (attributes.find(name) != attributes.end()) {
        return attributes[name];
    }
    GLuint location = glGetUniformLocation(ID, name.c_str());
    attributes[name] = location;
    return location;
}

void ComputeShader::checkCompileErrors(GLuint ComputeShader, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(ComputeShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(ComputeShader, 1024, NULL, infoLog);
            std::cout << "ERROR::ComputeShader_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else {
        glGetProgramiv(ComputeShader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(ComputeShader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}