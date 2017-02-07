//--------------------------------------------------------------------
// �ļ���:		dynamic_ib.h
// ��  ��:		
// ˵  ��:		
// ��������:	2009��6��24��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _DYNAMIC_IB_H
#define _DYNAMIC_IB_H


#include "../visual/i_resource.h"

// ��̬��������

class Render;

class CDynamicIB: public IDynamicIB
{
public:
	CDynamicIB(Render* pRender,const char* pstrIdent);
	virtual ~CDynamicIB();

	// �ͷ�
	virtual void Release();

	// ����
	bool Create(int format, unsigned int size);

	// ������Դ����
	void SetIndex(int value) { m_nIndex = value; }
	
private:
	CDynamicIB();
	CDynamicIB(const CDynamicIB&);
	CDynamicIB& operator=(const CDynamicIB&);
	
	// ����
	virtual void Destroy();
	// �豸����ʱ�Ĵ���
	virtual bool Restore();
	// �豸��ʧʱ�Ĵ���
	virtual bool Invalidate();

private:
	Render* m_pRender;
	int m_nIndex;
	const char* m_pstrIdent;
};

#endif // _DX_DYNAMIC_IB_H
