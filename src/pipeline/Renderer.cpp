#include "pipeline/Renderer.h"

#include "mesh/Normals.h"
#include "pipeline/DepthBuffer.h"
#include "pipeline/Projection.h"
#include "pipeline/Rasterizer.h"
#include "pipeline/ShadedRasterizer.h"

namespace cg {

namespace {

bool triangleIsValid(const Mesh& mesh, const Triangle& triangle) {
    const int vertexCount = static_cast<int>(mesh.vertices.size());
    return triangle.a >= 0 && triangle.a < vertexCount && triangle.b >= 0 &&
           triangle.b < vertexCount && triangle.c >= 0 && triangle.c < vertexCount;
}

bool projectVertex(const Vertex& vertex, const Camera& camera, int width, int height,
                   ScreenPoint& screen) {
    const Vec3 world{vertex.x, vertex.y, vertex.z};
    return projectToScreen(camera.worldToView(world), camera, width, height, screen);
}

bool projectShadedVertex(const Vertex& vertex, const Vec3& normal, const Camera& camera,
                         int width, int height, ShadedVertex& shaded) {
    const Vec3 world{vertex.x, vertex.y, vertex.z};
    ProjectedVertex projected;
    if (!projectToScreenWithDepth(camera.worldToView(world), camera, width, height, projected)) {
        return false;
    }

    shaded.screen = projected.screen;
    shaded.invZ = projected.invZ;
    shaded.world = world;
    shaded.normal = normal;
    return true;
}

void renderFilled(const Mesh& mesh, const Camera& camera, Canvas& canvas) {
    for (const Triangle& triangle : mesh.triangles) {
        if (!triangleIsValid(mesh, triangle)) {
            continue;
        }
        ScreenPoint a;
        ScreenPoint b;
        ScreenPoint c;
        if (!projectVertex(mesh.vertices[triangle.a], camera, canvas.width(), canvas.height(), a) ||
            !projectVertex(mesh.vertices[triangle.b], camera, canvas.width(), canvas.height(), b) ||
            !projectVertex(mesh.vertices[triangle.c], camera, canvas.width(), canvas.height(), c)) {
            continue;
        }
        fillTriangle(canvas, a, b, c, kWhite);
    }
}

void renderPoints(const Mesh& mesh, const Camera& camera, Canvas& canvas) {
    for (const Vertex& vertex : mesh.vertices) {
        ScreenPoint point;
        if (projectVertex(vertex, camera, canvas.width(), canvas.height(), point)) {
            canvas.drawPixel(point.x, point.y, kWhite);
        }
    }
}

void renderWireframe(const Mesh& mesh, const Camera& camera, Canvas& canvas) {
    for (const Triangle& triangle : mesh.triangles) {
        if (!triangleIsValid(mesh, triangle)) {
            continue;
        }
        ScreenPoint a;
        ScreenPoint b;
        ScreenPoint c;
        if (!projectVertex(mesh.vertices[triangle.a], camera, canvas.width(), canvas.height(), a) ||
            !projectVertex(mesh.vertices[triangle.b], camera, canvas.width(), canvas.height(), b) ||
            !projectVertex(mesh.vertices[triangle.c], camera, canvas.width(), canvas.height(), c)) {
            continue;
        }
        drawLine(canvas, a, b, kWhite);
        drawLine(canvas, b, c, kWhite);
        drawLine(canvas, c, a, kWhite);
    }
}

void renderFilledPhong(const Mesh& mesh, const Camera& camera, const Lighting& lighting,
                       Canvas& canvas) {
    const MeshNormals normals = computeMeshNormals(mesh);

    DepthBuffer depth(canvas.width(), canvas.height());
    depth.clear();

    for (const Triangle& triangle : mesh.triangles) {
        if (!triangleIsValid(mesh, triangle)) {
            continue;
        }
        ShadedVertex a;
        ShadedVertex b;
        ShadedVertex c;
        if (!projectShadedVertex(mesh.vertices[triangle.a], normals.vertex[triangle.a], camera,
                                 canvas.width(), canvas.height(), a) ||
            !projectShadedVertex(mesh.vertices[triangle.b], normals.vertex[triangle.b], camera,
                                 canvas.width(), canvas.height(), b) ||
            !projectShadedVertex(mesh.vertices[triangle.c], normals.vertex[triangle.c], camera,
                                 canvas.width(), canvas.height(), c)) {
            continue;
        }
        fillTriangleShaded(canvas, depth, a, b, c, camera.c, lighting);
    }
}

}

void renderMesh(const Mesh& mesh, const Camera& camera, Canvas& canvas, RenderMode mode) {
    canvas.clear(kBlack);

    switch (mode) {
        case RenderMode::Filled:
            renderFilled(mesh, camera, canvas);
            break;
        case RenderMode::Points:
            renderPoints(mesh, camera, canvas);
            break;
        case RenderMode::Wireframe:
            renderWireframe(mesh, camera, canvas);
            break;
    }
}

void renderMesh(const Mesh& mesh, const Camera& camera, const Lighting& lighting, Canvas& canvas,
                RenderMode mode) {
    if (mode != RenderMode::Filled) {
        renderMesh(mesh, camera, canvas, mode);
        return;
    }

    canvas.clear(kBlack);
    renderFilledPhong(mesh, camera, lighting, canvas);
}

}
