//
// Created by Dusha on 25/05/2024.
//

#pragma once

#include <string>
#include <glad/glad.h>

namespace rt {

    class ComputeShader {
    public:
        ComputeShader(const std::string &compPath);
        void use();
        void disuse();

    private:
        GLuint compileShader(const char *compCode);
        GLuint m_program;
    };

} // lgl
