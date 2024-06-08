//
// Created by Dusha on 07/06/2024.
//
#include "GeometryManager.hpp"
#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
// https://github.com/tinyobjloader/tinyobjloader/tree/release
#include "tiny_obj_loader.h"

namespace rt {

    std::vector<Sphere> GeometryManager::loadRTSpheres(bool log) {
        std::vector<Sphere> spheres;

        spheres.push_back(Sphere{
                {-0.75f, 0.0f, 0.0f}, 0.2f,
                {{0.77f, 0.55f, 1.0f}, 0.0f, 0.0f, 1.0f, {0.0f, 0.0f}}
        });
        spheres.push_back(Sphere{
                {-0.25f, 0.0f, 0.0f}, 0.2f,
                {{0.77f, 0.55f, 1.0f}, 0.0f, 0.0f, 0.75f, {0.0f, 0.0f}}
        });
        spheres.push_back(Sphere{
                {0.25f, 0.0f, 0.0f}, 0.2f,
                {{0.77f, 0.55f, 1.0f}, 0.0f, 0.0f, 0.5f, {0.0f, 0.0f}}
        });
        spheres.push_back(Sphere{
                {0.75f, 0.0f, 0.0f}, 0.2f,
                {{0.77f, 0.55f, 1.0f}, 0.0f, 0.0f, 0.25f, {0.0f, 0.0f}}
        });

        if (log) {
            std::cout << "Size of material struct : " << sizeof(Material) << std::endl;
            std::cout << "Size of sphere struct : " << sizeof(Sphere) << std::endl;
            std::cout << "Size of spheres vector : " << spheres.size() * sizeof(Sphere) << std::endl;
        }

        return spheres;
    }

    std::vector<Triangle> GeometryManager::loadTrianglesFromFile(const std::string &objPath, bool log) {

        std::vector<Triangle> triangles;
        Material glowingTest{
                {0.9f, 0.9f, 0.9f},
                0.0,
                0.3,
                0.0,
                {7.7, 7.7}
        };

        tinyobj::ObjReaderConfig reader_config;
        tinyobj::ObjReader reader;

        if (!reader.ParseFromFile(objPath, reader_config)) {
            if (!reader.Error().empty()) {
                std::cerr << "TinyObjReader: " << reader.Error();
            }
            exit(1);
        }
        if (!reader.Warning().empty()) {
            std::cout << "TinyObjReader: " << reader.Warning();
        }

        auto &attrib = reader.GetAttrib();
        auto &shapes = reader.GetShapes();

        // Loop over shapes
        for (size_t s = 0; s < shapes.size(); s++) {
            // Loop over faces (polygon)
            size_t index_offset = 0;
            for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
                Triangle triangle;

                // Loop over vertices in the face.
                for (size_t v = 0; v < 3; v++) {
                    // access to vertex
                    tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                    tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                    tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                    tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

                    if (v == 0) {
                        triangle.v0 = {vx, vy, vz};
                    } else if (v == 1) {
                        triangle.v1 = {vx, vy, vz};
                    } else {
                        triangle.v2 = {vx, vy, vz};
                    }
                }

                // Set the material for the triangle
                triangle.material = glowingTest;

                // Add the triangle to the vector
                triangles.push_back(triangle);

                index_offset += 3;
            }
        }

        if (log) {
            std::cout << "Number of triangles : " << triangles.size() << std::endl;
            std::cout << "Size of triangle struct: " << sizeof(Triangle) << std::endl;
            std::cout << "Size of triangles vector: " << triangles.size() * sizeof(Triangle) << std::endl;
        }

        return triangles;
    }

    std::vector<Node> GeometryManager::buildBVH(std::vector<Triangle>& triangles, unsigned int nBoxes){

        std::vector<Node> nodes(2*nBoxes - 1);

        for(auto tri : triangles)
            tri.centroid = (tri.v0 + tri.v1 + tri.v2) * 0.333f;

        Node& root = nodes[0];
        root.leftChild = root.rightChild = 0;
        root.firstPrim = 0;
        root.primCount = triangles.size();

    }

    void GeometryManager::updateNodeBounds(std::vector<Triangle>& triangles, std::vector<Node>& nodes, unsigned int nodeIndex){
        Node& node = nodes[nodeIndex];
        node.aabbMin = glm::vec3( 1e30f );
        node.aabbMax = glm::vec3( -1e30f );
        for (unsigned int first = node.firstPrim, i = 0; i < node.primCount; i++)
        {
            Triangle& leafTri = triangles[first + i];
            node.aabbMin = glm::min( node.aabbMin, leafTri.v0 );
            node.aabbMin = glm::min( node.aabbMin, leafTri.v1 );
            node.aabbMin = glm::min( node.aabbMin, leafTri.v2 );
            node.aabbMax = glm::max( node.aabbMax, leafTri.v0 );
            node.aabbMax = glm::max( node.aabbMax, leafTri.v1 );
            node.aabbMax = glm::max( node.aabbMax, leafTri.v2 );
        }
    }

    void GeometryManager::subdivide(std::vector<Triangle>& triangles, std::vector<Node>& nodes, unsigned int nodeIndex){
        Node& node = nodes[nodeIndex];
        glm::vec3 extent = node.aabbMax - node.aabbMin;
        int axis = 0;
        if(extent.y > extent.x)
            axis = 1;
        if(extent.z > extent[axis])
            axis = 2;
        float splitPos = node.aabbMin[axis] + extent[axis] * 0.5f;


        unsigned int i = node.firstPrim;
        unsigned int j = i + node.primCount - 1;
        while (i <= j)
        {
            if (triangles[i].centroid[axis] < splitPos)
                i++;
            else
                std::swap( triangles[i], triangles[j--] );
        }


        unsigned int leftCount = i - node.firstPrim;
        if (leftCount == 0 || leftCount == node.primCount) return;
        // create child nodes
        int leftChildIdx = m_nodesUsed++;
        int rightChildIdx = m_nodesUsed++;
        node.leftChild = leftChildIdx;
        nodes[leftChildIdx].firstPrim = node.firstPrim;
        nodes[leftChildIdx].primCount = leftCount;
        nodes[rightChildIdx].firstPrim = i;
        nodes[rightChildIdx].primCount = node.primCount - leftCount;
        node.primCount = 0;
        updateNodeBounds(triangles, nodes, leftChildIdx);
        updateNodeBounds(triangles, nodes, rightChildIdx);
        subdivide(triangles, nodes, leftChildIdx);
        subdivide(triangles, nodes, rightChildIdx);
    }

} // rt