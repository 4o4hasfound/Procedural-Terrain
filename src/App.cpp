#include "App.h"

App::App() {
	int success;
	window = new Window(success, Window::SCR_WIDTH, Window::SCR_HEIGHT);
	if (!success) {
		std::cout << "Failed to init GLFW and GLAD\n";
		return;
	}

    srand(time(NULL));
    camera = new Camera(glm::vec3(rand() % 100000, 1000, rand() % 100000));
    Window::camera = camera;
    Terrain::camera = camera;

    terrain             = new Terrain(50, 560);
    skybox              = new Skybox();
    gui                 = new GUI(this, window, terrain, camera);
    framebufferOBJ      = new FrameBufferOBJ(1980, 1080);
    waterReflection      = new FrameBufferOBJ(990, 540);
    waterRefraction     = new FrameBufferOBJ(990, 540);
    shadow              = new FrameBufferOBJ(1980, 1080);
    screenShader        = new ScreenShader("shaders/Screen/screen.frag");
    water               = new Water(terrain->getChunkSize() * terrain->getTerrainSize() * 2);
    sun                 = new Sun();

    skybox->Init();
    terrain->SetupTextures();
}

App::~App() {
    delete gui;
    delete camera;
    delete terrain;
    delete water;
    delete sun;
    delete skybox;
    delete framebufferOBJ;
    delete waterReflection;
    delete waterRefraction;
    delete screenShader;
    delete window;
}

void App::Run() {
	while (window->continueLoop()) {
		auto t_start = std::chrono::high_resolution_clock::now();

        Render();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (Window::useGUI) {
            gui->Render();
        }
		terrain->Update();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        auto t_end = std::chrono::high_resolution_clock::now();
        double dt = std::chrono::duration<double, std::milli>(t_end - t_start).count();

        window->processInput(dt);

        glfwSwapInterval(1);

		window->swapBuffersAndPollEvents();
	}
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    window->terminate();
}
glm::mat4 App::ProjInfiniteFarPlane(float fov, float WHratio, float near, float far) {
	const float tanHalfFovy = tan(fov / 2.0);

	glm::mat4 Result(0);
	float e = 1 / tanHalfFovy;
	Result[0][0] = e / WHratio;
	Result[1][1] = e;
	Result[2][2] = -0.999999999;
	Result[2][3] = -1.0;
	Result[3][2] = -0.01;
	return Result;

}

void App::GenSrcShot() {
    BYTE* pixels = new BYTE[3 * framebufferOBJ->W * framebufferOBJ->H];

    glReadPixels(0, 0, framebufferOBJ->W, framebufferOBJ->H, GL_BGR, GL_UNSIGNED_BYTE, pixels);

    // Convert to FreeImage format & save to file
    FIBITMAP* image = FreeImage_ConvertFromRawBits(pixels, framebufferOBJ->W, framebufferOBJ->H, 3 * framebufferOBJ->W, 24, 0x0000FF, 0xFF0000, 0x00FF00, false);

    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
    std::string str = oss.str();
    if (FreeImage_Save(FIF_BMP, image, ("image/" + str + ".bmp").c_str(), 0)) {
        std::cout << "Successfully saved image to " << (str + ".bmp\n");
    }
    else {
        std::cout << "Failed to save image to " << (str + ".bmp\n");
    }
    // Free resources
    FreeImage_Unload(image);
    delete[] pixels;
}

void App::RenderScene(glm::vec4 plane, bool doTess) {
    window->clearScreen(0.0f, 0.0f, 0.0f, 1.0f);

    glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)Window::SCR_WIDTH / (float)Window::SCR_HEIGHT, nearPlane, farPlane);
    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 id = glm::mat4(1.0);
    glm::mat4 model = glm::scale(id, glm::vec3(1.0));

    Shader* shader;

    //  -------------------- Sun -------------------- 
    shader = sun->getShad();
    shader->use();
    shader->setVec3("cameraPos", camera->Position);
    shader->setVec3("lightDir", lightDir[0], lightDir[1], lightDir[2]);
    shader->setFloat("screenRatio", (float)Window::SCR_WIDTH / (float)Window::SCR_HEIGHT);
    shader->setMat4("proj", projection);
    shader->setMat4("view", view);
    sun->Draw();

    //  -------------------- Terrain -------------------- 
    shader = terrain->getShad();
    shader->use();
    shader->setVec3("cameraPos", camera->Position);
    shader->setMat4("proj", projection);
    shader->setMat4("view", view);
    shader->setMat4("model", model);
    shader->setFloat("height", height);
    shader->setVec4("plane", plane);
    shader->setBool("doTess", doTess);
    shader->setVec3("lightDir", lightDir[0], lightDir[1], lightDir[2]);
    shader->setVec3("lightColor", glm::pow(lightColor[0], gamma), glm::pow(lightColor[1], gamma), glm::pow(lightColor[2], gamma));
    shader->setFloat("waterHeight", Water::height);
    shader->setFloat("gamma", gamma);
    terrain->Draw();

    //  -------------------- SkyBox -------------------- 
    shader = skybox->getShad();
    shader->use();
    shader->setVec3("lightColor", glm::pow(lightColor[0], gamma), glm::pow(lightColor[1], gamma), glm::pow(lightColor[2], gamma));
    skybox->Draw(projection, view, gamma);
}

void App::RenderFinalScene() {
    window->clearScreen(0.0f, 0.0f, 0.0f, 1.0f);

    glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)Window::SCR_WIDTH / (float)Window::SCR_HEIGHT, nearPlane, farPlane);
    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 id = glm::mat4(1.0);
    glm::mat4 model = glm::scale(id, glm::vec3(1.0));

    Shader* shader;

    //  -------------------- Sun -------------------- 
    shader = sun->getShad();
    shader->use();
    shader->setVec3("cameraPos", camera->Position);
    shader->setVec3("lightDir", lightDir[0], lightDir[1], lightDir[2]);
    shader->setFloat("screenRatio", (float)Window::SCR_WIDTH / (float)Window::SCR_HEIGHT);
    shader->setMat4("proj", projection);
    shader->setMat4("view", view);
    sun->Draw();

    //  -------------------- Water -------------------- 
    shader = water->getShad();
    shader->use();
    shader->setMat4("projection", projection);
    shader->setMat4("view", view);
    shader->setVec3("cameraPos", camera->Position);
    shader->setVec3("lightDir", lightDir[0], lightDir[1], lightDir[2]);
    shader->setFloat("near", nearPlane);
    shader->setFloat("far", farPlane);
    shader->setFloat("gamma", gamma);
    water->Draw(waterReflection->tex, waterRefraction->tex, waterRefraction->depthTex, window->getTime());

    //  -------------------- Terrain -------------------- 
    shader = terrain->getShad();
    shader->use();
    shader->setVec3("cameraPos", camera->Position);
    shader->setMat4("proj", projection);
    shader->setMat4("view", view);
    shader->setMat4("model", model);
    shader->setFloat("height", 1000);
    shader->setVec4("plane", 0.0, 0.0, 0.0, 0.0);
    shader->setBool("doTess", true);
    shader->setVec3("lightDir", lightDir[0], lightDir[1], lightDir[2]);
    shader->setVec3("lightColor", glm::pow(lightColor[0], gamma), glm::pow(lightColor[1], gamma), glm::pow(lightColor[2], gamma));
    shader->setFloat("waterHeight", Water::height);
    shader->setFloat("gamma", gamma);
    shader->setBool("usePar", usePar);
    shader->setFloat("height", height);
    terrain->Draw();

    //  -------------------- SkyBox -------------------- 
    shader = skybox->getShad();
    shader->use();
    shader->setVec3("lightColor", glm::pow(lightColor[0], gamma), glm::pow(lightColor[1], gamma), glm::pow(lightColor[2], gamma));
    skybox->Draw(projection, view, gamma);
}

void App::Render() {
    window->clearScreen(0.0f, 0.0f, 0.0f, 1.0f);

    float dis = 2 * (camera->Position.y - Water::height);
    Shader* shader;

    waterReflection->bind();
    camera->Position.y -= dis;
    camera->invertPitch();
    RenderScene(glm::vec4(0.0, 1.0, 0.0, -Water::height + 10.0), false);

    waterRefraction->bind();
    camera->Position.y += dis;
    camera->invertPitch();
    RenderScene(glm::vec4(0.0, -1.0, 0.0, Water::height), false);

    framebufferOBJ->bind();
    glPolygonMode(GL_FRONT_AND_BACK, window->isWireframeActive() ? GL_LINE : GL_FILL);
    RenderFinalScene();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (Window::genSrcShot) {
        Window::genSrcShot = false;
        GenSrcShot();
    }

    unbindCurrentFrameBuffer();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, framebufferOBJ->tex);
    shader = screenShader->getShader();
    shader->use();
    shader->setInt("text", 0);
    shader->setFloat("gamma", gamma);
    shader->setFloat("exposure", exposure);
    screenShader->Draw();
}


GUI::GUI() {

}

GUI::GUI(App* app, Window* window, Terrain* terrain, Camera* camera)
    : app(app), window(window), terrain(terrain), camera(camera) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window->getWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

GUI::~GUI() {

}

void GUI::Render() {
    switch (state) {
    case GUIState::MainMenu:
        teleportTo[0] = camera->Position.x; teleportTo[1] = camera->Position.y; teleportTo[2] = camera->Position.z;
        ImGui::Begin("Settings");
        ImGui::SetWindowFontScale(fontSize);
        if (ImGui::Button("Window settings")) {
            state = GUIState::Window;
        }
        if (ImGui::Button("World settings")) {
            state = GUIState::Settings;
        }
        if (ImGui::Button("Player settings")) {
            state = GUIState::Player;
        }
        if (ImGui::Button("Exit")) {
            glfwSetWindowShouldClose(window->w, true);
        }
        ImGui::End();
        break;
    case GUIState::Window:
        ImGui::Begin("Window settings");
        ImGui::SetWindowFontScale(fontSize);
        ImGui::Checkbox("VSync", &app->VSync);
        ImGui::SliderFloat("Gamma", &app->gamma, 1.0, 5.0);
        ImGui::SliderFloat("Exposure", &app->exposure, 0.01, 10.0);
        ImGui::NewLine();
        if (ImGui::Button("Back")) {
            state = GUIState::MainMenu;
        }
        ImGui::End();
        break;
    case GUIState::Settings:
        ImGui::Begin("World settings");
        ImGui::SetWindowFontScale(fontSize);
        ImGui::Checkbox("Use textures: ", &app->useTextures);
        ImGui::Checkbox("Use lightinig: ", &app->useLighting);
        ImGui::Checkbox("Use normal mapping: ", &app->useNormalMapping);
        ImGui::Checkbox("Use fog: ", &app->useFog);
        ImGui::Checkbox("Use skybox: ", &app->useSkyBox);
        ImGui::SliderFloat3("Light color: ", app->lightColor, 0.0, 10.0);
        ImGui::SliderFloat3("Light direction: ", app->lightDir, -1.0, 1.0);
        ImGui::SliderFloat("Height: ", &app->height, 0.0, 30000.0);
        ImGui::SliderFloat("Water Height", &Water::height, 0.0, 10000.0);
        ImGui::Checkbox("usePar", &app->usePar);
        ImGui::SliderFloat("height_scale", &app->heightScale, 0.0, 1.0);
        ImGui::NewLine();
        if (ImGui::Button("Back")) {
            state = GUIState::MainMenu;
        }
        ImGui::End();
        break;
    case GUIState::Player:
        ImGui::Begin("Player settings");
        ImGui::SetWindowFontScale(fontSize);
        ImGui::Text("Position:");
        ImGui::Text(("X: " + std::to_string(camera->Position.x) +
            ", Y: " + std::to_string(camera->Position.y) +
            ", Z: " + std::to_string(camera->Position.z)).c_str());
        ImGui::Text("Rotation:");
        ImGui::Text(("X: " + std::to_string(camera->Yaw) + ", Y: " + std::to_string(camera->Pitch)).c_str());
        ImGui::NewLine();
        ImGui::SliderFloat("Speed", &camera->MovementSpeed, 0.0f, 2000.0f);
        ImGui::NewLine();
        ImGui::Text("Teleport:");
        ImGui::Text("Position (X, Y, Z): "); ImGui::SameLine();
        ImGui::InputFloat3("##dev9", &teleportTo[0]);
        if (ImGui::Button("TP")) {
            camera->Position.x = teleportTo[0];
            camera->Position.y = teleportTo[1];
            camera->Position.z = teleportTo[2];
            teleportTo[0] = camera->Position.x; teleportTo[1] = camera->Position.y; teleportTo[2] = camera->Position.z;
        }
        ImGui::NewLine();
        if (ImGui::Button("Back")) {
            state = GUIState::MainMenu;
        }
        ImGui::End();

    }
}

void GUI::Update()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}
void GUI::Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

