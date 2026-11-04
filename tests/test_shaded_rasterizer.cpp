#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <limits>

#include "canvas/MemoryCanvas.h"
#include "math/Color.h"
#include "math/Vec3.h"
#include "pipeline/DepthBuffer.h"
#include "pipeline/Rasterizer.h"
#include "pipeline/ShadedRasterizer.h"
#include "shading/Lighting.h"
#include "shading/Phong.h"
#include "test_util.h"

namespace {

const cg::Vec3 kOlho{0.0F, 0.0F, 10.0F};
const cg::Vec3 kNormalZ{0.0F, 0.0F, 1.0F};

cg::ShadedVertex vertex(int x, int y, float invZ, const cg::Vec3& world, const cg::Vec3& normal) {
    cg::ShadedVertex shaded;
    shaded.screen = cg::ScreenPoint{x, y};
    shaded.invZ = invZ;
    shaded.world = world;
    shaded.normal = normal;
    return shaded;
}

cg::ShadedVertex planeVertex(const cg::ScreenPoint& point) {
    return vertex(point.x, point.y, 1.0F,
                  cg::Vec3{static_cast<float>(point.x), static_cast<float>(point.y), 0.0F},
                  kNormalZ);
}

cg::Lighting ambientOnly(const cg::Color& ambient) {
    cg::Lighting lighting;
    lighting.light.ambient = ambient;
    lighting.light.color = cg::Color{0.0F, 0.0F, 0.0F};
    lighting.light.position = kOlho;
    lighting.material.ka = 1.0F;
    lighting.material.kd = cg::Vec3{0.0F, 0.0F, 0.0F};
    lighting.material.od = cg::Vec3{1.0F, 1.0F, 1.0F};
    lighting.material.ks = 0.0F;
    lighting.material.eta = 1.0F;
    return lighting;
}

cg::Lighting diffuseWhite(const cg::Vec3& position) {
    cg::Lighting lighting;
    lighting.light.ambient = cg::Color{0.0F, 0.0F, 0.0F};
    lighting.light.color = cg::Color{255.0F, 255.0F, 255.0F};
    lighting.light.position = position;
    lighting.material.ka = 0.0F;
    lighting.material.kd = cg::Vec3{1.0F, 1.0F, 1.0F};
    lighting.material.od = cg::Vec3{1.0F, 1.0F, 1.0F};
    lighting.material.ks = 0.0F;
    lighting.material.eta = 1.0F;
    return lighting;
}

cg::Lighting diffuseSpecular() {
    cg::Lighting lighting;
    lighting.light.ambient = cg::Color{100.0F, 100.0F, 100.0F};
    lighting.light.color = cg::Color{200.0F, 100.0F, 50.0F};
    lighting.light.position = cg::Vec3{0.0F, 0.0F, 5.0F};
    lighting.material.ka = 0.2F;
    lighting.material.kd = cg::Vec3{0.5F, 0.3F, 0.2F};
    lighting.material.od = cg::Vec3{0.7F, 0.5F, 0.8F};
    lighting.material.ks = 0.5F;
    lighting.material.eta = 1.0F;
    return lighting;
}

int countPainted(const cg::MemoryCanvas& canvas) {
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

bool allDepthZero(const cg::DepthBuffer& depth) {
    for (int y = 0; y < depth.height(); ++y) {
        for (int x = 0; x < depth.width(); ++x) {
            if (depth.at(x, y) != 0.0F) {
                return false;
            }
        }
    }
    return true;
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

void expectSameCoverage(const cg::ScreenPoint& p0, const cg::ScreenPoint& p1,
                        const cg::ScreenPoint& p2) {
    cg::MemoryCanvas legacy{16, 16};
    cg::MemoryCanvas shaded{16, 16};
    cg::DepthBuffer depth{16, 16};

    cg::fillTriangle(legacy, p0, p1, p2, cg::kWhite);
    cg::fillTriangleShaded(shaded, depth, planeVertex(p0), planeVertex(p1), planeVertex(p2), kOlho,
                           ambientOnly(cg::Color{255.0F, 255.0F, 255.0F}));

    for (int y = 0; y < 16; ++y) {
        for (int x = 0; x < 16; ++x) {
            CHECK_EQ(shaded.pixelAt(x, y), legacy.pixelAt(x, y));
            const float expectedDepth = legacy.pixelAt(x, y) == cg::kWhite ? 1.0F : 0.0F;
            CHECK_NEAR(depth.at(x, y), expectedDepth, 1e-5F);
        }
    }
}

void triangulo_e_preenchido_e_profundidade_registrada() {
    cg::MemoryCanvas canvas{12, 12};
    cg::DepthBuffer depth{12, 12};

    cg::fillTriangleShaded(canvas, depth,
                           vertex(0, 0, 1.0F, cg::Vec3{0.0F, 0.0F, 0.0F}, kNormalZ),
                           vertex(10, 0, 1.0F, cg::Vec3{10.0F, 0.0F, 0.0F}, kNormalZ),
                           vertex(0, 10, 1.0F, cg::Vec3{0.0F, 10.0F, 0.0F}, kNormalZ), kOlho,
                           diffuseWhite(kOlho));

    CHECK(canvas.pixelAt(1, 1) != cg::kBlack);
    CHECK(canvas.pixelAt(3, 1) != cg::kBlack);
    CHECK(canvas.pixelAt(1, 3) != cg::kBlack);
    CHECK_EQ(canvas.pixelAt(9, 9), cg::kBlack);
    CHECK_NEAR(depth.at(1, 1), 1.0F, 1e-5F);
    CHECK_NEAR(depth.at(3, 1), 1.0F, 1e-5F);
    CHECK_NEAR(depth.at(1, 3), 1.0F, 1e-5F);
}

void cobertura_identica_ao_fill_triangle_legado() {
    expectSameCoverage(cg::ScreenPoint{0, 0}, cg::ScreenPoint{10, 0}, cg::ScreenPoint{0, 10});
    expectSameCoverage(cg::ScreenPoint{0, 0}, cg::ScreenPoint{4, 0}, cg::ScreenPoint{2, 3});
    expectSameCoverage(cg::ScreenPoint{-5, 5}, cg::ScreenPoint{15, 5}, cg::ScreenPoint{5, -20});
}

void clip_de_tela_nao_pinta_fora() {
    cg::MemoryCanvas legacy{16, 16};
    cg::MemoryCanvas shaded{16, 16};
    cg::DepthBuffer depth{16, 16};

    const cg::ScreenPoint a{-5, 5};
    const cg::ScreenPoint b{15, 5};
    const cg::ScreenPoint c{5, -20};

    cg::fillTriangle(legacy, a, b, c, cg::kWhite);
    cg::fillTriangleShaded(shaded, depth, planeVertex(a), planeVertex(b), planeVertex(c), kOlho,
                           ambientOnly(cg::Color{255.0F, 255.0F, 255.0F}));

    for (int y = 0; y < 16; ++y) {
        for (int x = 0; x < 16; ++x) {
            CHECK_EQ(shaded.pixelAt(x, y), legacy.pixelAt(x, y));
        }
    }

    CHECK_EQ(shaded.pixelAt(15, 0), cg::kBlack);
    CHECK_EQ(shaded.pixelAt(15, 1), cg::kBlack);
    CHECK_EQ(shaded.pixelAt(15, 2), cg::kBlack);
    CHECK_EQ(shaded.pixelAt(0, 15), cg::kBlack);
    CHECK_EQ(shaded.pixelAt(15, 15), cg::kBlack);
}

void area_grande_nao_overflowa() {
    const cg::ScreenPoint triangulos[2][3] = {
        {{0, 0}, {100000, 0}, {0, 100000}},
        {{0, 0}, {65536, 0}, {0, 65536}},
    };

    for (const auto& tri : triangulos) {
        cg::MemoryCanvas legacy{16, 16};
        cg::MemoryCanvas shaded{16, 16};
        cg::DepthBuffer depth{16, 16};

        cg::fillTriangle(legacy, tri[0], tri[1], tri[2], cg::kWhite);
        cg::fillTriangleShaded(
            shaded, depth,
            vertex(tri[0].x, tri[0].y, 1.0F,
                   cg::Vec3{static_cast<float>(tri[0].x), static_cast<float>(tri[0].y), 0.0F},
                   kNormalZ),
            vertex(tri[1].x, tri[1].y, 1.0F,
                   cg::Vec3{static_cast<float>(tri[1].x), static_cast<float>(tri[1].y), 0.0F},
                   kNormalZ),
            vertex(tri[2].x, tri[2].y, 1.0F,
                   cg::Vec3{static_cast<float>(tri[2].x), static_cast<float>(tri[2].y), 0.0F},
                   kNormalZ),
            kOlho, ambientOnly(cg::Color{255.0F, 255.0F, 255.0F}));

        for (int y = 0; y < 16; ++y) {
            for (int x = 0; x < 16; ++x) {
                CHECK_EQ(shaded.pixelAt(x, y), legacy.pixelAt(x, y));
            }
        }

        CHECK_EQ(countPainted(shaded), 256);
        CHECK(depth.at(0, 0) > 0.0F);
        CHECK(depth.at(15, 15) > 0.0F);
    }
}

void triangulo_mais_proximo_vence_independente_da_ordem() {
    const cg::ShadedVertex nearA =
        vertex(0, 0, 0.2F, cg::Vec3{0.0F, 0.0F, 0.0F}, kNormalZ);
    const cg::ShadedVertex nearB =
        vertex(10, 0, 0.2F, cg::Vec3{10.0F, 0.0F, 0.0F}, kNormalZ);
    const cg::ShadedVertex nearC =
        vertex(0, 10, 0.2F, cg::Vec3{0.0F, 10.0F, 0.0F}, kNormalZ);
    const cg::ShadedVertex farA =
        vertex(0, 0, 0.1F, cg::Vec3{0.0F, 0.0F, 0.0F}, kNormalZ);
    const cg::ShadedVertex farB =
        vertex(10, 0, 0.1F, cg::Vec3{10.0F, 0.0F, 0.0F}, kNormalZ);
    const cg::ShadedVertex farC =
        vertex(0, 10, 0.1F, cg::Vec3{0.0F, 10.0F, 0.0F}, kNormalZ);

    const cg::Lighting red = ambientOnly(cg::Color{255.0F, 0.0F, 0.0F});
    const cg::Lighting blue = ambientOnly(cg::Color{0.0F, 0.0F, 255.0F});

    cg::MemoryCanvas nearFirst{12, 12};
    cg::DepthBuffer nearFirstDepth{12, 12};
    cg::fillTriangleShaded(nearFirst, nearFirstDepth, nearA, nearB, nearC, kOlho, red);
    cg::fillTriangleShaded(nearFirst, nearFirstDepth, farA, farB, farC, kOlho, blue);

    cg::MemoryCanvas farFirst{12, 12};
    cg::DepthBuffer farFirstDepth{12, 12};
    cg::fillTriangleShaded(farFirst, farFirstDepth, farA, farB, farC, kOlho, blue);
    cg::fillTriangleShaded(farFirst, farFirstDepth, nearA, nearB, nearC, kOlho, red);

    CHECK_EQ(nearFirst.pixelAt(2, 2), 0xFF0000U);
    CHECK_EQ(farFirst.pixelAt(2, 2), 0xFF0000U);
    CHECK_NEAR(nearFirstDepth.at(2, 2), 0.2F, 1e-6F);
    CHECK_NEAR(farFirstDepth.at(2, 2), 0.2F, 1e-6F);
}

void empate_de_profundidade_mantem_o_primeiro() {
    const cg::ShadedVertex a = vertex(0, 0, 0.5F, cg::Vec3{0.0F, 0.0F, 0.0F}, kNormalZ);
    const cg::ShadedVertex b = vertex(10, 0, 0.5F, cg::Vec3{10.0F, 0.0F, 0.0F}, kNormalZ);
    const cg::ShadedVertex c = vertex(0, 10, 0.5F, cg::Vec3{0.0F, 10.0F, 0.0F}, kNormalZ);

    const cg::Lighting red = ambientOnly(cg::Color{255.0F, 0.0F, 0.0F});
    const cg::Lighting blue = ambientOnly(cg::Color{0.0F, 0.0F, 255.0F});

    cg::MemoryCanvas redFirst{12, 12};
    cg::DepthBuffer redFirstDepth{12, 12};
    cg::fillTriangleShaded(redFirst, redFirstDepth, a, b, c, kOlho, red);
    cg::fillTriangleShaded(redFirst, redFirstDepth, a, b, c, kOlho, blue);

    cg::MemoryCanvas blueFirst{12, 12};
    cg::DepthBuffer blueFirstDepth{12, 12};
    cg::fillTriangleShaded(blueFirst, blueFirstDepth, a, b, c, kOlho, blue);
    cg::fillTriangleShaded(blueFirst, blueFirstDepth, a, b, c, kOlho, red);

    CHECK_EQ(redFirst.pixelAt(2, 2), 0xFF0000U);
    CHECK_EQ(blueFirst.pixelAt(2, 2), 0x0000FFU);
    CHECK_NEAR(redFirstDepth.at(2, 2), 0.5F, 1e-6F);
    CHECK_NEAR(blueFirstDepth.at(2, 2), 0.5F, 1e-6F);
}

cg::Vec3 perspectiveWorld(const cg::ShadedVertex& a, const cg::ShadedVertex& b,
                          const cg::ShadedVertex& c) {
    const float wa = 0.6F;
    const float wb = 0.2F;
    const float wc = 0.2F;
    const float invZ = wa * a.invZ + wb * b.invZ + wc * c.invZ;
    return (a.world * (wa * a.invZ) + b.world * (wb * b.invZ) + c.world * (wc * c.invZ)) / invZ;
}

cg::Vec3 perspectiveNormal(const cg::ShadedVertex& a, const cg::ShadedVertex& b,
                           const cg::ShadedVertex& c) {
    const float wa = 0.6F;
    const float wb = 0.2F;
    const float wc = 0.2F;
    const float invZ = wa * a.invZ + wb * b.invZ + wc * c.invZ;
    return (a.normal * (wa * a.invZ) + b.normal * (wb * b.invZ) + c.normal * (wc * c.invZ)) /
           invZ;
}

void atributos_sao_interpolados_em_perspectiva() {
    const cg::ShadedVertex a = vertex(0, 0, 1.0F, cg::Vec3{0.0F, 0.0F, 1.0F}, cg::Vec3{0.0F, 0.0F, 1.0F});
    const cg::ShadedVertex b = vertex(10, 0, 0.5F, cg::Vec3{10.0F, 0.0F, 2.0F}, cg::Vec3{1.0F, 0.0F, 0.0F});
    const cg::ShadedVertex c = vertex(0, 10, 0.25F, cg::Vec3{0.0F, 10.0F, 4.0F}, cg::Vec3{0.0F, 1.0F, 0.0F});

    const cg::Vec3 eye{0.0F, 0.0F, 0.0F};
    const cg::Lighting lighting = diffuseWhite(cg::Vec3{1.333333F, 0.666667F, 2.0F});

    cg::MemoryCanvas canvas{12, 12};
    cg::DepthBuffer depth{12, 12};
    cg::fillTriangleShaded(canvas, depth, a, b, c, eye, lighting);

    const cg::Color expected = cg::phongColor(perspectiveWorld(a, b, c), perspectiveNormal(a, b, c),
                                              eye, lighting);
    CHECK(canvas.pixelAt(2, 2) != cg::kBlack);
    CHECK(maxChannelDifference(canvas.pixelAt(2, 2), expected.packed()) <= 1);

    const cg::Vec3 linearWorld = a.world * 0.6F + b.world * 0.2F + c.world * 0.2F;
    const cg::Vec3 linearNormal = a.normal * 0.6F + b.normal * 0.2F + c.normal * 0.2F;
    const cg::Color linear = cg::phongColor(linearWorld, linearNormal, eye, lighting);
    CHECK(maxChannelDifference(canvas.pixelAt(2, 2), linear.packed()) > 5);
}

void profundidade_e_interpolada_em_perspectiva() {
    const cg::ShadedVertex a = vertex(0, 0, 1.0F, cg::Vec3{0.0F, 0.0F, 1.0F}, kNormalZ);
    const cg::ShadedVertex b = vertex(10, 0, 0.5F, cg::Vec3{10.0F, 0.0F, 2.0F}, kNormalZ);
    const cg::ShadedVertex c = vertex(0, 10, 0.25F, cg::Vec3{0.0F, 10.0F, 4.0F}, kNormalZ);

    cg::MemoryCanvas canvas{12, 12};
    cg::DepthBuffer depth{12, 12};
    cg::fillTriangleShaded(canvas, depth, a, b, c, kOlho,
                           ambientOnly(cg::Color{255.0F, 255.0F, 255.0F}));

    CHECK_NEAR(depth.at(2, 2), 0.75F, 1e-5F);
}

void normal_nula_usa_normal_da_face() {
    const cg::Vec3 zero{0.0F, 0.0F, 0.0F};
    const cg::ShadedVertex a = vertex(0, 0, 1.0F, cg::Vec3{0.0F, 0.0F, 0.0F}, zero);
    const cg::ShadedVertex b = vertex(10, 0, 1.0F, cg::Vec3{10.0F, 0.0F, 0.0F}, zero);
    const cg::ShadedVertex c = vertex(0, 10, 1.0F, cg::Vec3{0.0F, 10.0F, 0.0F}, zero);

    cg::MemoryCanvas canvas{12, 12};
    cg::DepthBuffer depth{12, 12};
    cg::fillTriangleShaded(canvas, depth, a, b, c, kOlho,
                           diffuseWhite(cg::Vec3{0.0F, 0.0F, 20.0F}));

    CHECK(canvas.pixelAt(2, 2) != cg::kBlack);
    CHECK(canvas.pixelAt(1, 1) != cg::kBlack);
}

void normal_nula_usa_normal_da_face_em_mundo() {
    const cg::Vec3 zero{0.0F, 0.0F, 0.0F};
    const cg::ShadedVertex a = vertex(0, 0, 1.0F, cg::Vec3{0.0F, 0.0F, 0.0F}, zero);
    const cg::ShadedVertex b = vertex(10, 0, 1.0F, cg::Vec3{10.0F, 0.0F, 10.0F}, zero);
    const cg::ShadedVertex c = vertex(0, 10, 1.0F, cg::Vec3{0.0F, 10.0F, 5.0F}, zero);

    const cg::Vec3 world{2.0F, 2.0F, 3.0F};
    const cg::Vec3 faceNormal = (b.world - a.world).cross(c.world - a.world).normalized();
    const cg::Vec3 eye{0.0F, 0.0F, 20.0F};
    const cg::Lighting lighting = diffuseWhite(world + faceNormal * 10.0F);

    cg::MemoryCanvas canvas{12, 12};
    cg::DepthBuffer depth{12, 12};
    cg::fillTriangleShaded(canvas, depth, a, b, c, eye, lighting);

    const cg::Color expected = cg::phongColor(world, faceNormal, eye, lighting);
    CHECK(maxChannelDifference(canvas.pixelAt(2, 2), expected.packed()) <= 2);

    const cg::Vec3 screenNormal{0.0F, 0.0F, 1.0F};
    const cg::Color screenFallback = cg::phongColor(world, screenNormal, eye, lighting);
    CHECK(maxChannelDifference(canvas.pixelAt(2, 2), screenFallback.packed()) > 5);
}

void normal_nao_finita_usa_normal_da_face() {
    const float nan = std::numeric_limits<float>::quiet_NaN();
    const float inf = std::numeric_limits<float>::infinity();
    const float invalid[] = {nan, inf};
    const cg::Vec3 zero{0.0F, 0.0F, 0.0F};

    const cg::Vec3 world{2.0F, 2.0F, 3.0F};
    const cg::Vec3 faceNormal =
        cg::Vec3{10.0F, 0.0F, 10.0F}.cross(cg::Vec3{0.0F, 10.0F, 5.0F}).normalized();
    const cg::Vec3 eye{0.0F, 0.0F, 20.0F};
    const cg::Lighting lighting = diffuseWhite(world + faceNormal * 10.0F);
    const cg::Color expected = cg::phongColor(world, faceNormal, eye, lighting);

    for (const float value : invalid) {
        for (int invalidVertex = 0; invalidVertex < 3; ++invalidVertex) {
            cg::Vec3 normals[3] = {zero, zero, zero};
            normals[invalidVertex] = cg::Vec3{value, 0.0F, 0.0F};

            cg::MemoryCanvas canvas{12, 12};
            cg::DepthBuffer depth{12, 12};
            cg::fillTriangleShaded(
                canvas, depth, vertex(0, 0, 1.0F, cg::Vec3{0.0F, 0.0F, 0.0F}, normals[0]),
                vertex(10, 0, 1.0F, cg::Vec3{10.0F, 0.0F, 10.0F}, normals[1]),
                vertex(0, 10, 1.0F, cg::Vec3{0.0F, 10.0F, 5.0F}, normals[2]), eye, lighting);

            CHECK(maxChannelDifference(canvas.pixelAt(2, 2), expected.packed()) <= 2);
        }
    }
}

void normal_interpolada_e_renormalizada() {
    const cg::ShadedVertex a = vertex(0, 0, 1.0F, cg::Vec3{0.0F, 0.0F, 1.0F}, cg::Vec3{1.0F, 0.0F, 0.0F});
    const cg::ShadedVertex b = vertex(10, 0, 0.5F, cg::Vec3{10.0F, 0.0F, 2.0F}, cg::Vec3{0.0F, 1.0F, 0.0F});
    const cg::ShadedVertex c = vertex(0, 10, 0.25F, cg::Vec3{0.0F, 10.0F, 4.0F}, cg::Vec3{0.0F, 0.0F, 1.0F});

    const cg::Lighting lighting = diffuseSpecular();

    cg::MemoryCanvas canvas{12, 12};
    cg::DepthBuffer depth{12, 12};
    cg::fillTriangleShaded(canvas, depth, a, b, c, kOlho, lighting);

    const cg::Vec3 world = perspectiveWorld(a, b, c);
    const cg::Vec3 expectedNormal = perspectiveNormal(a, b, c);
    const cg::Color expected = cg::phongColor(world, expectedNormal, kOlho, lighting);
    CHECK(maxChannelDifference(canvas.pixelAt(2, 2), expected.packed()) <= 1);

    const cg::Vec3 linearNormal = a.normal * 0.6F + b.normal * 0.2F + c.normal * 0.2F;
    const cg::Color linear = cg::phongColor(world, linearNormal, kOlho, lighting);
    CHECK(maxChannelDifference(canvas.pixelAt(2, 2), linear.packed()) > 5);
}

void triangulo_degenerado_nao_pinta_nem_escreve_profundidade() {
    cg::MemoryCanvas equalCanvas{12, 12};
    cg::DepthBuffer equalDepth{12, 12};
    const cg::ShadedVertex repeated = vertex(5, 5, 1.0F, cg::Vec3{5.0F, 5.0F, 0.0F}, kNormalZ);
    cg::fillTriangleShaded(equalCanvas, equalDepth, repeated, repeated, repeated, kOlho,
                           ambientOnly(cg::Color{255.0F, 255.0F, 255.0F}));

    CHECK_EQ(countPainted(equalCanvas), 0);
    CHECK(allDepthZero(equalDepth));

    cg::MemoryCanvas collinearCanvas{12, 12};
    cg::DepthBuffer collinearDepth{12, 12};
    cg::fillTriangleShaded(collinearCanvas, collinearDepth, planeVertex(cg::ScreenPoint{0, 0}),
                           planeVertex(cg::ScreenPoint{5, 5}),
                           planeVertex(cg::ScreenPoint{10, 10}), kOlho,
                           ambientOnly(cg::Color{255.0F, 255.0F, 255.0F}));

    CHECK_EQ(countPainted(collinearCanvas), 0);
    CHECK(allDepthZero(collinearDepth));
}

void triangulo_com_invz_nao_positivo_e_rejeitado() {
    const float nan = std::numeric_limits<float>::quiet_NaN();
    const float inf = std::numeric_limits<float>::infinity();
    const float invalid[] = {0.0F, -1.0F, nan, inf};

    for (const float value : invalid) {
        for (int invalidVertex = 0; invalidVertex < 3; ++invalidVertex) {
            cg::ShadedVertex vertices[3] = {
                vertex(0, 0, 1.0F, cg::Vec3{0.0F, 0.0F, 0.0F}, kNormalZ),
                vertex(10, 0, 1.0F, cg::Vec3{10.0F, 0.0F, 0.0F}, kNormalZ),
                vertex(0, 10, 1.0F, cg::Vec3{0.0F, 10.0F, 0.0F}, kNormalZ),
            };
            vertices[invalidVertex].invZ = value;

            cg::MemoryCanvas canvas{12, 12};
            cg::DepthBuffer depth{12, 12};
            cg::fillTriangleShaded(canvas, depth, vertices[0], vertices[1], vertices[2], kOlho,
                                   ambientOnly(cg::Color{255.0F, 255.0F, 255.0F}));

            CHECK_EQ(countPainted(canvas), 0);
            CHECK(allDepthZero(depth));
        }
    }
}

void buffer_nao_quadrado_funciona() {
    const cg::ShadedVertex a = vertex(0, 0, 1.0F, cg::Vec3{0.0F, 0.0F, 0.0F}, kNormalZ);
    const cg::ShadedVertex b = vertex(10, 0, 0.5F, cg::Vec3{10.0F, 0.0F, 0.0F}, kNormalZ);
    const cg::ShadedVertex c = vertex(0, 7, 0.25F, cg::Vec3{0.0F, 7.0F, 0.0F}, kNormalZ);

    cg::MemoryCanvas canvas{12, 8};
    cg::DepthBuffer depth{12, 8};
    cg::fillTriangleShaded(canvas, depth, a, b, c, kOlho, diffuseWhite(kOlho));

    CHECK(canvas.pixelAt(2, 2) != cg::kBlack);
    CHECK(canvas.pixelAt(0, 7) != cg::kBlack);
    CHECK_EQ(canvas.pixelAt(2, 7), cg::kBlack);
    CHECK_NEAR(depth.at(2, 2), 0.6857143F, 1e-4F);
    CHECK_NEAR(depth.at(0, 7), 0.25F, 1e-4F);
}

void triangulo_fora_da_tela_nao_pinta() {
    cg::MemoryCanvas canvas{16, 16};
    cg::DepthBuffer depth{16, 16};

    cg::fillTriangleShaded(canvas, depth, planeVertex(cg::ScreenPoint{20, 20}),
                           planeVertex(cg::ScreenPoint{30, 20}),
                           planeVertex(cg::ScreenPoint{20, 30}), kOlho,
                           ambientOnly(cg::Color{255.0F, 255.0F, 255.0F}));

    CHECK_EQ(countPainted(canvas), 0);
    CHECK(allDepthZero(depth));
}

}

int main() {
    triangulo_e_preenchido_e_profundidade_registrada();
    cobertura_identica_ao_fill_triangle_legado();
    clip_de_tela_nao_pinta_fora();
    area_grande_nao_overflowa();
    triangulo_mais_proximo_vence_independente_da_ordem();
    empate_de_profundidade_mantem_o_primeiro();
    atributos_sao_interpolados_em_perspectiva();
    profundidade_e_interpolada_em_perspectiva();
    normal_nula_usa_normal_da_face();
    normal_nula_usa_normal_da_face_em_mundo();
    normal_nao_finita_usa_normal_da_face();
    normal_interpolada_e_renormalizada();
    triangulo_degenerado_nao_pinta_nem_escreve_profundidade();
    triangulo_com_invz_nao_positivo_e_rejeitado();
    buffer_nao_quadrado_funciona();
    triangulo_fora_da_tela_nao_pinta();

    return test::failureCount() == 0 ? 0 : 1;
}
