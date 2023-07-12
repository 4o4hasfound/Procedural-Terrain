#include "Window.h"

unsigned int Window::SCR_WIDTH = 1920;
unsigned int Window::SCR_HEIGHT = 1080;

Camera* Window::camera = nullptr;
bool Window::useGUI = false;
bool Window::keyBools[10] = { false, false,false, false, false, false, false, false, false, false };

bool Window::wireframe = false;
bool Window::firstMouse = true;
float Window::lastX = SCR_WIDTH / 2.0f;
float Window::lastY = SCR_HEIGHT / 2.0f;

bool Window::mouseCursorDisabled = false;
bool Window::genSrcShot = false;

Window::Window(int& success, unsigned int SCR_WIDTH, unsigned int SCR_HEIGHT, std::string name) {
	Window::SCR_WIDTH = SCR_WIDTH;
	Window::SCR_HEIGHT = SCR_HEIGHT;
	Window::camera = new Camera(glm::vec3(0, 0, 0));
	success = 1;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	bool fullScreen = false;
	w = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, name.c_str(), fullScreen ? glfwGetPrimaryMonitor(): NULL, NULL);
	if (!w) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		success = 0;
		return;
	}

	glfwMakeContextCurrent(w);
	glfwSetFramebufferSizeCallback(w, &Window::framebuffer_size_callback);
	glfwSetCursorPosCallback(w, &Window::mouse_callback);
	glfwSetScrollCallback(w, &Window::scroll_callback);
	//glfwSetInputMode(this->w, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	oldState = newState = GLFW_RELEASE;
	glfwSwapInterval(1);

	success = gladLoader() & success;
	if (success) {
		std::cout << "GLFW window correctly initialized!" << std::endl;
	}
	glEnable(GL_DEPTH_CLAMP);
	GLint viewport[4];

	glGetIntegerv(GL_VIEWPORT, viewport);
	screenWidth = viewport[2];
	screenHeight = viewport[3];


}

Window::~Window() {
	//terminate();
}

GLFWwindow* Window::getWindow() const { 
	return w;
}

void Window::processInput(float frameTime) {
	if (glfwGetKey(this->w, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		if (!keyBools[0]) {
			keyBools[0] = true;
			useGUI = !useGUI;
		}
	}
	else {
		keyBools[0] = false;
	}
	//glfwSetWindowShouldClose(this->w, true);
	if (!useGUI) {
		if (glfwGetKey(this->w, GLFW_KEY_W) == GLFW_PRESS)
			camera->ProcessKeyboard(FORWARD, frameTime);
		if (glfwGetKey(this->w, GLFW_KEY_S) == GLFW_PRESS)
			camera->ProcessKeyboard(BACKWARD, frameTime);
		if (glfwGetKey(this->w, GLFW_KEY_A) == GLFW_PRESS)
			camera->ProcessKeyboard(LEFT, frameTime);
		if (glfwGetKey(this->w, GLFW_KEY_D) == GLFW_PRESS)
			camera->ProcessKeyboard(RIGHT, frameTime);
		if (glfwGetKey(this->w, GLFW_KEY_SPACE) == GLFW_PRESS)
			camera->ProcessKeyboard(UP, frameTime);
		if (glfwGetKey(this->w, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			camera->ProcessKeyboard(DOWN, frameTime);

		newState = glfwGetMouseButton(this->w, GLFW_MOUSE_BUTTON_RIGHT);

		if (newState == GLFW_RELEASE && oldState == GLFW_PRESS) {
			glfwSetInputMode(this->w, GLFW_CURSOR, (mouseCursorDisabled
				? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL));
			mouseCursorDisabled = !mouseCursorDisabled;
			if (mouseCursorDisabled)
				firstMouse = true;
			//std::cout << "MOUSE R PRESSED!" << std::endl;
		}

		oldState = newState;

		// WIREFRAME
		if (glfwGetKey(this->w, GLFW_KEY_T) == GLFW_PRESS) {
			if (keyBools[4] == false) {
				//std::cout << "WIREFRAME" << std::endl;
				wireframe = !wireframe;
				keyBools[4] = true;
			}
		}
		else if (glfwGetKey(this->w, GLFW_KEY_T) == GLFW_RELEASE) {
			if (keyBools[4] == true) { keyBools[4] = false; } // Non aggiungere niente qui
		}
	}
	if (glfwGetKey(w, GLFW_KEY_F2) == GLFW_PRESS) {
		if (!keyBools[1]) {
			keyBools[1] = true;
			genSrcShot = true;
		}
	}
	else {
		keyBools[1] = false;
	}
}

void Window::terminate() {
    glfwTerminate();
}

void Window::clearScreen(float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool Window::isWireframeActive() {
    return Window::wireframe;
}

bool Window::continueLoop() {
    return !glfwWindowShouldClose(this->w);
}

void Window::swapBuffersAndPollEvents()  {
    glfwSwapBuffers(this->w);
    glfwPollEvents();
}

float Window::getTime() {
	return std::chrono::duration<double, std::deca>(std::chrono::high_resolution_clock::now() - startTime).count();
}

int Window::gladLoader() {
    glfwSetInputMode(this->w, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n";
        return 0;
    }
    return 1;
}

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void Window::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (!useGUI) {
		if (firstMouse) {
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;

		lastX = xpos;
		lastY = ypos;
		if (!mouseCursorDisabled)
			Window::camera->ProcessMouseMovement(xoffset, yoffset);
	}
}

void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if (!useGUI) {
		Window::camera->ProcessMouseScroll(yoffset);
	}
}
