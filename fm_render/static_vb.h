//--------------------------------------------------------------------
// 文件名:		static_vb.h
// 内  容:		
// 说  明:		
// 创建日期:	2009年6月25日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _STATIC_VB_H
#define _STATIC_VB_H



#include "../visual/i_resource.h"

// 静态顶点缓冲

class Render;

class CStaticVB: public IStaticVB
{
public:
	CStaticVB(Render* pRender);
	virtual ~CStaticVB();

	// 释放
	virtual void Release();

	// 创建
	bool Create(const void* pdata, unsigned int size);

	// 设置资源索引
	void SetIndex(int value) { m_nIndex = value; }

private:
	CStaticVB();
	CStaticVB(const CStaticVB&);
	CStaticVB& operator=(const CStaticVB&);
	
	// 销毁
	virtual void Destroy();
#ifdef PROCESS_LOST_DEVICE
	// 设备就绪时的处理
	virtual bool Restore();
	// 设备丢失时的处理
	virtual bool Invalidate();
#endif


private:
	Render* m_pRender;
	int m_nIndex;
};

#endif // _STATIC_VB_H
