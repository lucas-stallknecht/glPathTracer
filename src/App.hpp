//
// Created by Dusha on 25/05/2024.
//

#pragma once

#include "imgui.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imfilebrowser.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>


#include "Renderer.hpp"
#include "Camera.hpp"


namespace rt {

    class App {

        static constexpr int LAYOUT_WIDTH = 1860;
        static constexpr int HEIGHT = 960;

    public:
        App();
        void run();
        ~App();

    private:
        GLFWwindow *m_window;
        std::unique_ptr<Renderer> m_renderer;
        Camera m_cam{50.0, LAYOUT_WIDTH, HEIGHT, 0.1, 100.0};
        bool *m_keysArePressed;
        bool m_focused = false;
        bool m_firstMouse = true;
        glm::vec2 m_lastMousePosition = glm::vec2();
        ImGuiIO *m_io;


        void keyCallback(GLFWwindow* window, int key);
        void mouseCallback(GLFWwindow* window, float xpos, float ypos);
        void mouseButtonCallback(GLFWwindow *window,  int button, int action);
        void keyInput();
    };

} // rt
