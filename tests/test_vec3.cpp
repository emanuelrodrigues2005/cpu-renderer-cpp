#include "Vec3.h"
#include "test_util.h"

namespace {

void soma_e_subtracao_retornam_componentes_corretos() {
    const cg::Vec3 a{1.0F, 2.0F, 3.0F};
    const cg::Vec3 b{4.0F, 5.0F, 6.0F};

    const cg::Vec3 soma = a + b;
    CHECK_EQ(soma.x, 5.0F);
    CHECK_EQ(soma.y, 7.0F);
    CHECK_EQ(soma.z, 9.0F);

    const cg::Vec3 diferenca = b - a;
    CHECK_EQ(diferenca.x, 3.0F);
    CHECK_EQ(diferenca.y, 3.0F);
    CHECK_EQ(diferenca.z, 3.0F);
}

void produto_escalar_confere_com_calculo_manual() {
    const cg::Vec3 a{1.0F, 2.0F, 3.0F};
    const cg::Vec3 b{4.0F, 5.0F, 6.0F};

    CHECK_EQ(a.dot(b), 32.0F);
    CHECK_EQ(a.dot(cg::Vec3{0.0F, 0.0F, 0.0F}), 0.0F);

    const cg::Vec3 horizontal{1.0F, 0.0F, 0.0F};
    const cg::Vec3 vertical{0.0F, 1.0F, 0.0F};
    CHECK_EQ(horizontal.dot(vertical), 0.0F);
}

void produto_vetorial_segue_a_regra_da_mao_direita() {
    const cg::Vec3 x{1.0F, 0.0F, 0.0F};
    const cg::Vec3 y{0.0F, 1.0F, 0.0F};

    const cg::Vec3 z = x.cross(y);
    CHECK_EQ(z.x, 0.0F);
    CHECK_EQ(z.y, 0.0F);
    CHECK_EQ(z.z, 1.0F);

    const cg::Vec3 antiComutativo = y.cross(x);
    CHECK_EQ(antiComutativo.z, -1.0F);

    const cg::Vec3 paralelos = x.cross(cg::Vec3{2.0F, 0.0F, 0.0F});
    CHECK_EQ(paralelos.x, 0.0F);
    CHECK_EQ(paralelos.y, 0.0F);
    CHECK_EQ(paralelos.z, 0.0F);
}

void magnitude_confere_com_ternos_pitagoricos() {
    const cg::Vec3 ternoClassico{3.0F, 4.0F, 0.0F};
    const cg::Vec3 outroTerno{1.0F, 2.0F, 2.0F};
    const cg::Vec3 origem{0.0F, 0.0F, 0.0F};

    CHECK_EQ(ternoClassico.length(), 5.0F);
    CHECK_EQ(outroTerno.length(), 3.0F);
    CHECK_EQ(origem.length(), 0.0F);
}

void normalizacao_preserva_direcao_e_vira_unitario() {
    const cg::Vec3 v{0.0F, 3.0F, 4.0F};
    const cg::Vec3 unitario = v.normalized();

    CHECK_NEAR(unitario.x, 0.0F, 1e-6F);
    CHECK_NEAR(unitario.y, 0.6F, 1e-6F);
    CHECK_NEAR(unitario.z, 0.8F, 1e-6F);
    CHECK_NEAR(unitario.length(), 1.0F, 1e-6F);
}

void normalizacao_do_vetor_nulo_retorna_vetor_nulo() {
    const cg::Vec3 nulo{0.0F, 0.0F, 0.0F};
    const cg::Vec3 unitario = nulo.normalized();

    CHECK_EQ(unitario.x, 0.0F);
    CHECK_EQ(unitario.y, 0.0F);
    CHECK_EQ(unitario.z, 0.0F);
}

}

int main() {
    soma_e_subtracao_retornam_componentes_corretos();
    produto_escalar_confere_com_calculo_manual();
    produto_vetorial_segue_a_regra_da_mao_direita();
    magnitude_confere_com_ternos_pitagoricos();
    normalizacao_preserva_direcao_e_vira_unitario();
    normalizacao_do_vetor_nulo_retorna_vetor_nulo();

    return test::failureCount() == 0 ? 0 : 1;
}
