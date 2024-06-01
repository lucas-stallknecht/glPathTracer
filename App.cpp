//
// Created by Dusha on 25/05/2024.
//

#include "App.hpp"
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



    };

    void App::run() {

        while (!glfwWindowShouldClose(m_window)) {
            glfwSwapBuffers(m_window);
            keyInput();
            glfwPollEvents();
            m_renderer->updateCamera(m_cam.m_camPos, m_cam.m_camDir, m_cam.m_InvProjection);
            m_renderer->draw();
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

        m_cam.updateCamDirection(xOffset, yOffset);
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
            m_cam.moveForward();
            m_renderer->resetAccumulation();
        }
        if (m_keysArePressed['S']) {
            m_cam.moveBackward();
            m_renderer->resetAccumulation();
        }
        if (m_keysArePressed['A']) {
            m_cam.moveLeft();
            m_renderer->resetAccumulation();
        }
        if (m_keysArePressed['D']) {
            m_cam.moveRight();
            m_renderer->resetAccumulation();
        }
        // std::cout << "cam position : " << m_cam.m_camPos.x << " " <<  m_cam.m_camPos.y << " " <<  m_cam.m_camPos.z << std::endl;

    }

    App::~App() {
        glfwTerminate();
    };
} // lgl