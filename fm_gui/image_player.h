//--------------------------------------------------------------------
// �ļ���:		image_player.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��14��	
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _IMAGE_PLAYER_H
#define _IMAGE_PLAYER_H

#include "../public/macros.h"
#include "../public/core_type.h"
#include "../public/persistid.h"
#include "gui_utils.h"

// ����ͼԪ��ʾ

class Gui;
class IPainter;

class CImagePlayer
{
private:
	enum IMAGE_TYPE_ENUM
	{
		IMAGETYPE_GLYPH,		// ͼԪ		
		IMAGETYPE_ANIMATION,	// ����	
	};

public:
	CImagePlayer();
	~CImagePlayer();

	// ͼԪ����
	void SetName(const char* value);
	const char* GetName() const;
	
	// ͼƬ�ߴ�
	int GetWidth() const;
	int GetHeight() const;
	
	// �Ƿ����
	bool IsReady();
	// �Ƿ�������
	bool IsLoadCompelte();

	// ����
	bool Create(IPainter* pPainter, Gui* pGui, bool async_load);
	// �ͷ�
	bool Release();
	
	// ָ��λ�úͷ�ʽ��
	bool Draw(int mode, int x1, int y1, int x2, int y2, 
		int blend_color = 0xffffffff);
    bool DrawImage(int mode, int x1, int y1, int x2, int y2, 
		int blend_color = 0xffffffff);
    // ָ��λ�úͷ�ʽ��ͼ��ĳһ����
    bool DrawParts(int mode, const rect_t& dest, const rect_t& src);
    // ָ��λ�úͷ�ʽ��Բ��
	bool DrawRound(int mode, int x1, int y1, int x2, int y2, int fan_num);
	// ������ʽ��
	bool DrawZoomAt(double zoom, int x_offs, int y_offs, int x1, int y1, 
		int x2, int y2);
	// ���ָ��λ�õ�ALPHAֵ
	bool GetAlpha(int mode, int x1, int y1,	int x2, int y2, int x, int y, 
		int& alpha);

    // ������ͼ
    bool DrawImageFan(int x1, int y1, int x2, int y2, float begin_angle, float end_angle);
	
private:
    bool m_bLoadFinish;
	IPainter* m_pPainter;
	Gui* m_pGui;
	core_string m_strName;
	PERSISTID m_ResourceId;
	int m_nWidth;
	int m_nHeight;
	int m_nImageType;
};

#endif // _IMAGE_PLAYER_H

