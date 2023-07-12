#include"Shader.h"

Shader::Shader() {
    ID = glCreateProgram();
}

Shader::~Shader() {
    //glDeleteProgram(ID);
}

Shader* Shader::AttachShader(std::string filename, ShaderType type) {
    std::string ShaderCode = getShaderCode(filename.c_str(), type);
    const char* charShaderCode = ShaderCode.c_str();
    GLuint shader;

    switch (type) {
    case VERT:
        shader = glCreateShader(GL_VERTEX_SHADER);
        break;
    case FRAG:
        shader = glCreateShader(GL_FRAGMENT_SHADER);
        break;
    case GEOM:
        shader = glCreateShader(GL_GEOMETRY_SHADER);
        break;
    case TCS:
        shader = glCreateShader(GL_TESS_CONTROL_SHADER);
        break;
    case TES:
        shader = glCreateShader(GL_TESS_EVALUATION_SHADER);
        break;
    default:
        std::cout << "SHADER TYPE ERROR!" << std::endl;
        return this;
    }
    glShaderSource(shader, 1, &charShaderCode, NULL);
    glCompileShader(shader);
    if (checkCompileErrors(shader, type)) {
        glAttachShader(ID, shader);
        shaders.push_back(shader);
    }
    return this;

}
void Shader::LinkProgram() {
    glLinkProgram(ID);
    if (checkCompileErrors(ID, PROGRAM)) {
        linked = true;
        for (int i = 0, len = shaders.size(); i < len; i++) {
            glDeleteShader(shaders[i]);
        }
    }
}

void Shader::use() const {
    glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const {
    glUniform1i(getLocation(name), (int)value);
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(getLocation(name), value);
}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(getLocation(name), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const {
    glUniform2fv(getLocation(name), 1, &value[0]);
}
void Shader::setVec2(const std::string& name, float x, float y) const {
    glUniform2f(getLocation(name), x, y);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(getLocation(name), 1, &value[0]);
}
void Shader::setVec3(const std::string& name, float x, float y, float z) const {
    glUniform3f(getLocation(name), x, y, z);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const {
    glUniform4fv(getLocation(name), 1, &value[0]);
}
void Shader::setVec4(const std::string& name, float x, float y, float z, float w) {
    glUniform4f(getLocation(name), x, y, z, w);
}

void Shader::setMat2(const std::string& name, const glm::mat2& mat) const {
    glUniformMatrix2fv(getLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const std::string& name, const glm::mat3& mat) const {
    glUniformMatrix3fv(getLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(getLocation(name), 1, GL_FALSE, &mat[0][0]);
}

GLuint Shader::getLocation(const std::string& name) const {
    if (attributes.find(name) != attributes.end()) {
        return attributes[name];
    }
    GLuint location = glGetUniformLocation(ID, name.c_str());
    attributes[name] = location;
    return location;
}

std::string Shader::getShaderCode(const char* filePath, ShaderType type) {
    std::FILE* fp = std::fopen(filePath, "rb");
    if (fp)
    {
        std::string contents;
        std::fseek(fp, 0, SEEK_END);
        contents.resize(std::ftell(fp));
        std::rewind(fp);
        std::fread(&contents[0], 1, contents.size(), fp);
        std::fclose(fp);
        return(contents);
    }
    throw(errno);
}

bool Shader::checkCompileErrors(GLuint shader, ShaderType type) {
    GLint success;
    GLchar infoLog[1024];
    if (type == PROGRAM) {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: PROGRAM\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: ";
            switch (type) {
            case VERT:
                std::cout << "VERTEX"; break;
            case FRAG:
                std::cout << "FRAGMENT"; break;
            case GEOM:
                std::cout << "GEOMETRY"; break;
            case TCS:
                std::cout << "TESSELLATION CONTROL"; break;
            case TES:
                std::cout << "TESSELLATION EVALUATION"; break;
            }
            std::cout << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    return success;
}