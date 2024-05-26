//
// Created by Dusha on 25/05/2024.
//

#include "Shader.hpp"
#include <glad/glad.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>


namespace lgl {

    Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) : m_id(0)
    {
        std::string vertexCode, fragmentCode;
        const bool parseSuccess = parseShader(vertexPath, fragmentPath, vertexCode, fragmentCode);
        if(parseSuccess){
            m_id = createShader(vertexCode, fragmentCode);
        }
    }

    bool Shader::parseShader(const std::string& vertexPath, const std::string& fragmentPath,
                               std::string& vertexCode, std::string& fragmentCode)
    {
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode   = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch(std::ifstream::failure e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
            return false;
        }
        return true;
    }

    unsigned int Shader::compileShader(unsigned int shaderType, const std::string &source) {

        unsigned int shaderId = glCreateShader(shaderType);
        const char* src = source.c_str();

        glShaderSource(shaderId, 1, &src, nullptr);
        glCompileShader(shaderId);

        int  success;
        char infoLog[512];
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(shaderId, 512, nullptr, infoLog);
            std::cout << "ERROR::SHADER::" << shaderType << "::COMPILATION_FAILED\n" << infoLog << std::endl;
            return 0;
        }

        return shaderId;
    }

    unsigned int Shader::createShader(const std::string& vertexCode, const std::string& fragmentCode){
        unsigned int programId = glCreateProgram();
        unsigned int vid = compileShader(GL_VERTEX_SHADER, vertexCode);
        unsigned int fid = compileShader(GL_FRAGMENT_SHADER, fragmentCode);
        glAttachShader(programId, vid);
        glAttachShader(programId, fid);

        glLinkProgram(programId);
        glValidateProgram(programId);

        glDeleteShader(vid);
        glDeleteShader(fid);

        return programId;
    }

    void Shader::bind(){
        glUseProgram(m_id);
    }
    void Shader::unbind(){
        glUseProgram(0);
    }

    void Shader::setUniformVec4(const std::string& name, float f1, float f2, float f3, float f4){
        GLint loc = glGetUniformLocation(m_id, name.c_str());
        glUniform4f(loc, f1, f2, f3, f4);
    }


} // lgl