#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdlib>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <functional>

#include "shader.h"
#include "level.h"

// Handles the overarching drawing and input
// Needs OpenGL to be set up first
class Manager {
    GLFWwindow* window;

public:
    Manager(GLFWwindow* window, std::string level_filename)
        : window { window }, level { level_filename }
    {
        glfwSetWindowUserPointer(window, this);
        glfwGetWindowSize(window, &screen_width, &screen_height);

        initOGL();
        setupCallbacks();
    }

    void mainLoop() {
        glm::mat4 view = glm::lookAt(
                glm::vec3 { 0, 1, 3 },
                glm::vec3 { 2, 0, 2 },
                glm::vec3 { 0, 1, 0 });
        glm::mat4 camera = projection * view;

        while (!glfwWindowShouldClose(window)) {
            processInput();

            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            level.render(camera);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

private:
    int screen_width;
    int screen_height;

    world::Level level;
    glm::mat4 projection;

    void processInput() {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }

    void setupCallbacks() {
        glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    }

    void initOGL() {
        framebufferSizeCallback(window, screen_width, screen_height);

        glEnable(GL_DEPTH_TEST);
    }

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
        Manager* self = reinterpret_cast<Manager*>(glfwGetWindowUserPointer(window));

        self->screen_width = width;
        self->screen_height = height;

        glViewport(0, 0, self->screen_width, self->screen_height);

        float aspect = static_cast<float>(self->screen_width) / self->screen_height;
        self->projection = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 20.f);
    }
};

// Provides GLFW initialization, and then hands everything over to the 'Manager' class.
int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <level>\n";
        std::exit(1);
    }

    glfwSetErrorCallback([](auto err, auto desc) {
        std::cerr << "Error " << err << ": " << desc << std::endl;
    });

    if (!glfwInit())
       std::exit(1);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(640, 480, "Graphics", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window." << std::endl;
        glfwTerminate();
        std::exit(1);
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD." << std::endl;
        std::exit(1);
    }
    glfwSwapInterval(1);

    {
        Manager m { window, argv[1] };
        m.mainLoop();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
