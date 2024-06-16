//
// Created by Dusha on 25/05/2024.
//
#include "App.hpp"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <imfilebrowser.h>

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
        m_window = glfwCreateWindow(LAYOUT_WIDTH, HEIGHT, "Custom Path Tracing engine", nullptr, nullptr);
        if (m_window == nullptr) {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
        }
        glfwMakeContextCurrent(m_window);

        // Renderer and default scene elements
        m_renderer = std::make_unique<Renderer>(LAYOUT_WIDTH, HEIGHT);
        m_renderer->loadCubeMap("../resources/skyboxes/paris");
        m_renderer->loadScene("../resources/scenes/dragon_scene.txt");

        // Inputs
        glfwSetWindowUserPointer(m_window, this);
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        auto keyCallback = [](GLFWwindow *window, int key, int scancode, int action, int mods) {
            App *app = static_cast<App *>(glfwGetWindowUserPointer(window));
            app->keyCallback(window, key);
        };
        auto mouseCallback = [](GLFWwindow *window, double xpos, double ypos) {
            App *app = static_cast<App *>(glfwGetWindowUserPointer(window));
            app->mouseCallback(window, (float) xpos, (float) ypos);
        };
        auto mouseButtonCallback = [](GLFWwindow *window, int button, int action, int mods) {
            App *app = static_cast<App *>(glfwGetWindowUserPointer(window));
            app->mouseButtonCallback(window, button, action);
        };
        glfwSetCursorPosCallback(m_window, mouseCallback);
        glfwSetKeyCallback(m_window, keyCallback);
        glfwSetMouseButtonCallback(m_window, mouseButtonCallback);


        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        m_io = &ImGui::GetIO();
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(m_window,
                                     true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
        ImGui_ImplOpenGL3_Init();

    };

    void App::run() {

        ImGui::FileBrowser sceneDialog(0, std::filesystem::path("../resources/scenes/"));
        sceneDialog.SetTitle("Scene presets");
        sceneDialog.SetTypeFilters({ ".txt" });
        ImGui::FileBrowser sbDialog(ImGuiFileBrowserFlags_SelectDirectory, std::filesystem::path("../resources/skyboxes/"));
        sbDialog.SetTitle("Skybox");
        sbDialog.SetTypeFilters({ ".txt" });

        RenderOptions renderOptions = m_renderer->renderOptions;

        while (!glfwWindowShouldClose(m_window)) {

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            {
                ImGui::Begin("Options");
                ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / m_io->Framerate, m_io->Framerate);
                bool change = false;
                if (ImGui::CollapsingHeader("Rendering")) {
                    change |= ImGui::InputInt("Bounces", &renderOptions.bounces, 1, 2);
                    change |= ImGui::InputInt("Samples", &renderOptions.samples);
                    change |= ImGui::SliderInt("Jitter", &renderOptions.jitter, 1, 10);
                }
                if (ImGui::CollapsingHeader("Meshes")) {
                    change |= ImGui::Checkbox("Smooth shading", &renderOptions.smoothShading);
                    if(ImGui::Button("Scene preset"))
                        sceneDialog.Open();
                }
                if (ImGui::CollapsingHeader("Skybox")) {
                    change |= ImGui::Checkbox("Enable skybox", &renderOptions.enableSkybox);
                    change |= ImGui::Checkbox("Show skybox background", &renderOptions.showSkybox);
                    change |= ImGui::SliderFloat("Skybox intensity", &renderOptions.skyboxIntensity, 0.5, 5.0);
                    if(ImGui::Button("Skybox directory"))
                        sbDialog.Open();
                }
                if (ImGui::CollapsingHeader("Color Correction")) {
                    change |= ImGui::Checkbox("Gamma Correction", &renderOptions.gammaCorrection);
                    change |= ImGui::Checkbox("ACESFilm", &renderOptions.toneMapping);
                    change |= ImGui::SliderFloat("Exposure", &renderOptions.exposure, 0.5, 2.5);
                }

                ImGui::End();

                sceneDialog.Display();
                if(sceneDialog.HasSelected())
                {
                    m_renderer->loadScene(sceneDialog.GetSelected().string());
                    m_renderer->resetAccumulation();
                    sceneDialog.ClearSelected();
                }
                sbDialog.Display();
                if(sbDialog.HasSelected())
                {
                    m_renderer->loadCubeMap(sbDialog.GetSelected().string());
                    std::cout << sbDialog.GetSelected().string() << std::endl;
                    m_renderer->resetAccumulation();
                    sbDialog.ClearSelected();
                }

                if (change) {
                    m_renderer->resetAccumulation();
                    m_renderer->renderOptions = renderOptions;
                }
            }
            // swap front with back buffer
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
        m_keysArePressed[key] = (glfwGetKey(window, key) == GLFW_PRESS);
    }

    void App::mouseCallback(GLFWwindow *window, float xpos, float ypos) {

        // is right-clicked basically
        if (!m_focused)
            return;

        // the mouse was not focused the frame before
        if (m_firstMouse) {
            m_lastMousePosition.x = xpos;
            m_lastMousePosition.y = ypos;
            m_firstMouse = false;
        }
        float xOffset = xpos - m_lastMousePosition.x;
        float yOffset = m_lastMousePosition.y - ypos;

        if (abs(xOffset) > 0.0 || abs(yOffset) > 0.0)
            m_renderer->resetAccumulation();
        m_lastMousePosition.x = xpos;
        m_lastMousePosition.y = ypos;

        m_cam.updateCamDirection(xOffset, yOffset, m_io->DeltaTime);
    }

    void App::mouseButtonCallback(GLFWwindow *window, int button, int action) {
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
            m_focused = true;
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
            m_focused = false;
            m_firstMouse = true;
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

    }

    void App::keyInput() {
        if (m_keysArePressed['W'] && m_focused) {
            m_cam.moveForward(m_io->DeltaTime);
            m_renderer->resetAccumulation();
        }
        if (m_keysArePressed['S'] && m_focused) {
            m_cam.moveBackward(m_io->DeltaTime);
            m_renderer->resetAccumulation();
        }
        if (m_keysArePressed['A'] && m_focused) {
            m_cam.moveLeft(m_io->DeltaTime);
            m_renderer->resetAccumulation();
        }
        if (m_keysArePressed['D'] && m_focused) {
            m_cam.moveRight(m_io->DeltaTime);
            m_renderer->resetAccumulation();
        }
        if (m_keysArePressed['Q'] && m_focused) {
            m_cam.moveDown(m_io->DeltaTime);
            m_renderer->resetAccumulation();
        }
        if (m_keysArePressed[' '] && m_focused) {
            m_cam.moveUp(m_io->DeltaTime);
            m_renderer->resetAccumulation();
        }
    }

    App::~App() {
        glfwTerminate();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    };
} // rt