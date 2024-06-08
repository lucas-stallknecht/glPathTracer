//
// Created by Dusha on 07/06/2024.
//

#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace rt {

    struct Material {
        glm::vec3 diffuseCol;
        float emissiveStrength;
        float roughness;
        float specularPerc;
        glm::vec2 padding;
    };

    struct Sphere {
        glm::vec3 position;
        float radius;
        Material material;
    };

    struct Triangle{
        glm::vec3 v0;
        float padding1;
        glm::vec3 v1;
        float padding2;
        glm::vec3 v2;
        float padding3;
        glm::vec3 centroid;
        float padding4;
        Material material;
    };

    struct Node{
        // axis aligned bounding box
        glm::vec3 aabbMin;
        glm::vec3 aabbMax;
        unsigned int leftChild;
        unsigned int rightChild;
        unsigned int firstPrim;
        unsigned int primCount;
    };

    class GeometryManager {
    private:
        int m_nodesUsed = 1;
    public:
        static std::vector<Sphere> loadRTSpheres(bool log=false);
        static std::vector<Triangle> loadTrianglesFromFile(const std::string& objPath, bool log=false);
        std::vector<Node> buildBVH(std::vector<Triangle>& triangles, unsigned int nBoxes);
        void updateNodeBounds(std::vector<Triangle>& triangles, std::vector<Node>& nodes, unsigned int nodeIndex);
        void subdivide(std::vector<Triangle>& triangles, std::vector<Node>& nodes, unsigned int nodeIndex);
    };
};
