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
        float metallic;
        glm::vec2 padding;
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
        float padding[2];
    };

    class Mesh {
    public:
        Mesh(const std::string &objPath, int depth, int matOffset, int triOffset, bool log=false);
        void buildBVH(bool log=false);
        void traverseBVH(unsigned int index);

        std::vector<Triangle> m_triangles;
        std::vector<Node> m_nodes;
        std::vector<Material> m_materials;

    private:
        void loadTrianglesFromFile(const std::string& objPath, bool log=false);
        void updateNodeBounds(unsigned int nodeIndex);
        void subdivide(unsigned int nodeIndex, int currentDepth);
        float giveSplitPos(int axis, Node node);

        int m_nodesUsed = 1;
        int m_maxDepth;
        // since all the triangles are in the same buffer, we need to offset the indices
        int m_triangleOffset;
        int m_materialOffset;
    };
};
