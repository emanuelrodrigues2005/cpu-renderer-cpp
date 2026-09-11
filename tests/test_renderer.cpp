#include <algorithm>
#include <cstdint>
#include <string>

#include "pipeline/Camera.h"
#include "canvas/MemoryCanvas.h"
#include "mesh/Mesh.h"
#include "pipeline/Renderer.h"
#include "test_util.h"

namespace {

cg::Camera cameraSintetica() {
    cg::Camera camera;
    camera.u = cg::Vec3{1.0F, 0.0F, 0.0F};
    camera.v = cg::Vec3{0.0F, 1.0F, 0.0F};
    camera.n = cg::Vec3{0.0F, 0.0F, 1.0F};
    camera.c = cg::Vec3{0.0F, 0.0F, 0.0F};
    camera.d = 5.0F;
    camera.hx = 2.0F;
    camera.hy = 2.0F;
    return camera;
}

cg::Mesh malhaTriangulo(const cg::Vertex& a, const cg::Vertex& b, const cg::Vertex& c) {
    cg::Mesh mesh;
    mesh.vertices = {a, b, c};
    mesh.triangles.push_back(cg::Triangle{0, 1, 2});
    return mesh;
}

bool white(const cg::MemoryCanvas& canvas, int x, int y) {
    return canvas.pixelAt(x, y) == cg::kWhite;
}

int countWhite(const cg::MemoryCanvas& canvas) {
    int count = 0;
    for (int y = 0; y < canvas.height(); ++y) {
        for (int x = 0; x < canvas.width(); ++x) {
            if (white(canvas, x, y)) {
                ++count;
            }
        }
    }
    return count;
}

void renderiza_triangulo_preenchido() {
    cg::MemoryCanvas canvas{800, 600};
    const cg::Mesh mesh =
        malhaTriangulo({0.0F, 0.0F, 5.0F}, {1.5F, 0.0F, 5.0F}, {0.0F, 1.5F, 5.0F});

    cg::renderMesh(mesh, cameraSintetica(), canvas);

    CHECK(white(canvas, 400, 75));
    CHECK(white(canvas, 400, 300));
    CHECK(white(canvas, 700, 300));
    CHECK(white(canvas, 450, 150));
    CHECK(!white(canvas, 399, 150));
    CHECK(!white(canvas, 501, 150));
    CHECK(!white(canvas, 701, 300));
}

void triangulo_atras_da_camera_e_descartado() {
    cg::MemoryCanvas canvas{800, 600};
    const cg::Mesh mesh =
        malhaTriangulo({0.0F, 0.0F, 5.0F}, {1.5F, 0.0F, 5.0F}, {0.0F, 1.5F, -1.0F});

    cg::renderMesh(mesh, cameraSintetica(), canvas);

    CHECK_EQ(countWhite(canvas), 0);
}

void modelos_reais_renderizam_com_a_camera_do_pdf() {
    cg::Camera camera;
    std::string error;
    CHECK(cg::loadCameraFromFile("camera/camera.txt", camera, error));

    const char* paths[] = {"models/triangulo.byu", "models/piramide.byu", "models/maca.byu",
                           "models/maca2.byu",     "models/vaso.byu",     "models/calice2.byu"};

    for (const char* path : paths) {
        cg::Mesh mesh;
        CHECK(cg::loadMeshFromFile(path, mesh, error));

        cg::MemoryCanvas canvas{800, 600};
        cg::renderMesh(mesh, camera, canvas);

        CHECK(countWhite(canvas) > 0);
        CHECK(!white(canvas, 0, 0));
        CHECK(!white(canvas, 799, 599));
    }
}

void modo_pontos_desenha_apenas_vertices() {
    cg::MemoryCanvas canvas{800, 600};
    const cg::Mesh mesh =
        malhaTriangulo({0.0F, 0.0F, 5.0F}, {1.5F, 0.0F, 5.0F}, {0.0F, 1.5F, 5.0F});

    cg::renderMesh(mesh, cameraSintetica(), canvas, cg::RenderMode::Points);

    CHECK_EQ(countWhite(canvas), 3);
    CHECK(white(canvas, 400, 75));
    CHECK(white(canvas, 400, 300));
    CHECK(white(canvas, 700, 300));
    CHECK(!white(canvas, 450, 150));
}

void modo_arame_desenha_arestas() {
    cg::MemoryCanvas canvas{800, 600};
    const cg::Mesh mesh =
        malhaTriangulo({0.0F, 0.0F, 5.0F}, {1.5F, 0.0F, 5.0F}, {0.0F, 1.5F, 5.0F});

    cg::renderMesh(mesh, cameraSintetica(), canvas, cg::RenderMode::Wireframe);

    CHECK(white(canvas, 550, 300));
    CHECK(white(canvas, 400, 187));
    CHECK(!white(canvas, 450, 150));
    CHECK(!white(canvas, 500, 250));
}

void modelos_com_presets_preenchem_a_janela() {
    const struct {
        const char* model;
        const char* camera;
    } cases[] = {
        {"models/triangulo.byu", "camera/presets/triangulo.txt"},
        {"models/piramide.byu", "camera/presets/piramide.txt"},
        {"models/maca.byu", "camera/presets/maca.txt"},
        {"models/maca2.byu", "camera/presets/maca2.txt"},
        {"models/vaso.byu", "camera/presets/vaso.txt"},
        {"models/calice2.byu", "camera/presets/calice2.txt"},
    };

    for (const auto& testCase : cases) {
        cg::Camera camera;
        std::string error;
        CHECK(cg::loadCameraFromFile(testCase.camera, camera, error));

        cg::Mesh mesh;
        CHECK(cg::loadMeshFromFile(testCase.model, mesh, error));

        cg::MemoryCanvas canvas{800, 600};
        cg::renderMesh(mesh, camera, canvas);

        int top = canvas.height();
        int bottom = -1;
        for (int y = 0; y < canvas.height(); ++y) {
            for (int x = 0; x < canvas.width(); ++x) {
                if (white(canvas, x, y)) {
                    top = std::min(top, y);
                    bottom = std::max(bottom, y);
                }
            }
        }

        const int coverage = bottom >= 0 ? bottom - top + 1 : 0;
        CHECK(coverage >= canvas.height() * 60 / 100);
    }
}

}

int main() {
    renderiza_triangulo_preenchido();
    triangulo_atras_da_camera_e_descartado();
    modelos_reais_renderizam_com_a_camera_do_pdf();
    modo_pontos_desenha_apenas_vertices();
    modo_arame_desenha_arestas();
    modelos_com_presets_preenchem_a_janela();

    return test::failureCount() == 0 ? 0 : 1;
}
