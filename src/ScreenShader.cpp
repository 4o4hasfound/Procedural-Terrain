#include "ScreenShader.h"

unsigned int ScreenShader::quadVAO = 0;
unsigned int ScreenShader::quadVBO = 0;
bool ScreenShader::initialized = false;

ScreenShader::ScreenShader(const std::string fragmentPath) {
	initializeQuad();
	shad = new Shader();
	shad->AttachShader("shaders/Screen/screen.vert", VERT)
		->AttachShader(fragmentPath, FRAG)
		->LinkProgram();

}

ScreenShader::~ScreenShader() {
	//delete shad;
}

void ScreenShader::Draw() {
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

Shader* const ScreenShader::getShader() {
	return shad;
}

void ScreenShader::disableTests() {
	glDisable(GL_DEPTH_TEST);
}

void ScreenShader::enableTests() {
	glEnable(GL_DEPTH_TEST);
}

void ScreenShader::initializeQuad() {
	if (!initialized) {
		float vertices[] = {
			-1.0f, -1.0f, 0.0, 0.0,
			1.0f, -1.0f, 1.0, 0.0,
			-1.0f,  1.0f, 0.0, 1.0,
			1.0f,  1.0f, 1.0, 1.0,
			-1.0f,  1.0f, 0.0, 1.0,
			1.0f, -1.0f, 1.0, 0.0
		};

		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);
		ScreenShader::initialized = true;
	}
}
