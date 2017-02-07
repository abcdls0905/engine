//--------------------------------------------------------------------
// 文件名:		label.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月14日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _LABEL_H
#define _LABEL_H

#include "../public/core_type.h"
#include "i_control.h"

// 文字标签

class CMarkString;

class Label: public IControl
{
public:
	Label();
	virtual ~Label();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	virtual void SetText(const wchar_t* value);

	// 设置鼠标形状
	virtual int OnSetCursor(int x, int y);
	virtual int OnLButtonDown(int x, int y);
	
	virtual IControl* OnGetInWhat(int x, int y);

	// 对齐方式
	void SetAlignString(const char* value);
	const char* GetAlignString() const;

	// 背景填充
	void SetSolid(bool value);
	bool GetSolid() const;

	// 是否包含转义符号
	void SetMarked(bool value);
	bool GetMarked() const;

	// 是否透明
	void SetTransparent(bool value);
	bool GetTransparent() const;

	// 是否产生点击图片事件
	void SetClickEvent(bool value);
	bool GetClickEvent() const;

	// 是否旋转
	void SetRotate(bool value);
	bool GetRotate() const;

	// 旋转角度
	void SetAngleX(float value);
	float GetAngleX() const;
	void SetAngleY(float value);
	float GetAngleY() const;
	void SetAngleZ(float value);
	float GetAngleZ() const;
	
	// 引用项的鼠标形状
	void SetRefCursor(const char* value);
	const char* GetRefCursor() const;

    void SetIsLimitWidth(bool IsLimit);
    bool GetIsLimitWidth();

    void SetTextOffsetX(int offsetx);
    int GetTextOffsetX();

    void SetTextOffsetY(int offsety);
    int GetTextOffsetY();

    int GetTextWidth();

protected:
	// 判断指定位置是否透明
	bool TestTransparent(int x, int y);

private:
	int m_nAlign;
    int m_nTextOffsetX;
    int m_nTextOffsetY;
	bool m_bSolid;
	bool m_bMarked;
	bool m_bTransparent;
	bool m_bClickEvent;
	bool m_bRotate;
    bool m_bLimitWidth;
	FmVec3 m_vAngle;
	core_string m_strRefCursor;
	CMarkString* m_pMarkStr;
};

#endif // _LABEL_H

