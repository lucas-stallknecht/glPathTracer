//
// Created by Dusha on 25/05/2024.
//

#include "Renderer.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
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
        GLsizeiptr sphereVecSize = spheres.size() * sizeof(Sphere);


        GeometryManager helmet("../resources/dragon.obj", 23, 0, 0, true);
        std::vector<Triangle> triangles = helmet.m_triangles;
        std::vector<Material> materials = helmet.m_materials;
        std::vector<Node> nodes = helmet.m_nodes;
//        helmet.traverseBVH(0);

//        // ====== TEST SCENE ========
//        GeometryManager testScene("../resources/test_scene.obj", 1, 0, 0, false);
//        std::vector<Triangle> triangles = testScene.m_triangles;
//        std::vector<Material> materials = testScene.m_materials;
//        std::vector<Node> nodes = testScene.m_nodes;
//        testScene.traverseBVH(0);
//
//        // ====== MONKEY ========
//        GeometryManager monkey("../resources/suzanne_2.obj", 8, testScene.m_materials.size(),
//                               testScene.m_triangles.size(), testScene.m_nodes.size(),
//                               true, false);
//        // monkey.traverseBVH(0);
//
//        // ====== MERGE ========
//        std::vector<Triangle> triangles = testScene.m_triangles;
//        triangles.insert(triangles.end(),
//                         monkey.m_triangles.begin(),
//                         monkey.m_triangles.end());
//        std::vector<Node> nodes = testScene.m_nodes;
//        nodes.insert(nodes.end(),
//                     monkey.m_nodes.begin(),
//                     monkey.m_nodes.end());
//        std::vector<Material> materials = testScene.m_materials;
//        materials.insert(materials.end(),
//                         monkey.m_materials.begin(),
//                         monkey.m_materials.end());

        GLsizeiptr materialsVecSize = materials.size() * sizeof(Material);
        GLsizeiptr trianglesVecSize = triangles.size() * sizeof(Triangle);
        GLsizeiptr nodesVecSize = nodes.size() * sizeof(Node);


        glGenBuffers(1, &m_ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sphereVecSize + trianglesVecSize + nodesVecSize + materialsVecSize, nullptr, GL_STATIC_DRAW);
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, m_ssbo, 0 , sphereVecSize);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sphereVecSize, spheres.data());
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, m_ssbo, sphereVecSize , trianglesVecSize);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, sphereVecSize, trianglesVecSize, triangles.data());
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 3, m_ssbo, sphereVecSize + trianglesVecSize , nodesVecSize);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, sphereVecSize + trianglesVecSize, nodesVecSize, nodes.data());
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 4, m_ssbo, sphereVecSize + trianglesVecSize + nodesVecSize, materialsVecSize);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, sphereVecSize + trianglesVecSize + nodesVecSize, materialsVecSize, materials.data());

        m_compShader->use();
        m_compShader->setUniform1i("u_nSpheres", spheres.size());
        m_compShader->setUniform1i("u_nTriangles", triangles.size());


        // Skybox
        std::vector<std::string> faces
        {
            "../resources/skybox_paris/px.png",    // right
            "../resources/skybox_paris/nx.png",     // left
            "../resources/skybox_paris/py.png",      // top
            "../resources/skybox_paris/ny.png",   // bottom
            "../resources/skybox_paris/pz.png",    // front
            "../resources/skybox_paris/nz.png"      // back
        };
        GLuint cubemapTexture = loadCubemap(faces);

    }

    GLuint Renderer::loadCubemap(std::vector<std::string> faces)
    {
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

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

        return textureID;
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
        m_compShader->setUniform1i("u_jitter", renderOptions.jitter);
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