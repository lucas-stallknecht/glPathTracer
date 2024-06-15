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

    void ComputeShader::setUniform2fv(const std::string& name, std::vector<glm::vec2> v){
        GLint index = glGetUniformLocation(m_program, name.c_str());
        if(index > -1){
            glUniform2fv(index, v.size(), glm::value_ptr(v[0]));
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

    void ComputeShader::logComputeShaderCapabilities(){
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

} // rt