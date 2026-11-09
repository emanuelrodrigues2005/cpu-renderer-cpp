#include <cmath>
#include <limits>

#include "pipeline/DepthBuffer.h"
#include "test_util.h"

namespace {

constexpr float kTolerance = 1e-6F;

void buffer_novo_esta_zerado() {
    cg::DepthBuffer depth{4, 3};

    CHECK_EQ(depth.width(), 4);
    CHECK_EQ(depth.height(), 3);
    CHECK_NEAR(depth.at(0, 0), 0.0F, kTolerance);
    CHECK_NEAR(depth.at(3, 2), 0.0F, kTolerance);
}

void maior_invz_vence() {
    cg::DepthBuffer depth{4, 3};

    CHECK(depth.testAndSet(1, 1, 0.5F));
    CHECK_NEAR(depth.at(1, 1), 0.5F, kTolerance);

    CHECK(!depth.testAndSet(1, 1, 0.25F));
    CHECK_NEAR(depth.at(1, 1), 0.5F, kTolerance);

    CHECK(depth.testAndSet(1, 1, 1.0F));
    CHECK_NEAR(depth.at(1, 1), 1.0F, kTolerance);
}

void empate_mantem_o_primeiro() {
    cg::DepthBuffer depth{4, 3};

    CHECK(depth.testAndSet(2, 0, 0.75F));
    CHECK(!depth.testAndSet(2, 0, 0.75F));
    CHECK_NEAR(depth.at(2, 0), 0.75F, kTolerance);

    CHECK(depth.testAndSet(2, 0, 0.751F));
    CHECK_NEAR(depth.at(2, 0), 0.751F, kTolerance);
}

void clear_reseta_o_buffer() {
    cg::DepthBuffer depth{4, 3};

    CHECK(depth.testAndSet(0, 0, 0.5F));
    CHECK(depth.testAndSet(3, 2, 0.9F));

    depth.clear();

    CHECK_NEAR(depth.at(0, 0), 0.0F, kTolerance);
    CHECK_NEAR(depth.at(3, 2), 0.0F, kTolerance);
    CHECK(depth.testAndSet(0, 0, 0.1F));
    CHECK_NEAR(depth.at(0, 0), 0.1F, kTolerance);
}

void fora_dos_limites_e_ignorado() {
    cg::DepthBuffer depth{4, 3};

    CHECK(!depth.testAndSet(-1, 0, 1.0F));
    CHECK(!depth.testAndSet(0, -1, 1.0F));
    CHECK(!depth.testAndSet(4, 0, 1.0F));
    CHECK(!depth.testAndSet(0, 3, 1.0F));
    CHECK(!depth.testAndSet(4, 3, 1.0F));

    CHECK_NEAR(depth.at(0, 0), 0.0F, kTolerance);
    CHECK_NEAR(depth.at(3, 2), 0.0F, kTolerance);
    CHECK_NEAR(depth.at(0, 1), 0.0F, kTolerance);
    CHECK_NEAR(depth.at(1, 0), 0.0F, kTolerance);
    CHECK_NEAR(depth.at(2, 2), 0.0F, kTolerance);

    CHECK(depth.testAndSet(0, 1, 0.5F));
    CHECK(depth.testAndSet(0, 0, 0.7F));
    CHECK_NEAR(depth.at(0, 1), 0.5F, kTolerance);
    CHECK_NEAR(depth.at(0, 0), 0.7F, kTolerance);

    CHECK_NEAR(depth.at(-1, 0), 0.0F, kTolerance);
    CHECK_NEAR(depth.at(0, -1), 0.0F, kTolerance);
    CHECK_NEAR(depth.at(4, 0), 0.0F, kTolerance);
    CHECK_NEAR(depth.at(0, 3), 0.0F, kTolerance);
}

void dimensoes_negativas_viram_zero() {
    cg::DepthBuffer depth{-4, -3};

    CHECK_EQ(depth.width(), 0);
    CHECK_EQ(depth.height(), 0);
    CHECK(!depth.testAndSet(0, 0, 1.0F));
    CHECK_NEAR(depth.at(0, 0), 0.0F, kTolerance);

    depth.clear();
    CHECK_NEAR(depth.at(0, 0), 0.0F, kTolerance);
}

void dimensao_zero_e_valida() {
    cg::DepthBuffer depth{0, 5};

    CHECK_EQ(depth.width(), 0);
    CHECK_EQ(depth.height(), 5);
    CHECK(!depth.testAndSet(0, 0, 1.0F));
    CHECK_NEAR(depth.at(0, 0), 0.0F, kTolerance);
}

void invz_nao_finito_nao_vence() {
    cg::DepthBuffer depth{4, 3};

    CHECK(!depth.testAndSet(1, 1, std::numeric_limits<float>::quiet_NaN()));
    CHECK_NEAR(depth.at(1, 1), 0.0F, kTolerance);

    CHECK(!depth.testAndSet(1, 1, std::numeric_limits<float>::infinity()));
    CHECK_NEAR(depth.at(1, 1), 0.0F, kTolerance);

    CHECK(!depth.testAndSet(1, 1, -std::numeric_limits<float>::infinity()));
    CHECK_NEAR(depth.at(1, 1), 0.0F, kTolerance);

    CHECK(depth.testAndSet(1, 1, 0.5F));
    CHECK_NEAR(depth.at(1, 1), 0.5F, kTolerance);

    CHECK(!depth.testAndSet(1, 1, std::numeric_limits<float>::quiet_NaN()));
    CHECK_NEAR(depth.at(1, 1), 0.5F, kTolerance);
}

void invz_zero_ou_negativo_nao_vence() {
    cg::DepthBuffer depth{4, 3};

    CHECK(!depth.testAndSet(1, 1, 0.0F));
    CHECK_NEAR(depth.at(1, 1), 0.0F, kTolerance);

    CHECK(!depth.testAndSet(1, 1, -0.5F));
    CHECK_NEAR(depth.at(1, 1), 0.0F, kTolerance);

    CHECK(depth.testAndSet(1, 1, 0.5F));
    CHECK(!depth.testAndSet(1, 1, 0.0F));
    CHECK(!depth.testAndSet(1, 1, -1.0F));
    CHECK_NEAR(depth.at(1, 1), 0.5F, kTolerance);
}

void buffer_nao_quadrado_mapeia_indice() {
    cg::DepthBuffer depth{4, 3};

    CHECK(depth.testAndSet(3, 2, 0.5F));
    CHECK_NEAR(depth.at(3, 2), 0.5F, kTolerance);
    CHECK_NEAR(depth.at(3, 0), 0.0F, kTolerance);
    CHECK_NEAR(depth.at(0, 2), 0.0F, kTolerance);
    CHECK_NEAR(depth.at(1, 1), 0.0F, kTolerance);

    CHECK(depth.testAndSet(3, 0, 0.5F));
    CHECK(depth.testAndSet(0, 1, 0.7F));
    CHECK_NEAR(depth.at(3, 0), 0.5F, kTolerance);
    CHECK_NEAR(depth.at(0, 1), 0.7F, kTolerance);
    CHECK_NEAR(depth.at(3, 1), 0.0F, kTolerance);
    CHECK_NEAR(depth.at(0, 2), 0.0F, kTolerance);
}

}

int main() {
    buffer_novo_esta_zerado();
    maior_invz_vence();
    empate_mantem_o_primeiro();
    clear_reseta_o_buffer();
    fora_dos_limites_e_ignorado();
    dimensoes_negativas_viram_zero();
    dimensao_zero_e_valida();
    invz_nao_finito_nao_vence();
    invz_zero_ou_negativo_nao_vence();
    buffer_nao_quadrado_mapeia_indice();

    return test::failureCount() == 0 ? 0 : 1;
}
