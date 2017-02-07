
#pragma once
#include "../math/fm_math.h"
#include "../public/i_entity.h"
#include "../define/fmtouchdefine.h"

#include <string>
#include <vector>
#include <map>

class GameControl : public IEntity
{
public:
	GameControl(void);
	~GameControl(void);

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	virtual void Execute(float seconds);
	virtual bool MsgProc(unsigned int msg, size_t param1, size_t param2, int& result);

private:
    //Windows消息处理
    bool MsgMouseMove(size_t param1, size_t param2);
    bool MsgLButtonDown( size_t param1, size_t param2);
    bool MsgLButtonUp(size_t param1, size_t param2);
    bool MsgKeyDown(size_t param1, size_t param2);
    bool MsgKeyUp(size_t param1, size_t param2);

    //平板消息处理
    bool MsgTouchBegin(size_t param1, size_t param2);
    bool MsgTouchEnd(size_t param1, size_t param2);
    bool MsgTouchMove(size_t param1, size_t param2);
    bool MsgTouchCancel(size_t param1, size_t param2);
	//void MsgPauseAndResume(bool bPause);

private:
    //鼠标是否点到UI上
    bool IsMouseClickUI(size_t id, int x, int y);
    //测试是否点到模型了
    bool TestClickModel(int x, int y);

private:
    int     m_iStartX;
    int     m_iStartY;
	bool    m_bLBtnDown;        //左键是否按下
	bool    m_bMouseMove;       //鼠标是否移动

#ifdef _WIN32
    bool    m_bKBPathFinding;     //键盘寻路进行中
    bool    m_bKBJoyClicking;     //键盘方向键按下中
#endif
};
