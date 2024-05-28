//
// Created by Dusha on 25/05/2024.
//

#pragma once

#include "Shader.hpp"
#include "ComputeShader.hpp"
#include <glad/glad.h>
#include <GLFW//glfw3.h>
#include <memory>

namespace rt {

    class Renderer {

    public:
        Renderer(int width, int height, int offset);
        void draw();

    private:
        void initQuadOutput();
        void initRTSpheres();

        const int VP_WIDTH, VP_HEIGHT;
        std::unique_ptr<Shader> m_shader;
        std::unique_ptr<ComputeShader> m_compShader;
        GLuint m_vao;
        GLuint m_rtTexture;
    };

} // lgl
