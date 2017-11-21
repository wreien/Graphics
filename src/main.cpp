#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>

#include "shader.h"
#include "level.h"

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <level>\n";
        std::exit(1);
    }

    {
        world::Level level { argv[1] };
        auto [width, height] = level.size();

        std::cout << "Level size: " << width << ", " << height << std::endl;
        for (decltype(width) row = 0; row < width; row++) {
            for (decltype(height) col = 0; col < height; col++) {
                std::cout << level.altitude(col, row) << " ";
            }
            std::cout << "\n";
        }
    }

    glfwSetErrorCallback([](auto err, auto desc) {
        std::cerr << "Error " << err << ": " << desc << std::endl;
    });

    if (!glfwInit())
        return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    auto window = glfwCreateWindow(640, 480, "Graphics", nullptr, nullptr);
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

    glViewport(0, 0, 640, 480);
    glfwSetFramebufferSizeCallback(window, [](auto window, auto width, auto height) {
        glViewport(0, 0, width, height);
    });

    {
        graphics::Shader main { std::ifstream("shaders/main.vert"),
                                std::ifstream("shaders/main.frag") };

        float vertices[] = {
             0.5f,  0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
            -0.5f, -0.5f, 0.0f,
            -0.5f,  0.5f, 0.0f,
        };
        unsigned int indices[] = {
            0, 1, 3,
            1, 2, 3,
        };

        unsigned int vbo[2], vao;
        glGenVertexArrays(1, &vao);
        glGenBuffers(2, vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);

        while (!glfwWindowShouldClose(window)) {
            processInput(window);

            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glUseProgram(main);
            glBindVertexArray(vao);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(2, vbo);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
