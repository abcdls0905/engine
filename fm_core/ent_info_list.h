//--------------------------------------------------------------------
// 文件名:		ent_info_list.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月6日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _ENT_INFO_LIST_H
#define _ENT_INFO_LIST_H

#include "../public/macros.h"
#include "../utils/array_pod.h"

// 实体信息集合

class CEntInfo;

class CEntInfoList
{
public:
	CEntInfoList();
	~CEntInfoList();

	// 添加
	bool Add(CEntInfo* pEntInfo);
	// 获得数量
	size_t GetCount() const;
	// 获得指定位置内容
	CEntInfo* GetByIndex(size_t index) const;
	// 查找
	CEntInfo* Get(const char* name) const;

private:
	CEntInfoList(const CEntInfoList&);
	CEntInfoList& operator=(const CEntInfoList&);

	void Expand();
	
private:
	TArrayPod<CEntInfo*, 8> m_EntInfos;
	CEntInfo** m_pBuckets;
	size_t m_nSize;
};

#endif // _ENT_INFO_LIST_H
