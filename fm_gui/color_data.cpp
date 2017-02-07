//--------------------------------------------------------------------
// 文件名:		color_data.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月14日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "color_data.h"
#include "gui_utils.h"
#include "gui.h"

// CColorData

CColorData::CColorData()
{
	m_nValue = 0;
}

CColorData::~CColorData()
{
}

void CColorData::SetName(Gui* pGui, const char* value)
{
	//Assert(pGui != NULL);
	Assert(value != NULL);

	//m_strName = value;
	//m_nValue = pGui->GetColor(value);
	m_nValue = GuiUtil_ColorToInt(value);
}

result_string CColorData::GetName() const
{
	//if (m_strName.empty())
	//{
		return GuiUtil_IntToColor(m_nValue).c_str();
	//}
	//else
	//{
	//	return m_strName.c_str();
	//}
}

