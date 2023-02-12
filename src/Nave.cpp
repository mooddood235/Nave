#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

#include "WindowInfo.h"
#include "ShaderProgram.h"
#include "Model.h"
#include "Vertex.h"
#include "Camera.h"
#include "MathSphere.h"
#include "EnvironmentMap.h"
#include "../Scenes/DefaultScene.h"

void InitGLFW();
void InitGlAD();
void UpdateDeltaTime();

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
    EnvironmentMap environmentMap = EnvironmentMap("HDRIs/Shelter.hdr");
    
    // Load camera
    Camera camera = Camera(45, 0.1, 100);
    camera.Translate(glm::vec3(0, 0, 5));

    // Load scene
    DefaultScene defaultScene = DefaultScene();

    MathSphere mathSphere = MathSphere();

    // Set pre-runtime uniforms

    rayTraceShader.SetUnsignedInt("camera.viewPortWidth", WINDOWWIDTH);
    rayTraceShader.SetUnsignedInt("camera.viewPortHeight", WINDOWHEIGHT);

    defaultScene.SetMathSpheres(rayTraceShader);
    defaultScene.SetMeshes(rayTraceShader);

    rayTraceShader.SetUnsignedInt("maxDepth", 25);

    //---------------------------------------------------
    const unsigned int maxSamples = 3000;
    unsigned int currSample = 1;

    glm::mat4 lastCameraModelMatrix = camera.GetModelMatrix();
    glm::mat4 lastCameraProjectionMatrix = camera.GetProjectionMatrix();

    while (!glfwWindowShouldClose(window)) {
        // Setup
        UpdateDeltaTime();
        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
        
        camera.ProcessInput();

        if (lastCameraModelMatrix != camera.GetModelMatrix() || lastCameraProjectionMatrix != camera.GetProjectionMatrix()) {
            currSample = 1;

            glBindFramebuffer(GL_FRAMEBUFFER, rayTraceFBO);
            glClear(GL_COLOR_BUFFER_BIT);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            lastCameraModelMatrix = camera.GetModelMatrix();
            lastCameraProjectionMatrix = camera.GetProjectionMatrix();
        }

        if (currSample > maxSamples) continue;
        std::cout << currSample << "/" << maxSamples << std::endl;

        // Ray trace
        glBindFramebuffer(GL_FRAMEBUFFER, rayTraceFBO);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderTextures[0]);
        rayTraceShader.SetInt("cumulativeRenderTexture", 0);

        rayTraceShader.SetUnsignedInt("currSample", currSample);
        rayTraceShader.SetUnsignedInt("seed", rand());

        rayTraceShader.SetVec3("camera.position", camera.GetPosition());
        rayTraceShader.SetVec3("camera.xAxis", camera.GetXAxis());
        rayTraceShader.SetVec3("camera.yAxis", camera.GetYAxis());
        rayTraceShader.SetVec3("camera.zAxis", camera.GetZAxis());
        rayTraceShader.SetFloat("camera.focalLength", camera.GetFocalLength());

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, environmentMap.GetEnvironmentMap());
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