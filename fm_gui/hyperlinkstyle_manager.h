//--------------------------------------------------------------------
// �ļ���:		HyperLinkStyleManager.h
// ��  ��:		��������ʽ���ù���
// ˵  ��:		
// ��������:	2008��8��22��
// ������:		����ƽ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef HyperLinkStyleManager_h_2008_8_22
#define HyperLinkStyleManager_h_2008_8_22

#include "../public/i_entity.h"
#include "../public/core_mem.h"
#include "../utils/string_pod.h"
#include <string>
#include <vector>

// HyperLinkStyleManager

enum HyperLinkState
{
	HYPERLINK_NORMAL	= 0,
	HYPERLINK_MOUSEIN	= 1,
	HYPERLINK_PRESS		= 2,
	HYPERLINK_DISABLE	= 3,
	HYPERLINK_MAXSTATE  = 4,
};

struct StyleElement
{
	StyleElement()
	{
	}
    
	std::string	szFont;		// ����
	long size;				// �����С
	unsigned int color;		// ��ɫ
};

struct StyleItem
{
	StyleItem()
	{
	}

	std::string szStyleName;		// ��ʽ��

	StyleElement elements[HYPERLINK_MAXSTATE];
};

class HyperLinkStyleManager: public IEntity
{
public:
	HyperLinkStyleManager();
	
	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// ������������״̬��ȡ����
	bool GetStyleByState(const char* pStyleName, HyperLinkState state, 
		StyleElement*& pElement);

    // ������������Ϣ����
    bool OnClickHyperLink(std::wstring &wcsLinkData);
    bool OnMouseInHyperLink(std::wstring &wcsLinkData);
    bool OnMouseOutHyperLink(std::wstring &wcsLinkData);
    bool OnDoubleClickHyperLink(std::wstring &wcsLinkData);
	bool OnRightClickHyperLink(std::wstring &wcsLinkData);

private:
	std::vector<StyleItem> m_vStyleItems;
	TStringPod<char, size_t, TStringTraits<char>, 
		TCoreAlloc> m_mStyleNameToIndex;
	//std::map<std::string, long> m_mStyleNameToIndex;
};

#endif //HyperLinkStyleManager_h_2008_8_22
