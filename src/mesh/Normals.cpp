#include "mesh/Normals.h"

#include <cmath>
#include <cstddef>

namespace cg {

MeshNormals computeMeshNormals(const Mesh& mesh) {
    MeshNormals normals;
    normals.face.assign(mesh.triangles.size(), Vec3{});
    normals.vertex.assign(mesh.vertices.size(), Vec3{});

    for (std::size_t i = 0; i < mesh.triangles.size(); ++i) {
        const Triangle& triangle = mesh.triangles[i];
        if (triangle.a < 0 || triangle.b < 0 || triangle.c < 0 ||
            triangle.a >= static_cast<int>(mesh.vertices.size()) ||
            triangle.b >= static_cast<int>(mesh.vertices.size()) ||
            triangle.c >= static_cast<int>(mesh.vertices.size())) {
            continue;
        }

        const Vertex& va = mesh.vertices[triangle.a];
        const Vertex& vb = mesh.vertices[triangle.b];
        const Vertex& vc = mesh.vertices[triangle.c];
        const Vec3 a{va.x, va.y, va.z};
        const Vec3 b{vb.x, vb.y, vb.z};
        const Vec3 c{vc.x, vc.y, vc.z};
        const Vec3 normal = (b - a).cross(c - a);
        if (!std::isfinite(normal.x) || !std::isfinite(normal.y) || !std::isfinite(normal.z) ||
            normal.length() == 0.0F) {
            continue;
        }

        normals.face[i] = normal.normalized();
        normals.vertex[triangle.a] = normals.vertex[triangle.a] + normals.face[i];
        normals.vertex[triangle.b] = normals.vertex[triangle.b] + normals.face[i];
        normals.vertex[triangle.c] = normals.vertex[triangle.c] + normals.face[i];
    }

    for (Vec3& normal : normals.vertex) {
        normal = normal.normalized();
    }

    return normals;
}

}
