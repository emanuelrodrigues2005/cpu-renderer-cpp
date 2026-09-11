#include "Camera.h"

#include <cctype>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace cg {

namespace {

std::string normalizeLine(const std::string& line) {
    std::string normalized;
    normalized.reserve(line.size());
    for (const char ch : line) {
        if (ch == '#') {
            break;
        }
        normalized.push_back(ch == '=' || ch == ':' ? ' ' : ch);
    }
    return normalized;
}

std::string lowercased(std::string text) {
    for (char& ch : text) {
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    return text;
}

std::vector<std::string> split(const std::string& line) {
    std::istringstream stream(line);
    std::vector<std::string> tokens;
    std::string token;
    while (stream >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

bool toFloat(const std::string& token, float& value) {
    std::istringstream stream(token);
    stream >> value;
    return !stream.fail() && stream.eof();
}

bool readVec3(const std::vector<std::string>& tokens, std::size_t start, Vec3& vector) {
    return start + 2 < tokens.size() && toFloat(tokens[start], vector.x) &&
           toFloat(tokens[start + 1], vector.y) && toFloat(tokens[start + 2], vector.z);
}

}

Vec3 Camera::worldToView(const Vec3& point) const {
    const Vec3 relative{point.x - c.x, point.y - c.y, point.z - c.z};
    return Vec3{relative.dot(u), relative.dot(v), relative.dot(n)};
}

bool loadCameraFromFile(const std::string& path, Camera& camera, std::string& error) {
    std::ifstream input(path);
    if (!input) {
        error = "não foi possível abrir '" + path + "'";
        return false;
    }

    Camera loaded;
    bool hasN = false;
    bool hasV = false;
    bool hasC = false;
    bool hasD = false;
    bool hasHx = false;
    bool hasHy = false;

    std::string line;
    while (std::getline(input, line)) {
        const std::vector<std::string> tokens = split(normalizeLine(line));
        if (tokens.empty()) {
            continue;
        }

        const std::string key = lowercased(tokens[0]);
        std::size_t index = 1;

        if (key == "n") {
            if (!readVec3(tokens, index, loaded.n)) {
                error = "valor inválido para N em '" + path + "'";
                return false;
            }
            hasN = true;
        } else if (key == "v") {
            if (!readVec3(tokens, index, loaded.v)) {
                error = "valor inválido para V em '" + path + "'";
                return false;
            }
            hasV = true;
        } else if (key == "c") {
            if (!readVec3(tokens, index, loaded.c)) {
                error = "valor inválido para C em '" + path + "'";
                return false;
            }
            hasC = true;
        } else if (key == "d") {
            if (index >= tokens.size() || !toFloat(tokens[index], loaded.d)) {
                error = "valor inválido para d em '" + path + "'";
                return false;
            }
            hasD = true;
        } else if (key == "hx") {
            if (index >= tokens.size() || !toFloat(tokens[index], loaded.hx)) {
                error = "valor inválido para hx em '" + path + "'";
                return false;
            }
            hasHx = true;
        } else if (key == "hy") {
            if (index >= tokens.size() || !toFloat(tokens[index], loaded.hy)) {
                error = "valor inválido para hy em '" + path + "'";
                return false;
            }
            hasHy = true;
        } else {
            error = "campo desconhecido '" + tokens[0] + "' em '" + path + "'";
            return false;
        }
    }

    std::string missing;
    const auto noteMissing = [&missing](bool present, const char* name) {
        if (!present) {
            if (!missing.empty()) {
                missing += ", ";
            }
            missing += name;
        }
    };
    noteMissing(hasN, "N");
    noteMissing(hasV, "V");
    noteMissing(hasC, "C");
    noteMissing(hasD, "d");
    noteMissing(hasHx, "hx");
    noteMissing(hasHy, "hy");
    if (!missing.empty()) {
        error = "campo ausente: " + missing + " em '" + path + "'";
        return false;
    }

    if (loaded.n.length() == 0.0F || loaded.v.length() == 0.0F) {
        error = "vetor N ou V nulo em '" + path + "'";
        return false;
    }
    if (loaded.d <= 0.0F || loaded.hx <= 0.0F || loaded.hy <= 0.0F) {
        error = "d, hx e hy devem ser positivos em '" + path + "'";
        return false;
    }

    loaded.n = loaded.n.normalized();

    const float projection = loaded.v.dot(loaded.n);
    const Vec3 orthogonal{loaded.v.x - loaded.n.x * projection,
                          loaded.v.y - loaded.n.y * projection,
                          loaded.v.z - loaded.n.z * projection};
    if (orthogonal.length() == 0.0F) {
        error = "V é paralelo a N em '" + path + "'";
        return false;
    }
    loaded.v = orthogonal.normalized();
    loaded.u = loaded.v.cross(loaded.n).normalized();

    camera = loaded;
    return true;
}

}
