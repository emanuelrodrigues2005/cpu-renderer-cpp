#include <limits>

#include "math/Color.h"
#include "test_util.h"

namespace {

void empacotamento_usa_rrggbb() {
    const cg::Color vermelho{255.0F, 0.0F, 0.0F};
    const cg::Color verde{0.0F, 255.0F, 0.0F};
    const cg::Color azul{0.0F, 0.0F, 255.0F};
    const cg::Color exemplo{18.0F, 52.0F, 86.0F};

    CHECK_EQ(vermelho.packed(), 0xFF0000U);
    CHECK_EQ(verde.packed(), 0x00FF00U);
    CHECK_EQ(azul.packed(), 0x0000FFU);
    CHECK_EQ(exemplo.packed(), 0x123456U);
}

void canais_sao_limitados_e_arredondados() {
    const cg::Color limitado{-10.0F, 300.0F, 127.6F};
    const cg::Color arredondado{0.4F, 0.5F, 0.6F};
    const cg::Color arredondadoAposClamp{254.6F, 255.4F, -0.4F};

    CHECK_EQ(limitado.packed(), 0x00FF80U);
    CHECK_EQ(arredondado.packed(), 0x000101U);
    CHECK_EQ(arredondadoAposClamp.packed(), 0xFFFF00U);
}

void soma_e_escala_funcionam_por_componente() {
    const cg::Color a{10.0F, 20.0F, 30.0F};
    const cg::Color b{1.0F, 2.0F, 3.0F};
    const cg::Color soma = a + b;
    CHECK_EQ(soma.r, 11.0F);
    CHECK_EQ(soma.g, 22.0F);
    CHECK_EQ(soma.b, 33.0F);

    const cg::Color escalado = a * 2.0F;
    CHECK_EQ(escalado.r, 20.0F);
    CHECK_EQ(escalado.g, 40.0F);
    CHECK_EQ(escalado.b, 60.0F);

    const cg::Color prefixado = 3.0F * b;
    CHECK_EQ(prefixado.r, 3.0F);
    CHECK_EQ(prefixado.g, 6.0F);
    CHECK_EQ(prefixado.b, 9.0F);
}

void clamp_retorna_canais_no_intervalo() {
    const cg::Color color = cg::Color{-1.0F, 128.0F, 300.0F}.clamped();
    CHECK_EQ(color.r, 0.0F);
    CHECK_EQ(color.g, 128.0F);
    CHECK_EQ(color.b, 255.0F);
}

void canais_nao_finitos_nao_produzem_bits_invalidos() {
    const float nan = std::numeric_limits<float>::quiet_NaN();
    const float inf = std::numeric_limits<float>::infinity();
    const cg::Color cor{nan, inf, -inf};

    CHECK_EQ(cor.packed(), 0x00FF00U);
}

}

int main() {
    empacotamento_usa_rrggbb();
    canais_sao_limitados_e_arredondados();
    soma_e_escala_funcionam_por_componente();
    clamp_retorna_canais_no_intervalo();
    canais_nao_finitos_nao_produzem_bits_invalidos();

    return test::failureCount() == 0 ? 0 : 1;
}
