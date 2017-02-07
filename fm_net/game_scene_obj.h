//--------------------------------------------------------------------
// �ļ���:		game_scene_obj.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��2��18��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _GAME_SCENE_OBJ_H
#define _GAME_SCENE_OBJ_H

#include "../public/core_mem.h"
#include "../public/fast_str.h"
#include "../public/core_type.h"
#include "game_object.h"

// ��Ϸ�����еĶ���

class GameSceneObj: public GameObject
{
public:
	GameSceneObj();
	virtual ~GameSceneObj();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// λ��
	virtual len_t GetPosiX() const;
	virtual len_t GetPosiY() const;
	virtual len_t GetPosiZ() const;
	virtual len_t GetOrient() const;

	// �ƶ�Ŀ��
	virtual len_t GetDestX() const;
	virtual len_t GetDestY() const;
	virtual len_t GetDestZ() const;
	virtual len_t GetDestOrient() const;
	virtual len_t GetMoveSpeed() const;
	virtual len_t GetRotateSpeed() const;
	virtual len_t GetJumpSpeed() const;
	virtual len_t GetGravity() const;

	// �ƶ�ģʽ
	void SetMode(int value);
	virtual int GetMode() const;
	
	// �ƶ���Ϣ��ʱ
	void SetDelay(int value);
	virtual int GetDelay() const;

	// ����λ��
	bool SetLocation(len_t x, len_t y, len_t z, len_t orient);
	// �����ƶ�Ŀ��
	bool SetDestination(len_t x, len_t y, len_t z, len_t orient,
		len_t move_speed, len_t rotate_speed, len_t jump_speed, len_t gravity);

	// ���Ӷ���
	void SetLinkIdent(const char* value);
	virtual const char* GetLinkIdent() const;

	// ����λ��
	virtual len_t GetLinkX() const;
	virtual len_t GetLinkY() const;
	virtual len_t GetLinkZ() const;
	virtual len_t GetLinkOrient() const;

	// ��������λ��
	bool SetLinkPos(len_t x, len_t y, len_t z, len_t orient);

private:
	len_t m_fPosiX;
	len_t m_fPosiY;
	len_t m_fPosiZ;
	len_t m_fOrient;
	len_t m_fDestX;
	len_t m_fDestY;
	len_t m_fDestZ;
	len_t m_fDestOrient;
	len_t m_fMoveSpeed;
	len_t m_fRotateSpeed;
	len_t m_fJumpSpeed;
	len_t m_fGravity;
	int m_nMode;
	int m_nDelay;
	core_string m_strLinkIdent;
	len_t m_fLinkX;
	len_t m_fLinkY;
	len_t m_fLinkZ;
	len_t m_fLinkOrient;
};

#endif // _GAME_SCENE_OBJ_H

