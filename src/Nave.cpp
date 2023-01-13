#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

#include "WindowInfo.h"
#include "ShaderProgram.h"
#include "Mesh.h"
#include "Vertex.h"
#include "Camera.h"

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
    InitGLFW();
    InitGlAD();

    float currentTime = 0;

    glEnable(GL_FRAMEBUFFER_SRGB);

    ShaderProgram shader = ShaderProgram("src/Shaders/Model.vert", "src/Shaders/Debug_LocalPosition.frag");

    std::vector<Vertex> vertices = {
        Vertex(glm::vec3(-1, -1, 0), glm::vec3(0)),
        Vertex(glm::vec3(1, -1, 0), glm::vec3(0)),
        Vertex(glm::vec3(0, 1, 0), glm::vec3(0))
    };
    std::vector<unsigned int> indices = { 0, 1, 2 };

    Mesh triangle = Mesh(vertices, indices);
    triangle.Translate(glm::vec3(0, 0, -8));
    Camera camera = Camera(45.0f, 0.1f, 100.0f);

    while (!glfwWindowShouldClose(window)) {
        UpdateDeltaTime();
        glfwSwapBuffers(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
        
        camera.ProcessInput();

        shader.SetMat4("modelMatrix", triangle.GetModelMatrix());
        shader.SetMat4("viewMatrix", camera.GetViewMatrix());
        shader.SetMat4("projectionMatrix", camera.GetProjectionMatrix());

        shader.Use();
        triangle.Draw();
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