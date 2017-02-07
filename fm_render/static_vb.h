//--------------------------------------------------------------------
// �ļ���:		static_vb.h
// ��  ��:		
// ˵  ��:		
// ��������:	2009��6��25��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _STATIC_VB_H
#define _STATIC_VB_H



#include "../visual/i_resource.h"

// ��̬���㻺��

class Render;

class CStaticVB: public IStaticVB
{
public:
	CStaticVB(Render* pRender);
	virtual ~CStaticVB();

	// �ͷ�
	virtual void Release();

	// ����
	bool Create(const void* pdata, unsigned int size);

	// ������Դ����
	void SetIndex(int value) { m_nIndex = value; }

private:
	CStaticVB();
	CStaticVB(const CStaticVB&);
	CStaticVB& operator=(const CStaticVB&);
	
	// ����
	virtual void Destroy();
#ifdef PROCESS_LOST_DEVICE
	// �豸����ʱ�Ĵ���
	virtual bool Restore();
	// �豸��ʧʱ�Ĵ���
	virtual bool Invalidate();
#endif


private:
	Render* m_pRender;
	int m_nIndex;
};

#endif // _STATIC_VB_H
