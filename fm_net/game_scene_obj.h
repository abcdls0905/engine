//--------------------------------------------------------------------
// 文件名:		game_scene_obj.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年2月18日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _GAME_SCENE_OBJ_H
#define _GAME_SCENE_OBJ_H

#include "../public/core_mem.h"
#include "../public/fast_str.h"
#include "../public/core_type.h"
#include "game_object.h"

// 游戏场景中的对象

class GameSceneObj: public GameObject
{
public:
	GameSceneObj();
	virtual ~GameSceneObj();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// 位置
	virtual len_t GetPosiX() const;
	virtual len_t GetPosiY() const;
	virtual len_t GetPosiZ() const;
	virtual len_t GetOrient() const;

	// 移动目标
	virtual len_t GetDestX() const;
	virtual len_t GetDestY() const;
	virtual len_t GetDestZ() const;
	virtual len_t GetDestOrient() const;
	virtual len_t GetMoveSpeed() const;
	virtual len_t GetRotateSpeed() const;
	virtual len_t GetJumpSpeed() const;
	virtual len_t GetGravity() const;

	// 移动模式
	void SetMode(int value);
	virtual int GetMode() const;
	
	// 移动消息延时
	void SetDelay(int value);
	virtual int GetDelay() const;

	// 设置位置
	bool SetLocation(len_t x, len_t y, len_t z, len_t orient);
	// 设置移动目标
	bool SetDestination(len_t x, len_t y, len_t z, len_t orient,
		len_t move_speed, len_t rotate_speed, len_t jump_speed, len_t gravity);

	// 连接对象
	void SetLinkIdent(const char* value);
	virtual const char* GetLinkIdent() const;

	// 连接位置
	virtual len_t GetLinkX() const;
	virtual len_t GetLinkY() const;
	virtual len_t GetLinkZ() const;
	virtual len_t GetLinkOrient() const;

	// 设置连接位置
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

