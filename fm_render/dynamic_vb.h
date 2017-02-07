//--------------------------------------------------------------------
// 文件名:		dynamic_vb.h
// 内  容:		
// 说  明:		
// 创建日期:	2009年6月24日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _DYNAMIC_VB_H
#define _DYNAMIC_VB_H

#include "../visual/i_resource.h"

// 动态顶点缓冲

class Render;

class CDynamicVB: public IDynamicVB
{
public:
	CDynamicVB(Render* pRender,const char* pstrIdent);
	virtual ~CDynamicVB();

	// 释放
	virtual void Release();

	// 创建
	bool Create(unsigned int size);

	// 设置资源索引
	void SetIndex(int value) { m_nIndex = value; }

	// 返回创建VB的时候的标识串
	const char* GetIdent();
private:
	CDynamicVB();
	CDynamicVB(const CDynamicVB&);
	CDynamicVB& operator=(const CDynamicVB&);
	
	// 销毁
	virtual void Destroy();

	// 设备就绪时的处理
	virtual bool Restore();
	// 设备丢失时的处理
	virtual bool Invalidate();
private:
	Render* m_pRender;
	const char* m_pstrIdent;
	int m_nIndex;
};

#endif // _DYNAMIC_VB_H
