//--------------------------------------------------------------------
// 文件名:		round_box.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年3月20日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _ROUND_BOX_H
#define _ROUND_BOX_H

#include "../public/core_type.h"
#include "i_control.h"

// 圆形控件

class RoundBox: public IControl
{
public:
	RoundBox();
	virtual ~RoundBox();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	// 是否加载完成
	virtual bool GetLoadFinish();

    // 左键点击
	virtual int OnLButtonDown(int x, int y);
    // 鼠标移动
    virtual int OnMouseMove(int x, int y);

	virtual IControl* OnGetInWhat(int x, int y);
	
	// 图片名
	void SetImage(const char* value);
	const char* GetImage() const;

	// 宽度缩放比例
	void SetZoomWidth(double value);
	double GetZoomWidth() const;

	// 高度缩放比例
	void SetZoomHeight(double value);
	double GetZoomHeight() const;

	// 控件中心点在图片上的位置
	void SetCenterX(int value);
	int GetCenterX() const;
	void SetCenterY(int value);
	int GetCenterY() const;

	// 扇面数量
	void SetFanCount(int value);
	int GetFanCount() const;

	// 是否透明
	void SetTransparent(bool value);
	bool GetTransparent() const;

	// 图片尺寸
	int GetImageWidth() const;
	int GetImageHeight() const;

private:
	// 判断指定位置是否透明
	bool TestTransparent(int x, int y);
	
private:
	core_string m_strImage;
	PERSISTID m_TexId;
	double m_dZoomWidth;
	double m_dZoomHeight;
	int m_nCenterX;
	int m_nCenterY;
	int m_nFanCount;
	bool m_bTransparent;
};

#endif // _ROUND_BOX_H

