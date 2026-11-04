#include <limits>
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

void projecao_separa_coordenadas_normalizadas_da_tela() {
    cg::NormalizedPoint normalized;
    const bool ok = cg::projectToNormalized(
        cg::Vec3{1.0F, -1.0F, 5.0F}, cameraDoPdf(), normalized);

    CHECK(ok);
    CHECK_EQ(normalized.x, 0.5F);
    CHECK_EQ(normalized.y, -0.5F);

    cg::ScreenPoint screen;
    cg::normalizedToScreen(normalized, 800, 600, screen);
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

void projecao_com_profundidade_preserva_invz() {
    cg::ProjectedVertex projected;

    const bool ok = cg::projectToScreenWithDepth(cg::Vec3{0.0F, 0.0F, 5.0F}, cameraDoPdf(),
                                                 800, 600, projected);

    CHECK(ok);
    CHECK_EQ(projected.screen.x, 400);
    CHECK_EQ(projected.screen.y, 300);
    CHECK_NEAR(projected.viewZ, 5.0F, 1e-6F);
    CHECK_NEAR(projected.invZ, 0.2F, 1e-6F);
}

void projecao_com_profundidade_rejeita_z_nao_positivo() {
    cg::ProjectedVertex projected;

    const bool noPlano = cg::projectToScreenWithDepth(cg::Vec3{1.0F, 0.0F, 0.0F},
                                                      cameraDoPdf(), 800, 600, projected);
    CHECK(!noPlano);

    const bool atras = cg::projectToScreenWithDepth(cg::Vec3{1.0F, 0.0F, -2.0F},
                                                    cameraDoPdf(), 800, 600, projected);
    CHECK(!atras);

    const bool quaseNoPlano = cg::projectToScreenWithDepth(cg::Vec3{1.0F, 0.0F, 1e-30F},
                                                           cameraDoPdf(), 800, 600, projected);
    CHECK(!quaseNoPlano);
}

void projecao_com_profundidade_nao_muta_saida_em_falha() {
    cg::ProjectedVertex projected;
    projected.screen = cg::ScreenPoint{-1, -1};
    projected.viewZ = -1.0F;
    projected.invZ = -1.0F;

    const cg::Camera camera = cameraDoPdf();

    const bool noPlano =
        cg::projectToScreenWithDepth(cg::Vec3{1.0F, 1.0F, 0.0F}, camera, 800, 600, projected);
    CHECK(!noPlano);
    CHECK_EQ(projected.screen.x, -1);
    CHECK_EQ(projected.screen.y, -1);
    CHECK_NEAR(projected.viewZ, -1.0F, 1e-6F);
    CHECK_NEAR(projected.invZ, -1.0F, 1e-6F);

    const bool atras =
        cg::projectToScreenWithDepth(cg::Vec3{1.0F, 1.0F, -2.0F}, camera, 800, 600, projected);
    CHECK(!atras);
    CHECK_EQ(projected.screen.x, -1);
    CHECK_EQ(projected.screen.y, -1);
    CHECK_NEAR(projected.viewZ, -1.0F, 1e-6F);
    CHECK_NEAR(projected.invZ, -1.0F, 1e-6F);

    cg::Camera cameraInvalida = cameraDoPdf();
    cameraInvalida.d = 0.0F;
    const bool cameraInvalidaOk = cg::projectToScreenWithDepth(cg::Vec3{1.0F, 1.0F, 5.0F},
                                                               cameraInvalida, 800, 600,
                                                               projected);
    CHECK(!cameraInvalidaOk);
    CHECK_EQ(projected.screen.x, -1);
    CHECK_EQ(projected.screen.y, -1);
    CHECK_NEAR(projected.viewZ, -1.0F, 1e-6F);
    CHECK_NEAR(projected.invZ, -1.0F, 1e-6F);
}

void projecao_com_profundidade_rejeita_invz_nao_finito() {
    const float kDenormMin = std::numeric_limits<float>::denorm_min();

    cg::NormalizedPoint normalized;
    const bool normalizadoOk =
        cg::projectToNormalized(cg::Vec3{0.0F, 0.0F, kDenormMin}, cameraDoPdf(), normalized);
    CHECK(normalizadoOk);
    CHECK_NEAR(normalized.x, 0.0F, 1e-6F);
    CHECK_NEAR(normalized.y, 0.0F, 1e-6F);

    cg::ProjectedVertex recusado;
    recusado.screen = cg::ScreenPoint{-1, -1};
    recusado.viewZ = -1.0F;
    recusado.invZ = -1.0F;

    const bool recusadoOk = cg::projectToScreenWithDepth(cg::Vec3{0.0F, 0.0F, kDenormMin},
                                                         cameraDoPdf(), 800, 600, recusado);
    CHECK(!recusadoOk);
    CHECK_EQ(recusado.screen.x, -1);
    CHECK_EQ(recusado.screen.y, -1);
    CHECK_NEAR(recusado.viewZ, -1.0F, 1e-6F);
    CHECK_NEAR(recusado.invZ, -1.0F, 1e-6F);
}

void projecao_normalizada_nao_muta_saida_em_falha() {
    cg::NormalizedPoint normalized;
    normalized.x = -7.0F;
    normalized.y = 9.0F;

    const cg::Camera camera = cameraDoPdf();

    const bool noPlano =
        cg::projectToNormalized(cg::Vec3{1.0F, 1.0F, 0.0F}, camera, normalized);
    CHECK(!noPlano);
    CHECK_NEAR(normalized.x, -7.0F, 1e-6F);
    CHECK_NEAR(normalized.y, 9.0F, 1e-6F);

    const bool atras = cg::projectToNormalized(cg::Vec3{1.0F, 1.0F, -2.0F}, camera, normalized);
    CHECK(!atras);
    CHECK_NEAR(normalized.x, -7.0F, 1e-6F);
    CHECK_NEAR(normalized.y, 9.0F, 1e-6F);

    cg::Camera cameraInvalida = cameraDoPdf();
    cameraInvalida.hx = 0.0F;
    const bool cameraInvalidaOk =
        cg::projectToNormalized(cg::Vec3{1.0F, 1.0F, 5.0F}, cameraInvalida, normalized);
    CHECK(!cameraInvalidaOk);
    CHECK_NEAR(normalized.x, -7.0F, 1e-6F);
    CHECK_NEAR(normalized.y, 9.0F, 1e-6F);

    const bool quaseNoPlano =
        cg::projectToNormalized(cg::Vec3{1.0F, 1.0F, 1e-30F}, camera, normalized);
    CHECK(!quaseNoPlano);
    CHECK_NEAR(normalized.x, -7.0F, 1e-6F);
    CHECK_NEAR(normalized.y, 9.0F, 1e-6F);
}

void projecao_respeita_limite_de_coordenada_normalizada() {
    cg::ProjectedVertex aceito;
    const bool dentroDoLimite = cg::projectToScreenWithDepth(
        cg::Vec3{39960.0F, 0.0F, 1.0F}, cameraDoPdf(), 800, 600, aceito);
    CHECK(dentroDoLimite);
    CHECK_EQ(aceito.screen.x, 39960400);
    CHECK_EQ(aceito.screen.y, 300);

    cg::ProjectedVertex noLimite;
    const bool exatamenteNoLimite = cg::projectToScreenWithDepth(
        cg::Vec3{40000.0F, 0.0F, 1.0F}, cameraDoPdf(), 800, 600, noLimite);
    CHECK(exatamenteNoLimite);
    CHECK_EQ(noLimite.screen.x, 40000400);
    CHECK_EQ(noLimite.screen.y, 300);

    cg::ProjectedVertex recusado;
    recusado.screen = cg::ScreenPoint{-1, -1};
    recusado.viewZ = -1.0F;
    recusado.invZ = -1.0F;

    const bool foraDoLimite = cg::projectToScreenWithDepth(
        cg::Vec3{40040.0F, 0.0F, 1.0F}, cameraDoPdf(), 800, 600, recusado);
    CHECK(!foraDoLimite);
    CHECK_EQ(recusado.screen.x, -1);
    CHECK_EQ(recusado.screen.y, -1);
    CHECK_NEAR(recusado.viewZ, -1.0F, 1e-6F);
    CHECK_NEAR(recusado.invZ, -1.0F, 1e-6F);
}

void project_to_screen_continua_sendo_wrapper() {
    cg::ScreenPoint legado{-1, -1};
    const bool legadoOk =
        cg::projectToScreen(cg::Vec3{1.0F, 1.0F, 5.0F}, cameraDoPdf(), 800, 600, legado);
    CHECK(legadoOk);

    cg::ProjectedVertex comProfundidade;
    const bool comProfundidadeOk = cg::projectToScreenWithDepth(
        cg::Vec3{1.0F, 1.0F, 5.0F}, cameraDoPdf(), 800, 600, comProfundidade);
    CHECK(comProfundidadeOk);

    CHECK_EQ(legado.x, comProfundidade.screen.x);
    CHECK_EQ(legado.y, comProfundidade.screen.y);
    CHECK_EQ(legado.x, 600);
    CHECK_EQ(legado.y, 150);
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
    projecao_separa_coordenadas_normalizadas_da_tela();
    ponto_atras_da_camera_nao_e_visivel();
    arredondamento_usa_lround();
    coordenadas_fora_da_janela_nao_sao_limitadas();
    projecao_com_profundidade_preserva_invz();
    projecao_com_profundidade_rejeita_z_nao_positivo();
    projecao_com_profundidade_nao_muta_saida_em_falha();
    projecao_com_profundidade_rejeita_invz_nao_finito();
    projecao_normalizada_nao_muta_saida_em_falha();
    projecao_respeita_limite_de_coordenada_normalizada();
    project_to_screen_continua_sendo_wrapper();
    camera_e_projecao_com_o_exemplo_do_pdf();

    return test::failureCount() == 0 ? 0 : 1;
}
