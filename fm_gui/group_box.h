//--------------------------------------------------------------------
// �ļ���:		group_box.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��16��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _GROUP_BOX_H
#define _GROUP_BOX_H

#include "i_container.h"

// �ؼ�������

class GroupBox: public IContainer
{
public:
	GroupBox();
	virtual ~GroupBox();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

    // �Ƿ�͸��
    void SetTransparent(bool value);
    bool GetTransparent() const;

	// �Ƿ�Ҫ�����ӿؼ�
	void SetClipChildren(bool value) { m_bClipChildren = value; }
	bool GetClipChildren() const { return m_bClipChildren; }
	
    virtual int GetMinWidth() const { return 4; }
	virtual int GetMinHeight() const { return 4; }
	
	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	virtual int OnLButtonDown(int x, int y);
	virtual int OnDragMove(int x, int y);
    virtual IControl* OnGetInWhat(int x, int y);

	// �Ƿ�̶�
	void SetFixed(bool value);
	bool GetFixed() const;
	
private:
	bool m_bFixed;
	bool m_bTransparent;
	bool m_bClipChildren;
	int m_nClickX;
	int m_nClickY;
};

#endif // _GROUP_BOX_H
