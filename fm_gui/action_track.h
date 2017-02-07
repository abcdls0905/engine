//--------------------------------------------------------------------
// 文件名:		ActionTrack.h
// 内  容:		动作融合编辑器中的滑块
// 说  明:		
// 创建日期:	2010年6月22日
// 创建人:		刘建国
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _ACTIONTRACK_H
#define _ACTIONTRACK_H

#include "../public/core_type.h"
#include "i_control.h"

// 滑块

class ActionTrack: public IControl
{
public:
	ActionTrack();
	virtual ~ActionTrack();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	virtual int GetMinWidth() const { return 1; }
	virtual int GetMinHeight() const { return 1; }
	
	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	// 是否加载完成
	virtual bool GetLoadFinish();

	virtual int OnKeyDown(unsigned int vk);
	virtual int OnLButtonDown(int x, int y);
	virtual int OnLButtonUp(int x, int y);
	virtual int OnLButtonDoubleClick(int x, int y);
	virtual int OnLButtonHover(int x, int y);
	virtual int OnDragMove(int x, int y);
	
    // 判断指定位置是否透明
    virtual bool TestTransparent(int x, int y);

	// 对齐方式
	void SetAlignString(const char* value);
	const char* GetAlignString() const;

	// 鼠标持续按下是否作为重复点击
	void SetHoverRepeat(bool value);
	bool GetHoverRepeat() const;

	// 鼠标按下时是否产生事件
	void SetPushEvent(bool value);
	bool GetPushEvent() const;
	
    // 鼠标移出放开时，是否产生点击事件
    void SetOutUpEvent(bool value);
    bool GetOutUpEvent() const;

    // 鼠标拖拽，是否产生点击事件
    void SetDragEvent(bool value);
    bool GetDragEvent() const;

	// 内置的几种外观
	void SetStyleString(const char* value);
	const char* GetStyleString() const;
	
	// 正常状态下的贴图
	void SetNormalImage(const char* value);
	const char* GetNormalImage() const;
	
	// 鼠标定位时的贴图
	void SetFocusImage(const char* value);
	const char* GetFocusImage() const;
	
	// 按下时的贴图
	void SetPushImage(const char* value);
	const char* GetPushImage() const;
	
	// 被禁止时的贴图
	void SetDisableImage(const char* value);
	const char* GetDisableImage() const;

	// 图标名称
	void SetIconName(const char* value);
	const char* GetIconName() const;

	 // 是否在改变尺寸的状态
    void SetEditMode(int value);
    int GetEditMode() const;

	// 起始帧
	void SetStartFrame(int value);
    int GetStartFrame() const;

	// 结束帧
	void SetEndFrame(int value);
    int GetEndFrame() const;

	// 最大帧
	void SetMaxFrame(int value);
    int GetMaxFrame() const;

	void SetFrameChange(int value);
    int GetFrameChange() const;

	// 是否被分割
	void SetIsSlide(bool value);
    bool GetIsSlide() const;

	// 融入
	void SetFadeIn(float value);
	float GetFadeIn() const;

	// 融出
	void SetFadeOut(float value);
	float GetFadeOut() const;

	// 缩放
	void SetScale(float value);
	float GetScale() const;

	// 设置颜色
	void SetFaceColor(unsigned int value);
	unsigned int GetFaceColor() const;

	// 确定编辑模式
	int CheckEditMode(int x, int y);

	// 是否被锁定
	void SetIsLock(bool value);
    bool GetIsLock() const;

	// 是否按下“ctrl”键
	void SetIsControl(bool value);
    bool GetIsControl() const;

    // 设置按下状态
    void SetPressed(bool bValue);

private:
	// 是否处于按下状态
	bool GetPressed() const
	{
		return m_bPush;// && GetCapture();
	}

private:
	bool m_bPush;
	bool m_bHoverRepeat;
	bool m_bPushEvent;
    bool m_bOutUpEvent;
    bool m_bDragEvent;
	int m_nAlign;
	int m_nStyle;
	int m_nClickX;
	int m_nClickY;

    int m_nDragOldX;
    int m_nDragOldY;

	CImagePlayer m_FocusImage;
	CImagePlayer m_PushImage;
	CImagePlayer m_DisableImage;
	core_string m_strIcon;
	PERSISTID m_IconId;
	bool m_bIconLoaded;

	int m_nEditMode;
	int m_nStartFrame;
	int m_nEndFrame;
	int m_nMaxFrame;
	int m_nFrameChange;
	float m_fScale;

	int m_nFadeIn;
	int m_nFadeOut;
	int m_nFadeOutEnd;
	bool m_bSlide;
	bool m_bLock;
	bool m_bControl;

	unsigned int m_btnColor;
	unsigned int m_btnColorWhite;
};

#endif // _ACTIONTRACK_H
