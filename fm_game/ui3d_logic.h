// UI3D �߼���

#pragma once
#include "ui3d.h"
class UI3DLogic : public IEntity
{
	void TouchMove(int x, int y);

	void TouchDown(int x, int y);

	void TouchUp(int x, int y);
public:
	UI3DLogic();

	// ��ʼ��
	virtual bool Init(const IVarList& args);

	// �ر�
	virtual bool Shut();

	// ÿִ֡���߼����������ӵ����ж��У�
	virtual void Execute(float seconds);

	// ������Ϣ����
	virtual bool MsgProc(unsigned int msg, size_t param1, size_t param2, int& result);
public:
	//������ҳ��ʱ
	void OnTouchIn(int panel, int x, int y);

	//�뿪ҳ��ʱ
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