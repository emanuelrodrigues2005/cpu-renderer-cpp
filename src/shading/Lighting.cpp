#include "shading/Lighting.h"

#include <cctype>
#include <cmath>
#include <cstdlib>
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
    char* end = nullptr;
    const float parsed = std::strtof(token.c_str(), &end);
    if (end == token.c_str() || *end != '\0') {
        return false;
    }
    value = parsed;
    return true;
}

bool readVec3(const std::vector<std::string>& tokens, std::size_t start, Vec3& vector) {
    return tokens.size() == start + 3 && toFloat(tokens[start], vector.x) &&
           toFloat(tokens[start + 1], vector.y) && toFloat(tokens[start + 2], vector.z);
}

bool readColor(const std::vector<std::string>& tokens, std::size_t start, Color& color) {
    Vec3 vector;
    if (!readVec3(tokens, start, vector)) {
        return false;
    }
    color = Color{vector.x, vector.y, vector.z};
    return true;
}

bool isFinite(const Vec3& vector) {
    return std::isfinite(vector.x) && std::isfinite(vector.y) && std::isfinite(vector.z);
}

bool isFinite(const Color& color) {
    return std::isfinite(color.r) && std::isfinite(color.g) && std::isfinite(color.b);
}

bool inRange(const Color& color, float minimum, float maximum) {
    return color.r >= minimum && color.r <= maximum && color.g >= minimum &&
           color.g <= maximum && color.b >= minimum && color.b <= maximum;
}

bool inRange(const Vec3& vector, float minimum, float maximum) {
    return vector.x >= minimum && vector.x <= maximum && vector.y >= minimum &&
           vector.y <= maximum && vector.z >= minimum && vector.z <= maximum;
}

}

bool loadLightingFromFile(const std::string& path, Lighting& lighting, std::string& error) {
    std::ifstream input(path);
    if (!input) {
        error = "não foi possível abrir '" + path + "'";
        return false;
    }

    Lighting loaded;
    bool hasIamb = false;
    bool hasKa = false;
    bool hasIl = false;
    bool hasPl = false;
    bool hasKd = false;
    bool hasOd = false;
    bool hasKs = false;
    bool hasEta = false;

    std::string line;
    while (std::getline(input, line)) {
        const std::vector<std::string> tokens = split(normalizeLine(line));
        if (tokens.empty()) {
            continue;
        }

        const std::string key = lowercased(tokens[0]);
        std::size_t index = 1;

        if (key == "iamb") {
            if (hasIamb) {
                error = "campo duplicado 'Iamb' em '" + path + "'";
                return false;
            }
            if (!readColor(tokens, index, loaded.light.ambient)) {
                error = "valor inválido para Iamb em '" + path + "'";
                return false;
            }
            hasIamb = true;
        } else if (key == "ka") {
            if (hasKa) {
                error = "campo duplicado 'Ka' em '" + path + "'";
                return false;
            }
            if (tokens.size() != index + 1 || !toFloat(tokens[index], loaded.material.ka)) {
                error = "valor inválido para Ka em '" + path + "'";
                return false;
            }
            hasKa = true;
        } else if (key == "il") {
            if (hasIl) {
                error = "campo duplicado 'Il' em '" + path + "'";
                return false;
            }
            if (!readColor(tokens, index, loaded.light.color)) {
                error = "valor inválido para Il em '" + path + "'";
                return false;
            }
            hasIl = true;
        } else if (key == "pl") {
            if (hasPl) {
                error = "campo duplicado 'Pl' em '" + path + "'";
                return false;
            }
            if (!readVec3(tokens, index, loaded.light.position)) {
                error = "valor inválido para Pl em '" + path + "'";
                return false;
            }
            hasPl = true;
        } else if (key == "kd") {
            if (hasKd) {
                error = "campo duplicado 'Kd' em '" + path + "'";
                return false;
            }
            if (!readVec3(tokens, index, loaded.material.kd)) {
                error = "valor inválido para Kd em '" + path + "'";
                return false;
            }
            hasKd = true;
        } else if (key == "od") {
            if (hasOd) {
                error = "campo duplicado 'Od' em '" + path + "'";
                return false;
            }
            if (!readVec3(tokens, index, loaded.material.od)) {
                error = "valor inválido para Od em '" + path + "'";
                return false;
            }
            hasOd = true;
        } else if (key == "ks") {
            if (hasKs) {
                error = "campo duplicado 'Ks' em '" + path + "'";
                return false;
            }
            if (tokens.size() != index + 1 || !toFloat(tokens[index], loaded.material.ks)) {
                error = "valor inválido para Ks em '" + path + "'";
                return false;
            }
            hasKs = true;
        } else if (key == "eta") {
            if (hasEta) {
                error = "campo duplicado 'eta' em '" + path + "'";
                return false;
            }
            if (tokens.size() != index + 1 || !toFloat(tokens[index], loaded.material.eta)) {
                error = "valor inválido para eta em '" + path + "'";
                return false;
            }
            hasEta = true;
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
    noteMissing(hasIamb, "Iamb");
    noteMissing(hasKa, "Ka");
    noteMissing(hasIl, "Il");
    noteMissing(hasPl, "Pl");
    noteMissing(hasKd, "Kd");
    noteMissing(hasOd, "Od");
    noteMissing(hasKs, "Ks");
    noteMissing(hasEta, "eta");
    if (!missing.empty()) {
        error = "campo ausente: " + missing + " em '" + path + "'";
        return false;
    }

    if (!isFinite(loaded.light.ambient)) {
        error = "Iamb deve ser finito em '" + path + "'";
        return false;
    }

    if (!isFinite(loaded.light.color)) {
        error = "Il deve ser finito em '" + path + "'";
        return false;
    }

    if (!isFinite(loaded.light.position)) {
        error = "Pl deve ser finito em '" + path + "'";
        return false;
    }

    if (!isFinite(loaded.material.kd)) {
        error = "Kd deve ser finito em '" + path + "'";
        return false;
    }

    if (!isFinite(loaded.material.od)) {
        error = "Od deve ser finito em '" + path + "'";
        return false;
    }

    if (!std::isfinite(loaded.material.ka)) {
        error = "Ka deve ser finito em '" + path + "'";
        return false;
    }

    if (!std::isfinite(loaded.material.ks)) {
        error = "Ks deve ser finito em '" + path + "'";
        return false;
    }

    if (!std::isfinite(loaded.material.eta)) {
        error = "eta deve ser finito em '" + path + "'";
        return false;
    }

    if (!inRange(loaded.light.ambient, 0.0F, 255.0F)) {
        error = "Iamb deve estar em [0,255] em '" + path + "'";
        return false;
    }

    if (!inRange(loaded.light.color, 0.0F, 255.0F)) {
        error = "Il deve estar em [0,255] em '" + path + "'";
        return false;
    }

    if (!inRange(loaded.material.kd, 0.0F, 1.0F)) {
        error = "Kd deve estar em [0,1] em '" + path + "'";
        return false;
    }

    if (!inRange(loaded.material.od, 0.0F, 1.0F)) {
        error = "Od deve estar em [0,1] em '" + path + "'";
        return false;
    }

    if (loaded.material.ka < 0.0F || loaded.material.ka > 1.0F) {
        error = "Ka deve estar em [0,1] em '" + path + "'";
        return false;
    }

    if (loaded.material.ks < 0.0F || loaded.material.ks > 1.0F) {
        error = "Ks deve estar em [0,1] em '" + path + "'";
        return false;
    }

    if (loaded.material.eta < 1.0F) {
        error = "eta deve ser maior ou igual a 1 em '" + path + "'";
        return false;
    }

    lighting = loaded;
    return true;
}

}
