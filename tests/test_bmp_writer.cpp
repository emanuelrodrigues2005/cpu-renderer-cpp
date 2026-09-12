#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

#include "canvas/BmpWriter.h"
#include "canvas/MemoryCanvas.h"
#include "test_util.h"

namespace {

const char* kTestPath = "/tmp/cpu-renderer-bmp-test.bmp";

std::vector<unsigned char> readFile(const std::string& path) {
    std::ifstream input(path, std::ios::binary);
    return std::vector<unsigned char>(std::istreambuf_iterator<char>(input),
                                      std::istreambuf_iterator<char>());
}

std::uint32_t readU32(const std::vector<unsigned char>& bytes, std::size_t offset) {
    return static_cast<std::uint32_t>(bytes[offset]) |
           (static_cast<std::uint32_t>(bytes[offset + 1]) << 8) |
           (static_cast<std::uint32_t>(bytes[offset + 2]) << 16) |
           (static_cast<std::uint32_t>(bytes[offset + 3]) << 24);
}

void escreve_cabecalho_bmp_valido() {
    cg::MemoryCanvas canvas{2, 2};
    std::string error;

    const bool ok = cg::writeBmp(kTestPath, canvas, error);

    CHECK(ok);
    const std::vector<unsigned char> bytes = readFile(kTestPath);
    CHECK_EQ(bytes.size(), std::size_t{70});
    CHECK_EQ(bytes[0], static_cast<unsigned char>('B'));
    CHECK_EQ(bytes[1], static_cast<unsigned char>('M'));
    CHECK_EQ(readU32(bytes, 2), 70u);
    CHECK_EQ(readU32(bytes, 10), 54u);
    CHECK_EQ(readU32(bytes, 14), 40u);
    CHECK_EQ(readU32(bytes, 18), 2u);
    CHECK_EQ(readU32(bytes, 22), 2u);
    CHECK_EQ(readU32(bytes, 34), 16u);
    CHECK_EQ(bytes[28], static_cast<unsigned char>(24));
}

void escreve_pixels_de_baixo_para_cima_em_bgr() {
    cg::MemoryCanvas canvas{2, 2};
    canvas.drawPixel(0, 0, 0xFF0000);
    canvas.drawPixel(1, 0, 0x00FF00);
    canvas.drawPixel(0, 1, 0x0000FF);
    std::string error;

    CHECK(cg::writeBmp(kTestPath, canvas, error));

    const std::vector<unsigned char> bytes = readFile(kTestPath);
    CHECK_EQ(bytes[54], static_cast<unsigned char>(0xFF));
    CHECK_EQ(bytes[55], static_cast<unsigned char>(0x00));
    CHECK_EQ(bytes[56], static_cast<unsigned char>(0x00));
    CHECK_EQ(bytes[57], static_cast<unsigned char>(0x00));
    CHECK_EQ(bytes[58], static_cast<unsigned char>(0x00));
    CHECK_EQ(bytes[59], static_cast<unsigned char>(0x00));
    CHECK_EQ(bytes[61], static_cast<unsigned char>(0x00));
    CHECK_EQ(bytes[63], static_cast<unsigned char>(0x00));
    CHECK_EQ(bytes[64], static_cast<unsigned char>(0xFF));
    CHECK_EQ(bytes[66], static_cast<unsigned char>(0xFF));
    CHECK_EQ(bytes[67], static_cast<unsigned char>(0x00));
    CHECK_EQ(bytes[69], static_cast<unsigned char>(0x00));
}

void completa_a_linha_com_padding() {
    cg::MemoryCanvas canvas{1, 1};
    canvas.drawPixel(0, 0, 0xFFFFFF);
    std::string error;

    CHECK(cg::writeBmp(kTestPath, canvas, error));

    const std::vector<unsigned char> bytes = readFile(kTestPath);
    CHECK_EQ(bytes.size(), std::size_t{58});
    CHECK_EQ(bytes[54], static_cast<unsigned char>(0xFF));
    CHECK_EQ(bytes[55], static_cast<unsigned char>(0xFF));
    CHECK_EQ(bytes[56], static_cast<unsigned char>(0xFF));
    CHECK_EQ(bytes[57], static_cast<unsigned char>(0x00));
}

void caminho_invalido_falha_com_mensagem() {
    cg::MemoryCanvas canvas{2, 2};
    std::string error;

    const bool ok = cg::writeBmp("/diretorio/inexistente/arquivo.bmp", canvas, error);

    CHECK(!ok);
    CHECK(!error.empty());
}

}

int main() {
    escreve_cabecalho_bmp_valido();
    escreve_pixels_de_baixo_para_cima_em_bgr();
    completa_a_linha_com_padding();
    caminho_invalido_falha_com_mensagem();

    return test::failureCount() == 0 ? 0 : 1;
}
