//--------------------------------------------------------------------
// 文件名:		HyperLinkStyleManager.h
// 内  容:		超链接样式配置管理
// 说  明:		
// 创建日期:	2008年8月22日
// 创建人:		杨利平
// 版权所有:	苏州蜗牛电子有限公司
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
    
	std::string	szFont;		// 字体
	long size;				// 字体大小
	unsigned int color;		// 颜色
};

struct StyleItem
{
	StyleItem()
	{
	}

	std::string szStyleName;		// 样式名

	StyleElement elements[HYPERLINK_MAXSTATE];
};

class HyperLinkStyleManager: public IEntity
{
public:
	HyperLinkStyleManager();
	
	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// 跟据类型名和状态获取类型
	bool GetStyleByState(const char* pStyleName, HyperLinkState state, 
		StyleElement*& pElement);

    // 公共超链接信息处理
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
