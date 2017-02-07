//--------------------------------------------------------------------
// 文件名:		float_edit.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年11月20日
// 创建人:		
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _FLOAT_EDIT_H
#define _FLOAT_EDIT_H

#include "../public/core_type.h"
#include "edit.h"
#include "label.h"

// 浮点数输入框

class FloatEdit : public Edit
{
public:
	FloatEdit();
	virtual ~FloatEdit();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void PrepareDesignInfo(IDesignInfo* di);

	virtual int OnLButtonDown(int x, int y);
	virtual int OnLButtonUp(int x, int y);
	virtual int OnRButtonDown(int x, int y);
	virtual int OnDragMove(int x, int y);
	virtual int OnKeyDown(unsigned int vk);
	virtual int OnKeyUp(unsigned int vk);
	virtual int OnChar(unsigned int ch);
	virtual int OnGetCapture(const PERSISTID& lost_id);
	virtual int OnLostCapture(const PERSISTID& get_id);

	// 浮点值精度
	void SetFormat(const wchar_t* value);
	const wchar_t* GetFormat() const;

	// 拖动时的变化步长
	void SetDragStep(float value);
	float GetDragStep() const;

	// 默认值
	void SetDefault(float value);
	float GetDefault() const;

	// 最小值
	void SetMin(float value);
	float GetMin() const;

	// 最大值
	void SetMax(float value);
	float GetMax() const;

	// 循环
	void SetLoop(bool value);
	bool GetLoop() const;

	// 设置是否显示帮助信息
	void SetShowHintText(bool value);
	bool GetShowHintText() const;

	// 追加提示信息
	void SetAppendHintText(const wchar_t* name);
	const wchar_t* GetAppendHintText() const;

	// 检测浮点数文本框内是否发生变化
	int OnChanged(const PERSISTID& changed_id);

private:
	// 更新提示信息
	void UpdateHintText(float min, float max);
	// 检查值是否有效
	bool IsValid();
	// 设置浮点数文本值
	void SetValue(float value);

private:
	point_t m_ptLButtonDownPos;		// 计算拖动，用于修改值
	core_wstring m_wsFormat;		// 浮点值精度
	bool m_bIsInteger;				// 是否为整形
	bool m_bIsDragMethod;			// 是否是拖动状态
	float m_fDragStep;				// 拖动时的变化步长
	float m_fOldDragStep;			// 保存原变化步长
	float m_fDefault;				// 默认值
	float m_fMin;					// 最小值
	float m_fMax;					// 最大值
	bool m_bLoop;					// 循环
	bool m_bHadChangedStep_s;		// 压下KEY_SHIFT是否改变了步长
	bool m_bHadChangedStep_c;		// 压下KEY_CONTROL是否改变了步长
	point_t m_OldPoint;				// 鼠标前一次的位置
		
	bool m_bShowHintText;			// 是否现实提示文本信息
	core_wstring m_wsHintText;		// 提示文本信息
	core_wstring m_wsAppendHintText;// 附加的提示文本信息
	Label* m_pHintTextLabel;		// 提示文本标签
};

#endif // _FLOAT_EDIT_H
