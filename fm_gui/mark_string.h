//--------------------------------------------------------------------
// �ļ���:		mark_string.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��3��12��	
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _MARK_STRING_H
#define _MARK_STRING_H

#include "../public/macros.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"
#include "image_player.h"

// ����ַ����Ĵ���״̬

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

	// ��������
	void SetFont(const wchar_t* value, size_t size);

	const char* GetFont() const
	{
		return m_strFont;
	}

	// ������ɫ
	void SetColor(unsigned int value)
	{
		m_nColor = value;
	}

	unsigned int GetColor() const
	{
		return m_nColor;
	}

	// ������
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

// ����ַ���

class IPainter;
class Gui;

class CMarkString
{
public:
	static CMarkString* NewInstance(IPainter* painter);

	// �������ֵĳ���
	enum { FONT_SIZE = 128 };
	
	// ��ʾ����
	enum
	{
		PARTTYPE_STRING,	// �ַ���
		PARTTYPE_IMAGE,		// ͼԪ
	};
	
private:
	struct part_base_t
	{
		int nType;			// ����
		int nWidth;			// ���
		int nHeight;		// �߶�
		core_wstring wsRef;	// ��������
	};
	
	struct part_string_t: public part_base_t
	{
		core_string strFont;	// ����
		unsigned int nColor;	// ��ɫ
		core_wstring wsContent;	// ����
	};
	
	struct part_image_t: public part_base_t
	{
		CImagePlayer Image;		// ͼԪ
	};	
	
public:
	CMarkString();
	~CMarkString();
	
	// ɾ��
	void Release();

	// ���ַ�������
	int Create(Gui* gui, const wchar_t* ws);
	// ������ָ������������������
	int CreateInWidth(Gui* gui, const wchar_t* ws, int width, 
		CMarkStringStatus& status);
	
	// ���
	bool Write(float seconds, int x, int y, unsigned int color);
	
	// ��ָ����Χ���������ʼλ��
	bool GetCenterStart(int left, int top, int right, int bottom, 
		int& start_x, int& start_y);
	bool GetLeftStart(int left, int top, int right, int bottom, 
		int& start_x, int& start_y);
	bool GetRightStart(int left, int top, int right, int bottom, 
		int& start_x, int& start_y);
	
	// ��ø߶�
	int GetHeight() const;
	// ��ÿ��
	int GetWidth() const;
	// ���λ���ĸ�������
	int GetInPart(int offset_x, int offset_y);

	// ��÷ֲ��Ĳ�������
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

