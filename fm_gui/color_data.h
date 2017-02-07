//--------------------------------------------------------------------
// �ļ���:		color_data.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��14��	
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _COLOR_DATA_H
#define _COLOR_DATA_H

#include "../public/macros.h"
#include "../public/core_type.h"

// ��ɫ����

class Gui;

class CColorData
{
public:
	CColorData();
	~CColorData();

	// ����
	void SetName(Gui* pGui, const char* value);
	result_string GetName() const;
	
	// ��ɫֵ
	void SetValue(unsigned int value) { m_nValue = value; }
	unsigned int GetValue() const { return m_nValue; }

private:
	unsigned int m_nValue;
};

#endif // _COLORDATA_H

