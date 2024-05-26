//
// Created by Dusha on 25/05/2024.
//

#pragma once
#include "Shader.hpp"
#include <glad/glad.h>
#include <GLFW//glfw3.h>
#include <memory>


namespace lgl {

    class Renderer {

    public:
        Renderer(int width, int height);
        void draw();

    private:
        std::unique_ptr<Shader> m_shader;
        unsigned int m_vao;
    };

} // lgl
