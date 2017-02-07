//--------------------------------------------------------------------
// �ļ���:		adjuster.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��5��15��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _ADJUSTER_H
#define _ADJUSTER_H

#include "i_control.h"

// �ָ������

class Adjuster: public IControl
{
public:
	Adjuster();
	virtual ~Adjuster();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	virtual int GetMinWidth() const { return 1; }
	virtual int GetMinHeight() const { return 1; }
	
	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	virtual int OnLButtonDown(int x, int y);
	virtual int OnLButtonUp(int x, int y);
	virtual int OnDragMove(int x, int y);

private:
	// �Ƿ��ڰ���״̬
	bool GetPressed() const
	{
		return m_bPush && GetCapture();
	}
	
private:
	bool m_bPush;
	int m_nClickX;
	int m_nClickY;
};

#endif // _ADJUSTER_H
