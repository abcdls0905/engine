#pragma once

#include "defines.h"
#include "../visual/i_ui3d_mesh.h"

_UI3D_BEGIN

//����һ��n���Բ���壬ÿ����һ��node
extern IUI3DMesh * BuildCylinder(IUI3DMesh* mesh, int num, float w, float h, int fvf = FVF_POS, float maxu = 1.0f, float maxv = 1.0f);


_UI3D_END