//--------------------------------------------------------------------
// 文件名:		picture.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年5月22日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PICTURE_H
#define _PICTURE_H

#include "../public/core_type.h"
#include "i_control.h"

// 图片显示

class Picture: public IControl
{
public:
	Picture();
	virtual ~Picture();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);
	
	// 是否加载完成
	virtual bool GetLoadFinish();

	virtual int OnWidthChanged(int old);
	virtual int OnHeightChanged(int old);

	virtual int OnLButtonDown(int x, int y);
	virtual int OnLButtonUp(int x, int y);
    virtual int OnRButtonDown(int x, int y);
    virtual int OnRButtonUp(int x, int y);
	virtual int OnLButtonDoubleClick(int x, int y);
    virtual int OnMouseMove(int x, int y);

	virtual int OnDragMove(int x, int y);
	virtual int OnDragLeave(int x, int y);

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

	// 图片尺寸
	int GetImageWidth() const;
	int GetImageHeight() const;

    // 是否透明
    void SetTransparent(bool value);
    bool GetTransparent() const;

	// 获得图片格式
	const char* GetImageFormat();

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
    bool m_bTransparent;

	//拖动相关
	int m_nClickX;
	int m_nClickY;
};

#endif // _PICTURE_H
