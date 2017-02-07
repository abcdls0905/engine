//--------------------------------------------------------------------
// 文件名:		win_input.h
// 内  容:		
// 说  明:		
// 创建日期:	2013年6月20日
// 创建人:		张俊胜
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _WIN_INPUT_H_
#define _WIN_INPUT_H_

#include "../public/module.h"
#include "../public/core_type.h"

#include "../visual/i_input.h"
#include "../visual/i_render.h"

class CInput: public IEntity
{
public:
	// 初始化
	bool Init(const IVarList& args);

	virtual bool MsgProc(unsigned int msg, size_t param1, size_t param2, 
		int& result);

	bool Shut();

private:

	// 允许控制
	bool EnableControl();
	// 取消控制
	bool DisableControl();

	IInputManager* m_InputManager;
	IRender* m_pRender;
};





#endif