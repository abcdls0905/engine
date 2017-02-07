#pragma once

#include "defines.h"
#include "../visual/i_ui3d_mesh.h"

_UI3D_BEGIN

//构建一个n面的圆柱体，每面是一个node
extern IUI3DMesh * BuildCylinder(IUI3DMesh* mesh, int num, float w, float h, int fvf = FVF_POS, float maxu = 1.0f, float maxv = 1.0f);


_UI3D_END