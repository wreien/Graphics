#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>

#include "shader.h"
#include "level.h"

static int screen_width = 640;
static int screen_height = 480;

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void mainLoop(GLFWwindow* window, int argc, char** argv) {
    world::Level level { argv[1] };

    float aspect = static_cast<float>(screen_width) / screen_height;
    glm::mat4 projection = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 20.f);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(
                glm::vec3 { 0, 1, 3 },
                glm::vec3 { 2, 0, 2 },
                glm::vec3 { 0, 1, 0 });
        glm::mat4 modelview = projection * view;
        level.render(modelview);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <level>\n";
        std::exit(1);
    }

    glfwSetErrorCallback([](auto err, auto desc) {
        std::cerr << "Error " << err << ": " << desc << std::endl;
    });

    if (!glfwInit())
        return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    auto window =
        glfwCreateWindow(screen_width, screen_height, "Graphics", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window." << std::endl;
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD." << std::endl;
        return 2;
    }
    glfwSwapInterval(1);

    glViewport(0, 0, screen_width, screen_height);
    glfwSetFramebufferSizeCallback(window, [](auto window, auto width, auto height) {
        glViewport(0, 0, width, height);
        screen_width = width;
        screen_height = height;
    });

    glEnable(GL_DEPTH_TEST);

    mainLoop(window, argc, argv);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
