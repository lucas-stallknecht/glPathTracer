//
// Created by Dusha on 25/05/2024.
//

#include "Shader.hpp"
#include <glad/glad.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>


namespace rt {

    Shader::Shader(const std::string &vertPath, const std::string &fragPath) : m_program(
            0) {
        std::string vertCode, fragCode, compCode;
        parseShaderFile(vertPath, vertCode);
        parseShaderFile(fragPath, fragCode);

        m_program = compileShader(vertCode.c_str(), fragCode.c_str());
    }

    void Shader::parseShaderFile(const std::string &filePath,  std::string& sourceCode) {
        std::ifstream shaderFile;
        // ensure ifstream objects can throw exceptions:
        shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            // open files
            shaderFile.open(filePath);
            std::stringstream shaderStream;
            // read file's buffer contents into streams
            shaderStream << shaderFile.rdbuf();
            // close file handlers
            shaderFile.close();
            // convert stream into string
            sourceCode = shaderStream.str();
        }
        catch (std::ifstream::failure e) {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ : " << filePath << std::endl;
        }
    }

    void Shader::checkCompileErrors(GLuint shader, std::string type) {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog
                          << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog
                          << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }

    GLuint Shader::compileShader(const char *vertCode, const char *fragCode) {
        GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vertCode, nullptr);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");

        GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fragCode, nullptr);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");

        GLuint program = glCreateProgram();
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        glLinkProgram(program);
        checkCompileErrors(program, "PROGRAM");

        glDeleteShader(fragment);
        glDeleteShader(vertex);

        return program;
    }

    void Shader::use() {
        glUseProgram(m_program);
    }

    void Shader::disuse() {
        glUseProgram(0);
    }


} // rt