//
// Created by Dusha on 25/05/2024.
//

#pragma once

#include "Renderer.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>

namespace lgl {

    class App {

        static constexpr int DF_WIDTH = 1280;
        static constexpr int DF_HEIGHT = 720;

    public:

        App();
        void run();
        ~App();

    private:
        GLFWwindow *m_window;
        std::unique_ptr<Renderer> m_renderer;
    };

} // lgl
