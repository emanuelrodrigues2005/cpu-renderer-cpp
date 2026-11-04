#include <cmath>
#include <limits>

#include "math/Color.h"
#include "math/Vec3.h"
#include "shading/Lighting.h"
#include "shading/Phong.h"
#include "test_util.h"

namespace {

constexpr float kTolerancia = 1e-4F;

const cg::Vec3 kOrigem{0.0F, 0.0F, 0.0F};
const cg::Vec3 kNormalZ{0.0F, 0.0F, 1.0F};
const cg::Vec3 kOlhoZ{0.0F, 0.0F, 5.0F};

cg::Lighting iluminacaoBase() {
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

cg::Lighting somenteEspecular() {
    cg::Lighting lighting;
    lighting.light.ambient = cg::Color{0.0F, 0.0F, 0.0F};
    lighting.light.color = cg::Color{255.0F, 255.0F, 255.0F};
    lighting.material.ka = 0.0F;
    lighting.material.kd = cg::Vec3{0.0F, 0.0F, 0.0F};
    lighting.material.od = cg::Vec3{1.0F, 1.0F, 1.0F};
    lighting.material.ks = 1.0F;
    lighting.material.eta = 1.0F;
    return lighting;
}

void exige_ambiente(const cg::Vec3& ponto, const cg::Vec3& normal, const cg::Vec3& olho,
                    const cg::Lighting& lighting) {
    const cg::Color cor = cg::phongColor(ponto, normal, olho, lighting);

    CHECK_NEAR(cor.r, 20.0F, kTolerancia);
    CHECK_NEAR(cor.g, 20.0F, kTolerancia);
    CHECK_NEAR(cor.b, 20.0F, kTolerancia);
}

void ambiente_difusa_e_especular_conferem() {
    const cg::Color cor = cg::phongColor(kOrigem, kNormalZ, kOlhoZ, iluminacaoBase());

    CHECK_NEAR(cor.r, 190.0F, kTolerancia);
    CHECK_NEAR(cor.g, 85.0F, kTolerancia);
    CHECK_NEAR(cor.b, 53.0F, kTolerancia);
}

void especular_nao_e_tingido_por_od() {
    cg::Lighting lighting;
    lighting.light.ambient = cg::Color{0.0F, 0.0F, 0.0F};
    lighting.light.color = cg::Color{255.0F, 255.0F, 255.0F};
    lighting.light.position = kOlhoZ;
    lighting.material.ka = 0.0F;
    lighting.material.kd = cg::Vec3{0.0F, 0.0F, 0.0F};
    lighting.material.od = cg::Vec3{0.1F, 0.2F, 0.3F};
    lighting.material.ks = 1.0F;
    lighting.material.eta = 1.0F;

    const cg::Color cor = cg::phongColor(kOrigem, kNormalZ, kOlhoZ, lighting);

    CHECK_NEAR(cor.r, 255.0F, kTolerancia);
    CHECK_NEAR(cor.g, 255.0F, kTolerancia);
    CHECK_NEAR(cor.b, 255.0F, kTolerancia);
}

void ambiente_independe_de_od() {
    cg::Lighting lighting;
    lighting.light.ambient = cg::Color{100.0F, 50.0F, 200.0F};
    lighting.light.color = cg::Color{0.0F, 0.0F, 0.0F};
    lighting.light.position = cg::Vec3{0.0F, 0.0F, -5.0F};
    lighting.material.ka = 1.0F;
    lighting.material.kd = cg::Vec3{0.0F, 0.0F, 0.0F};
    lighting.material.od = cg::Vec3{0.0F, 0.0F, 0.0F};
    lighting.material.ks = 0.0F;
    lighting.material.eta = 1.0F;

    const cg::Color cor = cg::phongColor(kOrigem, kNormalZ, kOlhoZ, lighting);

    CHECK_NEAR(cor.r, 100.0F, kTolerancia);
    CHECK_NEAR(cor.g, 50.0F, kTolerancia);
    CHECK_NEAR(cor.b, 200.0F, kTolerancia);
}

void two_sided_inverte_normal_quando_nl_e_vn_negativos() {
    const cg::Vec3 normalInvertida{0.0F, 0.0F, -1.0F};

    const cg::Color cor = cg::phongColor(kOrigem, normalInvertida, kOlhoZ, iluminacaoBase());

    CHECK_NEAR(cor.r, 190.0F, kTolerancia);
    CHECK_NEAR(cor.g, 85.0F, kTolerancia);
    CHECK_NEAR(cor.b, 53.0F, kTolerancia);
}

void sem_difusa_nem_especular_quando_luz_atras() {
    cg::Lighting lighting = iluminacaoBase();
    lighting.light.position = cg::Vec3{0.0F, 0.0F, -5.0F};

    const cg::Color cor = cg::phongColor(kOrigem, kNormalZ, kOlhoZ, lighting);

    CHECK_NEAR(cor.r, 20.0F, kTolerancia);
    CHECK_NEAR(cor.g, 20.0F, kTolerancia);
    CHECK_NEAR(cor.b, 20.0F, kTolerancia);
}

void sem_especular_quando_rv_negativo() {
    const cg::Vec3 olhoOposto{0.0F, 0.0F, -5.0F};

    const cg::Color cor = cg::phongColor(kOrigem, kNormalZ, olhoOposto, iluminacaoBase());

    CHECK_NEAR(cor.r, 90.0F, kTolerancia);
    CHECK_NEAR(cor.g, 35.0F, kTolerancia);
    CHECK_NEAR(cor.b, 28.0F, kTolerancia);
}

void sem_especular_quando_nl_zero_com_rv_positivo() {
    cg::Lighting lighting;
    lighting.light.ambient = cg::Color{100.0F, 100.0F, 100.0F};
    lighting.light.color = cg::Color{255.0F, 255.0F, 255.0F};
    lighting.light.position = cg::Vec3{1.0F, 0.0F, 0.0F};
    lighting.material.ka = 0.2F;
    lighting.material.kd = cg::Vec3{0.0F, 0.0F, 0.0F};
    lighting.material.od = cg::Vec3{1.0F, 1.0F, 1.0F};
    lighting.material.ks = 1.0F;
    lighting.material.eta = 1.0F;

    exige_ambiente(kOrigem, kNormalZ, cg::Vec3{-1.0F, 0.0F, 0.0F}, lighting);
}

void sem_especular_quando_rv_zero() {
    const cg::Vec3 olhoLateral{1.0F, 0.0F, 0.0F};

    const cg::Color cor = cg::phongColor(kOrigem, kNormalZ, olhoLateral, iluminacaoBase());

    CHECK_NEAR(cor.r, 90.0F, kTolerancia);
    CHECK_NEAR(cor.g, 35.0F, kTolerancia);
    CHECK_NEAR(cor.b, 28.0F, kTolerancia);
}

void eta_maior_reduz_o_destaque() {
    const cg::Vec3 olhoEsquerda{-5.0F, 0.0F, 0.0F};
    const float diagonal = 7.0710678F;
    const cg::Vec3 fonteDiagonal{diagonal, 0.0F, diagonal};

    cg::Lighting sharp = somenteEspecular();
    sharp.light.position = fonteDiagonal;
    sharp.material.eta = 1.0F;

    cg::Lighting broad = somenteEspecular();
    broad.light.position = fonteDiagonal;
    broad.material.eta = 5.0F;

    const cg::Color comEtaBaixo = cg::phongColor(kOrigem, kNormalZ, olhoEsquerda, sharp);
    const cg::Color comEtaAlto = cg::phongColor(kOrigem, kNormalZ, olhoEsquerda, broad);

    CHECK_NEAR(comEtaBaixo.r, 255.0F * 0.70710678F, 1e-3F);
    CHECK(comEtaAlto.r < comEtaBaixo.r);
    CHECK_NEAR(comEtaAlto.r, 255.0F * std::pow(0.70710678F, 5.0F), 1e-3F);
}

void eta_fracionaria_escala_o_especular() {
    cg::Lighting lighting = somenteEspecular();
    lighting.light.position = cg::Vec3{3.0F, 0.0F, 4.0F};
    lighting.material.eta = 2.5F;

    const cg::Color cor = cg::phongColor(kOrigem, kNormalZ, kOlhoZ, lighting);
    const float esperado = 255.0F * std::pow(0.8F, 2.5F);

    CHECK_NEAR(cor.r, esperado, 1e-3F);
    CHECK_NEAR(cor.g, esperado, 1e-3F);
    CHECK_NEAR(cor.b, esperado, 1e-3F);
}

void normal_nao_unitaria_e_normalizada() {
    const cg::Vec3 normalEscalada{0.0F, 0.0F, 10.0F};
    const cg::Vec3 normalObliquaEscalada{0.0F, 6.0F, 8.0F};
    const cg::Vec3 normalObliqua{0.0F, 0.6F, 0.8F};

    const cg::Color unitaria = cg::phongColor(kOrigem, kNormalZ, kOlhoZ, iluminacaoBase());
    const cg::Color escalada = cg::phongColor(kOrigem, normalEscalada, kOlhoZ, iluminacaoBase());

    CHECK_NEAR(escalada.r, unitaria.r, kTolerancia);
    CHECK_NEAR(escalada.g, unitaria.g, kTolerancia);
    CHECK_NEAR(escalada.b, unitaria.b, kTolerancia);
    CHECK_NEAR(escalada.r, 190.0F, kTolerancia);

    const cg::Color obliqua = cg::phongColor(kOrigem, normalObliqua, kOlhoZ, iluminacaoBase());
    const cg::Color obliquaEscalada =
        cg::phongColor(kOrigem, normalObliquaEscalada, kOlhoZ, iluminacaoBase());

    CHECK_NEAR(obliquaEscalada.r, obliqua.r, kTolerancia);
    CHECK_NEAR(obliquaEscalada.g, obliqua.g, kTolerancia);
    CHECK_NEAR(obliquaEscalada.b, obliqua.b, kTolerancia);
    CHECK_NEAR(obliquaEscalada.r, 104.0F, kTolerancia);
    CHECK_NEAR(obliquaEscalada.g, 46.0F, kTolerancia);
    CHECK_NEAR(obliquaEscalada.b, 33.4F, kTolerancia);
}

void normal_nula_retorna_ambiente() {
    const cg::Vec3 normalNula{0.0F, 0.0F, 0.0F};

    const cg::Color cor = cg::phongColor(kOrigem, normalNula, kOlhoZ, iluminacaoBase());

    CHECK_NEAR(cor.r, 20.0F, kTolerancia);
    CHECK_NEAR(cor.g, 20.0F, kTolerancia);
    CHECK_NEAR(cor.b, 20.0F, kTolerancia);
}

void ponto_na_fonte_retorna_ambiente() {
    const cg::Vec3 ponto{0.0F, 0.0F, 5.0F};
    const cg::Vec3 olho{0.0F, 0.0F, 7.0F};

    exige_ambiente(ponto, kNormalZ, olho, iluminacaoBase());
}

void ponto_no_centro_da_camera_retorna_ambiente() {
    cg::Lighting lighting = iluminacaoBase();
    lighting.light.position = cg::Vec3{0.0F, 0.0F, 10.0F};
    const cg::Vec3 ponto{0.0F, 0.0F, 5.0F};

    exige_ambiente(ponto, kNormalZ, ponto, lighting);
}

void canais_sao_limitados_e_arredondados() {
    cg::Lighting saturado;
    saturado.light.ambient = cg::Color{255.0F, 255.0F, 255.0F};
    saturado.light.color = cg::Color{255.0F, 255.0F, 255.0F};
    saturado.light.position = kOlhoZ;
    saturado.material.ka = 1.0F;
    saturado.material.kd = cg::Vec3{1.0F, 1.0F, 1.0F};
    saturado.material.od = cg::Vec3{1.0F, 1.0F, 1.0F};
    saturado.material.ks = 1.0F;
    saturado.material.eta = 1.0F;

    const cg::Color saturada = cg::phongColor(kOrigem, kNormalZ, kOlhoZ, saturado);

    CHECK_EQ(saturada.packed(), 0xFFFFFFU);
    CHECK(saturada.r >= 0.0F && saturada.r <= 255.0F);
    CHECK(saturada.g >= 0.0F && saturada.g <= 255.0F);
    CHECK(saturada.b >= 0.0F && saturada.b <= 255.0F);

    cg::Lighting arredondaParaCima;
    arredondaParaCima.light.ambient = cg::Color{1.0F, 1.0F, 1.0F};
    arredondaParaCima.material.ka = 0.5F;
    arredondaParaCima.material.kd = cg::Vec3{0.0F, 0.0F, 0.0F};
    arredondaParaCima.material.ks = 0.0F;

    CHECK_EQ(cg::phongColor(kOrigem, kNormalZ, kOlhoZ, arredondaParaCima).packed(), 0x010101U);

    cg::Lighting arredondaParaBaixo;
    arredondaParaBaixo.light.ambient = cg::Color{1.0F, 1.0F, 1.0F};
    arredondaParaBaixo.material.ka = 0.2F;
    arredondaParaBaixo.material.kd = cg::Vec3{0.0F, 0.0F, 0.0F};
    arredondaParaBaixo.material.ks = 0.0F;

    CHECK_EQ(cg::phongColor(kOrigem, kNormalZ, kOlhoZ, arredondaParaBaixo).packed(), 0x000000U);
}

void exige_saida_finita(const cg::Vec3& ponto, const cg::Vec3& normal, const cg::Vec3& olho,
                        const cg::Lighting& lighting) {
    const cg::Color cor = cg::phongColor(ponto, normal, olho, lighting);

    CHECK(std::isfinite(cor.r));
    CHECK(std::isfinite(cor.g));
    CHECK(std::isfinite(cor.b));
    CHECK(cor.r >= 0.0F && cor.r <= 255.0F);
    CHECK(cor.g >= 0.0F && cor.g <= 255.0F);
    CHECK(cor.b >= 0.0F && cor.b <= 255.0F);
    CHECK((cor.packed() & 0xFF000000U) == 0U);
}

void entradas_nao_finitas_de_direcao_retornam_ambiente() {
    const float nan = std::numeric_limits<float>::quiet_NaN();
    const float inf = std::numeric_limits<float>::infinity();
    const cg::Vec3 valores[] = {
        {nan, 0.0F, 0.0F},  {0.0F, nan, 0.0F},  {0.0F, 0.0F, nan},
        {inf, 0.0F, 0.0F},  {0.0F, inf, 0.0F},  {0.0F, 0.0F, inf},
        {-inf, 0.0F, 0.0F}, {0.0F, -inf, 0.0F}, {0.0F, 0.0F, -inf},
    };

    for (const cg::Vec3& valor : valores) {
        exige_ambiente(valor, kNormalZ, kOlhoZ, iluminacaoBase());

        exige_ambiente(kOrigem, valor, kOlhoZ, iluminacaoBase());

        exige_ambiente(kOrigem, kNormalZ, valor, iluminacaoBase());

        cg::Lighting lighting = iluminacaoBase();
        lighting.light.position = valor;
        exige_ambiente(kOrigem, kNormalZ, kOlhoZ, lighting);
    }
}

void atribuiCanal(cg::Color& cor, int canal, float valor) {
    if (canal == 0) {
        cor.r = valor;
    } else if (canal == 1) {
        cor.g = valor;
    } else {
        cor.b = valor;
    }
}

void atribuiCanal(cg::Vec3& vetor, int canal, float valor) {
    if (canal == 0) {
        vetor.x = valor;
    } else if (canal == 1) {
        vetor.y = valor;
    } else {
        vetor.z = valor;
    }
}

void entradas_nao_finitas_nao_produzem_nan() {
    const float nan = std::numeric_limits<float>::quiet_NaN();
    const float inf = std::numeric_limits<float>::infinity();
    const float valores[] = {nan, inf, -inf};

    for (const float valor : valores) {
        for (int canal = 0; canal < 3; ++canal) {
            cg::Lighting lighting = iluminacaoBase();
            atribuiCanal(lighting.light.color, canal, valor);
            exige_saida_finita(kOrigem, kNormalZ, kOlhoZ, lighting);

            lighting = iluminacaoBase();
            atribuiCanal(lighting.light.ambient, canal, valor);
            exige_saida_finita(kOrigem, kNormalZ, kOlhoZ, lighting);

            lighting = iluminacaoBase();
            atribuiCanal(lighting.material.kd, canal, valor);
            exige_saida_finita(kOrigem, kNormalZ, kOlhoZ, lighting);

            lighting = iluminacaoBase();
            atribuiCanal(lighting.material.od, canal, valor);
            exige_saida_finita(kOrigem, kNormalZ, kOlhoZ, lighting);
        }

        cg::Lighting lighting = iluminacaoBase();
        lighting.material.ka = valor;
        exige_saida_finita(kOrigem, kNormalZ, kOlhoZ, lighting);

        lighting = iluminacaoBase();
        lighting.material.ks = valor;
        exige_saida_finita(kOrigem, kNormalZ, kOlhoZ, lighting);

        lighting = iluminacaoBase();
        lighting.material.eta = valor;
        exige_saida_finita(kOrigem, kNormalZ, kOlhoZ, lighting);
    }

    cg::Lighting lighting = iluminacaoBase();
    lighting.light.color = cg::Color{nan, inf, 50.0F};
    lighting.material.kd = cg::Vec3{nan, 0.3F, 0.2F};
    lighting.material.ks = nan;
    lighting.material.eta = inf;
    exige_saida_finita(kOrigem, kNormalZ, kOlhoZ, lighting);

    lighting = iluminacaoBase();
    lighting.light.ambient = cg::Color{nan, inf, -inf};
    lighting.light.color = cg::Color{nan, inf, -inf};
    lighting.material.ka = nan;
    lighting.material.kd = cg::Vec3{inf, nan, -inf};
    lighting.material.od = cg::Vec3{nan, inf, 0.8F};
    lighting.material.ks = inf;
    lighting.material.eta = nan;
    exige_saida_finita(kOrigem, kNormalZ, kOlhoZ, lighting);
}

}

int main() {
    ambiente_difusa_e_especular_conferem();
    especular_nao_e_tingido_por_od();
    ambiente_independe_de_od();
    two_sided_inverte_normal_quando_nl_e_vn_negativos();
    sem_difusa_nem_especular_quando_luz_atras();
    sem_especular_quando_rv_negativo();
    sem_especular_quando_nl_zero_com_rv_positivo();
    sem_especular_quando_rv_zero();
    eta_maior_reduz_o_destaque();
    eta_fracionaria_escala_o_especular();
    normal_nao_unitaria_e_normalizada();
    normal_nula_retorna_ambiente();
    ponto_na_fonte_retorna_ambiente();
    ponto_no_centro_da_camera_retorna_ambiente();
    canais_sao_limitados_e_arredondados();
    entradas_nao_finitas_de_direcao_retornam_ambiente();
    entradas_nao_finitas_nao_produzem_nan();

    return test::failureCount() == 0 ? 0 : 1;
}
