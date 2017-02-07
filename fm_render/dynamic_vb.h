//--------------------------------------------------------------------
// �ļ���:		dynamic_vb.h
// ��  ��:		
// ˵  ��:		
// ��������:	2009��6��24��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _DYNAMIC_VB_H
#define _DYNAMIC_VB_H

#include "../visual/i_resource.h"

// ��̬���㻺��

class Render;

class CDynamicVB: public IDynamicVB
{
public:
	CDynamicVB(Render* pRender,const char* pstrIdent);
	virtual ~CDynamicVB();

	// �ͷ�
	virtual void Release();

	// ����
	bool Create(unsigned int size);

	// ������Դ����
	void SetIndex(int value) { m_nIndex = value; }

	// ���ش���VB��ʱ��ı�ʶ��
	const char* GetIdent();
private:
	CDynamicVB();
	CDynamicVB(const CDynamicVB&);
	CDynamicVB& operator=(const CDynamicVB&);
	
	// ����
	virtual void Destroy();

	// �豸����ʱ�Ĵ���
	virtual bool Restore();
	// �豸��ʧʱ�Ĵ���
	virtual bool Invalidate();
private:
	Render* m_pRender;
	const char* m_pstrIdent;
	int m_nIndex;
};

#endif // _DYNAMIC_VB_H
