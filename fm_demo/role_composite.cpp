#include "role_composite.h"
#include "helper.h"
#include "../visual/i_actor.h"
#include "../visual/i_context.h"
#include "../visual/i_scene.h"
#include "../visual/i_terrain.h"


DECLARE_ENTITY(RoleComposite, 0, IEntity);
 

RoleComposite::RoleComposite(void)
{
}

RoleComposite::~RoleComposite(void)
{
}

// 创建一个角色
IActor* RoleComposite::CreateSceneObj(IScene* pScene,IWorld* pWorld,ITerrain* pTerrain,const char* pNPC,FmVec3 pos)
{
	IRenderContext* pContext = pScene->GetContext();
	IActor* pActor = (IActor*)GetCore()->CreateEntity("Actor");
	pActor->SetContext(pContext);

	char buf[256];
	sprintf(buf, "%s.ini", pNPC);
	bool b = pActor->CreateFromIni(buf);

	pActor->SetPosition(pos.x,pos.y,pos.z);
	pActor->SetAngle(0, 0, 0);

	bool ok = pTerrain->AddVisualRole("", pActor->GetID());
	if(!ok)
	{
		CORE_TRACE("can't add visual role");
	}

	pActor->Load();

	pScene->AddObject( pActor->GetID(),33);

	return pActor;
}

void RoleComposite::LoadRes()
{
//	m_PlayerCreateXml.Load(GetCore(),"share/CreateRole/PlayerCreate.xml");
//	m_PartDefineXml.Load(GetCore(),"share/CreateRole/PartDefine.xml");
}

bool RoleComposite::Init(const IVarList& args)
{
//	LoadRes();
	return true;
}

bool RoleComposite::Shut()
{
	return true;
}

