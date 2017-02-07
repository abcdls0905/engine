
#pragma once

#include "../public/i_entity.h"
#include <vector>
#include <string>
#include <map>

class SceneObjManager : public IEntity
{
private:
  PERSISTID player;
public:
  SceneObjManager();
  ~SceneObjManager();
  virtual bool Init(const IVarList& args);
  virtual bool Shut();
  virtual void Execute(float seconds);
  PERSISTID& GetPlayerId();
  void SetPlayer(const PERSISTID& id);
};