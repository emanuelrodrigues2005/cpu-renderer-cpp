#pragma once

#include <string>
#include <vector>

namespace cg {

struct Vertex {
    float x = 0.0F;
    float y = 0.0F;
    float z = 0.0F;
};

struct Triangle {
    int a = 0;
    int b = 0;
    int c = 0;
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
};

bool loadMeshFromFile(const std::string& path, Mesh& mesh, std::string& error);

}
