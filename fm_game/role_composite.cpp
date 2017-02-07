#include "role_composite.h"
#include "helper.h"
#include "../visual/i_actor.h"
#include "../fm_game/global.h"
#include "../visual/i_context.h"
#include "../visual/i_scene.h"
#include "../visual/i_terrain.h"

RoleComposite* g_rolecomposite = 0;

//�����������
enum ObjType
{
	TYPE_SCENE = 1, 	// ����
	TYPE_PLAYER = 2,	// ���
	TYPE_NPC = 4,		// NPC
	TYPE_ITEM = 8,		// ��Ʒ
	TYPE_HELPER = 16,	// ��������
};

#define  CUSTOM_PROP_STRING_STATE         "state"       //ģ��״̬
#define  CUSTOM_PROP_INT_STATE_INDEX   "state_index"  //ģ��״̬����
#define  CUSTOM_PROP_PRELOADFLG   "preploadflg"   //ģ��Ԥ���ر�ʾ

#define  CUSTOM_PROP_OBJECT_TERRAIN      "terrain"   //�����ĵ���
#define  CUSTOM_PROP_OBJECT_SCENE        "scene"   //���������ĳ���

#define  CUSTOM_PROP_STRING_CLIENT_IDENT   "client_ident"   //����Ŀͻ��������ʾ

#define  CUSTOM_PROP_STRING_MOUNT   "mount" // ����

inline const void* __loadfile(const char* fileaname, int& size, bool& iscache)
{
	FILE* fp = core_file::fopen(fileaname, "rb");
	if(fp == NULL)
		return false;
	core_file::fseek(fp, 0, SEEK_END);
	size_t dds_size = core_file::ftell(fp);
	core_file::fseek(fp, 0, SEEK_SET);
	IFileSys* pFileSys = g_pCore->GetFileSys();
	// ����ֱ�ӻ���ļ����ݣ�������ʱ�������ڴ�
	const void* dds_data = pFileSys->FileContent(fp);
	if (NULL == dds_data)
	{
		dds_data = (unsigned char*)CORE_ALLOC(dds_size + 1);
		core_file::fread((void*)dds_data, sizeof(char), dds_size, fp);
		((char*)dds_data)[dds_size] = 0;
		iscache = false;
	}
	else
	{
		iscache = true;
	}

	core_file::fclose(fp);
	size = dds_size;
	return dds_data;
}

DECLARE_ENTITY(RoleComposite, 0, IEntity);

RoleComposite::RoleComposite(void)
{
	g_rolecomposite = this;
}

RoleComposite::~RoleComposite(void)
{
}

IActor* RoleComposite::CreateSceneObj(IGameObject* pGameObj)
{
	IRenderContext* pContext = g_scene->GetContext();
	IActor* pActor = (IActor*)g_core->CreateEntity("Actor");
	pActor->SetContext(pContext);

	// �������͵Ĳ�ִͬ�в�ͬ�Ĳ���
	int iType = pGameObj->QueryPropInt("Type");

	// �ж������������
	if (iType == TYPE_PLAYER)
	{
		// ��������ģ�Ͷ���
		int iRoleIndex = pGameObj->QueryPropInt("RoleIndex");

		const char* action = m_PlayerCreateXml.Find("Property", "ID", iRoleIndex, "action");
		const char* main_model = m_PlayerCreateXml.Find("Property", "ID", iRoleIndex, "main_model");
		//const char* strEffectModelFile = m_PlayerCreateXml.Find("Property", "ID", iRoleIndex, "effect_model");

		bool result = pActor->SetActionEx(action, "pe_0h_stand", "", true);

		
		pActor->AddSkin("main_model", main_model);

		using namespace rapidxml;
		xml_node<> *root = m_PartDefineXml.m_doc.first_node("Object");
		xml_node<>* node = root->first_node("Property");
		while(node)
		{
			xml_attribute<>* att = node->first_attribute("ID");
			const char* prop = pGameObj->QueryPropString(att->value());
			if(prop && prop[0] && att->value())
			{
				char buf[256];
				sprintf(buf, "%s.xmod", prop);

				pActor->AddSkin(att->value(), buf);
			}
			node = node->next_sibling("Property");
		}
	}
	else
	{
		const char* p = pGameObj->QueryPropString("Resource");
		char buf[256];
		sprintf(buf, "ini/%s.ini", p);
		pActor->CreateFromIni(buf);
	}
	
	float x = pGameObj->GetPosiX(), y = pGameObj->GetPosiY(), z = pGameObj->GetPosiZ();
	pActor->SetPosition(x, y, z);
	pActor->SetAngle(0, pGameObj->GetOrient(), 0);
	bool ok = g_terrain->AddVisualRole("", pActor->GetID());
	if(!ok)
	{
		CORE_TRACE("can't add visual role");
	}
	return pActor;
}


void RoleComposite::LoadRes()
{
	m_PlayerCreateXml.Load("share/CreateRole/PlayerCreate.xml");
	m_PartDefineXml.Load("share/CreateRole/PartDefine.xml");
}

bool RoleComposite::Init(const IVarList& args)
{
	LoadRes();
	return true;
}

bool RoleComposite::Shut()
{
	return true;
}


IActor* role_composite_create(IGameObject* pGameObject)
{
	return g_rolecomposite->CreateSceneObj(pGameObject);
}