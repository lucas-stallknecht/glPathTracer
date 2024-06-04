//
// Created by Dusha on 25/05/2024.
//

#include "Renderer.hpp"
#include "Shader.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <iterator>
#include "imgui.h"
#include "OBJ_Loader.h"

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
        glGenBuffers(1, &m_ssbo);
        initQuadOutput();

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
        std::cout << "Size of a material struct : " << sizeof(Material) << std::endl;


        std::vector<Sphere> spheres = initRTSpheres();
        std::cout << "Size of sphere struct : " << sizeof(Sphere) << std::endl;
        std::cout << "Size of spheres vector : " << spheres.size() * sizeof(Sphere) << std::endl;
//        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_ssbo);
//        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, spheres.size() * sizeof(Sphere), spheres.data());

        m_compShader->use();
        m_compShader->setUniform1i("numSpheres", spheres.size());




        initModel("../resources/icosphere.obj");
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

    std::vector<Sphere> Renderer::initRTSpheres() {

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

        std::vector<Sphere> spheresVec;
        for(auto sph: spheres){
            spheresVec.push_back(sph);
        }
        std::cout << spheresVec.size() << std::endl;
        return spheresVec;
    };

    void Renderer::initModel(const std::string &modelPath) {
        objl::Loader loader;
        loader.LoadFile(modelPath);
        std::cout << loader.LoadedMeshes[0].MeshName << std::endl;

        struct Triangle{
            objl::Vector3 v0;
            objl::Vector3 v1;
            objl::Vector3 v2;
            int padding;
            Material material;
        };

        unsigned int numTriangles = loader.LoadedMeshes[0].Indices.size()/3;
        std::vector<Triangle> triangles(numTriangles);

        Material glowingTest{
                {0.9f, 0.9f, 0.9f},
                1.0,
                0.0,
                1.0,
                {7.7,7.7}
        };

        for(int i=0; i < numTriangles; i++){
            triangles[i].v0 = loader.LoadedMeshes[0].Vertices[loader.LoadedMeshes[0].Indices[i*3 + 0]].Position;
            triangles[i].v1 = loader.LoadedMeshes[0].Vertices[loader.LoadedMeshes[0].Indices[i*3 + 1]].Position;
            triangles[i].v2 = loader.LoadedMeshes[0].Vertices[loader.LoadedMeshes[0].Indices[i*3 + 2]].Position;
            triangles[i].material = glowingTest;
        }
        for(auto tri: triangles){
            std::cout << tri.v0.X << ' ' << tri.v0.Y << ' ' << tri.v0.Z << std::endl;
        }

        // glBufferData(GL_SHADER_STORAGE_BUFFER, 192+1440, nullptr, GL_STATIC_DRAW);
        std::cout << "Size of objl::Vector3 : " << sizeof(objl::Vector3) << std::endl;
        std::cout << "Size of triangle struct : " << sizeof(Triangle) << std::endl;
        std::cout << "Size of triangles vector : " << triangles.size() * sizeof(Triangle) << std::endl;
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_ssbo);
        // glBufferSubData(GL_SHADER_STORAGE_BUFFER, 192, triangles.size() * sizeof(Triangle), triangles.data());
        glBufferData(GL_SHADER_STORAGE_BUFFER, triangles.size() * sizeof(Triangle), triangles.data(), GL_STATIC_DRAW);

        m_compShader->use();
        m_compShader->setUniform1i("numTriangles", triangles.size());
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