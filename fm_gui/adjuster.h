//--------------------------------------------------------------------
// 文件名:		adjuster.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年5月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _ADJUSTER_H
#define _ADJUSTER_H

#include "i_control.h"

// 分割调节器

class Adjuster: public IControl
{
public:
	Adjuster();
	virtual ~Adjuster();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	virtual int GetMinWidth() const { return 1; }
	virtual int GetMinHeight() const { return 1; }
	
	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	virtual int OnLButtonDown(int x, int y);
	virtual int OnLButtonUp(int x, int y);
	virtual int OnDragMove(int x, int y);

private:
	// 是否处于按下状态
	bool GetPressed() const
	{
		return m_bPush && GetCapture();
	}
	
private:
	bool m_bPush;
	int m_nClickX;
	int m_nClickY;
};

#endif // _ADJUSTER_H
