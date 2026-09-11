#include "Renderer.h"

#include "Projection.h"
#include "Rasterizer.h"

namespace cg {

namespace {

bool projectVertex(const Vertex& vertex, const Camera& camera, int width, int height,
                   ScreenPoint& screen) {
    const Vec3 world{vertex.x, vertex.y, vertex.z};
    return projectToScreen(camera.worldToView(world), camera, width, height, screen);
}

void renderFilled(const Mesh& mesh, const Camera& camera, Canvas& canvas) {
    for (const Triangle& triangle : mesh.triangles) {
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

}
