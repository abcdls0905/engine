

#include "form_createrole.h"

#include "../global.h"
#include "../helper.h"

#include "../fm_gui/edit.h"
#include "../fm_gui/edit.h"
#include "../fm_gui/label.h"
#include "../fm_gui/button.h"
#include "../fm_gui/i_control.h"
#include "../fm_gui/check_button.h"

#include "../visual/i_game_receiver.h"
#include "../visual/i_game_sock.h"
#include "../visual/i_game_sender.h"
#include "../visual/i_scene.h"
#include "../visual/i_actor.h"
#include "../visual/i_terrain.h"
#include "../visual/i_camera.h"

#include "../util/actor_util.h"
#include "../action/action.h"

#include "../tools/string_util.h"
#include "../public/auto_mem.h"
#include "../define/roleinfodefine.h"

#include "../querymanager/items_query.h"
#include "../querymanager/staticdata_querymodule.h"

#include "../rolecomposite/role_composite.h"
#include "../stage_manager/stagemanager.h"

#include "../source_path_define.h"
#include "../custom_prop_define.h"

#include "form_clone_ui3d.h"

//
const float DOUBLE_PI = (2*PI);
const float SLIDE_YAW_SPEED = 0.005f;
const int MAX_ROLE_COUNT = 3;

// ��ɫΧ�ɵ�Բ�İ뾶
const float YAW_RADIUS = 2.0f;
const float ROTATE_DIST_SCALE = 0.01f;

//#define ROLE_POSITION -36.0f,10.0f,10.0f
#define ROLE_ANGLE		0.0f,1.5f,0.0f
#define STAGE_MODEL_SCALE 0.6f,0.6f,0.6f

#define STAGE_MODEL_INI "ini\\npc\\item\\item_stage_001.ini"

// Բ�ĵ��������λ��
const float DIS_TO_CAMERA = 13.0f;

// ��ɫ�����
static int ROLE_INDEX_ORDER[MAX_ROLE_COUNT] = 
{
	2,	// 1 �Ž�ɫ˳����2����ʿ��
	3,	// 2 �Ž�ɫ˳����3���̿ͣ�
	1,	// 3 �Ž�ɫ˳����1����è��
};

// ��ɫ����ת����˳������
static int RoleIndexToOrder(int roleIndex)
{
	if (roleIndex<=0 || roleIndex>MAX_ROLE_COUNT) {
		return 0;
	}

	return ROLE_INDEX_ORDER[roleIndex-1];
}


DECLARE_ENTITY(FormCreateRole, 0, IEntity);

FormCreateRole* FormCreateRole::Instance()
{
	FormCreateRole* pInstance = (FormCreateRole*)CHelper::GetGlobalEntity("FormCreateRole");
	if (pInstance == NULL)
	{
		pInstance = (FormCreateRole*)CHelper::CreateEntity("FormCreateRole");
	}
	return pInstance;
}

//
FormCreateRole::FormCreateRole(void)
{
	m_SelectRoleIndex = 0;
}

FormCreateRole::~FormCreateRole(void)
{
}


bool FormCreateRole::Init(const IVarList& args)
{
	return true;
}
bool FormCreateRole::Shut()
{
	return true;
}
//
void FormCreateRole::OnCreate()
{
	REG_CONTROL_EVENT("btn_create",		"on_click",	FormCreateRole::OnBtnCreateRole);
	REG_CONTROL_EVENT("btn_enter_game",	"on_click",	FormCreateRole::OnBtnEnterGame);
	REG_CONTROL_EVENT("btn_rand_name",	"on_click",	FormCreateRole::OnBtnRandName);

	REG_CONTROL_EVENT("btn_back",		"on_click",	FormCreateRole::OnBtnBack);

    event_connect(g_gamesock->GetID(), "on_connect_fail", EventDelegate(this, &FormCreateRole::OnSockClosed));
    event_connect(g_gamesock->GetID(), "on_close", EventDelegate(this, &FormCreateRole::OnSockClosed));

	CORE_ADD_EXECUTE(this);

	std::string exact_model_file = g_core->GetResourcePath() + std::string("share/createrole/playercreate_exact.xml");
	m_ExactModelConfig.LoadResource(exact_model_file);

}


void FormCreateRole::InitFormPosition()
{
	// �������
	Maximize();

	GuiUtils::gui_center_h(GetControl("lbl_title"));
	GuiUtils::gui_center_h(GetControl("lbl_bottom"));
	GuiUtils::gui_center_h(GetControl("btn_enter_game"));
	GuiUtils::gui_center_h(GetControl("group_create"));
}

//��ʾ����
void FormCreateRole::OnShow(bool bShow)
{
	m_bSlipping = false;
	m_bSlippingLeft = false;

	m_SelectRoleIndex = 0;
	m_CurYawAngle = 0.0f;

	m_ModelGroups.clear();
	m_ServerRoleList.clear();
	m_randName.LoadResource();

	IGameReceiver* pReceiver = (IGameReceiver*)g_core->GetEntity(g_gamesock->GetReceiver());
	if (pReceiver == NULL)
	{
		return;
	}

	int serverCount = pReceiver->GetRoleCount();
	for (int i=0; i<serverCount; i++)
	{
		const wchar_t* szRolePara = pReceiver->GetRolePara(i);
		//int serverIndex = pReceiver->GetRoleIndex(i);
		_parseRolePara(szRolePara, i);
	}

	serverCount = serverCount > MAX_ROLE_COUNT ? MAX_ROLE_COUNT : serverCount;

	for (int i=1; i<=MAX_ROLE_COUNT; i++)
	{
		// ���ݾ�ģ���ô�����ģ
		_createExactModel(i);
	}

	for (int i=0; i<serverCount; i++)
	{
		// ������������еĽ�ɫ���ظ��Թ�
		//int serverIndex = pReceiver->GetRoleIndex(i);
		_createServerRoleModel(i);
	}

	for (int i=1; i<=MAX_ROLE_COUNT; i++)
	{
		// ���������û�еĽ�ɫ���ͷ�����ظ����Թ�
		_createLocalRoleModel(i);
	}

	InitFormPosition();
	_setRolePosition();
}

void FormCreateRole::OnClosed()
{
	ModelGroupType::iterator itBeg = m_ModelGroups.begin();
	ModelGroupType::iterator itEnd = m_ModelGroups.end();
	for (int i=0; itBeg!=itEnd; i++,itBeg++)
	{
		int roleIndex = itBeg->first;
		_removeRoleModel(roleIndex);
	}

	m_randName.Clear();

    _removeRoleInfoUi();

    event_disconnect(g_gamesock->GetID(), "on_connect_fail", EventDelegate(this, &FormCreateRole::OnSockClosed));
    event_disconnect(g_gamesock->GetID(), "on_close", EventDelegate(this, &FormCreateRole::OnSockClosed));

	CORE_REMOVE_EXECUTE(this);
}

void FormCreateRole::Execute(float seconds)
{
	if (m_bSlipping)
	{
		if (m_bSlippingLeft)
		{
			m_CurYawAngle += seconds*SLIDE_YAW_SPEED*500;
		}
		else
		{
			m_CurYawAngle -= seconds*SLIDE_YAW_SPEED*500;
		}

		_setRolePosition();
	}
}

bool FormCreateRole::OnSlidLeft(float fSpeed, float fDist)
{
	m_CurYawAngle += SLIDE_YAW_SPEED*fDist;

	m_bSlippingLeft = fDist>0 ;


	_setRolePosition();
	return true;
}

bool FormCreateRole::OnSlidEnd(const IVarList& args)
{
	m_bSlipping = true;
	return true;
}

void FormCreateRole::OnSockClosed(const char* event, const IVarList& arg)
{
    if (!g_gamesock->GetConnected())
    {
        FormWaiting::ShowForReconnect();
    }
}

void FormCreateRole::StopSlid()
{
	m_bSlipping=false;
}


// ��ȡ�����������
std::wstring FormCreateRole::_getRolePara( const std::string& propName, int index)
{
	if(m_ServerRoleList.find(index)==m_ServerRoleList.end())
	{
		return L"";
	}

	PropValueType prop_value = m_ServerRoleList[index];
	if (prop_value.find(propName)==prop_value.end())
	{
		return L"";
	}

	return prop_value[propName];
}

// ��������˷����ĵ�¼��Ϣ
void FormCreateRole::_parseRolePara(const std::wstring& para, int index)
{
	PropValueType prop_value;
	CVarList args; StringUtil::ParseToVectorW(para.c_str(), L';', args);
	for (int i=0; i<args.GetCount(); i++)
	{
		std::wstring info = args.WideStrVal(i);
		CVarList args2; StringUtil::ParseToVectorW(info.c_str(), L',', args2);
		if (args2.GetCount() < 2)
		{
			continue;
		}

		std::string strKey = StringUtil::WideStrAsString(args2.WideStrVal(0));
		std::wstring wstrValue = args2.WideStrVal(1);

		prop_value[strKey] = wstrValue;
	}

	m_ServerRoleList[index] = prop_value;
}

// ��������˵Ľ�ɫ��ģ��
void FormCreateRole::_createServerRoleModel(int serverIndex)
{
	if (m_ServerRoleList.find(serverIndex)==m_ServerRoleList.end())
	{
		return;
	}

	std::wstring wsIndex = _getRolePara("RoleIndex", serverIndex);
	int roleIndex = StringUtil::WideStrAsInt(wsIndex.c_str());

	// �Ѿ��������ˣ�����
	if (m_ModelGroups.find(roleIndex)!=m_ModelGroups.end())
	{
		return;
	}

	// ����һ����ɫ��ģ
	IActor* pActor = simple_role_composite_create_expose(roleIndex);
	PropValueType prop_value = m_ServerRoleList[serverIndex];
	PropValueType::iterator itBeg = prop_value.begin();
	PropValueType::iterator itEnd = prop_value.end();
	for (; itBeg!=itEnd; itBeg++)
	{
		std::string prop_name = itBeg->first;
		std::wstring prop_value = itBeg->second;

		// �ӷ���˻�ȡ����������ģ��װ��
		simple_role_composite_refresh_part(pActor, prop_name, prop_value);
	}
	g_terrain->AddVisualRole("", pActor->GetID());
	g_terrain->SetPlayerID(pActor->GetID());

	m_ModelGroups[roleIndex].role_model = pActor->GetID();

	// ������ɫ���µ�̨��ģ��
	_createStageModel(roleIndex);
	
    // ������Ϣ��
    _createRoleInfoUi(roleIndex);
}

// �ñ������ݴ�����ɫģ��
void FormCreateRole::_createLocalRoleModel(int roleIndex)
{
	// �Ѿ��������ˣ�����
	if (m_ModelGroups.find(roleIndex)!=m_ModelGroups.end())
	{
		return;
	}

	// ����һ����ɫ��ģ
	IActor* pActor = simple_role_composite_create_expose(roleIndex);
	// ���ϳ���ʱ���õ�װ��
	simple_role_composite_add_born_equipment(pActor);
	g_terrain->AddVisualRole("", pActor->GetID());
	g_terrain->SetPlayerID(pActor->GetID());

	// �浽����
	m_ModelGroups[roleIndex].role_model = pActor->GetID();

	// ������ɫ���µ�̨��ģ��
	_createStageModel(roleIndex);

    _createRoleInfoUi(roleIndex);
}

// ������ɫ���µ�̨��ģ��
void FormCreateRole::_createStageModel(int roleIndex)
{
	// ����̨��
	IActor* pNpc = simple_npc_create_from_ini(STAGE_MODEL_INI);
	pNpc->SetScale(STAGE_MODEL_SCALE);
	g_terrain->AddVisualRole("", pNpc->GetID());

	m_ModelGroups[roleIndex].stage_model = pNpc->GetID();
}

// ������ɫ��Ϣui
void FormCreateRole::_createRoleInfoUi( int roleIndex )
{
    std::wstring wsLevel, wsName;
    bool bIsHave;

    int serverIndex = _getServerIndex(roleIndex);
    if (serverIndex < 0) // ������޴˽�ɫ
    {
        RoleComposite * pRoleComposite = (RoleComposite *)CHelper::GetGlobalEntity(GLOBAL_PROP_ENT_role_composite);
        if (NULL == pRoleComposite)
        {
            return ;
        }

        wsLevel = L"";
        wsName = UtilText(pRoleComposite->GetPropConfig(roleIndex, "Name").c_str());
        bIsHave = false;
    }
    else
    {
        IGameReceiver* pReceiver = (IGameReceiver*)g_core->GetEntity(g_gamesock->GetReceiver());
        if (pReceiver == NULL)
        {
            return;
        }

        wsLevel = _getRolePara("Level", serverIndex);
        wsName = pReceiver->GetRoleName(serverIndex);
        bIsHave = true;
    }

    FormCloneUI3D::Instance()->ShowRoleInfo(roleIndex, wsName, wsLevel, bIsHave);
}

// �Ƴ�������ɫui
void FormCreateRole::_removeRoleInfoUi()
{
    FormCloneUI3D::Instance()->DeleteAllMesh();
}

// ������ģ
void FormCreateRole::_createExactModel(int roleIndex)
{
	RoleComposite * pRoleComposite = (RoleComposite *)CHelper::GetGlobalEntity(GLOBAL_PROP_ENT_role_composite);
	if (NULL == pRoleComposite)
	{
		return ;
	}

	// ����˽�ɫ�����˾�ģ��ʹ�þ�ģ������ʹ�ô�ģ
	std::string exact_model_file = m_ExactModelConfig.GetExactModel(roleIndex);
	std::string stage_model_file = m_ExactModelConfig.GetStageModel(roleIndex);
	std::string  show_actin_name = m_ExactModelConfig.GetShowAction(roleIndex);
	if (!exact_model_file.empty())
	{
		// ������ɫ��ģ
		IActor* pActor = simple_npc_create_from_ini(exact_model_file);
		if (!pActor)
		{	
			return;
		}

		// ������ɫ�ŵ׵�̨��NPC
		IActor* pNpc = simple_npc_create_from_ini(stage_model_file);
		if (!pNpc)
		{	
			return;
		}

        _createRoleInfoUi(roleIndex);

		g_terrain->AddVisualRole("", pActor->GetID());
		g_terrain->SetPlayerID(pActor->GetID());

		pNpc->SetScale(STAGE_MODEL_SCALE);
		g_terrain->AddVisualRole("", pNpc->GetID());

		std::string strSex = pRoleComposite->GetPropConfig(roleIndex, "Sex", 0);
		CHelper::SetCustomInt(pActor, "sex", StringUtil::StringAsInt(strSex));

		m_ModelGroups[roleIndex].role_model = pActor->GetID();
		m_ModelGroups[roleIndex].stage_model = pNpc->GetID();
		m_ModelGroups[roleIndex].role_show_action = show_actin_name;
	}
}


// �Ƴ�ģ��
void FormCreateRole::_removeRoleModel(int roleIndex)
{
	if (m_ModelGroups.find(roleIndex)==m_ModelGroups.end())
	{
		return;
	}

	PERSISTID roleID = m_ModelGroups[roleIndex].role_model;
	PERSISTID stageID = m_ModelGroups[roleIndex].stage_model;
	IActor* actorModel = (IActor*)CHelper::GetEntity(roleID);
	IActor* stageModel = (IActor*)CHelper::GetEntity(stageID);
	if (actorModel && stageModel)
	{
		g_terrain->RemoveVisual(roleID);
		g_core->DeleteEntity(roleID);

		g_terrain->RemoveVisual(stageID);
		g_core->DeleteEntity(stageID);
	}
}

bool FormCreateRole::_is_angle_near(float first, float second)
{
	// 2�ȵ�ƫ��
	float DIS = 2 * PI/180.0f;
	if (fabs(first-second)<=DIS)
	{
		return true;
	}
	return false;
}
void FormCreateRole::_setRolePosition()
{
	float dis_ang = (float)DOUBLE_PI / MAX_ROLE_COUNT;

    RoleComposite * pRoleComposite = (RoleComposite *)CHelper::GetGlobalEntity(GLOBAL_PROP_ENT_role_composite);
    if (NULL == pRoleComposite)
    {
        return ;
    }

	// ���������λ�ü����Բ��Ӧ�õ�λ��
	float fov; FmVec3 center, ang;
	StageUtils::GetCameraPos(LOGIN_SCENE_NAME, CAMERA_INDEX_CREATE, center, ang, fov);
	center += FmVec3(sin(ang.y), 0, cos(ang.y))*DIS_TO_CAMERA;
	center += FmVec3(0, -0.5, 0);

	ModelGroupType::iterator itBeg = m_ModelGroups.begin();
	ModelGroupType::iterator itEnd = m_ModelGroups.end();
	for (; itBeg!=itEnd; itBeg++)
	{
		int roleIndex = itBeg->first;
		ModelGroup group = itBeg->second;
		PERSISTID actorId = group.role_model;
		PERSISTID stageId = group.stage_model;
		IActor* actorModel = (IActor*)CHelper::GetEntity(actorId);
		IActor* stageModel = (IActor*)CHelper::GetEntity(stageId);
		if (!actorModel || !stageModel)
		{
			continue;
		}

		if (fabs(m_CurYawAngle)>DOUBLE_PI)
		{
			m_CurYawAngle = 0.0f;
		}

		//float my_dis_ang = dis_ang*(roleIndex-1);
		int order_index = RoleIndexToOrder(roleIndex);
		float my_dis_ang = dis_ang*(order_index-1);
		float angY = my_dis_ang + m_CurYawAngle;
		if (_is_angle_near(angY, 0)||_is_angle_near(angY, DOUBLE_PI))
		{
			//ConsoleLog("zbj", "ang_Y = %0.3f", angY);

			// ֹͣ����
			StopSlid();
			_selectRoleChanged(roleIndex);

			// �Զ���������
			m_CurYawAngle = -my_dis_ang;
			angY = my_dis_ang + m_CurYawAngle;
		}

		// ���㵱ǰλ��
		FmVec3 p = center + FmVec3(YAW_RADIUS*cos(angY), 0, YAW_RADIUS*sin(angY));

		// ���ý�ɫ��̨��λ��
		g_terrain->RelocateVisual(actorModel->GetID(), p.x, p.y, p.z);
		g_terrain->RelocateVisual(stageModel->GetID(), p.x, p.y-0.5f, p.z);

        // ��ɫ�߶�
        float fRoleHegith = StringUtil::StringAsFloat(pRoleComposite->GetPropConfig(roleIndex, "Height")) + 0.3f;
        FmVec3 temp = p;
        temp.y += fRoleHegith;

        // ������Ϣ�Ƹ߶�
        FormCloneUI3D::Instance()->SetRoleInfoPos(roleIndex, temp);
		actorModel->SetAngle(ROLE_ANGLE);
	}
}

// ��roleIndex��ȡ�������������
int FormCreateRole::_getServerIndex(int roleIndex)
{
	PropValueMapType::iterator itBeg = m_ServerRoleList.begin();
	PropValueMapType::iterator itEnd = m_ServerRoleList.end();
	for (; itBeg!=itEnd; itBeg++)
	{
		int serverIndex = itBeg->first;

		PropValueType prop_value = itBeg->second;

		PropValueType::iterator iB = prop_value.begin();
		PropValueType::iterator iE = prop_value.end();
		for (; iB!=iE; iB++)
		{
			if(iB->first=="RoleIndex")
			{
				int role_index = StringUtil::WideStrAsInt(iB->second);
				if (role_index==roleIndex)
				{
					return serverIndex;
				}
			}
		}
	}

	return -1;
}

// ��ȡ������µ�λ�ã�������ɫʱ��
int FormCreateRole::_getServerNewLocation()
{
	IGameReceiver* pReceiver = (IGameReceiver*)g_core->GetEntity(g_gamesock->GetReceiver());
	if (pReceiver == NULL)
	{
		return 0;
	}

	std::set<int> setRoleIdxPos;

	int location = 0;
	int serverCount = pReceiver->GetRoleCount();
	for (int i=0; i<serverCount; i++)
	{
		// i ָ���Ƿ���˷����Ľ�ɫ��������˳�򣨵�½˳��
		// iΪ0 ��ʾ�ϴε�½�Ľ�ɫ
		// loc ָ���ǽ�ɫ������˳��
		int loc = pReceiver->GetRoleIndex(i);
		setRoleIdxPos.insert(loc);
	}

	for (int i=0; i<MAX_ROLE_COUNT; ++i)
	{
		if(setRoleIdxPos.find(i)==setRoleIdxPos.end())
		{
			location = i;
			break;
		}
	}
	return location;
}
// ������Ϸ
void FormCreateRole::OnBtnEnterGame(const char* szEvent, const IVarList& arg)
{
	IGameReceiver* pReceiver = (IGameReceiver*)g_core->GetEntity(g_gamesock->GetReceiver());
	if (pReceiver == NULL)
	{
		return;
	}

	int serverIndex = _getServerIndex(m_SelectRoleIndex);
	std::wstring RoleName = pReceiver->GetRoleName(serverIndex);

    CHelper::SetGlobalWString(GLOBAL_PROP_WSTRING_ROLE_NAME, RoleName.c_str());

	//ConsoleLog("zbj", "ServerIndex = %d, roleName = %s", serverIndex, StringUtil::WideStrAsString(RoleName.c_str()).c_str());
	g_gamesender->ChooseRole(RoleName.c_str());

	Button* btnEnterGame = (Button*)GetControl("btn_enter_game");
	if (btnEnterGame)
	{
		btnEnterGame->SetEnabled(false);
	}
}

// ������ɫ
void FormCreateRole::OnBtnCreateRole(const char* szEvent, const IVarList& arg)
{
	int srvLocation = _getServerNewLocation(); 
	std::wstring wszRoleName = GetControl("ipt_name")->GetText();
	if (wszRoleName.empty())
	{
		return;
	}

	CVarList ret, args;
	args<<srvLocation<<wszRoleName.c_str()<<StringUtil::IntAsString(m_SelectRoleIndex).c_str();
	g_gamesender->CreateRole(args, ret);

	Button* btnCreate = (Button*)GetControl("btn_create");
	if (btnCreate)
	{
	//	btnCreate->SetEnabled(false);
	}
}

std::wstring FormCreateRole::OnCreateSucceedMessage()
{
	const wchar_t* wszRoleName = GetControl("ipt_name")->GetText();

    CHelper::SetGlobalWString(GLOBAL_PROP_WSTRING_ROLE_NAME, wszRoleName);

	g_gamesender->ChooseRole(wszRoleName);

	return wszRoleName;
}

// ���ؽ�ɫѡ��
void FormCreateRole::OnBtnBack(const char* szEvent, const IVarList& arg)
{
	StageManager* sm = (StageManager*)CHelper::GetGlobalEntity(GLOBAL_PROP_ENT_stage_manager);
	if (sm)
	{
		sm->SwitchStage(STAGE_ROLES);
	}
}


void FormCreateRole::OnBtnRandName(const char* szEvent, const IVarList& arg)
{
	if (m_ModelGroups.find(m_SelectRoleIndex)==m_ModelGroups.end())
	{
		return;
	}

	PERSISTID roleId = m_ModelGroups[m_SelectRoleIndex].role_model;
	IActor* actor = (IActor*)CHelper::GetEntity(roleId);
	if (!actor)
	{
		return;
	}

	int sex = CHelper::QueryCustomInt(actor, "sex");

	GetControl("ipt_name")->SetText(m_randName.GetRandName(sex).c_str());
}

void FormCreateRole::_selectRoleChanged(int roleIndex)
{
	if (roleIndex==m_SelectRoleIndex)
	{
		return;
	}
	m_SelectRoleIndex = roleIndex;

	int serverIndex = _getServerIndex(roleIndex);
	if (serverIndex<0) // ������޴˽�ɫ
	{
		// ����һ�����������
		OnBtnRandName("", CVarList()<<0);

		GetControl("group_create")->SetVisible(true);
		GetControl("btn_enter_game")->SetVisible(false);
	}
	else
	{
		GetControl("group_create")->SetVisible(false);
		GetControl("btn_enter_game")->SetVisible(true);
	}

	// ����չʾ����
	_playShowAction(roleIndex);

	//ConsoleLog("zbj", "RoleIndex = %d  serverIndex = %d", roleIndex, serverIndex);
}

void FormCreateRole::_playShowAction(int roleIndex)
{
	Action* pAction = (Action*)CHelper::GetGlobalEntity(GLOBAL_PROP_ENT_ACTION);
	PERSISTID roleId = m_ModelGroups[roleIndex].role_model;
	IActor* actor = (IActor*)CHelper::GetEntity(roleId);
	if (!actor || !pAction)
	{
		return;
	}
	
	CHelper::SetCustomBool(actor, CUSTOM_PROP_OBJECT_CREATE_FINISH, true);

	std::string show_action = m_ModelGroups[roleIndex].role_show_action;
	if (pAction->ActionExists(actor, show_action.c_str()))
	{
		pAction->DoAction(actor, show_action.c_str());
	}
}

