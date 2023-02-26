#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <random>
#include <stb_image/stb_image.h>

#include "WindowInfo.h"
#include "ShaderProgram.h"
#include "Model.h"
#include "Vertex.h"
#include "Camera.h"
#include "MathSphere.h"
#include "EnvironmentMap.h"
#include "BVH.h"

void InitGLFW();
void InitGlAD();
void UpdateDeltaTime();
void APIENTRY glDebugOutput(GLenum source,
    GLenum type,
    unsigned int id,
    GLenum severity,
    GLsizei length,
    const char* message,
    const void* userParam);

GLFWwindow* window;

unsigned int WINDOWWIDTH;
unsigned int WINDOWHEIGHT;

float currentTime = 0;
float deltaTime = 0;


int main()
{
    // Setup
    InitGLFW();
    InitGlAD();

    glDebugMessageCallback(glDebugOutput, nullptr);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    glEnable(GL_FRAMEBUFFER_SRGB);

    // Generate framebuffers
    unsigned int rayTraceFBO;
    unsigned int renderTextures[2];

    glGenFramebuffers(1, &rayTraceFBO);
    glGenTextures(2, renderTextures);

    glBindFramebuffer(GL_FRAMEBUFFER, rayTraceFBO);

    for (unsigned int i = 0; i < 2; i++) {
        glBindTexture(GL_TEXTURE_2D, renderTextures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, WINDOWWIDTH, WINDOWHEIGHT, 0, GL_RGB, GL_FLOAT, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, renderTextures[i], 0);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    unsigned int colorAttachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, colorAttachments);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
    // Load shaders
    ShaderProgram rayTraceShader = ShaderProgram("src/Shaders/RenderQuad.vert", "src/Shaders/RayTrace.frag");
    ShaderProgram postProcessShader = ShaderProgram("src/Shaders/RenderQuad.vert", "src/Shaders/PostProcess.frag");

    // Load quad used for rendering
    Model renderQuad = Model("Models/Quad/Quad.fbx");

    // Load environment maps
    std::vector<EnvironmentMap> environmentMaps{
        EnvironmentMap("HDRIs/Shelter.hdr"),
        EnvironmentMap("HDRIs/Farm.hdr"),
        EnvironmentMap("HDRIs/Studio.hdr"),
        EnvironmentMap("HDRIs/Park.hdr"),
        EnvironmentMap("HDRIs/Black.hdr")
    };
    
    // Load camera
    Camera camera = Camera(45, 0.1, 100);
    camera.Translate(glm::vec3(0, 1, 2));

    // Load BVHs
    std::vector<BVH> bvhs{
        BVH::CornellBVH(),
        BVH::CameraBVH(),
        BVH::FruitsBVH(),
        BVH::LanternBVH()
    };
    bvhs[0].SetSSBOs(rayTraceShader);
    bvhs[0].MakeHandlesResident();

    // Set pre-runtime uniforms

    rayTraceShader.SetUnsignedInt("camera.viewPortWidth", WINDOWWIDTH);
    rayTraceShader.SetUnsignedInt("camera.viewPortHeight", WINDOWHEIGHT);

    rayTraceShader.SetUnsignedInt("maxDepth", 10);
    
    std::random_device rand_device;
    std::mt19937 generator(rand_device());
    std::uniform_int_distribution<int> distr(0, 1000000);

    //---------------------------------------------------
    const unsigned int maxSamples = 10000;
    unsigned int currSample = 1;

    glm::mat4 lastCameraModelMatrix = camera.GetModelMatrix();
    glm::mat4 lastCameraProjectionMatrix = camera.GetProjectionMatrix();

    unsigned int bvhsIndex = 0;
    unsigned int enviromentMapsIndex = 0;

    bool mouse1Pressed = false;
    bool mouse2Pressed = false;

    while (!glfwWindowShouldClose(window)) {
        // Setup
        UpdateDeltaTime();
        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
        
        camera.ProcessInput();

        glBindFramebuffer(GL_FRAMEBUFFER, rayTraceFBO);

        if (!mouse1Pressed && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
            bvhs[bvhsIndex].MakeHandlesNotResident();

            bvhsIndex = (bvhsIndex + 1) % bvhs.size();

            bvhs[bvhsIndex].SetSSBOs(rayTraceShader);
            bvhs[bvhsIndex].MakeHandlesResident();

            currSample = 1;
            glClear(GL_COLOR_BUFFER_BIT);

            mouse1Pressed = true;
        }
        if (!mouse2Pressed && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
            enviromentMapsIndex = (enviromentMapsIndex + 1) % environmentMaps.size();

            currSample = 1;
            glClear(GL_COLOR_BUFFER_BIT);

            mouse2Pressed = true;
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE) mouse1Pressed = false;
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_RELEASE) mouse2Pressed = false;

        if (lastCameraModelMatrix != camera.GetModelMatrix() || lastCameraProjectionMatrix != camera.GetProjectionMatrix()) {
            currSample = 1;

            glClear(GL_COLOR_BUFFER_BIT);

            lastCameraModelMatrix = camera.GetModelMatrix();
            lastCameraProjectionMatrix = camera.GetProjectionMatrix();
        }

        if (currSample > maxSamples) continue;
        
        //std::cout << currSample << "/" << maxSamples << std::endl;

        // Ray trace
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderTextures[0]);
        rayTraceShader.SetInt("cumulativeRenderTexture", 0);

        rayTraceShader.SetUnsignedInt("currSample", currSample);

        rayTraceShader.SetUnsignedInt("seed", distr(generator));

        rayTraceShader.SetVec3("camera.position", camera.GetPosition());
        rayTraceShader.SetVec3("camera.xAxis", camera.GetXAxis());
        rayTraceShader.SetVec3("camera.yAxis", camera.GetYAxis());
        rayTraceShader.SetVec3("camera.zAxis", camera.GetZAxis());
        rayTraceShader.SetFloat("camera.focalLength", camera.GetFocalLength());

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, environmentMaps[enviromentMapsIndex].GetEnvironmentMap());
        rayTraceShader.SetInt("environmentMap", 1);

        rayTraceShader.Use();
        renderQuad.Draw();
        ShaderProgram::Unuse();

        currSample++;

        // Post Process
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderTextures[1]);
        postProcessShader.SetInt("colorTexture", 0);

        postProcessShader.Use();
        renderQuad.Draw();
        ShaderProgram::Unuse();
    }
    glfwTerminate();
    return 0;
}
void InitGLFW() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);

    WINDOWWIDTH = videoMode->width;
    WINDOWHEIGHT = videoMode->height;


    window = glfwCreateWindow(WINDOWWIDTH , WINDOWHEIGHT, "Nave", monitor, NULL);

    if (!window) {
        std::cout << "ERROR: Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}
void InitGlAD() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "ERROR: Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
}
void UpdateDeltaTime() {
    float newCurrentTime = glfwGetTime();
    deltaTime = newCurrentTime - currentTime;
    currentTime = newCurrentTime;
}
void APIENTRY glDebugOutput(GLenum source,
    GLenum type,
    unsigned int id,
    GLenum severity,
    GLsizei length,
    const char* message,
    const void* userParam)
{
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
    case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
    case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
    case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
    case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
    case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
    case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
    case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}