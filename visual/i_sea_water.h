#ifndef _I_SEA_WATER_H
#define _I_SEA_WATER_H

#include "../public/module.h"

class ISeaWater : public IEntity
{
public:
	virtual bool UpdateSeaData() = 0;
};

#endif