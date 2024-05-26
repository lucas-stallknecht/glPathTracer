//
// Created by Dusha on 25/05/2024.
//

#include "Renderer.hpp"
#include "Shader.hpp"
#include <iostream>

namespace rt {

    Renderer::Renderer(int width, int height, int offset) {
        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            glfwTerminate();
        }
        glViewport(offset, 0, width, height);

        m_shader = std::make_unique<Shader>("../shaders/default.vert", "../shaders/default.frag");
        m_compShader = std::make_unique<ComputeShader>("../shaders/default.comp");

        float vertices[] = {
                // positions        // texture Coords
                -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };

        unsigned int VBO;
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &VBO);
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));


        const unsigned int TEXTURE_WIDTH = 512, TEXTURE_HEIGHT = 512;
        unsigned int texture;

        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGBA,
                     GL_FLOAT, NULL);

        glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

    }

    void Renderer::draw() {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        const float uniformIncr = sin(glfwGetTime() / 2.0f) - 0.2;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_shader->use();
        m_shader->setUniformVec4("u_col", uniformIncr, uniformIncr, uniformIncr, uniformIncr);
        glBindVertexArray(m_vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        m_compShader->use();
        glDispatchCompute((unsigned int)1280, (unsigned int)720, 1);

// make sure writing to image has finished before read
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }
} // lgl