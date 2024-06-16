//
// Created by Dusha on 25/05/2024.
//

#pragma once

#include <string>
#include <glad/glad.h>

namespace rt {

    class Shader {
    public:
        Shader(const std::string &vertPath, const std::string &fragPath);
        void use();
        void disuse();
        static void checkCompileErrors(GLuint shader, std::string type);
        static void parseShaderFile(const std::string &filePath,  std::string& sourceCode);
        void setUniform1i(const std::string& name, int i);
        void setUniform1f(const std::string& name, float f);

    private:
        GLuint compileShader(const char *vertCode, const char *fragCode);

        GLuint m_program;
    };

} // rt
