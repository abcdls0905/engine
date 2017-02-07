#ifndef _MOVIE_H
#define _MOVIE_H

#include "../public/module.h"
#include "../visual/dx_header.h"
#include <string>

class FxMovie: public IEntity
{
public:
	virtual bool Init(const IVarList& args) { return true; }
	virtual bool Shut() { return true; }
};

#endif // _MOVIE_H
