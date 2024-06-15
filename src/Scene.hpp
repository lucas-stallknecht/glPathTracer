//
// Created by Dusha on 15/06/2024.
//

#pragma once
#include "Mesh.hpp"
#include <string>

namespace rt {

    struct MeshInfo {
        std::string objPath;
        int depth;
        int smoothShading;
    };

    class Scene {
    public:
        explicit Scene(const std::string& sceneFilePath);
        void readSceneFile(const std::string& sceneFilePath);

        std::vector<MeshInfo> m_meshesInfo;
        std::vector<Triangle> m_triangles;
        std::vector<Material> m_materials;
        std::vector<Node> m_nodes;
    };

} // rt

