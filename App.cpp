//
// Created by Dusha on 25/05/2024.
//

#include "App.hpp"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <iostream>

namespace rt {


    App::App() {
        m_keysArePressed = new bool[512]{false};
        glfwInit();

        // Context params
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);


        // Window initialization
        m_window = glfwCreateWindow(LAYOUT_WIDTH , HEIGHT, "Custom Path Tracing engine", nullptr, nullptr);
        if (m_window == nullptr) {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
        }
        glfwMakeContextCurrent(m_window);


        m_renderer = std::make_unique<Renderer>(LAYOUT_WIDTH, HEIGHT);

        // Inputs
        glfwSetWindowUserPointer(m_window, this);
        auto keyCallback = [](GLFWwindow *window, int key, int scancode, int action, int mods) {
            App *app = static_cast<App *>(glfwGetWindowUserPointer(window));
            app->keyCallback(window, key);
        };
        glfwSetKeyCallback(m_window, keyCallback);


        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        auto mouseCallback = [](GLFWwindow *window, double xpos, double ypos) {
            App *app = static_cast<App *>(glfwGetWindowUserPointer(window));
            app->mouseCallback(window, (float)xpos,(float)ypos);
        };
        glfwSetCursorPosCallback(m_window, mouseCallback);

        auto mouseButtonCallback = [](GLFWwindow *window, int button, int action, int mods) {
            App *app = static_cast<App *>(glfwGetWindowUserPointer(window));
            app->mouseButtonCallback(window, button, action);
        };
        glfwSetMouseButtonCallback(m_window, mouseButtonCallback);


        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        m_io = &ImGui::GetIO();
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(m_window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
        ImGui_ImplOpenGL3_Init();
    };

    void App::run() {

        while (!glfwWindowShouldClose(m_window)) {

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            {
                static float f = 0.0f;
                static int counter = 0;

                ImGui::Begin("Infos");
                ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / m_io->Framerate, m_io->Framerate);
                ImGui::End();
            }


            glfwSwapBuffers(m_window);
            keyInput();
            glfwPollEvents();
            m_renderer->updateCamera(m_cam.m_camPos, m_cam.m_camDir, m_cam.m_InvProjection);
            m_renderer->draw();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
    };

    void App::keyCallback(GLFWwindow *window, int key) {
        {
            m_keysArePressed[key] = (glfwGetKey(window, key) == GLFW_PRESS);
        }
    }

    void App::mouseCallback(GLFWwindow *window, float xpos, float ypos) {

        if(!m_focused)
            return;

        if (m_firstMouse)
        {
            m_lastMousePosition.x = xpos;
            m_lastMousePosition.y = ypos;
            m_firstMouse = false;
        }
        float xOffset = xpos - m_lastMousePosition.x;
        float yOffset = m_lastMousePosition.y - ypos;

        if(abs(xOffset) > 0.0 || abs(yOffset) > 0.0)
            m_renderer->resetAccumulation();
        m_lastMousePosition.x = xpos;
        m_lastMousePosition.y = ypos;

        m_cam.updateCamDirection(xOffset, yOffset, m_io->DeltaTime);
    }

    void App::mouseButtonCallback(GLFWwindow *window,  int button, int action) {
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
            m_focused = true;
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE){
            m_focused = false;
            m_firstMouse = true;
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

    }

    void App::keyInput() {
        if (m_keysArePressed['W']) {
            m_cam.moveForward(m_io->DeltaTime);
            m_renderer->resetAccumulation();
        }
        if (m_keysArePressed['S']) {
            m_cam.moveBackward(m_io->DeltaTime);
            m_renderer->resetAccumulation();
        }
        if (m_keysArePressed['A']) {
            m_cam.moveLeft(m_io->DeltaTime);
            m_renderer->resetAccumulation();
        }
        if (m_keysArePressed['D']) {
            m_cam.moveRight(m_io->DeltaTime);
            m_renderer->resetAccumulation();
        }
        // std::cout << "cam position : " << m_cam.m_camPos.x << " " <<  m_cam.m_camPos.y << " " <<  m_cam.m_camPos.z << std::endl;

    }

    App::~App() {
        glfwTerminate();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    };
} // lgl