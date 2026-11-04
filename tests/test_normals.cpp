#include <cmath>
#include <limits>
#include <string>

#include "mesh/Mesh.h"
#include "mesh/Normals.h"
#include "test_util.h"

#define CHECK_VETOR(actual, expected, tolerance) \
    confereVetor(#actual, (actual), (expected), (tolerance), __FILE__, __LINE__)

namespace {

using cg::Vec3;

Vec3 cruzamentoLocal(const Vec3& u, const Vec3& v) {
    return Vec3{u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x};
}

Vec3 normalizadaLocal(const Vec3& v) {
    const float comprimento = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (!(comprimento > 0.0F) || !std::isfinite(comprimento)) {
        return Vec3{};
    }
    return Vec3{v.x / comprimento, v.y / comprimento, v.z / comprimento};
}

Vec3 normalDaFaceLocal(const Vec3& a, const Vec3& b, const Vec3& c) {
    const Vec3 ab{b.x - a.x, b.y - a.y, b.z - a.z};
    const Vec3 ac{c.x - a.x, c.y - a.y, c.z - a.z};
    return normalizadaLocal(cruzamentoLocal(ab, ac));
}

float distanciaLocal(const Vec3& u, const Vec3& v) {
    const float dx = u.x - v.x;
    const float dy = u.y - v.y;
    const float dz = u.z - v.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

void confereVetor(const char* expressao, const Vec3& atual, const Vec3& esperado, float tolerancia,
                  const char* arquivo, int linha) {
    const bool perto = std::fabs(atual.x - esperado.x) <= tolerancia &&
                       std::fabs(atual.y - esperado.y) <= tolerancia &&
                       std::fabs(atual.z - esperado.z) <= tolerancia;
    if (!perto) {
        test::reportFailure(arquivo, linha,
                            std::string(expressao) + " = (" + std::to_string(atual.x) + ", " +
                                std::to_string(atual.y) + ", " + std::to_string(atual.z) +
                                "), esperado (" + std::to_string(esperado.x) + ", " +
                                std::to_string(esperado.y) + ", " + std::to_string(esperado.z) + ")");
    }
}

cg::Mesh malhaTriangulo(const cg::Vertex& a, const cg::Vertex& b, const cg::Vertex& c) {
    cg::Mesh mesh;
    mesh.vertices = {a, b, c};
    mesh.triangles.push_back(cg::Triangle{0, 1, 2});
    return mesh;
}

void normal_da_face_aponta_para_z_positivo() {
    const cg::Mesh mesh =
        malhaTriangulo({0.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F}, {0.0F, 1.0F, 0.0F});
    const cg::MeshNormals normals = cg::computeMeshNormals(mesh);

    CHECK_NEAR(normals.face[0].x, 0.0F, 1e-6F);
    CHECK_NEAR(normals.face[0].y, 0.0F, 1e-6F);
    CHECK_NEAR(normals.face[0].z, 1.0F, 1e-6F);
    for (const cg::Vec3& normal : normals.vertex) {
        CHECK_NEAR(normal.z, 1.0F, 1e-6F);
        CHECK_NEAR(normal.length(), 1.0F, 1e-6F);
    }
}

void winding_invertido_inverte_a_normal() {
    const cg::Mesh mesh =
        malhaTriangulo({0.0F, 0.0F, 0.0F}, {0.0F, 1.0F, 0.0F}, {1.0F, 0.0F, 0.0F});
    const cg::MeshNormals normals = cg::computeMeshNormals(mesh);

    CHECK_NEAR(normals.face[0].z, -1.0F, 1e-6F);
}

void vertice_compartilhado_recebe_media_das_faces() {
    cg::Mesh mesh;
    mesh.vertices = {
        {0.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F}, {0.0F, 1.0F, 0.0F}, {1.0F, 1.0F, 0.0F}};
    mesh.triangles = {cg::Triangle{0, 1, 2}, cg::Triangle{3, 2, 1}};
    const cg::MeshNormals normals = cg::computeMeshNormals(mesh);

    CHECK_EQ(normals.face.size(), static_cast<std::size_t>(2));
    CHECK_NEAR(normals.face[0].z, 1.0F, 1e-6F);
    CHECK_NEAR(normals.face[1].z, 1.0F, 1e-6F);
    CHECK_NEAR(normals.vertex[1].z, 1.0F, 1e-6F);
    CHECK_NEAR(normals.vertex[2].z, 1.0F, 1e-6F);
}

void face_degenerada_nao_gera_nan() {
    const cg::Mesh mesh =
        malhaTriangulo({0.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F}, {2.0F, 0.0F, 0.0F});
    const cg::MeshNormals normals = cg::computeMeshNormals(mesh);

    CHECK_EQ(normals.face[0].x, 0.0F);
    CHECK_EQ(normals.face[0].y, 0.0F);
    CHECK_EQ(normals.face[0].z, 0.0F);
    for (const cg::Vec3& normal : normals.vertex) {
        CHECK(std::isfinite(normal.x));
        CHECK(std::isfinite(normal.y));
        CHECK(std::isfinite(normal.z));
    }
}

void tetraedro_tem_normais_unitarias_por_vertice() {
    const Vec3 a{0.0F, 0.0F, 0.0F};
    const Vec3 b{1.0F, 0.0F, 0.0F};
    const Vec3 c{0.0F, 1.0F, 0.0F};
    const Vec3 d{0.0F, 0.0F, 1.0F};

    cg::Mesh mesh;
    mesh.vertices = {{a.x, a.y, a.z}, {b.x, b.y, b.z}, {c.x, c.y, c.z}, {d.x, d.y, d.z}};
    mesh.triangles = {cg::Triangle{0, 2, 1}, cg::Triangle{0, 1, 3}, cg::Triangle{0, 3, 2},
                      cg::Triangle{1, 2, 3}};
    const cg::MeshNormals normals = cg::computeMeshNormals(mesh);

    CHECK_EQ(normals.face.size(), static_cast<std::size_t>(4));
    CHECK_EQ(normals.vertex.size(), static_cast<std::size_t>(4));

    const float raizDeTresInversa = 1.0F / std::sqrt(3.0F);
    CHECK_VETOR(normals.face[0], (Vec3{0.0F, 0.0F, -1.0F}), 1e-5F);
    CHECK_VETOR(normals.face[1], (Vec3{0.0F, -1.0F, 0.0F}), 1e-5F);
    CHECK_VETOR(normals.face[2], (Vec3{-1.0F, 0.0F, 0.0F}), 1e-5F);
    CHECK_VETOR(normals.face[3],
                (Vec3{raizDeTresInversa, raizDeTresInversa, raizDeTresInversa}), 1e-5F);

    for (const Vec3& normal : normals.face) {
        CHECK_NEAR(normal.length(), 1.0F, 1e-5F);
    }

    CHECK_VETOR(normals.vertex[0],
                (Vec3{-raizDeTresInversa, -raizDeTresInversa, -raizDeTresInversa}), 1e-5F);

    const Vec3 faceAcb = normalDaFaceLocal(a, c, b);
    const Vec3 faceAbd = normalDaFaceLocal(a, b, d);
    const Vec3 faceAdc = normalDaFaceLocal(a, d, c);
    const Vec3 faceBcd = normalDaFaceLocal(b, c, d);

    CHECK_VETOR(normals.vertex[1], (normalizadaLocal(faceAcb + faceAbd + faceBcd)), 1e-5F);
    CHECK_VETOR(normals.vertex[2], (normalizadaLocal(faceAcb + faceAdc + faceBcd)), 1e-5F);
    CHECK_VETOR(normals.vertex[3], (normalizadaLocal(faceAbd + faceAdc + faceBcd)), 1e-5F);

    for (const Vec3& normal : normals.vertex) {
        CHECK_NEAR(normal.length(), 1.0F, 1e-5F);
    }
}

void media_de_vertice_nao_pondera_por_area() {
    cg::Mesh mesh;
    mesh.vertices = {{0.0F, 0.0F, 0.0F},
                     {1.0F, 0.0F, 0.0F},
                     {0.0F, 1.0F, 0.0F},
                     {2.0F, 0.0F, 0.0F},
                     {0.0F, 4.0F, 0.0F},
                     {0.0F, 0.0F, 1.0F}};
    mesh.triangles = {cg::Triangle{0, 1, 2}, cg::Triangle{0, 3, 4}, cg::Triangle{0, 5, 1}};
    const cg::MeshNormals normals = cg::computeMeshNormals(mesh);

    CHECK_VETOR(normals.face[0], (Vec3{0.0F, 0.0F, 1.0F}), 1e-5F);
    CHECK_VETOR(normals.face[1], (Vec3{0.0F, 0.0F, 1.0F}), 1e-5F);
    CHECK_VETOR(normals.face[2], (Vec3{0.0F, 1.0F, 0.0F}), 1e-5F);

    const float raizDeCincoInversa = 1.0F / std::sqrt(5.0F);
    CHECK_VETOR(normals.vertex[0],
                (Vec3{0.0F, raizDeCincoInversa, 2.0F * raizDeCincoInversa}), 1e-5F);

    const Vec3 mediaPonderadaPorArea = normalizadaLocal(Vec3{0.0F, 0.5F, 4.5F});
    CHECK(distanciaLocal(normals.vertex[0], mediaPonderadaPorArea) > 1e-3F);
}

void normal_de_vertice_nula_e_finita() {
    cg::Mesh comVerticeIsolado;
    comVerticeIsolado.vertices = {{0.0F, 0.0F, 0.0F},
                                  {1.0F, 0.0F, 0.0F},
                                  {0.0F, 1.0F, 0.0F},
                                  {5.0F, 5.0F, 5.0F}};
    comVerticeIsolado.triangles = {cg::Triangle{0, 1, 2}};
    const cg::MeshNormals normaisIsolado = cg::computeMeshNormals(comVerticeIsolado);

    CHECK_EQ(normaisIsolado.vertex.size(), static_cast<std::size_t>(4));
    CHECK_VETOR(normaisIsolado.vertex[3], (Vec3{0.0F, 0.0F, 0.0F}), 0.0F);
    CHECK(std::isfinite(normaisIsolado.vertex[3].x));
    CHECK(std::isfinite(normaisIsolado.vertex[3].y));
    CHECK(std::isfinite(normaisIsolado.vertex[3].z));

    cg::Mesh degenerada;
    degenerada.vertices = {{0.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F}, {2.0F, 0.0F, 0.0F}};
    degenerada.triangles = {cg::Triangle{0, 1, 2}};
    const cg::MeshNormals normaisDegenerada = cg::computeMeshNormals(degenerada);

    for (const Vec3& normal : normaisDegenerada.vertex) {
        CHECK_VETOR(normal, (Vec3{0.0F, 0.0F, 0.0F}), 0.0F);
        CHECK(std::isfinite(normal.x));
        CHECK(std::isfinite(normal.y));
        CHECK(std::isfinite(normal.z));
    }
}

void face_degenerada_nao_contribui_para_o_vertice() {
    cg::Mesh mesh;
    mesh.vertices = {{0.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F}, {0.0F, 1.0F, 0.0F},
                     {2.0F, 0.0F, 0.0F}};
    mesh.triangles = {cg::Triangle{0, 1, 2}, cg::Triangle{0, 1, 3}};
    const cg::MeshNormals normals = cg::computeMeshNormals(mesh);

    CHECK_VETOR(normals.face[0], (Vec3{0.0F, 0.0F, 1.0F}), 1e-5F);
    CHECK_VETOR(normals.face[1], (Vec3{0.0F, 0.0F, 0.0F}), 0.0F);
    CHECK_VETOR(normals.vertex[0], (Vec3{0.0F, 0.0F, 1.0F}), 1e-5F);
    CHECK_VETOR(normals.vertex[1], (Vec3{0.0F, 0.0F, 1.0F}), 1e-5F);
    CHECK_VETOR(normals.vertex[2], (Vec3{0.0F, 0.0F, 1.0F}), 1e-5F);
    CHECK_VETOR(normals.vertex[3], (Vec3{0.0F, 0.0F, 0.0F}), 0.0F);
}

void normal_nao_finita_e_rejeitada() {
    cg::Mesh comNan;
    comNan.vertices = {{0.0F, 0.0F, 0.0F},
                       {1.0F, 0.0F, 0.0F},
                       {0.0F, 1.0F, 0.0F},
                       {std::numeric_limits<float>::quiet_NaN(), 0.0F, 0.0F}};
    comNan.triangles = {cg::Triangle{0, 1, 2}, cg::Triangle{0, 1, 3}, cg::Triangle{0, 2, 3}};
    const cg::MeshNormals normaisComNan = cg::computeMeshNormals(comNan);

    CHECK_EQ(normaisComNan.face.size(), static_cast<std::size_t>(3));
    CHECK_EQ(normaisComNan.vertex.size(), static_cast<std::size_t>(4));
    CHECK_VETOR(normaisComNan.face[0], (Vec3{0.0F, 0.0F, 1.0F}), 1e-5F);
    CHECK_VETOR(normaisComNan.face[1], (Vec3{}), 0.0F);
    CHECK_VETOR(normaisComNan.face[2], (Vec3{}), 0.0F);
    CHECK_VETOR(normaisComNan.vertex[0], (Vec3{0.0F, 0.0F, 1.0F}), 1e-5F);
    CHECK_VETOR(normaisComNan.vertex[1], (Vec3{0.0F, 0.0F, 1.0F}), 1e-5F);
    CHECK_VETOR(normaisComNan.vertex[2], (Vec3{0.0F, 0.0F, 1.0F}), 1e-5F);
    CHECK_VETOR(normaisComNan.vertex[3], (Vec3{}), 0.0F);

    cg::Mesh comInfinito;
    comInfinito.vertices = {{0.0F, 0.0F, 0.0F},
                            {1.0F, 0.0F, 0.0F},
                            {0.0F, 1.0F, 0.0F},
                            {std::numeric_limits<float>::infinity(), 0.0F, 0.0F}};
    comInfinito.triangles = {cg::Triangle{0, 1, 2}, cg::Triangle{0, 1, 3}, cg::Triangle{0, 2, 3}};
    const cg::MeshNormals normaisComInfinito = cg::computeMeshNormals(comInfinito);

    CHECK_EQ(normaisComInfinito.face.size(), static_cast<std::size_t>(3));
    CHECK_EQ(normaisComInfinito.vertex.size(), static_cast<std::size_t>(4));
    CHECK_VETOR(normaisComInfinito.face[0], (Vec3{0.0F, 0.0F, 1.0F}), 1e-5F);
    CHECK_VETOR(normaisComInfinito.face[1], (Vec3{}), 0.0F);
    CHECK_VETOR(normaisComInfinito.face[2], (Vec3{}), 0.0F);
    CHECK_VETOR(normaisComInfinito.vertex[0], (Vec3{0.0F, 0.0F, 1.0F}), 1e-5F);
    CHECK_VETOR(normaisComInfinito.vertex[1], (Vec3{0.0F, 0.0F, 1.0F}), 1e-5F);
    CHECK_VETOR(normaisComInfinito.vertex[2], (Vec3{0.0F, 0.0F, 1.0F}), 1e-5F);
    CHECK_VETOR(normaisComInfinito.vertex[3], (Vec3{}), 0.0F);
}

void indice_fora_da_faixa_e_ignorado() {
    cg::Mesh mesh;
    mesh.vertices = {{0.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F}, {0.0F, 1.0F, 0.0F}};
    mesh.triangles = {cg::Triangle{0, 1, 2}, cg::Triangle{0, 1, 99}};
    const cg::MeshNormals normals = cg::computeMeshNormals(mesh);

    CHECK_EQ(normals.face.size(), static_cast<std::size_t>(2));
    CHECK_VETOR(normals.face[0], (Vec3{0.0F, 0.0F, 1.0F}), 1e-5F);
    CHECK_VETOR(normals.face[1], (Vec3{}), 0.0F);
    CHECK_VETOR(normals.vertex[0], (Vec3{0.0F, 0.0F, 1.0F}), 1e-5F);
    CHECK_VETOR(normals.vertex[1], (Vec3{0.0F, 0.0F, 1.0F}), 1e-5F);
    CHECK_VETOR(normals.vertex[2], (Vec3{0.0F, 0.0F, 1.0F}), 1e-5F);
}

void modelos_reais_tem_normais_finitas() {
    const char* paths[] = {"models/triangulo.byu", "models/piramide.byu", "models/maca.byu",
                           "models/maca2.byu",     "models/vaso.byu",     "models/calice2.byu"};

    for (const char* path : paths) {
        cg::Mesh mesh;
        std::string error;
        CHECK(cg::loadMeshFromFile(path, mesh, error));
        const cg::MeshNormals normals = cg::computeMeshNormals(mesh);
        CHECK_EQ(normals.face.size(), mesh.triangles.size());
        CHECK_EQ(normals.vertex.size(), mesh.vertices.size());

        for (const cg::Vec3& normal : normals.face) {
            CHECK(std::isfinite(normal.x));
            CHECK(std::isfinite(normal.y));
            CHECK(std::isfinite(normal.z));
            CHECK(normal.length() == 0.0F || std::fabs(normal.length() - 1.0F) < 1e-5F);
        }
        bool temNormalDeFaceNaoNula = false;
        for (const cg::Vec3& normal : normals.face) {
            if (normal.length() > 0.0F) {
                temNormalDeFaceNaoNula = true;
            }
        }
        if (!temNormalDeFaceNaoNula) {
            test::reportFailure(__FILE__, __LINE__,
                                std::string(path) + " não tem nenhuma normal de face não nula");
        }
        for (const cg::Vec3& normal : normals.vertex) {
            CHECK(std::isfinite(normal.x));
            CHECK(std::isfinite(normal.y));
            CHECK(std::isfinite(normal.z));
            CHECK(normal.length() == 0.0F || std::fabs(normal.length() - 1.0F) < 1e-5F);
        }
    }
}

}

int main() {
    normal_da_face_aponta_para_z_positivo();
    winding_invertido_inverte_a_normal();
    vertice_compartilhado_recebe_media_das_faces();
    face_degenerada_nao_gera_nan();
    tetraedro_tem_normais_unitarias_por_vertice();
    media_de_vertice_nao_pondera_por_area();
    normal_de_vertice_nula_e_finita();
    face_degenerada_nao_contribui_para_o_vertice();
    normal_nao_finita_e_rejeitada();
    indice_fora_da_faixa_e_ignorado();
    modelos_reais_tem_normais_finitas();

    return test::failureCount() == 0 ? 0 : 1;
}
