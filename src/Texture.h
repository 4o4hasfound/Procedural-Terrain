#pragma once
#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <GLM/glm.hpp>
#include <glad/glad.h>
#include <stb/stb_image.h>
#include <iostream>

class Texture {
public:
	enum Type {
		DIFFUSE,
		SPECULAR
	};

	Texture() {}
	Texture(std::string filename, GLenum type = GL_RGB, Type textureType = Type::DIFFUSE);
	~Texture();
	void Bind();
	Type getType();
	unsigned int getID();
	std::string getFileName();

private:
	std::string filename;
	unsigned int id;
	Type type;
};

#endif