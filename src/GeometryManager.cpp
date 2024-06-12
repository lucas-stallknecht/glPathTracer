//
// Created by Dusha on 07/06/2024.
//
#include "GeometryManager.hpp"
#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
// https://github.com/tinyobjloader/tinyobjloader/tree/release
#include "tiny_obj_loader.h"

namespace rt {

    GeometryManager::GeometryManager(const std::string &objPath, int depth, int matOffset, int triOffset, bool log)
    : m_nodes(std::pow(2, depth + 1) - 1), m_maxDepth(depth)
    ,m_materialOffset(matOffset) ,m_triangleOffset(triOffset) {
        loadTrianglesFromFile(objPath, log);


        buildBVH(log);
    }

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

    void GeometryManager::loadTrianglesFromFile(const std::string &objPath, bool log) {

        std::vector<Triangle> triangles;
        std::vector<Material> materials;

        tinyobj::ObjReaderConfig reader_config;
        reader_config.mtl_search_path = "../resources/";
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
        auto &mats = reader.GetMaterials();

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
                        triangle.v0.position = {vx, vy, vz};
                    } else if (v == 1) {
                        triangle.v1.position = {vx, vy, vz};
                    } else {
                        triangle.v2.position = {vx, vy, vz};
                    }



                    if (idx.normal_index >= 0) {
                        tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
                        tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
                        tinyobj::real_t nz = attrib.normals[3*size_t(idx.normal_index)+2];

                        if (v == 0) {
                            triangle.v0.normal = {nx, ny, nz};
                        } else if (v == 1) {
                            triangle.v1.normal = {nx, ny, nz};
                        } else {
                            triangle.v2.normal = {nx, ny, nz};
                        }
                    }

                }

                triangle.matIndex = m_materialOffset + shapes[s].mesh.material_ids[f];

                // Add the triangle to the vector
                triangles.push_back(triangle);

                index_offset += 3;
            }
        }

        for (const auto &mat: mats){
            // loop over materials
            Material m{
                    glm::vec3(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]),
                    mat.emission[0],
                    mat.roughness,
                    mat.metallic,
                    glm::vec2(8.8,8.8)
            };
            materials.push_back(m);
        }


        if (log) {
            std::cout << "Number of triangles : " << triangles.size() << std::endl;
            std::cout << "Size of triangle struct: " << sizeof(Triangle) << std::endl;
            std::cout << "Size of triangles vector: " << triangles.size() * sizeof(Triangle) << std::endl;
            std::cout << "Number of materials : " << materials.size() << std::endl;
            std::cout << "Size of material struct: " << sizeof(Material) << std::endl;
            std::cout << "Size of materials vector: " << materials.size() * sizeof(Material) << std::endl;
        }

        m_triangles = triangles;
        m_materials = materials;
    }

    void GeometryManager::buildBVH(bool log){

        if (log) {
            std::cout << "Number of nodes : " << m_nodes.size() << std::endl;
            std::cout << "Size of node struct: " << sizeof(Node) << std::endl;
            std::cout << "Size of nodes vector: " << m_nodes.size() * sizeof(Node) << std::endl;
        }

        // Initialize the root node and start recursions
        for(auto& tri : m_triangles){
            tri.centroid = (tri.v0.position + tri.v1.position + tri.v2.position) * 0.333f;
        }

        Node& root = m_nodes[0];
        root.leftChild = root.rightChild = 0;
        root.firstPrim = m_triangleOffset + 0;
        root.primCount = m_triangles.size();

        updateNodeBounds(0);
        subdivide(0, 0);
    }

    void GeometryManager::updateNodeBounds(unsigned int nodeIndex){
        Node& node = m_nodes[nodeIndex];
        node.aabbMin = glm::vec3( 1e30f );
        node.aabbMax = glm::vec3( -1e30f );
        // checks all the triangles contained in a bounding box and adjusts the min and max pos
        for (unsigned int first = node.firstPrim, i = 0; i < node.primCount; i++)
        {
            Triangle& leafTri = m_triangles[first + i - m_triangleOffset];
            node.aabbMin = glm::min( node.aabbMin, leafTri.v0.position );
            node.aabbMin = glm::min( node.aabbMin, leafTri.v1.position );
            node.aabbMin = glm::min( node.aabbMin, leafTri.v2.position );
            node.aabbMax = glm::max( node.aabbMax, leafTri.v0.position );
            node.aabbMax = glm::max( node.aabbMax, leafTri.v1.position );
            node.aabbMax = glm::max( node.aabbMax, leafTri.v2.position );
        }
    }

    void GeometryManager::subdivide(unsigned int nodeIndex, int currentDepth) {
        Node& node = m_nodes[nodeIndex];

        glm::vec3 extent = node.aabbMax - node.aabbMin;
        // select the axis of the split plane
        int axis = 0;
        if (extent.y > extent.x) axis = 1;
        if (extent.z > extent[axis]) axis = 2;
        // middle of the extent ==> TODO uniform distribution across all bounding boxes
        float splitPos = node.aabbMin[axis] + extent[axis] * 0.5f;

        // puts all the triangles with centroid before the splitPos on the left
        // i will help determine the triangles count for the leftChild
        unsigned int i = node.firstPrim;
        unsigned int j = i + node.primCount - 1;
        while (i <= j) {
            if (m_triangles[i - m_triangleOffset].centroid[axis] < splitPos)
                i++;
            else
                std::swap(m_triangles[i - m_triangleOffset], m_triangles[j-- - m_triangleOffset]);
        }
        unsigned int leftCount = i - node.firstPrim;
        unsigned int rightCount = node.primCount - leftCount;

        if(currentDepth < m_maxDepth && leftCount > 1 && rightCount > 1){
            int leftChildIdx = m_nodesUsed++;
            int rightChildIdx = m_nodesUsed++;
            node.leftChild = leftChildIdx;
            node.rightChild = rightChildIdx;

            m_nodes[leftChildIdx].firstPrim = node.firstPrim;
            m_nodes[leftChildIdx].primCount = leftCount;

            m_nodes[rightChildIdx].firstPrim = i;
            m_nodes[rightChildIdx].primCount = rightCount;

            // the node is not a leaf anymore
            node.primCount = 0;

            // recalculate the bounding box for each child
            updateNodeBounds(leftChildIdx);
            updateNodeBounds(rightChildIdx);

            // build the tree recursively
            subdivide(leftChildIdx, currentDepth + 1);
            subdivide(rightChildIdx, currentDepth + 1);
        }
    }



    void GeometryManager::traverseBVH(unsigned int index) {
        Node& node = m_nodes[index];
        std::cout << "Index : " << index << " FirstPrim : " << node.firstPrim << " PrimCount : " << node.primCount
        << " Left child : " << node.leftChild << " Right child : " << node.rightChild << std::endl;

        if(node.primCount == 0){
            traverseBVH(node.leftChild);
            traverseBVH(node.rightChild);
        }
    }

} // rt