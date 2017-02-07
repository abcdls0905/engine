//--------------------------------------------------------------------
// �ļ���:		SceneBox.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��5��31��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _SCENEBOX_H
#define _SCENEBOX_H

#include "i_control.h"

// SceneBox
// Ƕ�볡����

class SceneBox: public IControl
{
public:
	SceneBox();
	virtual ~SceneBox();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual int GetMinWidth() const { return 16; }
	virtual int GetMinHeight() const { return 16; }
	
	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	virtual int OnLButtonDown(int x, int y);
	virtual int OnLButtonUp(int x, int y);
	virtual int OnLButtonDoubleClick(int x, int y);
	virtual int OnRButtonDown(int x, int y);
	virtual int OnRButtonUp(int x, int y);
	virtual int OnDragMove(int x, int y);
	virtual int OnDragLeave(int x, int y);
	virtual int OnMouseMove(int x, int y);
	
	virtual bool TestTransparent(int x, int y);

	// Ƕ�볡������
	void SetSceneID(const PERSISTID& id);
	PERSISTID GetSceneID() const;

    //// ͸��
    //void SetTransparent(bool bTrans);
    //bool GetTransparent();

	// �����һ�ε����λ��
	int GetOldX() const;
	int GetOldY() const;

private:
	// ���ʵ������ʾ�豸�ϵ����λ��
	int GetRealDeviceX(int x);
	int GetRealDeviceY(int y);

private:
	PERSISTID m_SceneID;
    //bool m_bTransparent;
	int m_nOldX;
	int m_nOldY;
};

#endif // _SCENEBOX_H
