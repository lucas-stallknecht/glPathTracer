//
// Created by Dusha on 07/06/2024.
//

#pragma once
#include <vector>
#include <string>

namespace rt {

    struct Material {
        float diffuseCol[3];
        float emissiveStrength;
        float roughness;
        float specularPerc;
        float padding[2];
    };

    struct Sphere {
        float pos[3];
        float radius;
        Material material;
    };

    struct Triangle{
        float v0[3];
        float padding1;
        float v1[3];
        float padding2;
        float v2[3];
        float padding3;
        Material material;
    };

    class Loader {
    public:
        static std::vector<Sphere> loadRTSpheres(bool log=false);
        static std::vector<Triangle> loadTrianglesFromFile(const std::string& objPath, bool log=false);

    };
};
