//
// Created by Dusha on 25/05/2024.
//

#include "ComputeShader.hpp"
#include "Shader.hpp"
#include <glad/glad.h>
#include <string>
#include <iostream>
#include <sstream>

namespace rt {

    ComputeShader::ComputeShader(const std::string &compPath) : m_program(
            0) {
        std::string vertCode, fragCode, compCode;
        Shader::parseShaderFile(compPath, compCode);

        m_program = compileShader(compCode.c_str());
    }

    GLuint ComputeShader::compileShader(const char *compCode) {

        GLuint compute = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(compute, 1, &compCode, nullptr);
        glCompileShader(compute);
        Shader::checkCompileErrors(compute, "COMPUTE");

        GLuint program = glCreateProgram();
        glAttachShader(program, compute);
        glLinkProgram(program);
        Shader::checkCompileErrors(program, "PROGRAM");

        glDeleteShader(compute);

        return program;
    }

    void ComputeShader::use() {
        glUseProgram(m_program);
    }

    void ComputeShader::disuse()  {
        glUseProgram(0);
    }


} // lgl