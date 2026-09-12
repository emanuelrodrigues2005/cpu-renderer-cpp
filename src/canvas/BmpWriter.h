#pragma once

#include <string>

#include "canvas/MemoryCanvas.h"

namespace cg {

bool writeBmp(const std::string& path, const MemoryCanvas& canvas, std::string& error);

}
