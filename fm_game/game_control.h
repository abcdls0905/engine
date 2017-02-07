
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
    //Windows��Ϣ����
    bool MsgMouseMove(size_t param1, size_t param2);
    bool MsgLButtonDown( size_t param1, size_t param2);
    bool MsgLButtonUp(size_t param1, size_t param2);
    bool MsgKeyDown(size_t param1, size_t param2);
    bool MsgKeyUp(size_t param1, size_t param2);

    //ƽ����Ϣ����
    bool MsgTouchBegin(size_t param1, size_t param2);
    bool MsgTouchEnd(size_t param1, size_t param2);
    bool MsgTouchMove(size_t param1, size_t param2);
    bool MsgTouchCancel(size_t param1, size_t param2);
	//void MsgPauseAndResume(bool bPause);

private:
    //����Ƿ�㵽UI��
    bool IsMouseClickUI(size_t id, int x, int y);
    //�����Ƿ�㵽ģ����
    bool TestClickModel(int x, int y);

private:
    int     m_iStartX;
    int     m_iStartY;
	bool    m_bLBtnDown;        //����Ƿ���
	bool    m_bMouseMove;       //����Ƿ��ƶ�

#ifdef _WIN32
    bool    m_bKBPathFinding;     //����Ѱ·������
    bool    m_bKBJoyClicking;     //���̷����������
#endif
};
