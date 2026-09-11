#pragma once

#include <cstdint>

#include "canvas/Canvas.h"
#include "pipeline/Projection.h"

namespace cg {

void fillTriangle(Canvas& canvas, const ScreenPoint& a, const ScreenPoint& b, const ScreenPoint& c,
                  std::uint32_t color);

void drawLine(Canvas& canvas, const ScreenPoint& a, const ScreenPoint& b, std::uint32_t color);

}
