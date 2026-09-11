#pragma once

#include <cstdint>

#include "Canvas.h"
#include "Projection.h"

namespace cg {

void fillTriangle(Canvas& canvas, const ScreenPoint& a, const ScreenPoint& b, const ScreenPoint& c,
                  std::uint32_t color);

void drawLine(Canvas& canvas, const ScreenPoint& a, const ScreenPoint& b, std::uint32_t color);

}
