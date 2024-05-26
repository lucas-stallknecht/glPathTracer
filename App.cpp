//
// Created by Dusha on 25/05/2024.
//

#include "App.hpp"
#include <iostream>

namespace rt {
    App::App() {
        glfwInit();

        // Context params
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Window initialization
        m_window = glfwCreateWindow(LAYOUT_WIDTH + PANEL_WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
        if (m_window == nullptr) {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
        }
        glfwMakeContextCurrent(m_window);

        m_renderer = std::make_unique<Renderer>(LAYOUT_WIDTH, HEIGHT, PANEL_WIDTH);
    };

    void App::run() {

        while (!glfwWindowShouldClose(m_window)) {
            glfwSwapBuffers(m_window);
            glfwPollEvents();
            m_renderer->draw();
        }
    };

    App::~App() {
        std::cout << "Thanks for using my engine :D" << std::endl;
        glfwTerminate();
    };
} // lgl