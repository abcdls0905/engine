//--------------------------------------------------------------------
// 文件名:		radio_button.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _RADIO_BUTTON_H
#define _RADIO_BUTTON_H

#include "i_control.h"
#include "check_button.h"

// 单选框

class RadioButton : public CheckButton  
{
public:
	RadioButton();
	virtual ~RadioButton();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	virtual bool SetCheckValue(bool value);
};

#endif // _RADIO_BUTTON_H

