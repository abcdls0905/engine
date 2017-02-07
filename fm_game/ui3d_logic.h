// UI3D 逻辑类

#pragma once
#include "ui3d.h"
class UI3DLogic : public IEntity
{
	void TouchMove(int x, int y);

	void TouchDown(int x, int y);

	void TouchUp(int x, int y);
public:
	UI3DLogic();

	// 初始化
	virtual bool Init(const IVarList& args);

	// 关闭
	virtual bool Shut();

	// 每帧执行逻辑（如果已添加到运行队列）
	virtual void Execute(float seconds);

	// 窗口消息处理
	virtual bool MsgProc(unsigned int msg, size_t param1, size_t param2, int& result);
public:
	//触摸到页面时
	void OnTouchIn(int panel, int x, int y);

	//离开页面时
	void OnTouchOut(int panel, int x, int y);
	
	void OnTouchDown(int panel, int x, int y);

	void OnTouchUp(int panel, int x, int y);

	void SetVisible(bool);

	bool GetVisible(void);

protected:

	void RotateUI3D(float seconds);
	
protected:
	int m_mousefocus;
	FmVec2 m_uv;
	UI3DX* m_ui3d;
};