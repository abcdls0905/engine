//--------------------------------------------------------------------
// �ļ���:		game_scene_obj.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2008��2��18��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "game_scene_obj.h"
#include "../public/inlines.h"
#include <math.h>

/// \file game_scene_obj.cpp
/// \brief ��Ϸ�����еĶ���

/// entity: GameSceneObj
/// \brief ��Ϸ�����еĶ���ʵ��
/// \see �̳���GameObject
DECLARE_ENTITY(GameSceneObj, NULL, GameObject);

/// readonly: float PosiX
/// \brief ��ǰλ��X
DECLARE_PROPERTY_GET(len_t, GameSceneObj, PosiX, GetPosiX);
/// readonly: float PosiY
/// \brief ��ǰλ��Y
DECLARE_PROPERTY_GET(len_t, GameSceneObj, PosiY, GetPosiY);
/// readonly: float PosiZ
/// \brief ��ǰλ��Z
DECLARE_PROPERTY_GET(len_t, GameSceneObj, PosiZ, GetPosiZ);
/// readonly: float Orient
/// \brief ��ǰ����
DECLARE_PROPERTY_GET(len_t, GameSceneObj, Orient, GetOrient);

/// readonly: float DestX
/// \brief Ŀ���X
DECLARE_PROPERTY_GET(len_t, GameSceneObj, DestX, GetDestX);
/// readonly: float DestY
/// \brief Ŀ���X
DECLARE_PROPERTY_GET(len_t, GameSceneObj, DestY, GetDestY);
/// readonly: float DestZ
/// \brief Ŀ���X
DECLARE_PROPERTY_GET(len_t, GameSceneObj, DestZ, GetDestZ);
/// readonly: float DestOrient
/// \brief Ŀ�귽��
DECLARE_PROPERTY_GET(len_t, GameSceneObj, DestOrient, GetDestOrient);
/// readonly: float MoveSpeed
/// \brief �ƶ��ٶ�
DECLARE_PROPERTY_GET(len_t, GameSceneObj, MoveSpeed, GetMoveSpeed);
/// readonly: float RotateSpeed
/// \brief ת���ٶ�
DECLARE_PROPERTY_GET(len_t, GameSceneObj, RotateSpeed, GetRotateSpeed);
/// readonly: float JumpSpeed
/// \brief �����ٶ�
DECLARE_PROPERTY_GET(len_t, GameSceneObj, JumpSpeed, GetJumpSpeed);
/// readonly: float Gravity
/// \brief �������ٶ�
DECLARE_PROPERTY_GET(len_t, GameSceneObj, Gravity, GetGravity);
/// readonly: int Mode
/// \brief �ƶ�ģʽ
DECLARE_PROPERTY_GET(int, GameSceneObj, Mode, GetMode);

// GameSceneObj

GameSceneObj::GameSceneObj()
{
	m_fPosiX = 0.0F;
	m_fPosiY = 0.0F;
	m_fPosiZ = 0.0F;
	m_fOrient = 0.0F;
	m_fDestX = 0.0F;
	m_fDestY = 0.0F;
	m_fDestZ = 0.0F;
	m_fDestOrient = 0.0F;
	m_fMoveSpeed = 0.0F;
	m_fRotateSpeed = 0.0F;
	m_fJumpSpeed = 0.0F;
	m_fGravity = 0.0F;
	m_nMode = 0;
	m_nDelay = 0;
	m_fLinkX = 0.0F;
	m_fLinkY = 0.0F;
	m_fLinkZ = 0.0F;
	m_fLinkOrient = 0.0F;
}

GameSceneObj::~GameSceneObj()
{
}

bool GameSceneObj::Init(const IVarList& args)
{
	if (!GameObject::Init(args))
	{
		return false;
	}
	
	return true;
}

bool GameSceneObj::Shut()
{
	return GameObject::Shut();
}

len_t GameSceneObj::GetPosiX() const
{
	return m_fPosiX;
}

len_t GameSceneObj::GetPosiY() const
{
	return m_fPosiY;
}

len_t GameSceneObj::GetPosiZ() const
{
	return m_fPosiZ;
}

len_t GameSceneObj::GetOrient() const
{
	return m_fOrient;
}

len_t GameSceneObj::GetDestX() const
{
	return m_fDestX;
}

len_t GameSceneObj::GetDestY() const
{
	return m_fDestY;
}

len_t GameSceneObj::GetDestZ() const
{
	return m_fDestZ;
}

len_t GameSceneObj::GetDestOrient() const
{
	return m_fDestOrient;
}

len_t GameSceneObj::GetMoveSpeed() const
{
	return m_fMoveSpeed;
}

len_t GameSceneObj::GetRotateSpeed() const
{
	return m_fRotateSpeed;
}

len_t GameSceneObj::GetJumpSpeed() const
{
	return m_fJumpSpeed;
}

len_t GameSceneObj::GetGravity() const
{
	return m_fGravity;
}

void GameSceneObj::SetMode(int value)
{
	m_nMode = value;
}

int GameSceneObj::GetMode() const
{
	return m_nMode;
}

void GameSceneObj::SetDelay(int value)
{
	m_nDelay = value;
}

int GameSceneObj::GetDelay() const
{
	return m_nDelay;
}

bool GameSceneObj::SetLocation(len_t x, len_t y, len_t z, len_t orient)
{
	m_fPosiX = x;
	m_fPosiY = y;
	m_fPosiZ = z;
	m_fOrient = orient;

	return true;
}

bool GameSceneObj::SetDestination(len_t x, len_t y, len_t z, len_t orient, 
	len_t move_speed, len_t rotate_speed, len_t jump_speed, len_t gravity)
{
	m_fDestX = x;
	m_fDestY = y;
	m_fDestZ = z;
	m_fDestOrient = orient;
	m_fMoveSpeed = move_speed;
	m_fRotateSpeed = rotate_speed;
	m_fJumpSpeed = jump_speed;
	m_fGravity = gravity;

	return true;
}

void GameSceneObj::SetLinkIdent(const char* value)
{
	Assert(value != NULL);
	
	m_strLinkIdent = value;
}

const char* GameSceneObj::GetLinkIdent() const
{
	return m_strLinkIdent.c_str();
}

len_t GameSceneObj::GetLinkX() const
{
	return m_fLinkX;
}

len_t GameSceneObj::GetLinkY() const
{
	return m_fLinkY;
}

len_t GameSceneObj::GetLinkZ() const
{
	return m_fLinkZ;
}

len_t GameSceneObj::GetLinkOrient() const
{
	return m_fLinkOrient;
}

bool GameSceneObj::SetLinkPos(len_t x, len_t y, len_t z, len_t orient)
{
	m_fLinkX = x;
	m_fLinkY = y;
	m_fLinkZ = z;
	m_fLinkOrient = orient;

	return true;
}

