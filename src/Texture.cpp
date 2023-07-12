#include "Texture.h"

Texture::Texture(std::string filename, GLenum type, Type textureType) : filename(filename), type(textureType) {
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, (nrChannels == 3 ? GL_RGB : GL_RGBA), GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Error when loading " << filename << "\n";
    }
    stbi_image_free(data);
}

Texture::~Texture() {
    //glDeleteTextures(1, &id);
}

void Texture::Bind() {
    glBindTexture(GL_TEXTURE_2D, id);
}

Texture::Type Texture::getType() {
    return type;
}

unsigned int Texture::getID() {
    return id;
}

std::string Texture::getFileName() {
    return filename;
}
