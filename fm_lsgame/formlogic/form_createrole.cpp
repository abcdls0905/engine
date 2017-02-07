

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

// 角色围成的圆的半径
const float YAW_RADIUS = 2.0f;
const float ROTATE_DIST_SCALE = 0.01f;

//#define ROLE_POSITION -36.0f,10.0f,10.0f
#define ROLE_ANGLE		0.0f,1.5f,0.0f
#define STAGE_MODEL_SCALE 0.6f,0.6f,0.6f

#define STAGE_MODEL_INI "ini\\npc\\item\\item_stage_001.ini"

// 圆心到摄像机的位置
const float DIS_TO_CAMERA = 13.0f;

// 角色排序表
static int ROLE_INDEX_ORDER[MAX_ROLE_COUNT] = 
{
	2,	// 1 号角色顺序是2（剑士）
	3,	// 2 号角色顺序是3（刺客）
	1,	// 3 号角色顺序是1（熊猫）
};

// 角色索引转换到顺序索引
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
	// 窗口最大化
	Maximize();

	GuiUtils::gui_center_h(GetControl("lbl_title"));
	GuiUtils::gui_center_h(GetControl("lbl_bottom"));
	GuiUtils::gui_center_h(GetControl("btn_enter_game"));
	GuiUtils::gui_center_h(GetControl("group_create"));
}

//显示界面
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
		// 根据精模配置创建精模
		_createExactModel(i);
	}

	for (int i=0; i<serverCount; i++)
	{
		// 创建服务端已有的角色，重复略过
		//int serverIndex = pReceiver->GetRoleIndex(i);
		_createServerRoleModel(i);
	}

	for (int i=1; i<=MAX_ROLE_COUNT; i++)
	{
		// 创建服务端没有的角色，和服务端重复的略过
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


// 获取分析后的数据
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

// 分析服务端发来的登录信息
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

// 创建服务端的角色的模型
void FormCreateRole::_createServerRoleModel(int serverIndex)
{
	if (m_ServerRoleList.find(serverIndex)==m_ServerRoleList.end())
	{
		return;
	}

	std::wstring wsIndex = _getRolePara("RoleIndex", serverIndex);
	int roleIndex = StringUtil::WideStrAsInt(wsIndex.c_str());

	// 已经创建过了，返回
	if (m_ModelGroups.find(roleIndex)!=m_ModelGroups.end())
	{
		return;
	}

	// 创建一个角色裸模
	IActor* pActor = simple_role_composite_create_expose(roleIndex);
	PropValueType prop_value = m_ServerRoleList[serverIndex];
	PropValueType::iterator itBeg = prop_value.begin();
	PropValueType::iterator itEnd = prop_value.end();
	for (; itBeg!=itEnd; itBeg++)
	{
		std::string prop_name = itBeg->first;
		std::wstring prop_value = itBeg->second;

		// 从服务端获取的数据设置模型装备
		simple_role_composite_refresh_part(pActor, prop_name, prop_value);
	}
	g_terrain->AddVisualRole("", pActor->GetID());
	g_terrain->SetPlayerID(pActor->GetID());

	m_ModelGroups[roleIndex].role_model = pActor->GetID();

	// 创建角色脚下的台子模型
	_createStageModel(roleIndex);
	
    // 创建信息板
    _createRoleInfoUi(roleIndex);
}

// 用本地数据创建角色模型
void FormCreateRole::_createLocalRoleModel(int roleIndex)
{
	// 已经创建过了，返回
	if (m_ModelGroups.find(roleIndex)!=m_ModelGroups.end())
	{
		return;
	}

	// 创建一个角色裸模
	IActor* pActor = simple_role_composite_create_expose(roleIndex);
	// 加上出生时配置的装备
	simple_role_composite_add_born_equipment(pActor);
	g_terrain->AddVisualRole("", pActor->GetID());
	g_terrain->SetPlayerID(pActor->GetID());

	// 存到表里
	m_ModelGroups[roleIndex].role_model = pActor->GetID();

	// 创建角色脚下的台子模型
	_createStageModel(roleIndex);

    _createRoleInfoUi(roleIndex);
}

// 创建角色脚下的台子模型
void FormCreateRole::_createStageModel(int roleIndex)
{
	// 创建台子
	IActor* pNpc = simple_npc_create_from_ini(STAGE_MODEL_INI);
	pNpc->SetScale(STAGE_MODEL_SCALE);
	g_terrain->AddVisualRole("", pNpc->GetID());

	m_ModelGroups[roleIndex].stage_model = pNpc->GetID();
}

// 创建角色信息ui
void FormCreateRole::_createRoleInfoUi( int roleIndex )
{
    std::wstring wsLevel, wsName;
    bool bIsHave;

    int serverIndex = _getServerIndex(roleIndex);
    if (serverIndex < 0) // 服务端无此角色
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

// 移除创建角色ui
void FormCreateRole::_removeRoleInfoUi()
{
    FormCloneUI3D::Instance()->DeleteAllMesh();
}

// 创建精模
void FormCreateRole::_createExactModel(int roleIndex)
{
	RoleComposite * pRoleComposite = (RoleComposite *)CHelper::GetGlobalEntity(GLOBAL_PROP_ENT_role_composite);
	if (NULL == pRoleComposite)
	{
		return ;
	}

	// 如果此角色设置了精模，使用精模，否则，使用粗模
	std::string exact_model_file = m_ExactModelConfig.GetExactModel(roleIndex);
	std::string stage_model_file = m_ExactModelConfig.GetStageModel(roleIndex);
	std::string  show_actin_name = m_ExactModelConfig.GetShowAction(roleIndex);
	if (!exact_model_file.empty())
	{
		// 创建角色精模
		IActor* pActor = simple_npc_create_from_ini(exact_model_file);
		if (!pActor)
		{	
			return;
		}

		// 创建角色脚底的台子NPC
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


// 移除模型
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
	// 2度的偏差
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

	// 根据摄像机位置计算出圆心应该的位置
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

			// 停止滑动
			StopSlid();
			_selectRoleChanged(roleIndex);

			// 自动对齐坐标
			m_CurYawAngle = -my_dis_ang;
			angY = my_dis_ang + m_CurYawAngle;
		}

		// 计算当前位置
		FmVec3 p = center + FmVec3(YAW_RADIUS*cos(angY), 0, YAW_RADIUS*sin(angY));

		// 设置角色及台子位置
		g_terrain->RelocateVisual(actorModel->GetID(), p.x, p.y, p.z);
		g_terrain->RelocateVisual(stageModel->GetID(), p.x, p.y-0.5f, p.z);

        // 角色高度
        float fRoleHegith = StringUtil::StringAsFloat(pRoleComposite->GetPropConfig(roleIndex, "Height")) + 0.3f;
        FmVec3 temp = p;
        temp.y += fRoleHegith;

        // 设置信息牌高度
        FormCloneUI3D::Instance()->SetRoleInfoPos(roleIndex, temp);
		actorModel->SetAngle(ROLE_ANGLE);
	}
}

// 从roleIndex获取服务端数据索引
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

// 获取服务端新的位置，创建角色时用
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
		// i 指的是服务端发来的角色数据排列顺序（登陆顺序）
		// i为0 表示上次登陆的角色
		// loc 指的是角色创建的顺序
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
// 进入游戏
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

// 创建角色
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

// 返回角色选择
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
	if (serverIndex<0) // 服务端无此角色
	{
		// 产生一个随机的名字
		OnBtnRandName("", CVarList()<<0);

		GetControl("group_create")->SetVisible(true);
		GetControl("btn_enter_game")->SetVisible(false);
	}
	else
	{
		GetControl("group_create")->SetVisible(false);
		GetControl("btn_enter_game")->SetVisible(true);
	}

	// 播放展示动作
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

