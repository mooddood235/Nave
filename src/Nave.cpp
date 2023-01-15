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
    glEnable(GL_DEPTH_TEST);

    // Load shaders
    ShaderProgram renderQuadShader = ShaderProgram("src/Shaders/RenderQuad.vert", "src/Shaders/RenderQuad.frag");

    // Load quad used for rendering
    Model renderQuad = Model("Models/Quad/Quad.fbx");

    // Load scene objects
    Camera camera = Camera(45, 0.1, 100);

    MathSphere mathSphere = MathSphere(glm::vec3(0, 0, -2), 1);

    renderQuadShader.SetFloat("camera.viewPortWidth", WINDOWWIDTH);
    renderQuadShader.SetFloat("camera.viewPortHeight", WINDOWHEIGHT);

    renderQuadShader.SetVec3("mathSpheres[0].position", mathSphere.GetPosition());
    renderQuadShader.SetFloat("mathSpheres[0].radius", mathSphere.GetRadius());

    while (!glfwWindowShouldClose(window)) {
        // Setup
        UpdateDeltaTime();
        glfwSwapBuffers(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
        
        camera.ProcessInput();

        // Set uniforms
        renderQuadShader.SetVec3("camera.position", camera.GetPosition());
        renderQuadShader.SetVec3("camera.xAxis", camera.GetXAxis());
        renderQuadShader.SetVec3("camera.yAxis", camera.GetYAxis());
        renderQuadShader.SetVec3("camera.zAxis", camera.GetZAxis());
        renderQuadShader.SetFloat("camera.focalLength", camera.GetFocalLength());

        // Ray trace
        renderQuadShader.Use();
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

    window = glfwCreateWindow(videoMode->width, videoMode->height, "Nave", monitor, NULL);

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