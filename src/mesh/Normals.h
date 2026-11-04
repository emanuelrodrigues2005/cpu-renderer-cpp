#pragma once

#include <vector>

#include "math/Vec3.h"
#include "mesh/Mesh.h"

namespace cg {

struct MeshNormals {
    std::vector<Vec3> face;
    std::vector<Vec3> vertex;
};

MeshNormals computeMeshNormals(const Mesh& mesh);

}
