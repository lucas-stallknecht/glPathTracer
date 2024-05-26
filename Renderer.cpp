//
// Created by Dusha on 25/05/2024.
//

#include "Renderer.hpp"
#include <iostream>
#include "Shader.hpp"

namespace lgl {

    Renderer::Renderer(int width, int height) {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            glfwTerminate();
        }
        glViewport(0, 0, width, height);

        m_shader = std::make_unique<Shader>("../shaders/default.vert", "../shaders/default.frag");

        float vertices[] = {
                // pos                          col
                0.0f, 0.4f, 0.0f, 1.0, 0.0, 0.0,
                -0.25f, -0.3f, 0.0f, 0.0, 1.0, 0.0,
                0.25f,  -0.3f, 0.0f, 0.0, 0.0, 1.0
        };
        int indices[] = {
                0, 1 ,2
        };
        unsigned int VBO;
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &VBO);
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // copies the previously defined vertex data into the buffer's memory
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // linking vertex buffer array to shader attribute
        // note that glVertexAttribPointer operates on what is currently bound
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // index buffer test
        unsigned int EBO;
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    }

    void Renderer::draw() {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        const float uniformIncr = sin(glfwGetTime() / 2.0f) - 0.2;

         m_shader->bind();
         m_shader->setUniformVec4("u_col", uniformIncr, uniformIncr, uniformIncr, uniformIncr);
         glBindVertexArray(m_vao);
         // glDrawArrays(GL_TRIANGLES, 0, 3);
         glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
    }
} // lgl