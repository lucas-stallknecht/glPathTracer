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
#include <vector>


namespace rt {



    struct RenderOptions {
        int bounces;
        int samples;
        float jitter;
    };

    class Renderer {

    public:
        Renderer(int width, int height);
        void draw();
        void updateCamera(glm::vec3 camPos, glm::vec3 camDir, glm::mat4 invProjection);
        void resetAccumulation();

    private:
        void initQuadOutput();
        static void printAvailableGroupSizes();

        const int VP_WIDTH, VP_HEIGHT;
        std::unique_ptr<Shader> m_shader;
        std::unique_ptr<ComputeShader> m_compShader;
        GLuint m_vao;
        GLuint m_ssbo;
        GLuint m_rtTexture;
        int m_frame = 0;
    };

} // rt
