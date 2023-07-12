#pragma once
#ifndef _APP_H_
#define _APP_H

#include <iostream>
#include <chrono>
#include <memory>
#include <functional>
#include <stdlib.h>
#include <time.h>
#include <GLM/glm.hpp>
#include <FreeImage.h>
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"
#include "Window.h"
#include "Camera.h"
#include "Plane.h"
#include "Terrain.h"
#include "Terrain2.hpp"
#include "Skybox.h"
#include "FrameBufferObject.h"
#include "ScreenShader.h"
#include "Water.h"
#include"Model.h"

class App;

enum class GUIState {
	MainMenu,
	Window,
	Settings,
	Player
};

class GUI {
public:
	GUI();
	GUI(App* app, Window* window, Terrain* terrain, Camera* camera);
	~GUI();
	void Render();
	void Update();
	void Shutdown();

private:
	App* app;
	Window* window;
	Terrain* terrain;
	Camera* camera;
	GUIState state = GUIState::MainMenu;
	float teleportTo[3];
	float fontSize = 3.0;
};


class App {
public:
	App();
	~App();
	void Run();
	
	static bool Gui;

	bool VSync = true;
	bool useTextures = true;
	bool useLighting = true;
	bool useNormalMapping = true;
	bool useFog = true;
	bool useSkyBox = true;
	float height=20000.0;
	float gamma = 1.0;
	float exposure = 1.0;
	float lightColor[3] = { 1.0, 1.0, 1.0 };
	float lightDir[3] = { 1.0, 0.7, 1.0 };
	bool usePar = true;
	float heightScale=0.1;
private:
	void InitGui();
	void RenderGui();
	void GenSrcShot();
	GUI* gui;
	Window* window;
	Camera* camera;
	Terrain* terrain;
	Water* water;
	Shader* shader;
	Shader* depthShader;
	Skybox* skybox;
	Shader* sunShader;
	FrameBufferOBJ* framebufferOBJ;
	FrameBufferOBJ* waterReflection;
	FrameBufferOBJ* waterRefraction;
	FrameBufferOBJ* shadow;
	ScreenShader* screenShader;
	GLuint vao;
	float nearPlane = 1.0, farPlane = 50000.0;
	void RenderScene(glm::vec4 plane, bool doTess);
	void RenderFinalScene();
	glm::mat4 ProjInfiniteFarPlane(float fov, float WHratio, float near, float far);
};

#endif