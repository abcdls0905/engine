//--------------------------------------------------------------------
// �ļ���:		IBindEntity
// ��  ��:	    ��Entity�ӿ�
// ˵  ��:		
//
// ��������:	2012��7��18��
// ������:		zj
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------
#ifndef  BIND_ENTITY_H
#define	 BIND_ENTITY_H

#include "../public/i_entity.h"

class IBindEntity:public IEntity
{
public:
	IBindEntity(){};
	~IBindEntity(){};

public:
	//
	virtual void SetRole(const PERSISTID& psid) = 0;
};

#endif