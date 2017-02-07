//--------------------------------------------------------------------
// 文件名:		mark_string.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年3月12日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _MARK_STRING_H
#define _MARK_STRING_H

#include "../public/macros.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"
#include "image_player.h"

// 标记字符串的创建状态

class CMarkStringStatus
{
public:
	CMarkStringStatus()
	{
		m_strFont[0] = 0;
		m_nColor = 0;
		m_pRef = NULL;
		m_nRefMemSize = 0;
	}

	~CMarkStringStatus();

	// 文字字体
	void SetFont(const wchar_t* value, size_t size);

	const char* GetFont() const
	{
		return m_strFont;
	}

	// 文字颜色
	void SetColor(unsigned int value)
	{
		m_nColor = value;
	}

	unsigned int GetColor() const
	{
		return m_nColor;
	}

	// 引用项
	void SetRef(const wchar_t* value, size_t size);

	const wchar_t* GetRef() const
	{
		if (NULL == m_pRef)
		{
			return L"";
		}

		return m_pRef;
	}

private:
	char m_strFont[128];
	unsigned int m_nColor;
	wchar_t* m_pRef;
	size_t m_nRefMemSize;
};

// 标记字符串

class IPainter;
class Gui;

class CMarkString
{
public:
	static CMarkString* NewInstance(IPainter* painter);

	// 字体名字的长度
	enum { FONT_SIZE = 128 };
	
	// 显示类型
	enum
	{
		PARTTYPE_STRING,	// 字符串
		PARTTYPE_IMAGE,		// 图元
	};
	
private:
	struct part_base_t
	{
		int nType;			// 类型
		int nWidth;			// 宽度
		int nHeight;		// 高度
		core_wstring wsRef;	// 引用数据
	};
	
	struct part_string_t: public part_base_t
	{
		core_string strFont;	// 字体
		unsigned int nColor;	// 颜色
		core_wstring wsContent;	// 内容
	};
	
	struct part_image_t: public part_base_t
	{
		CImagePlayer Image;		// 图元
	};	
	
public:
	CMarkString();
	~CMarkString();
	
	// 删除
	void Release();

	// 从字符串创建
	int Create(Gui* gui, const wchar_t* ws);
	// 创建在指定宽度内能输出的内容
	int CreateInWidth(Gui* gui, const wchar_t* ws, int width, 
		CMarkStringStatus& status);
	
	// 输出
	bool Write(float seconds, int x, int y, unsigned int color);
	
	// 在指定范围内输出的起始位置
	bool GetCenterStart(int left, int top, int right, int bottom, 
		int& start_x, int& start_y);
	bool GetLeftStart(int left, int top, int right, int bottom, 
		int& start_x, int& start_y);
	bool GetRightStart(int left, int top, int right, int bottom, 
		int& start_x, int& start_y);
	
	// 获得高度
	int GetHeight() const;
	// 获得宽度
	int GetWidth() const;
	// 获得位于哪个部件中
	int GetInPart(int offset_x, int offset_y);

	// 获得分拆后的部件数据
	size_t GetPartCount() const;
	int GetPartType(size_t index) const;
	const wchar_t* GetPartRef(size_t index) const;
	const wchar_t* GetPartContent(size_t index) const;

private:
	CMarkString(const CMarkString&);
	CMarkString& operator=(const CMarkString&);
	
	void ReleaseAll();

	bool AddStringPart(wchar_t* buf, size_t& count, 
		const CMarkStringStatus& status);
	bool AddImagePart(Gui* gui, const char* image, 
		const CMarkStringStatus& status);

private:
	IPainter* m_pPainter;
	TArrayPod<part_base_t*, 1, TCoreAlloc> m_Parts;
	int m_nWidth;
	int m_nHeight;
};

#endif // _MARK_STRING_H

