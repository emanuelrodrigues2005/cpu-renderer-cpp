#include <string>

#include "pipeline/Camera.h"
#include "pipeline/Projection.h"
#include "test_util.h"

namespace {

cg::Camera cameraDoPdf() {
    cg::Camera camera;
    camera.d = 5.0F;
    camera.hx = 2.0F;
    camera.hy = 2.0F;
    return camera;
}

void centro_da_vista_projeta_no_centro_da_tela() {
    cg::ScreenPoint screen;

    const bool ok =
        cg::projectToScreen(cg::Vec3{0.0F, 0.0F, 5.0F}, cameraDoPdf(), 800, 600, screen);

    CHECK(ok);
    CHECK_EQ(screen.x, 400);
    CHECK_EQ(screen.y, 300);
}

void formulas_conferem_com_calculo_manual() {
    cg::ScreenPoint screen;

    const bool positivo =
        cg::projectToScreen(cg::Vec3{1.0F, 1.0F, 5.0F}, cameraDoPdf(), 800, 600, screen);
    CHECK(positivo);
    CHECK_EQ(screen.x, 600);
    CHECK_EQ(screen.y, 150);

    const bool negativo =
        cg::projectToScreen(cg::Vec3{1.0F, -1.0F, 5.0F}, cameraDoPdf(), 800, 600, screen);
    CHECK(negativo);
    CHECK_EQ(screen.x, 600);
    CHECK_EQ(screen.y, 450);
}

void ponto_atras_da_camera_nao_e_visivel() {
    cg::ScreenPoint screen{-1, -1};

    const bool noPlanoZ =
        cg::projectToScreen(cg::Vec3{0.0F, 0.0F, 0.0F}, cameraDoPdf(), 800, 600, screen);

    CHECK(!noPlanoZ);
    CHECK_EQ(screen.x, -1);
    CHECK_EQ(screen.y, -1);

    const bool atras =
        cg::projectToScreen(cg::Vec3{1.0F, 1.0F, -20.0F}, cameraDoPdf(), 800, 600, screen);

    CHECK(!atras);
    CHECK_EQ(screen.x, -1);
    CHECK_EQ(screen.y, -1);
}

void arredondamento_usa_lround() {
    cg::ScreenPoint screen;

    const bool ok =
        cg::projectToScreen(cg::Vec3{0.0F, 0.0F, 5.0F}, cameraDoPdf(), 801, 601, screen);

    CHECK(ok);
    CHECK_EQ(screen.x, 401);
    CHECK_EQ(screen.y, 301);
}

void coordenadas_fora_da_janela_nao_sao_limitadas() {
    cg::ScreenPoint screen;

    const bool ok =
        cg::projectToScreen(cg::Vec3{3.0F, 0.0F, 5.0F}, cameraDoPdf(), 800, 600, screen);

    CHECK(ok);
    CHECK_EQ(screen.x, 1000);
    CHECK_EQ(screen.y, 300);
}

void camera_e_projecao_com_o_exemplo_do_pdf() {
    cg::Camera camera;
    std::string error;
    CHECK(cg::loadCameraFromFile("camera/camera.txt", camera, error));

    cg::ScreenPoint origem;
    const bool origemVisivel = cg::projectToScreen(
        camera.worldToView(cg::Vec3{0.0F, 0.0F, 0.0F}), camera, 800, 600, origem);
    CHECK(origemVisivel);
    CHECK_EQ(origem.x, 400);
    CHECK_EQ(origem.y, 300);

    cg::ScreenPoint acima;
    const bool acimaVisivel = cg::projectToScreen(
        camera.worldToView(cg::Vec3{0.0F, 1.0F, 0.0F}), camera, 800, 600, acima);
    CHECK(acimaVisivel);
    CHECK_EQ(acima.x, 400);
    CHECK_EQ(acima.y, 301);
}

}

int main() {
    centro_da_vista_projeta_no_centro_da_tela();
    formulas_conferem_com_calculo_manual();
    ponto_atras_da_camera_nao_e_visivel();
    arredondamento_usa_lround();
    coordenadas_fora_da_janela_nao_sao_limitadas();
    camera_e_projecao_com_o_exemplo_do_pdf();

    return test::failureCount() == 0 ? 0 : 1;
}
