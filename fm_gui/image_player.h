//--------------------------------------------------------------------
// 文件名:		image_player.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月14日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _IMAGE_PLAYER_H
#define _IMAGE_PLAYER_H

#include "../public/macros.h"
#include "../public/core_type.h"
#include "../public/persistid.h"
#include "gui_utils.h"

// 界面图元显示

class Gui;
class IPainter;

class CImagePlayer
{
private:
	enum IMAGE_TYPE_ENUM
	{
		IMAGETYPE_GLYPH,		// 图元		
		IMAGETYPE_ANIMATION,	// 动画	
	};

public:
	CImagePlayer();
	~CImagePlayer();

	// 图元名称
	void SetName(const char* value);
	const char* GetName() const;
	
	// 图片尺寸
	int GetWidth() const;
	int GetHeight() const;
	
	// 是否就绪
	bool IsReady();
	// 是否加载完成
	bool IsLoadCompelte();

	// 创建
	bool Create(IPainter* pPainter, Gui* pGui, bool async_load);
	// 释放
	bool Release();
	
	// 指定位置和方式画
	bool Draw(int mode, int x1, int y1, int x2, int y2, 
		int blend_color = 0xffffffff);
    bool DrawImage(int mode, int x1, int y1, int x2, int y2, 
		int blend_color = 0xffffffff);
    // 指定位置和方式画图的某一部分
    bool DrawParts(int mode, const rect_t& dest, const rect_t& src);
    // 指定位置和方式画圆形
	bool DrawRound(int mode, int x1, int y1, int x2, int y2, int fan_num);
	// 缩放形式画
	bool DrawZoomAt(double zoom, int x_offs, int y_offs, int x1, int y1, 
		int x2, int y2);
	// 获得指定位置的ALPHA值
	bool GetAlpha(int mode, int x1, int y1,	int x2, int y2, int x, int y, 
		int& alpha);

    // 画扇形图
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

