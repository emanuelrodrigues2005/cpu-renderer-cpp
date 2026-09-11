#include <cstddef>
#include <string>

#include "mesh/Mesh.h"
#include "test_util.h"

namespace {

bool contem(const std::string& texto, const std::string& trecho) {
    return texto.find(trecho) != std::string::npos;
}

void carrega_triangulo_com_contagens_corretas() {
    cg::Mesh mesh;
    std::string error;

    const bool ok = cg::loadMeshFromFile("models/triangulo.byu", mesh, error);

    CHECK(ok);
    CHECK_EQ(mesh.vertices.size(), std::size_t{3});
    CHECK_EQ(mesh.triangles.size(), std::size_t{1});
}

void primeiro_vertice_do_triangulo_tem_coordenadas_do_arquivo() {
    cg::Mesh mesh;
    std::string error;

    const bool ok = cg::loadMeshFromFile("models/triangulo.byu", mesh, error);

    CHECK(ok);
    CHECK_EQ(mesh.vertices[0].x, 50.0F);
    CHECK_EQ(mesh.vertices[0].y, 0.0F);
    CHECK_EQ(mesh.vertices[0].z, 0.0F);
}

void primeiro_triangulo_fica_zero_based_na_fronteira() {
    cg::Mesh mesh;
    std::string error;

    const bool ok = cg::loadMeshFromFile("models/triangulo.byu", mesh, error);

    CHECK(ok);
    CHECK_EQ(mesh.triangles[0].a, 0);
    CHECK_EQ(mesh.triangles[0].b, 1);
    CHECK_EQ(mesh.triangles[0].c, 2);
}

void todos_os_modelos_reais_carregam_com_as_contagens_esperadas() {
    const struct {
        const char* path;
        std::size_t vertices;
        std::size_t triangles;
    } cases[] = {
        {"models/triangulo.byu", 3, 1},   {"models/piramide.byu", 4, 4},
        {"models/maca.byu", 762, 1520},   {"models/maca2.byu", 781, 1540},
        {"models/vaso.byu", 681, 1340},   {"models/calice2.byu", 1062, 2120},
    };

    for (const auto& testCase : cases) {
        cg::Mesh mesh;
        std::string error;

        const bool ok = cg::loadMeshFromFile(testCase.path, mesh, error);

        CHECK(ok);
        CHECK_EQ(mesh.vertices.size(), testCase.vertices);
        CHECK_EQ(mesh.triangles.size(), testCase.triangles);
    }
}

void sobras_apos_ultimo_triangulo_sao_ignoradas() {
    cg::Mesh mesh;
    std::string error;

    const bool ok = cg::loadMeshFromFile("models/piramide.byu", mesh, error);

    CHECK(ok);
    CHECK_EQ(mesh.vertices.size(), std::size_t{4});
    CHECK_EQ(mesh.triangles.size(), std::size_t{4});
}

void arquivo_inexistente_falha_sem_alterar_a_malha() {
    cg::Mesh mesh;
    mesh.vertices.push_back(cg::Vertex{1.0F, 2.0F, 3.0F});
    mesh.triangles.push_back(cg::Triangle{0, 0, 0});
    std::string error;

    const bool ok = cg::loadMeshFromFile("models/inexistente.byu", mesh, error);

    CHECK(!ok);
    CHECK(!error.empty());
    CHECK_EQ(mesh.vertices.size(), std::size_t{1});
    CHECK_EQ(mesh.triangles.size(), std::size_t{1});
}

void header_invalido_falha_com_mensagem() {
    cg::Mesh mesh;
    std::string error;

    const bool ok = cg::loadMeshFromFile("tests/fixtures/header-invalido.byu", mesh, error);

    CHECK(!ok);
    CHECK(contem(error, "cabeçalho"));
}

void vertice_incompleto_falha_com_mensagem() {
    cg::Mesh mesh;
    std::string error;

    const bool ok = cg::loadMeshFromFile("tests/fixtures/vertice-incompleto.byu", mesh, error);

    CHECK(!ok);
    CHECK(contem(error, "vértice"));
}

void indice_fora_do_intervalo_falha_com_mensagem() {
    cg::Mesh mesh;
    std::string error;

    const bool ok = cg::loadMeshFromFile("tests/fixtures/indice-zero.byu", mesh, error);

    CHECK(!ok);
    CHECK(contem(error, "índice"));
    CHECK(contem(error, "[1, 3]"));
}

void indice_maior_que_a_contagem_falha_com_mensagem() {
    cg::Mesh mesh;
    std::string error;

    const bool ok = cg::loadMeshFromFile("tests/fixtures/indice-fora.byu", mesh, error);

    CHECK(!ok);
    CHECK(contem(error, "índice"));
}

void contagem_negativa_falha_com_mensagem() {
    cg::Mesh mesh;
    std::string error;

    const bool ok = cg::loadMeshFromFile("tests/fixtures/contagem-negativa.byu", mesh, error);

    CHECK(!ok);
    CHECK(contem(error, "cabeçalho"));
}

}

int main() {
    carrega_triangulo_com_contagens_corretas();
    primeiro_vertice_do_triangulo_tem_coordenadas_do_arquivo();
    primeiro_triangulo_fica_zero_based_na_fronteira();
    todos_os_modelos_reais_carregam_com_as_contagens_esperadas();
    sobras_apos_ultimo_triangulo_sao_ignoradas();
    arquivo_inexistente_falha_sem_alterar_a_malha();
    header_invalido_falha_com_mensagem();
    vertice_incompleto_falha_com_mensagem();
    indice_fora_do_intervalo_falha_com_mensagem();
    indice_maior_que_a_contagem_falha_com_mensagem();
    contagem_negativa_falha_com_mensagem();

    return test::failureCount() == 0 ? 0 : 1;
}
