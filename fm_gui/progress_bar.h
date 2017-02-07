//--------------------------------------------------------------------
// 文件名:		progress_bar.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PROGRESS_BAR_H
#define _PROGRESS_BAR_H

#include "../public/core_type.h"
#include "i_control.h"

// 进度条

class ProgressBar: public IControl
{
public:
	ProgressBar();
	virtual ~ProgressBar();
	
	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	// 是否加载完成
	virtual bool GetLoadFinish();

	// 得到或设置最小值
	void SetMinimum(int value);
	int GetMinimum() const;

	// 得到或设置最大值
	void SetMaximum(int value);
	int GetMaximum() const;

	// 得到或设置当前值
	void SetCurValue(int value);
	int GetCurValue() const;

    // 设置显示进度文本
    void SetTextVisible(bool value);
    bool GetTextVisible() const;

    // 进度文本显示方式
    void SetTextPercent(bool value);
    bool GetTextPercent() const;

	// 设置进度文本颜色
	void SetTextColor(unsigned int val);
	unsigned int GetTextColor() const;
	void SetTextColorString(const char* val);
	result_string GetTextColorString() const;

	// 与边的间距
	void SetWidthGap(int val);
	int GetWidthGap() const;
	void SetHeightGap(int val);
	int GetHeightGap() const;

    // 进度显示模式
    void SetProgressMode(const char* mode);
    const char* GetProgressMode() const;

	// 进度贴图
	void SetProgressImage(const char* value);
	const char* GetProgressImage() const;

protected:
	int m_nMinimum;
	int m_nMaximum;
	int m_nValue;
	int m_nHeightGap;
	int m_nWidthGap;
    bool m_bTextVisible;
    bool m_bTextPercent;
	CColorData m_TextColor;

	CImagePlayer m_ProgressImage;

    enum ProgressMode
    {
        LEFT_TO_RIGHT = 0,
        RIGHT_TO_LEFT = 1,
        BOTTOM_TO_TOP = 2,
        TOP_TO_BOTTOM = 3,
        MAX_MODE = 4,
    };

    ProgressMode m_eProgressMode;
};

#endif // _PROGRESS_BAR_H
