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
        unsigned int leftChild;
        glm::vec3 aabbMax;
        unsigned int rightChild;
        unsigned int firstPrim;
        unsigned int primCount;
        glm::vec2 padding;
    };

    class GeometryManager {
    public:
        GeometryManager(const std::string& objPath, int depth, bool log=false);
        static std::vector<Sphere> loadRTSpheres(bool log=false);
        void buildBVH(bool log=false);
        void traverseBVH(unsigned int index);

        std::vector<Triangle> m_triangles;
        std::vector<Node> m_nodes;

    private:
        void loadTrianglesFromFile(const std::string& objPath, bool log=false);

        void updateNodeBounds(unsigned int nodeIndex);
        void subdivide(unsigned int nodeIndex, int currentDepth);

        int m_nodesUsed = 1;
        int m_maxDepth;
    };
};
