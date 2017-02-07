//--------------------------------------------------------------------
// 文件名:		color_data.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月14日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _COLOR_DATA_H
#define _COLOR_DATA_H

#include "../public/macros.h"
#include "../public/core_type.h"

// 颜色数据

class Gui;

class CColorData
{
public:
	CColorData();
	~CColorData();

	// 名称
	void SetName(Gui* pGui, const char* value);
	result_string GetName() const;
	
	// 颜色值
	void SetValue(unsigned int value) { m_nValue = value; }
	unsigned int GetValue() const { return m_nValue; }

private:
	unsigned int m_nValue;
};

#endif // _COLORDATA_H

