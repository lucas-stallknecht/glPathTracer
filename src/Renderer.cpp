//
// Created by Dusha on 25/05/2024.
//

#include "Renderer.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include "Mesh.hpp"
#include "Scene.hpp"

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
    }

    void Renderer::loadCubeMap(const std::string& skyboxDirectory)
    {
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        std::vector<std::string> faces
                {
                        skyboxDirectory + "/px.png",    // right
                        skyboxDirectory + "/nx.png",     // left
                        skyboxDirectory + "/py.png",      // top
                        skyboxDirectory + "/ny.png",   // bottom
                        skyboxDirectory + "/pz.png",    // front
                        skyboxDirectory + "/nz.png"      // back
                };

        int width, height, nrChannels;
        for (unsigned int i = 0; i < faces.size(); i++)
        {
            unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
            if (data)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                             0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
                );
                stbi_image_free(data);
            }
            else
            {
                std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
                stbi_image_free(data);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    void Renderer::loadScene(const std::string& scenePath) {

        Scene scene(scenePath);

        std::vector<MeshInfo> meshFiles = scene.m_meshesInfo;
        std::vector<Triangle> triangles;
        std::vector<Node> nodes;
        std::vector<Material> materials;
        std::vector<glm::vec2> meshInfoList;
        for(const auto &meshInfo: meshFiles) {
            Mesh geo(meshInfo.objPath, meshInfo.depth, materials.size(), triangles.size(), false);
            triangles.insert(triangles.end(),geo.m_triangles.begin(), geo.m_triangles.end());
            materials.insert(materials.end(),geo.m_materials.begin(), geo.m_materials.end());
            meshInfoList.emplace_back(nodes.size(), (int)meshInfo.smoothShading);
            nodes.insert(nodes.end(),geo.m_nodes.begin(), geo.m_nodes.end());
        }

        GLsizeiptr materialsVecSize = materials.size() * sizeof(Material);
        GLsizeiptr trianglesVecSize = triangles.size() * sizeof(Triangle);
        GLsizeiptr nodesVecSize = nodes.size() * sizeof(Node);
        GLsizeiptr totalSize = 0;

        glGenBuffers(1, &m_ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, trianglesVecSize + nodesVecSize + materialsVecSize, nullptr, GL_STATIC_DRAW);

        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, m_ssbo, totalSize , trianglesVecSize);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, totalSize, trianglesVecSize, triangles.data());
        totalSize += trianglesVecSize;
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, m_ssbo, totalSize , nodesVecSize);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, totalSize, nodesVecSize, nodes.data());
        totalSize += nodesVecSize;
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 3, m_ssbo, totalSize, materialsVecSize);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, totalSize, materialsVecSize, materials.data());

        m_compShader->use();
        m_compShader->setUniform1i("u_nMeshes", meshInfoList.size());
        m_compShader->setUniform2fv("meshInfo", meshInfoList);
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
        m_compShader->setUniform1i("u_jitter", renderOptions.jitter);
        m_compShader->setUniform1i("u_enableSkybox", renderOptions.enableSkybox);
        m_compShader->setUniform1i("u_showSkybox", renderOptions.showSkybox);
        m_compShader->setUniform1i("u_smoothShading", renderOptions.smoothShading);
        m_compShader->setUniform1f("u_skyboxIntensity", renderOptions.skyboxIntensity);
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