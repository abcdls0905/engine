//--------------------------------------------------------------------
// �ļ���:		static_ib.h
// ��  ��:		
// ˵  ��:		
// ��������:	2009��6��25��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _STATIC_IB_H
#define _STATIC_IB_H



#include "../visual/i_resource.h"

// ��̬��������

class Render;

class CStaticIB: public IStaticIB
{
public:
	CStaticIB(Render* pRender);
	virtual ~CStaticIB();

	// �ͷ�
	virtual void Release();

	// ����
	bool Create(int format, const void* pdata, unsigned int size);

	// ������Դ����
	void SetIndex(int value) { m_nIndex = value; }
	
private:
	CStaticIB();
	CStaticIB(const CStaticIB&);
	CStaticIB& operator=(const CStaticIB&);
	
	// ����
	virtual void Destroy();

	// �豸����ʱ�Ĵ���
#ifdef PROCESS_LOST_DEVICE
	virtual bool Restore();
	// �豸��ʧʱ�Ĵ���
	virtual bool Invalidate();
#endif

private:
	Render* m_pRender;
	int m_nIndex;
};

#endif // _DX_STATIC_IB_H
