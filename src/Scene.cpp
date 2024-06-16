//
// Created by Dusha on 15/06/2024.
//

#include "Scene.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

namespace rt {
    Scene::Scene(const std::string &sceneFilePath) {
        readSceneFile(sceneFilePath);
    }

    void Scene::readSceneFile(const std::string &sceneFilePath) {

        std::ifstream sceneFile;
        sceneFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            sceneFile.open(sceneFilePath);

            if (!sceneFile.is_open()) {
                throw std::ios_base::failure("Failed to open the file." + sceneFilePath);
            }

            std::string line;
            while(std::getline(sceneFile, line)) {

                std::stringstream ssLine = std::stringstream(line);

                std::string prefix;
                ssLine >> prefix;

                if(prefix == "m-") {
                    std::string meshPath, depth, smoothShading;
                    ssLine >> meshPath >> depth >> smoothShading;
                    m_meshesInfo.push_back(MeshInfo{meshPath,
                                                    std::stoi(depth),
                                                    std::stoi(smoothShading)});
                }
            }

            sceneFile.close();
        }
        catch (std::ifstream::failure e) {
            // getLine will return an error with there is the eof but won't stop one step before, weird.
            if (!sceneFile.eof()){
                std::cout << "ERROR::SCENE::FILE_NOT_SUCCESFULLY_READ : " << sceneFilePath << " " << e.what() << std::endl;
            }
        }
    }
} // rt