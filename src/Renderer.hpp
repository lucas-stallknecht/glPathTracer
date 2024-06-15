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
        int bounces = 5;
        int samples = 1;
        int jitter = 1;
        bool enableSkybox = true;
        bool showSkybox = false;
        bool smoothShading = false;
        float skyboxIntensity = 2.5;
    };

    class Renderer {

    public:
        Renderer(int width, int height);
        void draw();
        void updateCamera(glm::vec3 camPos, glm::vec3 camDir, glm::mat4 invProjection);
        void resetAccumulation();
        void loadCubeMap(const std::string& skyboxDirectory);
        void loadScene(const std::string& scenePath);

        RenderOptions renderOptions;

    private:
        void initializeRenderQuad();

        const int VP_WIDTH, VP_HEIGHT;
        std::unique_ptr<Shader> m_shader;
        std::unique_ptr<ComputeShader> m_compShader;
        GLuint m_vao;
        GLuint m_ssbo;
        GLuint m_rtTexture;
        int m_frame = 0;
    };

} // rt
