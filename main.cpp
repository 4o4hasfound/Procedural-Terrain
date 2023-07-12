//#include <iostream>
//#include <chrono>
//#include <memory>
//#include <stdlib.h>
//#include <time.h>
//#include <GLM/glm.hpp>
//#include <FreeImage.h>
//#include "../imgui/imgui.h"
//#include "../imgui/imgui_impl_glfw.h"
//#include "../imgui/imgui_impl_opengl3.h"
//#include "Window.h"
//#include "Camera.h"
//#include "Plane.h"
//#include "Terrain.h"
//#include "Skybox.h"
//#include "FrameBufferObject.h"
//#include "ScreenShader.h"
//#include "Water.h"
//
//enum class GUIState {
//	MainMenu,
//	Window,
//	Settings,
//	Player
//};
//
//class GUII {
//public:
//	GUII();
//	GUII(Window* window, Terrain* terrain, Camera* camera);
//	~GUII();
//	void Render();
//	void Update();
//	void Shutdown();
//
//private:
//	Window* window;
//	Terrain* terrain;
//	Camera* camera;
//	GUIState state = GUIState::MainMenu;
//	float teleportTo[3];
//	float fontSize = 3.0;
//};
//
//static bool Gui;
//
//bool VSync = true;
//bool useTextures = true;
//bool useLighting = true;
//bool useNormalMapping = true;
//bool useFog = true;
//bool useSkyBox = true;
//float height = 10000.0;
//float gamma = 1.0;
//float exposure = 1.0;
//float lightColor[3] = { 1.0, 1.0, 1.0 };
//float lightDir[3] = { 1.0, 0.7, 1.0 };
//bool usePar = true;
//float heightScale = 0.1;
//void InitGui();
//void RenderGui();
//void GenSrcShot();
//GUII* gui;
//Window* window;
//Camera* camera;
//Terrain* terrain;
//Water* water;
//Shader* shader;
//Shader* depthShader;
//Skybox* skybox;
//Shader* sunShader;
//FrameBufferOBJ* framebufferOBJ;
//FrameBufferOBJ* waterReflection;
//FrameBufferOBJ* waterRefraction;
//FrameBufferOBJ* shadow;
//ScreenShader* screenShader;
//GLuint vao;
//float nearPlane = 1.0, farPlane = 100000.0;
//void RenderScene(glm::vec4 plane, bool doTess);
//void RenderFinalScene();
//int main1() {
//    int success;
//    window = new Window(success, Window::SCR_WIDTH, Window::SCR_HEIGHT);
//    if (!success) {
//        std::cout << "Failed to init GLFW and GLAD\n";
//        return 0;
//    }
//    srand(time(NULL));
//    camera = new Camera(glm::vec3(rand() % 100000, 1000, rand() % 100000));
//    Window::camera = camera;
//    Terrain::camera = camera;
//    terrain = new Terrain(45, 500);
//    skybox = new Skybox();
//    gui = new GUII(window, terrain, camera);
//    skybox->Init();
//    terrain->SetupTextures();
//    shader = terrain->getShad();
//    framebufferOBJ = new FrameBufferOBJ(1980, 1080);
//    waterReflection = new FrameBufferOBJ(990, 540);
//    waterRefraction = new FrameBufferOBJ(990, 540);
//    //shadow = new FrameBufferOBJ(1980, 1080);
//    screenShader = new ScreenShader("shaders/screen.frag");
//    //depthShader = new Shader();
//    //depthShader->AttachShader("shaders/simpleDepthShader.vert", VERT)
//    //    ->AttachShader("shaders/simpleDepthShader.frag", FRAG)
//    //    ->LinkProgram();
//    water = new Water(terrain->getChunkSize() * terrain->getTerrainSize() * 2);
//    sunShader = new Shader();
//    sunShader->AttachShader("shaders/sun.vert", VERT)
//        ->AttachShader("shaders/sun.frag", FRAG)
//        ->AttachShader("shaders/sun.geom", GEOM)
//        ->LinkProgram();
//    GLuint vbo;
//    glGenBuffers(1, &vbo);
//
//    float points[] = {
//        0.0, 0.0
//    };
//
//    glBindBuffer(GL_ARRAY_BUFFER, vbo);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
//    glGenVertexArrays(1, &vao);
//    glBindVertexArray(vao);
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
//    glEnable(GL_CULL_FACE);
//    while (window->continueLoop()) {
//        auto t_start = std::chrono::high_resolution_clock::now();
//        window->clearScreen(0.075f, 0.133f, 0.173f, 1.0f);
//        auto t_end = std::chrono::high_resolution_clock::now();
//        float dis = 2 * (camera->Position.y - Water::height);
//        //glm::mat4 lightProjection = glm::ortho(1.0, 10000.0, 1.0, 10000.0, 1.0, 10000.0);
//        //glm::mat4 lightView = glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f),
//        //    glm::vec3(0.0f, 0.0f, 0.0f),
//        //    glm::vec3(0.0f, 1.0f, 0.0f));
//        //glm::mat4 lightSpaceMatrix = lightProjection * lightView;
//        waterReflection->bind();
//        camera->Position.y -= dis;
//        camera->invertPitch();
//        RenderScene(glm::vec4(0.0, 1.0, 0.0, -Water::height + 30.0f), true);
//        waterRefraction->bind();
//        camera->Position.y += dis;
//        camera->invertPitch();
//        RenderScene(glm::vec4(0.0, -1.0, 0.0, Water::height), true);
//        //shadow->bind();
//        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        //depthShader->use();
//        //depthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
//        //Shader* terrainShad = terrain->getShad();
//        //terrainShad->use();
//        framebufferOBJ->bind();
//        glPolygonMode(GL_FRONT_AND_BACK, window->isWireframeActive() ? GL_LINE : GL_FILL);
//        RenderFinalScene();
//        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//
//        if (Window::genSrcShot) {
//            Window::genSrcShot = false;
//            GenSrcShot();
//        }
//        unbindCurrentFrameBuffer();
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, framebufferOBJ->tex);
//        Shader* shad = screenShader->getShader();
//        shad->use();
//        shad->setInt("text", 0);
//        shad->setFloat("gamma", gamma);
//        shad->setFloat("exposure", exposure);
//        screenShader->Draw();
//        ImGui_ImplOpenGL3_NewFrame();
//        ImGui_ImplGlfw_NewFrame();
//        ImGui::NewFrame();
//
//        if (Window::useGUI) {
//            gui->Render();
//        }
//        terrain->Update();
//        double elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end - t_start).count();
//        window->processInput(elapsed_time_ms);
//        glfwSwapInterval(1);
//        ImGui::Render();
//        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//        window->swapBuffersAndPollEvents();
//    }
//    ImGui_ImplOpenGL3_Shutdown();
//    ImGui_ImplGlfw_Shutdown();
//    ImGui::DestroyContext();
//    window->terminate();
//    return 0;
//}
//
//void GenSrcShot() {
//    BYTE* pixels = new BYTE[3 * framebufferOBJ->W * framebufferOBJ->H];
//
//    glReadPixels(0, 0, framebufferOBJ->W, framebufferOBJ->H, GL_BGR, GL_UNSIGNED_BYTE, pixels);
//
//    // Convert to FreeImage format & save to file
//    FIBITMAP* image = FreeImage_ConvertFromRawBits(pixels, framebufferOBJ->W, framebufferOBJ->H, 3 * framebufferOBJ->W, 24, 0x0000FF, 0xFF0000, 0x00FF00, false);
//
//    auto t = std::time(nullptr);
//    auto tm = *std::localtime(&t);
//    std::ostringstream oss;
//    oss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
//    std::string str = oss.str();
//    if (FreeImage_Save(FIF_BMP, image, ("image/" + str + ".bmp").c_str(), 0)) {
//        std::cout << "Successfully saved image to " << (str + ".bmp\n");
//    }
//    else {
//        std::cout << "Failed to save image to " << (str + ".bmp\n");
//    }
//    // Free resources
//    FreeImage_Unload(image);
//    delete[] pixels;
//}
//
//void RenderScene(glm::vec4 plane, bool doTess) {
//    glEnable(GL_CULL_FACE);
//    glClearColor(0.075f, 0.133f, 0.173f, 1.0f);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)Window::SCR_WIDTH / (float)Window::SCR_HEIGHT, nearPlane, farPlane);
//    glm::mat4 view = camera->GetViewMatrix();
//    glm::mat4 id = glm::mat4(1.0);
//    glm::mat4 model = glm::scale(id, glm::vec3(1.0));
//    sunShader->use();
//    sunShader->setVec3("cameraPos", camera->Position);
//    sunShader->setVec3("lightDir", lightDir[0], lightDir[1], lightDir[2]);
//    sunShader->setFloat("screenRatio", (float)Window::SCR_WIDTH / (float)Window::SCR_HEIGHT);
//    sunShader->setMat4("proj", projection);
//    sunShader->setMat4("view", view);
//    glBindVertexArray(vao);
//    glDrawArrays(GL_POINTS, 0, 1);
//    glEnable(GL_CLIP_DISTANCE0);
//    shader = terrain->getShad();
//    shader->use();
//    shader->setVec3("cameraPos", camera->Position);
//    shader->setMat4("proj", projection);
//    shader->setMat4("view", view);
//    shader->setMat4("model", model);
//    shader->setFloat("height", height);
//    shader->setVec4("plane", plane);
//    shader->setBool("doTess", doTess);
//    shader->setVec3("lightDir", lightDir[0], lightDir[1], lightDir[2]);
//    shader->setVec3("lightColor", glm::pow(lightColor[0], gamma), glm::pow(lightColor[1], gamma), glm::pow(lightColor[2], gamma));
//    shader->setFloat("waterHeight", Water::height);
//    shader->setFloat("gamma", gamma);
//    terrain->Draw();
//    glDisable(GL_CLIP_DISTANCE0);
//    Shader* skyboxShad = skybox->getShad();
//    skyboxShad->use();
//    skyboxShad->setVec3("lightColor", glm::pow(lightColor[0], gamma), glm::pow(lightColor[1], gamma), glm::pow(lightColor[2], gamma));
//    skybox->Draw(projection, view, gamma);
//}
//
//void RenderFinalScene() {
//    glClearColor(0.075f, 0.133f, 0.173f, 1.0f);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)Window::SCR_WIDTH / (float)Window::SCR_HEIGHT, nearPlane, farPlane);
//    glm::mat4 view = camera->GetViewMatrix();
//    glm::mat4 id = glm::mat4(1.0);
//    glm::mat4 model = glm::scale(id, glm::vec3(1.0));
//    sunShader->use();
//    sunShader->setVec3("cameraPos", camera->Position);
//    sunShader->setVec3("lightDir", lightDir[0], lightDir[1], lightDir[2]);
//    sunShader->setFloat("screenRatio", (float)Window::SCR_WIDTH / (float)Window::SCR_HEIGHT);
//    sunShader->setMat4("proj", projection);
//    sunShader->setMat4("view", view);
//    glBindVertexArray(vao);
//    glDrawArrays(GL_POINTS, 0, 1);
//    glDisable(GL_CULL_FACE);
//    Shader* shad = water->getShad();
//    shad->use();
//    shad->setMat4("projection", projection);
//    shad->setMat4("view", view);
//    shad->setVec3("cameraPos", camera->Position);
//    shad->setVec3("lightDir", lightDir[0], lightDir[1], lightDir[2]);
//    shad->setFloat("near", nearPlane);
//    shad->setFloat("far", farPlane);
//    shad->setFloat("gamma", gamma);
//    water->Draw(waterReflection->tex, waterRefraction->tex, waterRefraction->depthTex, window->getTime());
//    glEnable(GL_CULL_FACE);
//    glEnable(GL_CLIP_DISTANCE0);
//    shader = terrain->getShad();
//    shader->use();
//    shader->setVec3("cameraPos", camera->Position);
//    shader->setMat4("proj", projection);
//    shader->setMat4("view", view);
//    shader->setMat4("model", model);
//    shader->setFloat("height", height);
//    shader->setVec4("plane", 0.0, 0.0, 0.0, 0.0);
//    shader->setBool("doTess", true);
//    shader->setVec3("lightDir", lightDir[0], lightDir[1], lightDir[2]);
//    shader->setVec3("lightColor", glm::pow(lightColor[0], gamma), glm::pow(lightColor[1], gamma), glm::pow(lightColor[2], gamma));
//    shader->setFloat("waterHeight", Water::height);
//    shader->setFloat("gamma", gamma);
//    shader->setBool("usePar", usePar);
//    shader->setFloat("height_scale", heightScale);
//    terrain->Draw();
//    glDisable(GL_CLIP_DISTANCE0);
//    Shader* skyboxShad = skybox->getShad();
//    skyboxShad->use();
//    skyboxShad->setVec3("lightColor", glm::pow(lightColor[0], gamma), glm::pow(lightColor[1], gamma), glm::pow(lightColor[2], gamma));
//    skybox->Draw(projection, view, gamma);
//}
//
//GUII::GUII() {
//
//}
//
//GUII::GUII(Window* window, Terrain* terrain, Camera* camera)
//    : window(window), terrain(terrain), camera(camera) {
//    IMGUI_CHECKVERSION();
//    ImGui::CreateContext();
//    ImGuiIO& io = ImGui::GetIO(); (void)io;
//    ImGui::StyleColorsDark();
//    ImGui_ImplGlfw_InitForOpenGL(window->getWindow(), true);
//    ImGui_ImplOpenGL3_Init("#version 330");
//}
//
//GUII::~GUII() {
//
//}
//
//void GUII::Render() {
//    switch (state) {
//    case GUIState::MainMenu:
//        teleportTo[0] = camera->Position.x; teleportTo[1] = camera->Position.y; teleportTo[2] = camera->Position.z;
//        ImGui::Begin("Settings");
//        ImGui::SetWindowFontScale(fontSize);
//        if (ImGui::Button("Window settings")) {
//            state = GUIState::Window;
//        }
//        if (ImGui::Button("World settings")) {
//            state = GUIState::Settings;
//        }
//        if (ImGui::Button("Player settings")) {
//            state = GUIState::Player;
//        }
//        if (ImGui::Button("Exit")) {
//            glfwSetWindowShouldClose(window->w, true);
//        }
//        ImGui::End();
//        break;
//    case GUIState::Window:
//        ImGui::Begin("Window settings");
//        ImGui::SetWindowFontScale(fontSize);
//        ImGui::Checkbox("VSync", &VSync);
//        ImGui::SliderFloat("Gamma", &gamma, 1.0, 5.0);
//        ImGui::SliderFloat("Exposure", &exposure, 0.01, 10.0);
//        ImGui::NewLine();
//        if (ImGui::Button("Back")) {
//            state = GUIState::MainMenu;
//        }
//        ImGui::End();
//        break;
//    case GUIState::Settings:
//        ImGui::Begin("World settings");
//        ImGui::SetWindowFontScale(fontSize);
//        ImGui::Checkbox("Use textures: ", &useTextures);
//        ImGui::Checkbox("Use lightinig: ", &useLighting);
//        ImGui::Checkbox("Use normal mapping: ", &useNormalMapping);
//        ImGui::Checkbox("Use fog: ", &useFog);
//        ImGui::Checkbox("Use skybox: ", &useSkyBox);
//        ImGui::SliderFloat3("Light color: ", lightColor, 0.0, 10.0);
//        ImGui::SliderFloat3("Light direction: ", lightDir, -1.0, 1.0);
//        ImGui::SliderFloat("Height: ", &height, 0.0, 10000.0);
//        ImGui::SliderFloat("Water Height", &Water::height, 0.0, 10000.0);
//        ImGui::Checkbox("usePar", &usePar);
//        ImGui::SliderFloat("height_scale", &heightScale, 0.0, 1.0);
//        ImGui::NewLine();
//        if (ImGui::Button("Back")) {
//            state = GUIState::MainMenu;
//        }
//        ImGui::End();
//        break;
//    case GUIState::Player:
//        ImGui::Begin("Player settings");
//        ImGui::SetWindowFontScale(fontSize);
//        ImGui::Text("Position:");
//        ImGui::Text(("X: " + std::to_string(camera->Position.x) +
//            ", Y: " + std::to_string(camera->Position.y) +
//            ", Z: " + std::to_string(camera->Position.z)).c_str());
//        ImGui::Text("Rotation:");
//        ImGui::Text(("X: " + std::to_string(camera->Yaw) + ", Y: " + std::to_string(camera->Pitch)).c_str());
//        ImGui::NewLine();
//        ImGui::SliderFloat("Speed", &camera->MovementSpeed, 0.0f, 2000.0f);
//        ImGui::NewLine();
//        ImGui::Text("Teleport:");
//        ImGui::Text("Position (X, Y, Z): "); ImGui::SameLine();
//        ImGui::InputFloat3("##dev9", &teleportTo[0]);
//        if (ImGui::Button("TP")) {
//            camera->Position.x = teleportTo[0];
//            camera->Position.y = teleportTo[1];
//            camera->Position.z = teleportTo[2];
//            teleportTo[0] = camera->Position.x; teleportTo[1] = camera->Position.y; teleportTo[2] = camera->Position.z;
//        }
//        ImGui::NewLine();
//        if (ImGui::Button("Back")) {
//            state = GUIState::MainMenu;
//        }
//        ImGui::End();
//
//    }
//}
//
//void GUII::Update()
//{
//    ImGui_ImplOpenGL3_NewFrame();
//    ImGui_ImplGlfw_NewFrame();
//    ImGui::NewFrame();
//}
//void GUII::Shutdown() {
//    ImGui_ImplOpenGL3_Shutdown();
//    ImGui_ImplGlfw_Shutdown();
//    ImGui::DestroyContext();
//}
//


#include<App.h>
int main() {
    App app;
    app.Run();
    return 0;
}
