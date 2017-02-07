

#pragma once

#include <vector>
#include <map>
#include <string>

#include "../public/i_entity.h"
#include "../public/Var_List.h"
#include "../visual/i_vis_base.h"
#include "../action/action_queue.h"

class Action : public IEntity
{
public:
  ~Action();
  virtual bool Init(const IVarList& args);
  virtual bool Shut();
public:
  static Action* GetInstance();
};
