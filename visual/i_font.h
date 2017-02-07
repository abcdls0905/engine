//--------------------------------------------------------------------
// �ļ���:		i_font.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��26��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _VISUAL_I_FONT_H
#define _VISUAL_I_FONT_H

#include "../public/macros.h"

// �������

struct char_info_t
{
	unsigned short nCharWidth;
	unsigned short nCharHeight;
	short nXAdvance;
	short nYOffset;
	short nXOffset;
	short nUserYOffset;
	short nUserXOffset;
};

class IFont
{
public:
	virtual ~IFont() = 0;

	// �ͷ�
	virtual void Release() = 0;
	// ��ʼ��
	virtual bool Initialize() = 0;
	// �ر�
	virtual bool Shutdown() = 0;
	
	// ������ּ��
	virtual int GetInterval() = 0;
	// ������ָ߶�
	virtual int GetTextHeight() = 0;
	// ���ָ���������ֿ��
	virtual int GetTextLenWidth(const wchar_t* text, size_t len) = 0;
	// ��ȡ��һ�������ַ��Ŀ��
	virtual int GetNextLanguageCharacter(const wchar_t * text, const size_t begin) { return 1; };
	// ������ʱ��Ӱ/���ɫ����ͬ��ɫ������ʱ���ú��ٵ�����ӿڣ������������Ч
	virtual bool SetShadowColor(unsigned int color) { return true; }
	// ���ָ����������
	virtual bool WriteTextLen(int x, int y, float depth_z, float depth_w, 
		unsigned int color, const wchar_t* text, size_t len, float ratio) = 0;
	// ���������ͼ
	virtual bool DumpTex(const char* file_name) = 0;

	// ����
	virtual void SetName(const char* value) = 0;
	virtual const char* GetName() = 0;

	virtual void SetXOffset(wchar_t ch, int offset){};

	virtual void SetYOffset(wchar_t ch, int offset){};
	
	virtual int GetXOffset(wchar_t ch)  {return 0;};

	virtual int GetYOffset(wchar_t ch)  {return 0;};

	virtual bool GetCharDataInfo(wchar_t ch, char_info_t & info){ return false; }


};

inline IFont::~IFont() {}

#endif // _VISUAL_I_FONT_H
