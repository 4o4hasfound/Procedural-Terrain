#pragma once
#ifndef _SCREEN_SHADER_H_
#define _SCREEN_SHADER_H_

#include <iostream>

#include "Shader.h"
#include "FrameBufferObject.h"

class ScreenShader {
public:
	ScreenShader(const std::string fragmentPath);
	~ScreenShader();

	void Draw();

	Shader* const getShader();
	static void disableTests();
	static void enableTests();
private:
	Shader* shad;
	static unsigned int quadVAO, quadVBO;
	static bool initialized;

	void initializeQuad();
};


#endif