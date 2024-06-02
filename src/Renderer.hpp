//
// Created by Dusha on 25/05/2024.
//

#pragma once

#include "Shader.hpp"
#include "ComputeShader.hpp"
#include <glad/glad.h>
#include <GLFW//glfw3.h>
#include <glm/glm.hpp>
#include <memory>

namespace rt {

    class Renderer {

    public:
        Renderer(int width, int height);
        void draw();
        void updateCamera(glm::vec3 camPos, glm::vec3 camDir, glm::mat4 invProjection);
        void resetAccumulation();

    private:
        void initQuadOutput();
        void initRTSpheres();

        const int VP_WIDTH, VP_HEIGHT;
        std::unique_ptr<Shader> m_shader;
        std::unique_ptr<ComputeShader> m_compShader;
        GLuint m_vao;
        GLuint m_rtTexture;
        int m_frames = 0;
    };

} // lgl
