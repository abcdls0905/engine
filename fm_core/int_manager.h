//--------------------------------------------------------------------
// 文件名:		int_manager.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年2月8日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _INT_MANAGER_H
#define _INT_MANAGER_H

#include "../public/macros.h"
#include "../utils/array_pod.h"

// 功能接口管理

class IInterface;
class IIntCreator;
class CCore;

class CIntManager
{
private:
	struct int_creator_t
	{
		IIntCreator* pCreator;
		unsigned int nHash;
		IInterface* pInterface;
		int nRefCount;
		int64_t nOrder;
	};

	// 排序比较函数
	static int SortCompare(const void* elem1, const void* elem2);
	
public:
	explicit CIntManager(CCore* pCore);
	~CIntManager();

	// 释放所有动能接口
	void ReleaseAll();

	// 添加创建器
	bool AddCreator(IIntCreator* pCreator);

	// 查找
	bool Find(const char* name) const;
	bool FindIndex(const char* name, size_t& index) const;
	// 获得数量
	size_t GetCount() const;
	// 获得名称
	const char* GetName(size_t index) const;
	// 获得创建器
	IIntCreator* GetCreator(size_t index) const;
	// 获得接口（可能为NULL）
	IInterface* GetInterface(size_t index) const;
	// 获得接口的引用数
	int GetRefCount(size_t index) const;
	
	// 创建接口或增加接口的引用
	IInterface* GetReference(const char* name);
	// 释放接口
	void ReleaseReference(IInterface* pInterface);

	// 调用需要每帧运行的接口函数
	void ExecuteFrameBegin();
	void ExecuteFrameEnd();

	// 导出所有功能接口信息到文件
	bool Dump(const char* file_name);
	
private:
	CIntManager();
	CIntManager(const CIntManager&);
	CIntManager& operator=(const CIntManager&);

	bool AddFrameExecute(IInterface* pInterface);
	bool RemoveFrameExecute(IInterface* pInterface);

private:
	CCore* m_pCore;
	TArrayPod<int_creator_t, 1> m_IntCreators;
	TArrayPod<IInterface*, 1> m_FrameExecute;
	int64_t m_nOrderCount;
};

#endif // _INT_MANAGER_H
