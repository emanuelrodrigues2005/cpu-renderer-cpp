#include <string>

#include "shading/Lighting.h"
#include "test_util.h"

namespace {

bool contem(const std::string& texto, const std::string& trecho) {
    return texto.find(trecho) != std::string::npos;
}

bool mesmoConteudo(const cg::Lighting& a, const cg::Lighting& b) {
    return a.light.ambient.r == b.light.ambient.r && a.light.ambient.g == b.light.ambient.g &&
           a.light.ambient.b == b.light.ambient.b && a.light.color.r == b.light.color.r &&
           a.light.color.g == b.light.color.g && a.light.color.b == b.light.color.b &&
           a.light.position.x == b.light.position.x &&
           a.light.position.y == b.light.position.y &&
           a.light.position.z == b.light.position.z && a.material.ka == b.material.ka &&
           a.material.kd.x == b.material.kd.x && a.material.kd.y == b.material.kd.y &&
           a.material.kd.z == b.material.kd.z && a.material.od.x == b.material.od.x &&
           a.material.od.y == b.material.od.y && a.material.od.z == b.material.od.z &&
           a.material.ks == b.material.ks && a.material.eta == b.material.eta;
}

void carrega_exemplo_do_pdf() {
    cg::Lighting lighting;
    std::string error;
    CHECK(cg::loadLightingFromFile("lighting/lighting-pdf.txt", lighting, error));

    CHECK_EQ(lighting.light.ambient.r, 100.0F);
    CHECK_EQ(lighting.light.ambient.g, 100.0F);
    CHECK_EQ(lighting.light.ambient.b, 100.0F);
    CHECK_EQ(lighting.light.color.r, 127.0F);
    CHECK_EQ(lighting.light.color.g, 213.0F);
    CHECK_EQ(lighting.light.color.b, 254.0F);
    CHECK_EQ(lighting.light.position.x, 60.0F);
    CHECK_EQ(lighting.light.position.y, 5.0F);
    CHECK_EQ(lighting.light.position.z, -10.0F);
    CHECK_EQ(lighting.material.ka, 0.2F);
    CHECK_EQ(lighting.material.kd.x, 0.5F);
    CHECK_EQ(lighting.material.kd.y, 0.3F);
    CHECK_EQ(lighting.material.kd.z, 0.2F);
    CHECK_EQ(lighting.material.od.x, 0.7F);
    CHECK_EQ(lighting.material.od.y, 0.5F);
    CHECK_EQ(lighting.material.od.z, 0.8F);
    CHECK_EQ(lighting.material.ks, 0.5F);
    CHECK_EQ(lighting.material.eta, 1.0F);
    CHECK(error.empty());
}

void carrega_arquivo_padrao() {
    cg::Lighting lighting;
    std::string error;

    const bool ok = cg::loadLightingFromFile("lighting/lighting.txt", lighting, error);

    CHECK(ok);
    CHECK(error.empty());
    CHECK_EQ(lighting.light.ambient.r, 100.0F);
    CHECK_EQ(lighting.light.ambient.g, 100.0F);
    CHECK_EQ(lighting.light.ambient.b, 100.0F);
    CHECK_EQ(lighting.material.ka, 0.2F);
    CHECK_EQ(lighting.light.color.r, 255.0F);
    CHECK_EQ(lighting.light.color.g, 245.0F);
    CHECK_EQ(lighting.light.color.b, 220.0F);
    CHECK_EQ(lighting.light.position.x, 200.0F);
    CHECK_EQ(lighting.light.position.y, -200.0F);
    CHECK_EQ(lighting.light.position.z, 400.0F);
    CHECK_EQ(lighting.material.kd.x, 0.5F);
    CHECK_EQ(lighting.material.kd.y, 0.3F);
    CHECK_EQ(lighting.material.kd.z, 0.2F);
    CHECK_EQ(lighting.material.od.x, 0.7F);
    CHECK_EQ(lighting.material.od.y, 0.5F);
    CHECK_EQ(lighting.material.od.z, 0.8F);
    CHECK_EQ(lighting.material.ks, 0.5F);
    CHECK_EQ(lighting.material.eta, 1.0F);
}

void parser_aceita_variacoes_de_formato() {
    cg::Lighting lighting;
    std::string error;
    CHECK(cg::loadLightingFromFile("tests/fixtures/lighting-tolerante.txt", lighting, error));
    CHECK_EQ(lighting.light.color.r, 127.0F);
    CHECK_EQ(lighting.light.position.z, -10.0F);
    CHECK_EQ(lighting.material.eta, 1.0F);
}

void campo_ausente_falha() {
    cg::Lighting lighting;
    std::string error;
    CHECK(!cg::loadLightingFromFile("tests/fixtures/lighting-campo-ausente.txt", lighting, error));
    CHECK(!error.empty());
}

void campo_desconhecido_falha() {
    cg::Lighting lighting;
    std::string error;
    CHECK(!cg::loadLightingFromFile("tests/fixtures/lighting-campo-desconhecido.txt", lighting,
                                    error));
    CHECK(!error.empty());
}

void valores_fora_do_intervalo_falham() {
    cg::Lighting lighting;
    std::string error;
    CHECK(!cg::loadLightingFromFile("tests/fixtures/lighting-range-invalido.txt", lighting, error));
    CHECK(!error.empty());

    CHECK(!cg::loadLightingFromFile("tests/fixtures/lighting-eta-invalido.txt", lighting, error));
    CHECK(!error.empty());
    CHECK(contem(error, "eta"));
    CHECK(contem(error, "lighting-eta-invalido.txt"));

    CHECK(!cg::loadLightingFromFile("tests/fixtures/lighting-nao-finito.txt", lighting, error));
    CHECK(!error.empty());

    CHECK(!cg::loadLightingFromFile("tests/fixtures/lighting-token-extra.txt", lighting, error));
    CHECK(!error.empty());

    CHECK(!cg::loadLightingFromFile("tests/fixtures/lighting-chave-duplicada.txt", lighting,
                                    error));
    CHECK(!error.empty());
}

void erros_de_intervalo_citam_campo_individual_e_caminho() {
    cg::Lighting lighting;
    std::string error;

    CHECK(!cg::loadLightingFromFile("tests/fixtures/lighting-iamb-invalido.txt", lighting, error));
    CHECK(contem(error, "Iamb"));
    CHECK(contem(error, "lighting-iamb-invalido.txt"));
    // A mensagem não deve citar o campo parceiro que hoje compartilha a mesma validação.
    CHECK(!contem(error, "Il"));
    CHECK_EQ(lighting.light.ambient.r, 0.0F);

    error.clear();
    CHECK(!cg::loadLightingFromFile("tests/fixtures/lighting-il-invalido.txt", lighting, error));
    CHECK(contem(error, "Il"));
    CHECK(contem(error, "lighting-il-invalido.txt"));
    CHECK(!contem(error, "Iamb"));
    CHECK_EQ(lighting.light.color.r, 0.0F);

    error.clear();
    CHECK(!cg::loadLightingFromFile("tests/fixtures/lighting-ka-invalido.txt", lighting, error));
    CHECK(contem(error, "Ka"));
    CHECK(contem(error, "lighting-ka-invalido.txt"));
    CHECK(!contem(error, "Ks"));
    CHECK_EQ(lighting.material.ka, 0.0F);

    error.clear();
    CHECK(!cg::loadLightingFromFile("tests/fixtures/lighting-ks-invalido.txt", lighting, error));
    CHECK(contem(error, "Ks"));
    CHECK(contem(error, "lighting-ks-invalido.txt"));
    CHECK(!contem(error, "Ka"));
    CHECK_EQ(lighting.material.ks, 0.0F);

    error.clear();
    CHECK(!cg::loadLightingFromFile("tests/fixtures/lighting-kd-invalido.txt", lighting, error));
    CHECK(contem(error, "Kd"));
    CHECK(contem(error, "lighting-kd-invalido.txt"));
    CHECK(!contem(error, "Od"));
    CHECK_EQ(lighting.material.kd.x, 0.0F);

    error.clear();
    CHECK(!cg::loadLightingFromFile("tests/fixtures/lighting-od-invalido.txt", lighting, error));
    CHECK(contem(error, "Od"));
    CHECK(contem(error, "lighting-od-invalido.txt"));
    CHECK(!contem(error, "Kd"));
    CHECK_EQ(lighting.material.od.x, 0.0F);
}

void erro_de_valor_nao_finito_cita_campo_e_caminho() {
    cg::Lighting lighting;
    std::string error;

    const bool ok =
        cg::loadLightingFromFile("tests/fixtures/lighting-nao-finito.txt", lighting, error);

    CHECK(!ok);
    CHECK(contem(error, "Pl deve ser finito"));
    CHECK(!contem(error, "valor inválido"));
    CHECK(contem(error, "lighting-nao-finito.txt"));
}

void erro_de_escalar_nao_finito_cita_campo_e_caminho() {
    cg::Lighting lighting;
    std::string error;

    const bool ok =
        cg::loadLightingFromFile("tests/fixtures/lighting-eta-nao-finito.txt", lighting, error);

    CHECK(!ok);
    CHECK(contem(error, "eta deve ser finito"));
    CHECK(!contem(error, "valor inválido"));
    CHECK(contem(error, "lighting-eta-nao-finito.txt"));
}

void erro_de_cor_nao_finita_cita_campo_e_caminho() {
    cg::Lighting lighting;
    std::string error;

    const bool ok =
        cg::loadLightingFromFile("tests/fixtures/lighting-il-infinito.txt", lighting, error);

    CHECK(!ok);
    CHECK(contem(error, "Il"));
    CHECK(contem(error, "finito"));
    CHECK(contem(error, "Il deve ser finito"));
    CHECK(!contem(error, "valor inválido"));
    CHECK(contem(error, "lighting-il-infinito.txt"));
}

void erro_de_formato_invalido_cita_campo_e_caminho() {
    cg::Lighting lighting;
    std::string error;

    const bool ok = cg::loadLightingFromFile("tests/fixtures/lighting-ka-formato-invalido.txt",
                                             lighting, error);

    CHECK(!ok);
    CHECK(contem(error, "Ka"));
    CHECK(contem(error, "valor inválido"));
    CHECK(!contem(error, "finito"));
    CHECK(contem(error, "lighting-ka-formato-invalido.txt"));

    error.clear();
    const bool trailing =
        cg::loadLightingFromFile("tests/fixtures/lighting-ka-trailing.txt", lighting, error);

    CHECK(!trailing);
    CHECK(contem(error, "Ka"));
    CHECK(contem(error, "valor inválido"));
    CHECK(!contem(error, "finito"));
    CHECK(contem(error, "lighting-ka-trailing.txt"));
}

void erros_estruturais_citam_campo_e_caminho() {
    cg::Lighting lighting;
    std::string error;

    CHECK(!cg::loadLightingFromFile("tests/fixtures/lighting-campo-ausente.txt", lighting, error));
    CHECK(contem(error, "eta"));
    CHECK(contem(error, "lighting-campo-ausente.txt"));

    error.clear();
    CHECK(!cg::loadLightingFromFile("tests/fixtures/lighting-campo-desconhecido.txt", lighting,
                                    error));
    CHECK(contem(error, "desconhecido"));
    CHECK(contem(error, "lighting-campo-desconhecido.txt"));

    error.clear();
    CHECK(
        !cg::loadLightingFromFile("tests/fixtures/lighting-chave-duplicada.txt", lighting, error));
    CHECK(contem(error, "duplicado"));
    CHECK(contem(error, "Ka"));
    CHECK(contem(error, "lighting-chave-duplicada.txt"));

    error.clear();
    CHECK(!cg::loadLightingFromFile("tests/fixtures/lighting-token-extra.txt", lighting, error));
    CHECK(contem(error, "Ka"));
    CHECK(contem(error, "lighting-token-extra.txt"));
}

void arquivo_inexistente_falha() {
    cg::Lighting lighting;
    std::string error;
    CHECK(!cg::loadLightingFromFile("tests/fixtures/lighting-inexistente.txt", lighting, error));
    CHECK(!error.empty());
    CHECK(contem(error, "lighting-inexistente.txt"));
}

void carga_invalida_preserva_o_estado_anterior() {
    cg::Lighting lighting;
    std::string error;
    CHECK(cg::loadLightingFromFile("lighting/lighting-pdf.txt", lighting, error));

    const cg::Lighting original = lighting;

    CHECK(!cg::loadLightingFromFile("tests/fixtures/lighting-range-invalido.txt", lighting, error));
    CHECK_EQ(lighting.material.ka, original.material.ka);
    CHECK_EQ(lighting.light.color.r, original.light.color.r);
    CHECK(mesmoConteudo(lighting, original));

    CHECK(!cg::loadLightingFromFile("tests/fixtures/lighting-nao-finito.txt", lighting, error));
    CHECK(mesmoConteudo(lighting, original));

    CHECK(!cg::loadLightingFromFile("tests/fixtures/lighting-ka-formato-invalido.txt", lighting,
                                    error));
    CHECK(mesmoConteudo(lighting, original));
}

}

int main() {
    carrega_exemplo_do_pdf();
    carrega_arquivo_padrao();
    parser_aceita_variacoes_de_formato();
    campo_ausente_falha();
    campo_desconhecido_falha();
    valores_fora_do_intervalo_falham();
    erros_de_intervalo_citam_campo_individual_e_caminho();
    erro_de_valor_nao_finito_cita_campo_e_caminho();
    erro_de_escalar_nao_finito_cita_campo_e_caminho();
    erro_de_cor_nao_finita_cita_campo_e_caminho();
    erro_de_formato_invalido_cita_campo_e_caminho();
    erros_estruturais_citam_campo_e_caminho();
    arquivo_inexistente_falha();
    carga_invalida_preserva_o_estado_anterior();

    return test::failureCount() == 0 ? 0 : 1;
}
