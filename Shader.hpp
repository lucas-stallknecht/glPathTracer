//
// Created by Dusha on 25/05/2024.
//

#pragma once

#include <string>

namespace lgl {

    class Shader
    {
    public:
        Shader(const std::string& vertexPath, const std::string& fragmentPath);
        void bind();
        void unbind();

        void setUniformVec4(const std::string& name, float f1, float f2, float f3, float f4);

    private:
        bool parseShader(const std::string& vertexPath, const std::string& fragmentPath,
                           std::string& vertexCode,
                           std::string& fragmentCode);
        unsigned int compileShader(unsigned int shaderType, const std::string& source);
        unsigned int createShader(const std::string& vertexCode, const std::string& fragmentCode);

        unsigned int m_id;
    };

} // lgl
