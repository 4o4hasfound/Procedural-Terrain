#include "TerrainEngine.h"

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        // positions
        glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
        glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
        glm::vec3 pos3(1.0f, -1.0f, 0.0f);
        glm::vec3 pos4(1.0f, 1.0f, 0.0f);
        // texture coordinates
        glm::vec2 uv1(0.0f, 1.0f);
        glm::vec2 uv2(0.0f, 0.0f);
        glm::vec2 uv3(1.0f, 0.0f);
        glm::vec2 uv4(1.0f, 1.0f);
        // normal vector
        glm::vec3 nm(0.0f, 0.0f, 1.0f);

        // calculate tangent/bitangent vectors of both triangles
        glm::vec3 tangent1, bitangent1;
        glm::vec3 tangent2, bitangent2;
        // triangle 1
        // ----------
        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

        // triangle 2
        // ----------
        edge1 = pos3 - pos1;
        edge2 = pos4 - pos1;
        deltaUV1 = uv3 - uv1;
        deltaUV2 = uv4 - uv1;

        f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);


        bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);


        float quadVertices[] = {
            // positions            // normal         // texcoords  // tangent                          // bitangent
            pos1.x, pos1.y, pos1.z, uv1.x, uv1.y,
            pos2.x, pos2.y, pos2.z, uv2.x, uv2.y,
            pos3.x, pos3.y, pos3.z, uv3.x, uv3.y,

            pos1.x, pos1.y, pos1.z, uv1.x, uv1.y,
            pos3.x, pos3.y, pos3.z, uv3.x, uv3.y,
            pos4.x, pos4.y, pos4.z, uv4.x, uv4.y
        };
        // configure plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
namespace TerrainEngine {
    GLFWwindow* window;

    const unsigned int SCR_WIDTH = 2560;
    const unsigned int SCR_HEIGHT = 1440;

    float lastX = SCR_WIDTH / 2.0f;
    float lastY = SCR_HEIGHT / 2.0f;
    bool firstMouse = true;

    bool wireFrame = false;
    bool f2Pressed = false;
    bool tabPressed = false;

    const int CHUNK_SIZE = 50;
    const int TERRAIN_WIDTH = 40;
    const int TERRAIN_HEIGHT = 40;
    int TerrainSize[2] = { TERRAIN_WIDTH,TERRAIN_HEIGHT };

    int screenWidth;
    int screenHeight;

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    glm::vec3 lightDir = { 0.5f, -1.0f, 0.0f };

    bool playerInf = false;
    bool gui = false;
    bool escPressed = false;

    bool useTextures = true;
    bool useSkyBox = true;
    bool useNormalMapping = true;
    bool useParallaxMapping = true;
    bool useLighting = true;
    bool useColoring = true;
    bool drawWater = true;
    bool usePhysics = true;
    float heightScale = 400.0f;
    float waterHeight = -0.1f;
    float grassHeight = 0.05f;
    float snowHeight = 0.2f;
    float speed;

    float teleportTo[3] = { 0.0f,0.0f,0.0f };

    Camera camera;
    Player player;
    glm::vec3 lastPos;
    Terrain terrain;
    Skybox skybox;

    void Init() {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Terrain Generation", glfwGetPrimaryMonitor(), NULL);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return;
        }
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return;
        }
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 42.7f, -10.0f);
        speed = camera.MovementSpeed;
        player = Player(&camera, &terrain, 6.0f, -5);
        lastPos = camera.Position;
        terrain = Terrain(&camera, SCR_WIDTH, SCR_HEIGHT, TERRAIN_WIDTH, TERRAIN_HEIGHT, CHUNK_SIZE);
        skybox.Init();

        GLint viewport[4];

        glGetIntegerv(GL_VIEWPORT, viewport);
        screenWidth = viewport[2];
        screenHeight = viewport[3];

        teleportTo[0] = camera.Position.x;
        teleportTo[1] = camera.Position.y;
        teleportTo[2] = camera.Position.z;
        
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    void Run() {
        while (!glfwWindowShouldClose(window))
        {
            terrain.Generate();
            float currentFrame = static_cast<float>(glfwGetTime());
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            if(usePhysics) player.step(deltaTime);
            processInput(window);

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 3000.0f);
            glm::mat4 view = camera.GetViewMatrix();

            if (useSkyBox) skybox.Draw(projection, view);
            terrain.Draw(projection, view);

            if (gui) {
                if (playerInf) RenderPlayerInf();
                else RenderSettings();
            }
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwTerminate();
        return;
    }

    void ComputeShaderTest() {
        ComputeShader computeShader("shaders/HeightMap.comp");
        Shader screenQuad("shaders/debug.vert", "shaders/debug.frag");
        unsigned heightMap = computeShader.genTexture(10000, 10000);
        computeShader.use();
        computeShader.setVec2("pos", 432.0, 5522.0);
        computeShader.setInt("octaves", 13);
        computeShader.setFloat("gDispFactor", 1.0);
        computeShader.setFloat("freq", 0.01);
        computeShader.setFloat("power", 1.0);
        computeShader.Compute(10000, 10000);
        while (!glfwWindowShouldClose(window))
        {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            screenQuad.use();
            screenQuad.setInt("tex", 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, heightMap);
            renderQuad();

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwTerminate();
        return;
    }

    void RenderSettings() {
        ImGui::Begin("Settings");
        ImGui::SetWindowFontScale(3.0);
        ImGui::Checkbox("useTextures", &terrain.useTextures);
        ImGui::Checkbox("useSkyBox", &useSkyBox);
        ImGui::Checkbox("useNormalMapping", &terrain.useNormalMapping);
        ImGui::Checkbox("useLighting", &terrain.useLighting);
        ImGui::Checkbox("useColoring", &terrain.useColoring);
        ImGui::NewLine();
        ImGui::Checkbox("drawWater", &terrain.drawWater);
        ImGui::NewLine();
        ImGui::Checkbox("usePhysics", &usePhysics);
        ImGui::NewLine();
        ImGui::SliderFloat("Height Scale", &terrain.heightScale, 1.0f, 1000.0f);
        ImGui::SliderFloat("Snow Level", &terrain.snowLevel, terrain.grassLevel, 1.0f);
        ImGui::SliderFloat("Grass Level", &terrain.grassLevel, terrain.waterLevel, terrain.snowLevel);
        ImGui::SliderFloat("Water Level", &terrain.waterLevel, -1.0f, terrain.grassLevel);
        ImGui::NewLine();
        ImGui::Text("Resize terrain size");
        ImGui::InputInt2("##dev9", &TerrainSize[0]);
        if (ImGui::Button("Resize")) {
            terrain.Resize(TerrainSize[0], TerrainSize[1]);
        }
        ImGui::NewLine();
        if (ImGui::Button("Player information")) {
            playerInf = true;
        }
        if (ImGui::Button("Quit")) {
            glfwSetWindowShouldClose(window, true);
        }
        ImGui::End();
    }

    void RenderPlayerInf()  {
        ImGui::Begin("Player information");
        ImGui::SetWindowFontScale(3.0);
        ImGui::Text("Position:");
        ImGui::Text(("X: " + std::to_string(std::round(camera.Position.x * 10) / 10.0) +
            ", Y: " + std::to_string(std::round(camera.Position.y * 10) / 10.0) +
            ", Z: " + std::to_string(std::round(camera.Position.z * 10) / 10.0)).c_str());
        ImGui::Text("Rotation:");
        ImGui::Text(("X: " + std::to_string(std::round(camera.Yaw * 10) / 10.0) + ", Y: " + std::to_string(std::round(camera.Pitch*10)/10.0)).c_str());
        ImGui::NewLine();
        ImGui::SliderFloat("Speed", &camera.MovementSpeed, 0.1f, 100.0f);
        ImGui::NewLine();
        ImGui::Text("Teleport:");
        ImGui::Text("Position (X, Y, Z): "); ImGui::SameLine();
        ImGui::InputFloat3("##dev9", &teleportTo[0]);
        if (ImGui::Button("TP")) {
            camera.Position.x = teleportTo[0];
            camera.Position.y = teleportTo[1];
            camera.Position.z = teleportTo[2];
            teleportTo[0] = camera.Position.x; teleportTo[1] = camera.Position.y; teleportTo[2] = camera.Position.z;
        }
        if (ImGui::Button("Settings")) playerInf = false;
        ImGui::End();
    }

    void processInput(GLFWwindow* window) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            if (!escPressed) {
                escPressed = true;
                if (gui) {
                    gui = false;
                    playerInf = false;
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    camera.lock = false;
                }
                else {
                    gui = true;
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    camera.lock = true;
                    teleportTo[0] = camera.Position.x;
                    teleportTo[1] = camera.Position.y;
                    teleportTo[2] = camera.Position.z;
                    TerrainSize[0] = terrain.getTerrainWidth();
                    TerrainSize[1] = terrain.getTerrainHeight();
                }
            }
        }
        else {
            escPressed = false;
        }

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
            camera.run = 1;
        else
            camera.run = 0;
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE) {
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                camera.ProcessKeyboard(FORWARD, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                camera.ProcessKeyboard(BACKWARD, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                camera.ProcessKeyboard(LEFT, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                camera.ProcessKeyboard(RIGHT, deltaTime);
            if (glm::abs(camera.Position.x - lastPos.x) >= CHUNK_SIZE || glm::abs(camera.Position.z - lastPos.z) >= CHUNK_SIZE) {
                lastPos = glm::vec3(glm::floor(camera.Position.x / CHUNK_SIZE) * CHUNK_SIZE, camera.Position.y, glm::floor(camera.Position.z / CHUNK_SIZE) * CHUNK_SIZE);
                terrain.Update(lastPos);
            }
        }
        else {
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                lightDir.x += 0.001;
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                lightDir.x -= 0.001;
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                lightDir.y += 0.001;
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                lightDir.y -= 0.001;
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            if (usePhysics) player.jump();
            else camera.ProcessKeyboard(UP, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            camera.ProcessKeyboard(DOWN, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) {
            if (!f2Pressed) {
                f2Pressed = true;

                BYTE* pixels = new BYTE[3 * screenWidth * screenHeight];

                glReadPixels(0, 0, screenWidth, screenHeight, GL_BGR, GL_UNSIGNED_BYTE, pixels);

                // Convert to FreeImage format & save to file
                FIBITMAP* image = FreeImage_ConvertFromRawBits(pixels, screenWidth, screenHeight, 3 * screenWidth, 24, 0x0000FF, 0xFF0000, 0x00FF00, false);

                auto t = std::time(nullptr);
                auto tm = *std::localtime(&t);
                std::ostringstream oss;
                oss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
                std::string str = oss.str();
                if (FreeImage_Save(FIF_BMP, image, ("images/" + str + ".bmp").c_str(), 0)) {
                    std::cout << "Successfully saved image to " << (str + ".bmp\n");
                }
                else {
                    std::cout << "Failed to save image to " << (str + ".bmp\n");
                }
                // Free resources
                FreeImage_Unload(image);
                delete[] pixels;
            }
        }
        else {
            f2Pressed = false;
        }
        if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
            if (!tabPressed) {
                tabPressed = true;
                if (wireFrame) {
                    wireFrame = false;
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }
                else {
                    wireFrame = true;
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                }
            }
        }
        else {
            tabPressed = false;
        }
    }

    void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    }

    void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;

        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
    }

    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
        camera.ProcessMouseScroll(static_cast<float>(yoffset));
    }
}
