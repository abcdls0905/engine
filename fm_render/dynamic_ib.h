//--------------------------------------------------------------------
// 文件名:		dynamic_ib.h
// 内  容:		
// 说  明:		
// 创建日期:	2009年6月24日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _DYNAMIC_IB_H
#define _DYNAMIC_IB_H


#include "../visual/i_resource.h"

// 动态索引缓冲

class Render;

class CDynamicIB: public IDynamicIB
{
public:
	CDynamicIB(Render* pRender,const char* pstrIdent);
	virtual ~CDynamicIB();

	// 释放
	virtual void Release();

	// 创建
	bool Create(int format, unsigned int size);

	// 设置资源索引
	void SetIndex(int value) { m_nIndex = value; }
	
private:
	CDynamicIB();
	CDynamicIB(const CDynamicIB&);
	CDynamicIB& operator=(const CDynamicIB&);
	
	// 销毁
	virtual void Destroy();
	// 设备就绪时的处理
	virtual bool Restore();
	// 设备丢失时的处理
	virtual bool Invalidate();

private:
	Render* m_pRender;
	int m_nIndex;
	const char* m_pstrIdent;
};

#endif // _DX_DYNAMIC_IB_H
