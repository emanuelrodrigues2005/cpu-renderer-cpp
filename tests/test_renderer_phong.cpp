#include <cstdint>
#include <cstdlib>
#include <set>
#include <string>

#include "canvas/MemoryCanvas.h"
#include "mesh/Mesh.h"
#include "pipeline/Camera.h"
#include "pipeline/Renderer.h"
#include "shading/Lighting.h"
#include "shading/Phong.h"
#include "test_util.h"

namespace {

constexpr std::uint32_t kCorAmbiente = 0x6432C8U;

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

cg::Camera cameraEm(const cg::Vec3& centro) {
    cg::Camera camera = cameraSintetica();
    camera.c = centro;
    return camera;
}

cg::Mesh malhaTriangulo(float z) {
    cg::Mesh mesh;
    mesh.vertices = {{0.0F, 0.0F, z}, {1.5F, 0.0F, z}, {0.0F, 1.5F, z}};
    mesh.triangles.push_back(cg::Triangle{0, 1, 2});
    return mesh;
}

cg::Lighting ambiente(cg::Color ambient) {
    cg::Lighting lighting;
    lighting.light.ambient = ambient;
    lighting.light.color = cg::Color{0.0F, 0.0F, 0.0F};
    lighting.light.position = cg::Vec3{0.0F, 0.0F, 20.0F};
    lighting.material.ka = 1.0F;
    lighting.material.kd = cg::Vec3{0.0F, 0.0F, 0.0F};
    lighting.material.od = cg::Vec3{1.0F, 1.0F, 1.0F};
    lighting.material.ks = 0.0F;
    lighting.material.eta = 1.0F;
    return lighting;
}

cg::Lighting difusa(cg::Color corDaLuz, const cg::Vec3& posicao) {
    cg::Lighting lighting;
    lighting.light.ambient = cg::Color{0.0F, 0.0F, 0.0F};
    lighting.light.color = corDaLuz;
    lighting.light.position = posicao;
    lighting.material.ka = 0.0F;
    lighting.material.kd = cg::Vec3{1.0F, 1.0F, 1.0F};
    lighting.material.od = cg::Vec3{1.0F, 1.0F, 1.0F};
    lighting.material.ks = 0.0F;
    lighting.material.eta = 1.0F;
    return lighting;
}

int countNonBlack(const cg::MemoryCanvas& canvas) {
    int count = 0;
    for (int y = 0; y < canvas.height(); ++y) {
        for (int x = 0; x < canvas.width(); ++x) {
            if (canvas.pixelAt(x, y) != cg::kBlack) {
                ++count;
            }
        }
    }
    return count;
}

int colorCount(const cg::MemoryCanvas& canvas) {
    std::set<std::uint32_t> colors;
    for (int y = 0; y < canvas.height(); ++y) {
        for (int x = 0; x < canvas.width(); ++x) {
            const std::uint32_t color = canvas.pixelAt(x, y);
            if (color != cg::kBlack) {
                colors.insert(color);
            }
        }
    }
    return static_cast<int>(colors.size());
}

int channelOf(std::uint32_t packed, int index) {
    const int shift = 8 * (2 - index);
    return static_cast<int>((packed >> shift) & 0xFFU);
}

int maxChannelDifference(std::uint32_t lhs, std::uint32_t rhs) {
    int difference = 0;
    for (int index = 0; index < 3; ++index) {
        const int delta = std::abs(channelOf(lhs, index) - channelOf(rhs, index));
        if (delta > difference) {
            difference = delta;
        }
    }
    return difference;
}

void triangulo_sintetico_recebe_cor_ambiente_conhecida() {
    cg::MemoryCanvas canvas{800, 600};

    cg::renderMesh(malhaTriangulo(5.0F), cameraSintetica(), ambiente(cg::Color{100.0F, 50.0F, 200.0F}),
                   canvas);

    CHECK_EQ(canvas.pixelAt(400, 300), kCorAmbiente);
    CHECK_EQ(canvas.pixelAt(500, 280), kCorAmbiente);
    CHECK_EQ(canvas.pixelAt(0, 0), cg::kBlack);
    CHECK_EQ(canvas.pixelAt(799, 0), cg::kBlack);
    CHECK_EQ(canvas.pixelAt(0, 599), cg::kBlack);
    CHECK_EQ(canvas.pixelAt(799, 599), cg::kBlack);
}

void cor_difusa_conhecida_e_nao_branca() {
    cg::MemoryCanvas canvas{800, 600};

    cg::renderMesh(malhaTriangulo(5.0F), cameraSintetica(),
                   difusa(cg::Color{255.0F, 0.0F, 0.0F}, cg::Vec3{0.0F, 0.0F, 20.0F}), canvas);

    CHECK_EQ(canvas.pixelAt(400, 300), 0xFF0000U);
    CHECK_EQ(canvas.pixelAt(500, 280), 0xFF0000U);
    CHECK(canvas.pixelAt(400, 300) != cg::kWhite);
}

void zbuffer_distingue_cor_do_distante() {
    cg::Mesh perto = malhaTriangulo(5.0F);

    cg::Mesh longe;
    longe.vertices = {{0.0F, 0.0F, 10.0F}, {3.0F, 0.0F, 10.0F}, {0.0F, 3.0F, 10.0F}};
    longe.triangles.push_back(cg::Triangle{0, 1, 2});

    const cg::Lighting lighting =
        difusa(cg::Color{255.0F, 255.0F, 255.0F}, cg::Vec3{10.0F, 10.0F, 20.0F});
    const cg::Camera camera = cameraSintetica();

    cg::MemoryCanvas pertoSozinho{800, 600};
    cg::renderMesh(perto, camera, lighting, pertoSozinho);
    cg::MemoryCanvas longeSozinho{800, 600};
    cg::renderMesh(longe, camera, lighting, longeSozinho);

    const std::uint32_t corPertoCentro = pertoSozinho.pixelAt(400, 300);
    const std::uint32_t corPertoInterno = pertoSozinho.pixelAt(500, 280);
    const std::uint32_t corLongeCentro = longeSozinho.pixelAt(400, 300);
    const std::uint32_t corLongeInterno = longeSozinho.pixelAt(500, 280);
    CHECK(countNonBlack(longeSozinho) > 0);
    CHECK(corPertoCentro != cg::kBlack);
    CHECK(corLongeCentro != cg::kBlack);
    CHECK(corPertoInterno != cg::kBlack);
    CHECK(corLongeInterno != cg::kBlack);
    CHECK(corPertoCentro != corLongeCentro);
    CHECK(corPertoInterno != corLongeInterno);

    CHECK_EQ(countNonBlack(pertoSozinho), countNonBlack(longeSozinho));
    int coberturaDivergente = 0;
    for (int y = 0; y < pertoSozinho.height(); ++y) {
        for (int x = 0; x < pertoSozinho.width(); ++x) {
            const bool pertoPintado = pertoSozinho.pixelAt(x, y) != cg::kBlack;
            const bool longePintado = longeSozinho.pixelAt(x, y) != cg::kBlack;
            if (pertoPintado != longePintado) {
                ++coberturaDivergente;
            }
        }
    }
    CHECK_EQ(coberturaDivergente, 0);

    cg::Mesh sobreposto;
    sobreposto.vertices = {perto.vertices[0], perto.vertices[1], perto.vertices[2],
                           longe.vertices[0], longe.vertices[1], longe.vertices[2]};
    sobreposto.triangles = {cg::Triangle{0, 1, 2}, cg::Triangle{3, 4, 5}};

    cg::MemoryCanvas pertoPrimeiro{800, 600};
    cg::renderMesh(sobreposto, camera, lighting, pertoPrimeiro);
    CHECK_EQ(pertoPrimeiro.pixelAt(400, 300), corPertoCentro);
    CHECK_EQ(pertoPrimeiro.pixelAt(500, 280), corPertoInterno);

    sobreposto.triangles = {cg::Triangle{3, 4, 5}, cg::Triangle{0, 1, 2}};
    cg::MemoryCanvas longePrimeiro{800, 600};
    cg::renderMesh(sobreposto, camera, lighting, longePrimeiro);
    CHECK_EQ(longePrimeiro.pixelAt(400, 300), corPertoCentro);
    CHECK_EQ(longePrimeiro.pixelAt(500, 280), corPertoInterno);
}

void profundidade_e_recriada_a_cada_chamada() {
    cg::MemoryCanvas canvas{800, 600};
    const cg::Camera camera = cameraSintetica();

    cg::renderMesh(malhaTriangulo(5.0F), camera, ambiente(cg::Color{255.0F, 0.0F, 0.0F}), canvas);
    CHECK_EQ(canvas.pixelAt(400, 300), 0xFF0000U);

    cg::renderMesh(malhaTriangulo(10.0F), camera, ambiente(cg::Color{0.0F, 255.0F, 0.0F}), canvas);
    CHECK_EQ(canvas.pixelAt(400, 300), 0x00FF00U);
}

void points_e_wireframe_sao_iguais_ao_legado() {
    const cg::Camera camera = cameraSintetica();
    const cg::Mesh mesh = malhaTriangulo(5.0F);
    const cg::Lighting lighting = ambiente(cg::Color{100.0F, 50.0F, 200.0F});
    const cg::RenderMode modes[] = {cg::RenderMode::Points, cg::RenderMode::Wireframe};

    for (const cg::RenderMode mode : modes) {
        cg::MemoryCanvas legado{800, 600};
        cg::MemoryCanvas iluminado{800, 600};
        cg::renderMesh(mesh, camera, legado, mode);
        cg::renderMesh(mesh, camera, lighting, iluminado, mode);

        CHECK(countNonBlack(legado) > 0);
        CHECK_EQ(countNonBlack(iluminado), countNonBlack(legado));

        int divergencias = 0;
        for (int y = 0; y < legado.height(); ++y) {
            for (int x = 0; x < legado.width(); ++x) {
                if (iluminado.pixelAt(x, y) != legado.pixelAt(x, y)) {
                    ++divergencias;
                }
            }
        }
        CHECK_EQ(divergencias, 0);
    }
}

void caminho_legado_permanece_branco() {
    cg::MemoryCanvas legado{800, 600};
    cg::renderMesh(malhaTriangulo(5.0F), cameraSintetica(), legado);
    CHECK_EQ(legado.pixelAt(400, 300), cg::kWhite);

    cg::MemoryCanvas iluminado{800, 600};
    cg::renderMesh(malhaTriangulo(5.0F), cameraSintetica(), ambiente(cg::Color{100.0F, 50.0F, 200.0F}),
                   iluminado);
    CHECK_EQ(iluminado.pixelAt(400, 300), kCorAmbiente);
    CHECK(iluminado.pixelAt(400, 300) != cg::kWhite);
}

void iluminacao_usa_centro_da_camera_em_mundo() {
    cg::Lighting lighting;
    lighting.light.ambient = cg::Color{0.0F, 0.0F, 0.0F};
    lighting.light.color = cg::Color{255.0F, 255.0F, 255.0F};
    lighting.light.position = cg::Vec3{0.0F, 0.0F, -5.0F};
    lighting.material.ka = 0.0F;
    lighting.material.kd = cg::Vec3{1.0F, 1.0F, 1.0F};
    lighting.material.od = cg::Vec3{1.0F, 1.0F, 1.0F};
    lighting.material.ks = 1.0F;
    lighting.material.eta = 1.0F;

    cg::MemoryCanvas canvas{800, 600};
    cg::renderMesh(malhaTriangulo(0.0F), cameraEm(cg::Vec3{0.0F, 0.0F, -10.0F}), lighting, canvas);

    CHECK_EQ(canvas.pixelAt(400, 300), cg::kWhite);
    CHECK_EQ(cg::phongColor(cg::Vec3{0.0F, 0.0F, 0.0F}, cg::Vec3{0.0F, 0.0F, 1.0F},
                            cg::Vec3{0.0F, 0.0F, 0.0F}, lighting)
                 .packed(),
             cg::kBlack);
}

void eye_usa_centro_da_camera_em_mundo() {
    const cg::Camera camera = cameraEm(cg::Vec3{10.0F, 0.0F, -10.0F});
    const cg::Lighting lighting =
        difusa(cg::Color{255.0F, 255.0F, 255.0F}, cg::Vec3{20.0F, 10.0F, 10.0F});

    cg::Mesh mesh;
    mesh.vertices = {{10.0F, 0.0F, 0.0F}, {11.5F, 0.0F, 0.0F}, {10.0F, 1.5F, 0.0F}};
    mesh.triangles.push_back(cg::Triangle{0, 1, 2});

    cg::MemoryCanvas canvas{800, 600};
    cg::renderMesh(mesh, camera, lighting, canvas);

    const cg::Vec3 point{10.0F, 0.0F, 0.0F};
    const cg::Vec3 normal{0.0F, 0.0F, 1.0F};
    const cg::Color expected = cg::phongColor(point, normal, camera.c, lighting);
    const cg::Color viewExpected = cg::phongColor(camera.worldToView(point), normal, camera.c, lighting);

    CHECK(canvas.pixelAt(400, 300) != cg::kBlack);
    CHECK(maxChannelDifference(canvas.pixelAt(400, 300), expected.packed()) <= 1);
    CHECK(maxChannelDifference(expected.packed(), viewExpected.packed()) > 5);
    CHECK(maxChannelDifference(canvas.pixelAt(400, 300), viewExpected.packed()) > 5);
}

void two_sided_face_com_normal_invertida_recebe_luz() {
    const cg::Camera camera = cameraSintetica();
    const cg::Lighting lighting =
        difusa(cg::Color{255.0F, 255.0F, 255.0F}, cg::Vec3{0.0F, 0.0F, -10.0F});
    const cg::Vec3 pontoInterior{0.5F, 0.5F, 5.0F};
    const cg::Vec3 normalInvertida{0.0F, 0.0F, -1.0F};
    const cg::Color esperado = cg::phongColor(pontoInterior, normalInvertida, camera.c, lighting);
    CHECK_EQ(esperado.packed(), cg::kWhite);

    constexpr int kPixelInteriorX = 500;
    constexpr int kPixelInteriorY = 225;

    cg::MemoryCanvas normaisParaLonge{800, 600};
    cg::renderMesh(malhaTriangulo(5.0F), camera, lighting, normaisParaLonge);
    CHECK(countNonBlack(normaisParaLonge) > 0);
    const std::uint32_t corInterior =
        normaisParaLonge.pixelAt(kPixelInteriorX, kPixelInteriorY);
    CHECK(maxChannelDifference(corInterior, esperado.packed()) <= 1);

    cg::Mesh windingInvertido = malhaTriangulo(5.0F);
    windingInvertido.triangles[0] = cg::Triangle{0, 2, 1};
    cg::MemoryCanvas normaisParaCamera{800, 600};
    cg::renderMesh(windingInvertido, camera, lighting, normaisParaCamera);
    CHECK(countNonBlack(normaisParaCamera) > 0);
    const std::uint32_t corControle =
        normaisParaCamera.pixelAt(kPixelInteriorX, kPixelInteriorY);
    CHECK(maxChannelDifference(corControle, esperado.packed()) <= 1);
}

void normais_de_vertice_diferem_de_face() {
    const cg::Vec3 a{-1.0F, 0.0F, 5.0F};
    const cg::Vec3 b{1.0F, 0.0F, 5.0F};
    const cg::Vec3 c1{0.0F, 1.0F, 4.5F};
    const cg::Vec3 c2{0.0F, -1.0F, 4.5F};

    const cg::Vec3 face1 = (b - a).cross(c1 - a).normalized();
    const cg::Vec3 face2 = (c2 - a).cross(b - a).normalized();
    const cg::Vec3 vertexNormal = (face1 + face2).normalized();

    cg::Mesh mesh;
    mesh.vertices = {{a.x, a.y, a.z}, {b.x, b.y, b.z}, {c1.x, c1.y, c1.z},
                     {c2.x, c2.y, c2.z}};
    mesh.triangles = {cg::Triangle{0, 1, 2}, cg::Triangle{0, 3, 1}};

    const cg::Camera camera = cameraSintetica();
    const cg::Lighting lighting =
        difusa(cg::Color{255.0F, 255.0F, 255.0F}, cg::Vec3{10.0F, 10.0F, 20.0F});
    const cg::Vec3 point{0.0F, 0.0F, 5.0F};

    const cg::Color expectedVertex = cg::phongColor(point, vertexNormal, camera.c, lighting);
    const cg::Color expectedFace1 = cg::phongColor(point, face1, camera.c, lighting);
    const cg::Color expectedFace2 = cg::phongColor(point, face2, camera.c, lighting);

    CHECK(maxChannelDifference(expectedVertex.packed(), expectedFace1.packed()) > 5);
    CHECK(maxChannelDifference(expectedVertex.packed(), expectedFace2.packed()) > 5);

    cg::MemoryCanvas canvas{800, 600};
    cg::renderMesh(mesh, camera, lighting, canvas);

    const std::uint32_t rendered = canvas.pixelAt(400, 300);
    CHECK(rendered != cg::kBlack);
    CHECK(maxChannelDifference(rendered, expectedVertex.packed()) <= 1);
    CHECK(maxChannelDifference(rendered, expectedFace1.packed()) > 5);
    CHECK(maxChannelDifference(rendered, expectedFace2.packed()) > 5);
}

void triangulo_com_vertice_atras_da_camera_e_descartado() {
    cg::Mesh mesh = malhaTriangulo(5.0F);
    mesh.vertices[2].z = -1.0F;

    cg::MemoryCanvas canvas{800, 600};
    cg::renderMesh(mesh, cameraSintetica(), ambiente(cg::Color{255.0F, 255.0F, 255.0F}), canvas);

    CHECK_EQ(countNonBlack(canvas), 0);
}

void modelos_reais_renderizam_com_phong() {
    cg::Camera camera;
    cg::Lighting lighting;
    std::string error;
    CHECK(cg::loadCameraFromFile("camera/camera.txt", camera, error));
    CHECK(cg::loadLightingFromFile("lighting/lighting.txt", lighting, error));

    const char* paths[] = {"models/triangulo.byu", "models/piramide.byu", "models/maca.byu",
                           "models/maca2.byu",     "models/vaso.byu",     "models/calice2.byu"};

    for (const char* path : paths) {
        cg::Mesh mesh;
        CHECK(cg::loadMeshFromFile(path, mesh, error));

        cg::MemoryCanvas canvas{400, 300};
        cg::renderMesh(mesh, camera, lighting, canvas);

        CHECK(countNonBlack(canvas) > 0);
        CHECK(colorCount(canvas) >= 2);
    }
}

void canvas_pequeno_nao_quebra() {
    const cg::Camera camera = cameraSintetica();
    const cg::Mesh mesh = malhaTriangulo(5.0F);
    const cg::Lighting lighting = ambiente(cg::Color{100.0F, 50.0F, 200.0F});

    cg::MemoryCanvas minusculo{1, 1};
    cg::renderMesh(mesh, camera, lighting, minusculo);
    CHECK_EQ(minusculo.width(), 1);
    CHECK_EQ(minusculo.height(), 1);

    cg::MemoryCanvas vazio{0, 0};
    cg::renderMesh(mesh, camera, lighting, vazio);
    CHECK_EQ(vazio.width(), 0);
    CHECK_EQ(vazio.height(), 0);
}

void indice_invalido_e_ignorado() {
    const cg::Camera camera = cameraSintetica();
    const cg::Lighting lighting = ambiente(cg::Color{100.0F, 50.0F, 200.0F});
    const cg::Mesh valido = malhaTriangulo(5.0F);

    cg::Mesh comIndiceInvalido = valido;
    comIndiceInvalido.triangles.push_back(cg::Triangle{0, 1, 99});

    cg::MemoryCanvas somenteValido{800, 600};
    cg::renderMesh(valido, camera, lighting, somenteValido);

    cg::MemoryCanvas comInvalido{800, 600};
    cg::renderMesh(comIndiceInvalido, camera, lighting, comInvalido);

    int divergencias = 0;
    for (int y = 0; y < somenteValido.height(); ++y) {
        for (int x = 0; x < somenteValido.width(); ++x) {
            if (comInvalido.pixelAt(x, y) != somenteValido.pixelAt(x, y)) {
                ++divergencias;
            }
        }
    }
    CHECK_EQ(divergencias, 0);
}

}

int main() {
    triangulo_sintetico_recebe_cor_ambiente_conhecida();
    cor_difusa_conhecida_e_nao_branca();
    zbuffer_distingue_cor_do_distante();
    profundidade_e_recriada_a_cada_chamada();
    points_e_wireframe_sao_iguais_ao_legado();
    caminho_legado_permanece_branco();
    iluminacao_usa_centro_da_camera_em_mundo();
    eye_usa_centro_da_camera_em_mundo();
    two_sided_face_com_normal_invertida_recebe_luz();
    normais_de_vertice_diferem_de_face();
    triangulo_com_vertice_atras_da_camera_e_descartado();
    modelos_reais_renderizam_com_phong();
    canvas_pequeno_nao_quebra();
    indice_invalido_e_ignorado();

    return test::failureCount() == 0 ? 0 : 1;
}
