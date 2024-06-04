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

    class Renderer {

    public:
        Renderer(int width, int height);
        void draw();
        void updateCamera(glm::vec3 camPos, glm::vec3 camDir, glm::mat4 invProjection);
        void resetAccumulation();

    private:
        void initQuadOutput();
        std::vector<Sphere> initRTSpheres();
        void initModel(const std::string &modelPath);

        const int VP_WIDTH, VP_HEIGHT;
        std::unique_ptr<Shader> m_shader;
        std::unique_ptr<ComputeShader> m_compShader;
        GLuint m_vao;
        GLuint m_ssbo;
        GLuint m_rtTexture;
        int m_frames = 0;
    };

} // lgl
