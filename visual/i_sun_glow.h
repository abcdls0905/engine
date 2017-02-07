#ifndef __I_SUN_GLOW_H
#define __I_SUN_GLOW_H

#include "../visual/i_vis_base.h"

class ISunGlow: public IVisBase
{
public:
	virtual bool Load() = 0;
	virtual bool AddFlare(float distance, float size, int sub_tex, const char* color) = 0;
};

#endif