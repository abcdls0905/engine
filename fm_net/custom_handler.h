//--------------------------------------------------------------------
// 文件名:		custom_handler.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年7月24日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _CUSTOM_HANDLER_H
#define _CUSTOM_HANDLER_H

#include "../public/module.h"

// 自定义消息处理器

class CustomHandler: public IEntity
{
public:
	CustomHandler();
	virtual ~CustomHandler();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// 处理消息
	bool Process(const IVarList& msg);
};

#endif // _CUSTOM_HANDLER_H

