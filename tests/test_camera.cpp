#include <string>

#include "Camera.h"
#include "test_util.h"

namespace {

bool contem(const std::string& texto, const std::string& trecho) {
    return texto.find(trecho) != std::string::npos;
}

void carrega_parametros_do_camera_txt_do_pdf() {
    cg::Camera camera;
    std::string error;

    const bool ok = cg::loadCameraFromFile("camera/camera.txt", camera, error);

    CHECK(ok);
    CHECK_EQ(camera.c.x, 0.0F);
    CHECK_EQ(camera.c.y, -500.0F);
    CHECK_EQ(camera.c.z, 500.0F);
    CHECK_EQ(camera.d, 5.0F);
    CHECK_EQ(camera.hx, 2.0F);
    CHECK_EQ(camera.hy, 2.0F);
}

void base_da_camera_e_ortonormal_com_u_na_direcao_x() {
    cg::Camera camera;
    std::string error;

    const bool ok = cg::loadCameraFromFile("camera/camera.txt", camera, error);

    CHECK(ok);
    CHECK_NEAR(camera.n.length(), 1.0F, 1e-5F);
    CHECK_NEAR(camera.v.length(), 1.0F, 1e-5F);
    CHECK_NEAR(camera.u.length(), 1.0F, 1e-5F);
    CHECK_NEAR(camera.u.x, 1.0F, 1e-5F);
    CHECK_NEAR(camera.u.y, 0.0F, 1e-5F);
    CHECK_NEAR(camera.u.z, 0.0F, 1e-5F);
    CHECK_NEAR(camera.n.dot(camera.v), 0.0F, 1e-5F);
    CHECK_NEAR(camera.n.dot(camera.u), 0.0F, 1e-5F);
    CHECK_NEAR(camera.v.dot(camera.u), 0.0F, 1e-5F);
}

void world_to_view_confere_com_valores_calculados_a_mao() {
    cg::Camera camera;
    std::string error;

    const bool ok = cg::loadCameraFromFile("camera/camera.txt", camera, error);

    CHECK(ok);

    const cg::Vec3 origem = camera.worldToView(cg::Vec3{0.0F, 0.0F, 0.0F});
    CHECK_NEAR(origem.x, 0.0F, 1e-3F);
    CHECK_NEAR(origem.y, 0.0F, 1e-3F);
    CHECK_NEAR(origem.z, 707.1068F, 1e-3F);

    const cg::Vec3 ponto = camera.worldToView(cg::Vec3{0.0F, 1.0F, 0.0F});
    CHECK_NEAR(ponto.x, 0.0F, 1e-3F);
    CHECK_NEAR(ponto.y, -0.7071068F, 1e-3F);
    CHECK_NEAR(ponto.z, 707.8139F, 1e-3F);
}

void parser_tolera_comentarios_separadores_e_caixa() {
    cg::Camera camera;
    std::string error;

    const bool ok =
        cg::loadCameraFromFile("tests/fixtures/camera-tolerante.txt", camera, error);

    CHECK(ok);
    CHECK_EQ(camera.c.y, -500.0F);
    CHECK_EQ(camera.d, 5.0F);
    CHECK_EQ(camera.hx, 2.0F);
    CHECK_NEAR(camera.u.x, 1.0F, 1e-5F);
}

void camera_com_v_paralelo_a_n_e_rejeitada() {
    cg::Camera camera;
    std::string error;

    const bool ok =
        cg::loadCameraFromFile("tests/fixtures/camera-v-paralelo.txt", camera, error);

    CHECK(!ok);
    CHECK(contem(error, "paralelo"));
}

void camera_com_escalar_nao_positivo_e_rejeitada() {
    cg::Camera camera;
    std::string error;

    const bool ok =
        cg::loadCameraFromFile("tests/fixtures/camera-escalar-invalido.txt", camera, error);

    CHECK(!ok);
    CHECK(contem(error, "positivo"));
}

void campos_ausentes_e_numeros_invalidos_falham() {
    cg::Camera camera;
    std::string error;

    const bool ausente =
        cg::loadCameraFromFile("tests/fixtures/camera-campo-ausente.txt", camera, error);

    CHECK(!ausente);
    CHECK(contem(error, "ausente"));
    CHECK(contem(error, "hy"));

    error.clear();

    const bool invalido =
        cg::loadCameraFromFile("tests/fixtures/camera-numero-invalido.txt", camera, error);

    CHECK(!invalido);
    CHECK(contem(error, "d"));
}

void falha_nao_altera_a_camera_de_destino() {
    cg::Camera camera;
    std::string error;
    CHECK(cg::loadCameraFromFile("camera/camera.txt", camera, error));

    const cg::Camera original = camera;

    CHECK(!cg::loadCameraFromFile("tests/fixtures/camera-escalar-invalido.txt", camera, error));
    CHECK_EQ(camera.hx, original.hx);
    CHECK_NEAR(camera.n.y, original.n.y, 1e-6F);
    CHECK_NEAR(camera.u.x, original.u.x, 1e-6F);
}

}

int main() {
    carrega_parametros_do_camera_txt_do_pdf();
    base_da_camera_e_ortonormal_com_u_na_direcao_x();
    world_to_view_confere_com_valores_calculados_a_mao();
    parser_tolera_comentarios_separadores_e_caixa();
    camera_com_v_paralelo_a_n_e_rejeitada();
    camera_com_escalar_nao_positivo_e_rejeitada();
    campos_ausentes_e_numeros_invalidos_falham();
    falha_nao_altera_a_camera_de_destino();

    return test::failureCount() == 0 ? 0 : 1;
}
