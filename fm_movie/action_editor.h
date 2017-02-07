//--------------------------------------------------------------------
// 文件名:		ActionEditor.h
// 内  容:		
// 说  明:		
// 创建日期:	2012年10月16日
// 创建人:		刘捷
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#ifndef __ACTION_EDITOR_H__
#define __ACTION_EDITOR_H__

#include "i_frame_receiver_user.h"
#include "../public/module.h"

// 动作编辑类

class ActionEditor: public IFrameReceiverUser
{
public:
	ActionEditor() {}
	virtual ~ActionEditor() {}

private:
	virtual void ProcessFrameData(char* pFrameBuffer) {}
};

#endif // __ACTION_EDITOR_H__
