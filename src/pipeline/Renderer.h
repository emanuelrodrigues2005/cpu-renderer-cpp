#pragma once

#include "pipeline/Camera.h"
#include "canvas/Canvas.h"
#include "mesh/Mesh.h"
#include "shading/Lighting.h"

namespace cg {

enum class RenderMode {
    Filled,
    Points,
    Wireframe,
};

void renderMesh(const Mesh& mesh, const Camera& camera, Canvas& canvas,
                RenderMode mode = RenderMode::Filled);

void renderMesh(const Mesh& mesh, const Camera& camera, const Lighting& lighting,
                Canvas& canvas, RenderMode mode = RenderMode::Filled);

}
