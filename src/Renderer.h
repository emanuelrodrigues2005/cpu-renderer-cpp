#pragma once

#include "Camera.h"
#include "Canvas.h"
#include "Mesh.h"

namespace cg {

enum class RenderMode {
    Filled,
    Points,
    Wireframe,
};

void renderMesh(const Mesh& mesh, const Camera& camera, Canvas& canvas,
                RenderMode mode = RenderMode::Filled);

}
