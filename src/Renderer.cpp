//
// Created by Dusha on 25/05/2024.
//

#include "Renderer.hpp"
#include "Shader.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <iterator>
#include "imgui.h"

namespace rt {

    Renderer::Renderer(int width, int height) : VP_WIDTH(width), VP_HEIGHT(height){
        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            glfwTerminate();
        }
        glViewport(0, 0, VP_WIDTH, VP_HEIGHT);

        m_shader = std::make_unique<Shader>("../shaders/path_tracing.vert", "../shaders/path_tracing.frag");
        m_compShader = std::make_unique<ComputeShader>("../shaders/path_tracing.comp");

        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
        initQuadOutput();
        initRTSpheres();
    }

    void Renderer::initQuadOutput() {
        float vertices[] = {
                // positions        // texture Coords
                -1.0f,  1.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f,
                1.0f,  1.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 1.0f, 0.0f,
        };

        unsigned int VBO;

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // vertices positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        // texcoords
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        // Texture where the CS is going to write on
        glGenTextures(1, &m_rtTexture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_rtTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, VP_WIDTH, VP_HEIGHT, 0, GL_RGBA,
                     GL_FLOAT, NULL);
        glBindImageTexture(0, m_rtTexture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_rtTexture);
    }

    void Renderer::initRTSpheres() {
        struct Material {
            float diffuseCol[3];
            float emissiveStrength;
            float roughness;
            float specularPerc;
            float padding[2];
        };
        struct Sphere {
            float pos[3];
            float radius;
            Material material;
        };

        Sphere spheres[] = {
                // right ball
                {
                        {-0.75f, 0.0f, 0.0f},
                        0.2,
                        {{0.9f, 0.9f, 0.9f}, 0.0, 0.0, 1.0, {7.7,7.7}},
                },
                // middle ball
                {
                        {-0.25f, 0.0f, 0.0f},
                        0.2,
                        {{0.9f, 0.9f, 0.9f}, 0.0, 0.0, 0.75, {7.7,7.7}},
                },
                {
                        {0.25f, 0.0f, 0.0f},
                        0.2,
                        {{0.9f, 0.9f, 0.9f}, 0.0, 0.0, 0.5, {7.7,7.7}},
                },
                // left ball
                {
                        {0.75f, 0.0f, 0.0f},
                        0.2,
                        {{0.9f, 0.9f, 0.9f}, 0.0, 0.0, 0.25, {7.7,7.7}},
                },

        };

        GLuint ssbo;
        glGenBuffers(1, &ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        std::cout << "Size of a material struct : " << sizeof(Material) << std::endl;
        std::cout << "Size of sphere struct : " << sizeof(Sphere) << std::endl;
        std::cout << "Size of spheres array : " << sizeof(spheres) << std::endl;
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(spheres), spheres, GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo);

        m_compShader->use();
        m_compShader->setUniform1i("numSpheres", std::size(spheres));
    }

    void Renderer::updateCamera(glm::vec3 camPos, glm::vec3 camDir, glm::mat4 invProjection){
        m_compShader->use();

        glm::mat4 view = glm::lookAt(camPos, camPos + camDir, glm::vec3(0, 1, 0));
        glm::mat4 invView = glm::inverse(view);

        m_compShader->setUniformVec3("camPos", camPos);
        m_compShader->setUniformMat4("invView", invView);
        m_compShader->setUniformMat4("invProjection", invProjection);
    }

    void Renderer::resetAccumulation() {
        m_frames = 0;
    }

    void Renderer::draw() {

        static int bounces = 5, samples = 1;
        bool change = false;
        change |= ImGui::InputInt("Bounces", &bounces, 1, 2);
        change |= ImGui::InputInt("Samples", &samples);
        ImGui::End();

        if(change)
            m_frames = 0;
        else
            m_frames++;

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindImageTexture(0, m_rtTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
        m_compShader->use();
        m_compShader->setUniform1i("frame", m_frames);
        m_compShader->setUniform1i("BOUNCES", bounces);
        m_compShader->setUniform1i("RAYS_PER_PIXEL", samples);
        glDispatchCompute((unsigned int)VP_WIDTH, (unsigned int)VP_HEIGHT, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        m_shader->use();
        glBindVertexArray(m_vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_rtTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
} // lgl