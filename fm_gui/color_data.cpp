//--------------------------------------------------------------------
// �ļ���:		color_data.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��14��	
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
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

