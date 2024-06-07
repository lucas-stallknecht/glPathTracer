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

    void ComputeShader::setUniform1i(const std::string& name, int i){
        GLint index = glGetUniformLocation(m_program, name.c_str());
        if(index > -1){
            glUniform1i(index, i);
        }
    }

    void ComputeShader::setUniform1f(const std::string& name, float f){
        GLint index = glGetUniformLocation(m_program, name.c_str());
        if(index > -1){
            glUniform1f(index, f);
        }
    }

    void ComputeShader::setUniformVec3(const std::string& name, glm::vec3 v){
        GLint index = glGetUniformLocation(m_program, name.c_str());
        if(index > -1){
            glUniform3fv(index, 1, &v[0]);
        }
    }
    void ComputeShader::setUniformMat4(const std::string& name, glm::mat4 m){
        GLint index = glGetUniformLocation(m_program, name.c_str());
        if(index > -1){
            glUniformMatrix4fv(index, 1, GL_FALSE, &m[0][0]);
        }
    }

} // rt