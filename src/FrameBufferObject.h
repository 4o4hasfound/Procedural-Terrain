#pragma once
#ifndef _FRAME_BUFFER_OBJECT_
#define _FRAME_BUFFER_OBJECT_

#include <iostream>

#include <glad/glad.h>
#include "Window.h"

void bindFrameBuffer(int frameBuffer, int width, int height);

void unbindCurrentFrameBuffer(int scrWidth, int scrHeight);
void unbindCurrentFrameBuffer();

unsigned int createFrameBuffer();

unsigned int createTextureAttachment(int width, int height);

unsigned int* createColorAttachments(int width, int height, unsigned int nColorAttachments);

unsigned int createDepthTextureAttachment(int width, int height);

unsigned int createDepthBufferAttachment(int width, int height);

unsigned int createRenderBufferAttachment(int width, int height);

class FrameBufferOBJ {
public:
	FrameBufferOBJ(int W, int H);
	FrameBufferOBJ(int W, int H, int numColorAttachments);
	unsigned int FBO, renderBuffer, depthTex;
	unsigned int tex;
	int W, H;
	unsigned int getColorAttachmentTex(int i);
	void bind();
private:
	int nColorAttachments;
	unsigned int* colorAttachments;
};
#endif