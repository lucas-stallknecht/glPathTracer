//
// Created by Dusha on 07/06/2024.
//

#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace rt {

    struct Vertex {
        glm::vec3 position;
        float pad1;
        glm::vec3 normal;
        float pad2;
    }; // 32 bytes

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
        Vertex v0;
        Vertex v1;
        Vertex v2;
        glm::vec3 centroid;
        int matIndex;
    };

    struct Node{
        // axis aligned bounding box
        glm::vec3 aabbMin;
        unsigned int leftChild;
        glm::vec3 aabbMax;
        unsigned int rightChild;
        unsigned int firstPrim;
        unsigned int primCount;
        int hitLink;
        int missLink;
    };

    class GeometryManager {
    public:
        GeometryManager(const std::string& objPath, int depth, bool log=false);
        static std::vector<Sphere> loadRTSpheres(bool log=false);
        void buildBVH(bool log=false);
        void traverseBVH(unsigned int index);

        std::vector<Triangle> m_triangles;
        std::vector<Node> m_nodes;
        std::vector<Material> m_materials;

    private:
        void loadTrianglesFromFile(const std::string& objPath, bool log=false);

        void updateNodeBounds(unsigned int nodeIndex);
        void subdivide(unsigned int nodeIndex, int currentDepth);
        int getRightChild(unsigned int parentIndex, unsigned int requestorIndex);
        void buildLinks(unsigned int nodeIndex);

        int m_nodesUsed = 1;
        int m_maxDepth;
        // gives the parentIndex for a node
        // has been created to avoid exceeding 48 bytes for a node struct
        std::vector<int> m_parents;
    };
};
