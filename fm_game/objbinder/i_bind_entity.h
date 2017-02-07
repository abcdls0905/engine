//--------------------------------------------------------------------
// 文件名:		IBindEntity
// 内  容:	    绑定Entity接口
// 说  明:		
//
// 创建日期:	2012年7月18日
// 创建人:		zj
// 版权所有:	苏州蜗牛电子有限公司
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