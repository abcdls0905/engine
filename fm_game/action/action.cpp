
#include "action.h"
#include "../helper.h"
#include "../interface/global_prop_define.h"

Action::~Action()
{

}

bool Action::Init(const IVarList& args)
{
  return true;
}

bool Action::Shut()
{
  return true;
}

Action* Action::GetInstance()
{
  Action* m_pInstance = (Action*)CHelper::GetGlobalEntity(GLOBAL_PROP_ENT_ACTION);
  if (m_pInstance == NULL)
  {
    m_pInstance = (Action*)CHelper::CreateEntity("Action", GLOBAL_PROP_ENT_ACTION);
  }
  return m_pInstance;
}
