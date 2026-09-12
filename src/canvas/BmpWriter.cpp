#include "canvas/BmpWriter.h"

#include <cstdint>
#include <fstream>

namespace cg {

namespace {

void writeU16(std::ofstream& output, std::uint16_t value) {
    const char bytes[2] = {static_cast<char>(value & 0xFF),
                           static_cast<char>((value >> 8) & 0xFF)};
    output.write(bytes, 2);
}

void writeU32(std::ofstream& output, std::uint32_t value) {
    const char bytes[4] = {static_cast<char>(value & 0xFF),
                           static_cast<char>((value >> 8) & 0xFF),
                           static_cast<char>((value >> 16) & 0xFF),
                           static_cast<char>((value >> 24) & 0xFF)};
    output.write(bytes, 4);
}

}

bool writeBmp(const std::string& path, const MemoryCanvas& canvas, std::string& error) {
    const int width = canvas.width();
    const int height = canvas.height();
    const std::uint32_t rowStride = static_cast<std::uint32_t>((width * 3 + 3) / 4 * 4);
    const std::uint32_t imageSize = rowStride * static_cast<std::uint32_t>(height);
    const std::uint32_t dataOffset = 14 + 40;

    std::ofstream output(path, std::ios::binary);
    if (!output) {
        error = "não foi possível criar '" + path + "'";
        return false;
    }

    output.put('B');
    output.put('M');
    writeU32(output, dataOffset + imageSize);
    writeU16(output, 0);
    writeU16(output, 0);
    writeU32(output, dataOffset);

    writeU32(output, 40);
    writeU32(output, static_cast<std::uint32_t>(width));
    writeU32(output, static_cast<std::uint32_t>(height));
    writeU16(output, 1);
    writeU16(output, 24);
    writeU32(output, 0);
    writeU32(output, imageSize);
    writeU32(output, 2835);
    writeU32(output, 2835);
    writeU32(output, 0);
    writeU32(output, 0);

    const char padding[3] = {0, 0, 0};
    for (int y = height - 1; y >= 0; --y) {
        for (int x = 0; x < width; ++x) {
            const std::uint32_t color = canvas.pixelAt(x, y);
            const char bgr[3] = {static_cast<char>(color & 0xFF),
                                 static_cast<char>((color >> 8) & 0xFF),
                                 static_cast<char>((color >> 16) & 0xFF)};
            output.write(bgr, 3);
        }
        const std::uint32_t rowBytes = static_cast<std::uint32_t>(width) * 3;
        if (rowBytes < rowStride) {
            output.write(padding, static_cast<std::streamsize>(rowStride - rowBytes));
        }
    }

    if (!output) {
        error = "falha ao escrever '" + path + "'";
        return false;
    }
    return true;
}

}
