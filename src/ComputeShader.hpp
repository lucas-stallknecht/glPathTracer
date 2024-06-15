//
// Created by Dusha on 25/05/2024.
//

#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

namespace rt {

    class ComputeShader {
    public:
        ComputeShader(const std::string &compPath);
        void use();
        void disuse();
        void setUniform1i(const std::string& name, int i);
        void setUniform1f(const std::string& name, float f);
        void setUniform2fv(const std::string& name, std::vector<glm::vec2> v);
        void setUniformVec3(const std::string& name, glm::vec3 v);
        void setUniformMat4(const std::string& name, glm::mat4 m);
        void logComputeShaderCapabilities();

    private:
        bool shouldAccumulate = true;
        GLuint compileShader(const char *compCode);
        GLuint m_program;
    };

} // rt
