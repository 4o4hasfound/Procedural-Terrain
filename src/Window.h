#pragma once
#ifndef _WINDOW_H_
#define _WINDOW_H_
#include <iostream>
#include <iomanip>
#include <ctime>
#include <cmath>
#include <chrono>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "FreeImage.h"


#include "Camera.h"
#include "Terrain.h"
#include "Skybox.h"

class Window {
public:
	Window(int& success, unsigned int SCR_WIDTH = 2500, unsigned int SCR_HEIGHT = 1200, std::string name = "Procedural Terrain Generation");
	~Window();
	GLFWwindow* w;
	GLFWwindow* getWindow() const;

	void processInput(float frameTime);

	static unsigned int SCR_WIDTH;
	static unsigned int SCR_HEIGHT;

	static bool useGUI;
	static bool genSrcShot;

	void terminate();
	void clearScreen(float r = 0.0, float g = 0.0, float b = 0.0, float a = 1.0);
	bool isWireframeActive();
	bool continueLoop();
	void swapBuffersAndPollEvents();
	float getTime();
	static Camera* camera;
private:
	int oldState, newState;
	int gladLoader();
	float screenWidth, screenHeight;
	std::chrono::steady_clock::time_point startTime = std::chrono::high_resolution_clock::now();
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	static bool keyBools[10];

	static bool mouseCursorDisabled;

	static bool wireframe;

	static bool firstMouse;

	static float lastX;
	static float lastY;
	
	glm::vec3 lastPos;

	std::string name;
};
#endif