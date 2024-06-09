//
// Created by Dusha on 25/05/2024.
//

#include "Renderer.hpp"
#include "Shader.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "GeometryManager.hpp"

namespace rt {

    Renderer::Renderer(int width, int height) : VP_WIDTH(width), VP_HEIGHT(height){
        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
             std::cout << "Failed to initialize GLAD" << std::endl;
            glfwTerminate();
        }
        glViewport(0, 0, VP_WIDTH, VP_HEIGHT);

        // Shaders
        m_shader = std::make_unique<Shader>("../shaders/path_tracing.vert", "../shaders/path_tracing.frag");
        m_compShader = std::make_unique<ComputeShader>("../shaders/path_tracing.comp");

        // Final quad output with texture
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
        m_shader->use();
        initializeRenderQuad();

        // Scene objects
        std::vector<Sphere> spheres = GeometryManager::loadRTSpheres(false);
        spheres = std::vector<Sphere>(0);
        GLsizeiptr sphereVecSize = spheres.size() * sizeof(Sphere);

        GeometryManager monkey("../resources/helmet.obj", 10, true);
        monkey.buildBVH(true);
        // monkey.traverseBVH(0);

        std::vector<Triangle> triangles = monkey.m_triangles;
        GLsizeiptr trianglesVecSize = triangles.size() * sizeof(Triangle);

        std::vector<Node> nodes = monkey.m_nodes;
        GLsizeiptr nodesVecSize = nodes.size() * sizeof(Node);

        glGenBuffers(1, &m_ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sphereVecSize + trianglesVecSize + nodesVecSize, nullptr, GL_STATIC_DRAW);
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, m_ssbo, 0 , sphereVecSize);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sphereVecSize, spheres.data());
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, m_ssbo, sphereVecSize , trianglesVecSize);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, sphereVecSize, trianglesVecSize, triangles.data());
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 3, m_ssbo, sphereVecSize + trianglesVecSize , nodesVecSize);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, sphereVecSize + trianglesVecSize, nodesVecSize, nodes.data());

        m_compShader->use();
        m_compShader->setUniform1i("u_nSpheres", spheres.size());
        m_compShader->setUniform1i("u_nTriangles", triangles.size());
    }

    void Renderer::initializeRenderQuad() {
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
        // Vertices positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        // Texcoords
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
                     GL_FLOAT, nullptr);
        glBindImageTexture(0, m_rtTexture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    }

    void Renderer::logComputeShaderCapabilities(){
        int work_grp_cnt[3];
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);
        std::cout << "Max work groups per compute shader" <<
                  " x:" << work_grp_cnt[0] <<
                  " y:" << work_grp_cnt[1] <<
                  " z:" << work_grp_cnt[2] << "\n";

        int work_grp_size[3];
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);
        std::cout << "Max work group sizes" <<
                  " x:" << work_grp_size[0] <<
                  " y:" << work_grp_size[1] <<
                  " z:" << work_grp_size[2] << "\n";

        int work_grp_inv;
        glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
        std::cout << "Max invocations count per work group: " << work_grp_inv << "\n";
    };

    void Renderer::updateCamera(glm::vec3 camPos, glm::vec3 camDir, glm::mat4 invProjection){
        m_compShader->use();

        glm::mat4 view = glm::lookAt(camPos, camPos + camDir, glm::vec3(0, 1, 0));
        glm::mat4 invView = glm::inverse(view);

        m_compShader->setUniformVec3("camPos", camPos);
        m_compShader->setUniformMat4("invView", invView);
        m_compShader->setUniformMat4("invProjection", invProjection);
    }

    void Renderer::resetAccumulation() {
        m_frame = 0;
    }

    void Renderer::draw() {

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindImageTexture(0, m_rtTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
        m_compShader->use();
        m_compShader->setUniform1i("u_frame", m_frame);
        m_compShader->setUniform1i("u_bounces", renderOptions.bounces);
        m_compShader->setUniform1i("u_samples", renderOptions.samples);
        m_compShader->setUniform1f("u_jitter", renderOptions.jitter);
        glDispatchCompute((unsigned int)VP_WIDTH/8, (unsigned int)VP_HEIGHT/8, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        m_shader->use();
        glBindVertexArray(m_vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_rtTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        m_frame++;
    }
} // rt