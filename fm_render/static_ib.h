//--------------------------------------------------------------------
// 文件名:		static_ib.h
// 内  容:		
// 说  明:		
// 创建日期:	2009年6月25日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _STATIC_IB_H
#define _STATIC_IB_H



#include "../visual/i_resource.h"

// 静态索引缓冲

class Render;

class CStaticIB: public IStaticIB
{
public:
	CStaticIB(Render* pRender);
	virtual ~CStaticIB();

	// 释放
	virtual void Release();

	// 创建
	bool Create(int format, const void* pdata, unsigned int size);

	// 设置资源索引
	void SetIndex(int value) { m_nIndex = value; }
	
private:
	CStaticIB();
	CStaticIB(const CStaticIB&);
	CStaticIB& operator=(const CStaticIB&);
	
	// 销毁
	virtual void Destroy();

	// 设备就绪时的处理
#ifdef PROCESS_LOST_DEVICE
	virtual bool Restore();
	// 设备丢失时的处理
	virtual bool Invalidate();
#endif

private:
	Render* m_pRender;
	int m_nIndex;
};

#endif // _DX_STATIC_IB_H
