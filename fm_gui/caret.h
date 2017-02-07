//--------------------------------------------------------------------
// 文件名:		caret.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _CARET_H
#define _CARET_H

#include "../public/module.h"
#include "../public/core_type.h"

// 输入光标

class IPainter;
class IControl;

class Caret: public IEntity
{
private:
	enum { MAX_CARET_WIDTH = 256 };
	enum { MAX_CARET_HEIGHT = 256 };
	
public:
	Caret();
	virtual ~Caret();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	// 名字
	void SetName(const char* name);
	const char* GetName() const;
	unsigned int GetHash() const;
	
	// 显示的时间间隔
	void SetShowInterval(float value);
	float GetShowInterval() const;
	
	// 隐藏的时间间隔
	void SetHideInterval(float value);
	float GetHideInterval() const;

	// 比例宽度
	void SetWidthScale(float value);
	float GetWidthScale() const;

	// 比例高度
	void SetHeightScale(float value);
	float GetHeightScale() const;
	
	// 固定宽度
	void SetFixWidth(int value);
	int GetFixWidth() const;
	
	// 固定高度
	void SetFixHeight(int value);
	int GetFixHeight() const;
	
	// 颜色
	void SetColorString(const char* val);
	result_string GetColorString() const;

	// 获得实际的尺寸
	int GetWidth(int width);
	int GetHeight(int height);
	unsigned int GetActualColor(unsigned int color);

	// 创建
	bool Create();
	// 重置
	void Reset();
	// 刷新
	void Flash(float time);
	
	// 在指定位置画出
	bool Draw(int left, int bottom, int width, int height, unsigned int color);
	
private:
	IPainter* m_pPainter;
	core_string m_strName;
	unsigned int m_nHash;
	bool m_bDisplay;
	float m_fShowInterval;
	float m_fHideInterval;
	float m_fFlashCount;
	float m_fWidthScale;
	float m_fHeightScale;
	int m_nFixWidth;
	int m_nFixHeight;
	unsigned int m_nColor;
};

#endif // _CARET_H

