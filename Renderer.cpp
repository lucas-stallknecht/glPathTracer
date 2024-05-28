//
// Created by Dusha on 25/05/2024.
//

#include "Renderer.hpp"
#include "Shader.hpp"
#include <iostream>
#include <vector>

namespace rt {

    Renderer::Renderer(int width, int height, int offset) : VP_WIDTH(width), VP_HEIGHT(height){
        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            glfwTerminate();
        }
        glViewport(offset, 0, VP_WIDTH, VP_HEIGHT);

        m_shader = std::make_unique<Shader>("../shaders/default.vert", "../shaders/default.frag");
        m_compShader = std::make_unique<ComputeShader>("../shaders/default.comp");

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
        struct alignas(16) Material {
            float difusseCol[3];
            float emissiveCol[3];
        };
        struct alignas(16) Sphere {
            float pos[3];
            Material material;
            float radius;
            float padding;
        };
        // Define spheres
        std::vector<Sphere> spheres = {
                {{0.0f, 0.0f, 0.0f}, {{1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}, 1.0f, 0.0f},
                {{-1.0f, 0.5f, 0.0f},  {{0.0f, 1.0f, 0.0f},{0.0f, 0.0f, 0.0f}}, 1.5f, 0.0f}
                // Add more spheres as needed
        };

        // NEED TO SWITCH TO SSBO

        GLuint ubo;
        glGenBuffers(1, &ubo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ubo);
        std::cout << sizeof(Sphere);
        glBufferData(GL_SHADER_STORAGE_BUFFER, spheres.size() * sizeof(Sphere), spheres.data(), GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ubo);

        m_compShader->use();
        m_compShader->setUniform1i("numSpheres", spheres.size());
    }

    void Renderer::draw() {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_compShader->use();
        glDispatchCompute((unsigned int)VP_WIDTH, (unsigned int)VP_HEIGHT, 1);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_rtTexture);

        m_shader->use();
        glBindVertexArray(m_vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);



// make sure writing to image has finished before read
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }
} // lgl