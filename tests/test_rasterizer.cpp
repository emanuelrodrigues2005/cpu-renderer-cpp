#include <cstdint>

#include "MemoryCanvas.h"
#include "Rasterizer.h"
#include "test_util.h"

namespace {

int countWhite(const cg::MemoryCanvas& canvas) {
    int count = 0;
    for (int y = 0; y < canvas.height(); ++y) {
        for (int x = 0; x < canvas.width(); ++x) {
            if (canvas.pixelAt(x, y) != cg::kBlack) {
                ++count;
            }
        }
    }
    return count;
}

bool white(const cg::MemoryCanvas& canvas, int x, int y) {
    return canvas.pixelAt(x, y) == cg::kWhite;
}

void triangulo_retangulo_preenche_a_area() {
    cg::MemoryCanvas canvas{16, 16};

    cg::fillTriangle(canvas, cg::ScreenPoint{0, 0}, cg::ScreenPoint{4, 0}, cg::ScreenPoint{0, 4},
                     cg::kWhite);

    CHECK_EQ(countWhite(canvas), 15);
    CHECK(white(canvas, 4, 0));
    CHECK(white(canvas, 2, 2));
    CHECK(white(canvas, 0, 4));
    CHECK(!white(canvas, 4, 1));
}

void triangulo_com_vertice_no_topo() {
    cg::MemoryCanvas canvas{16, 16};

    cg::fillTriangle(canvas, cg::ScreenPoint{2, 0}, cg::ScreenPoint{0, 2}, cg::ScreenPoint{4, 2},
                     cg::kWhite);

    CHECK_EQ(countWhite(canvas), 9);
    CHECK(white(canvas, 2, 0));
    CHECK(white(canvas, 1, 1));
    CHECK(white(canvas, 3, 1));
    CHECK(white(canvas, 0, 2));
    CHECK(white(canvas, 4, 2));
}

void aresta_horizontal_com_arredondamento() {
    cg::MemoryCanvas canvas{16, 16};

    cg::fillTriangle(canvas, cg::ScreenPoint{0, 0}, cg::ScreenPoint{4, 0}, cg::ScreenPoint{2, 3},
                     cg::kWhite);

    CHECK_EQ(countWhite(canvas), 12);
    CHECK(white(canvas, 0, 0));
    CHECK(white(canvas, 4, 0));
    CHECK(white(canvas, 1, 1));
    CHECK(!white(canvas, 0, 1));
    CHECK(white(canvas, 1, 2));
    CHECK(!white(canvas, 0, 2));
    CHECK(white(canvas, 2, 3));
}

void triangulos_degenerados_nao_pintam() {
    cg::MemoryCanvas canvas{16, 16};

    cg::fillTriangle(canvas, cg::ScreenPoint{0, 0}, cg::ScreenPoint{2, 2}, cg::ScreenPoint{4, 4},
                     cg::kWhite);
    CHECK_EQ(countWhite(canvas), 0);

    cg::fillTriangle(canvas, cg::ScreenPoint{0, 0}, cg::ScreenPoint{4, 0}, cg::ScreenPoint{2, 0},
                     cg::kWhite);
    CHECK_EQ(countWhite(canvas), 0);

    cg::fillTriangle(canvas, cg::ScreenPoint{2, 2}, cg::ScreenPoint{2, 2}, cg::ScreenPoint{2, 2},
                     cg::kWhite);
    CHECK_EQ(countWhite(canvas), 0);
}

void fora_da_janela_e_recortado() {
    cg::MemoryCanvas canvas{16, 16};

    cg::fillTriangle(canvas, cg::ScreenPoint{-10, -10}, cg::ScreenPoint{-5, -10},
                     cg::ScreenPoint{-10, -5}, cg::kWhite);

    CHECK_EQ(countWhite(canvas), 0);

    cg::fillTriangle(canvas, cg::ScreenPoint{-2, 0}, cg::ScreenPoint{2, 0},
                     cg::ScreenPoint{-2, 4}, cg::kWhite);

    CHECK_EQ(countWhite(canvas), 6);
    CHECK(white(canvas, 0, 0));
    CHECK(white(canvas, 0, 1));
    CHECK(white(canvas, 1, 1));
    CHECK(white(canvas, 0, 2));
    CHECK(!white(canvas, 0, 3));
}

void ordem_dos_vertices_nao_importa() {
    cg::MemoryCanvas primeiro{16, 16};
    cg::fillTriangle(primeiro, cg::ScreenPoint{0, 0}, cg::ScreenPoint{4, 0}, cg::ScreenPoint{0, 4},
                     cg::kWhite);

    cg::MemoryCanvas segundo{16, 16};
    cg::fillTriangle(segundo, cg::ScreenPoint{0, 4}, cg::ScreenPoint{4, 0}, cg::ScreenPoint{0, 0},
                     cg::kWhite);

    CHECK_EQ(countWhite(primeiro), countWhite(segundo));

    for (int y = 0; y < 16; ++y) {
        for (int x = 0; x < 16; ++x) {
            CHECK_EQ(primeiro.pixelAt(x, y), segundo.pixelAt(x, y));
        }
    }
}

}

int main() {
    triangulo_retangulo_preenche_a_area();
    triangulo_com_vertice_no_topo();
    aresta_horizontal_com_arredondamento();
    triangulos_degenerados_nao_pintam();
    fora_da_janela_e_recortado();
    ordem_dos_vertices_nao_importa();

    return test::failureCount() == 0 ? 0 : 1;
}
