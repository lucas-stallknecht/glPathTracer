//
// Created by Dusha on 07/06/2024.
//
#include "Loader.hpp"
#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
// https://github.com/tinyobjloader/tinyobjloader/tree/release
#include "tiny_obj_loader.h"

namespace rt {

    std::vector<Sphere> Loader::loadRTSpheres(bool log) {
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


    std::vector<Triangle> Loader::loadTrianglesFromFile(const std::string &objPath, bool log) {

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
                        triangle.v0[0] = vx;
                        triangle.v0[1] = vy;
                        triangle.v0[2] = vz;
                    } else if (v == 1) {
                        triangle.v1[0] = vx;
                        triangle.v1[1] = vy;
                        triangle.v1[2] = vz;
                    } else {
                        triangle.v2[0] = vx;
                        triangle.v2[1] = vy;
                        triangle.v2[2] = vz;
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
} // rt