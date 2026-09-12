#include "mesh/Mesh.h"

#include <fstream>
#include <utility>

namespace cg {

bool loadMeshFromFile(const std::string& path, Mesh& mesh, std::string& error) {
    std::ifstream input(path);
    if (!input) {
        error = "não foi possível abrir '" + path + "'";
        return false;
    }

    int vertexCount = 0;
    int triangleCount = 0;
    if (!(input >> vertexCount >> triangleCount)) {
        error = "cabeçalho inválido em '" + path + "'";
        return false;
    }
    if (vertexCount < 0 || triangleCount < 0) {
        error = "cabeçalho inválido em '" + path + "'";
        return false;
    }

    Mesh loaded;
    for (int i = 0; i < vertexCount; ++i) {
        Vertex vertex;
        if (!(input >> vertex.x >> vertex.y >> vertex.z)) {
            error = "vértice incompleto em '" + path + "'";
            return false;
        }
        loaded.vertices.push_back(vertex);
    }

    for (int i = 0; i < triangleCount; ++i) {
        Triangle triangle;
        if (!(input >> triangle.a >> triangle.b >> triangle.c)) {
            error = "triângulo incompleto em '" + path + "'";
            return false;
        }
        if (triangle.a < 1 || triangle.a > vertexCount || triangle.b < 1 ||
            triangle.b > vertexCount || triangle.c < 1 || triangle.c > vertexCount) {
            error = "índice fora do intervalo [1, " + std::to_string(vertexCount) + "] em '" + path +
                    "'";
            return false;
        }
        triangle.a -= 1;
        triangle.b -= 1;
        triangle.c -= 1;
        loaded.triangles.push_back(triangle);
    }

    mesh = std::move(loaded);
    return true;
}

}
