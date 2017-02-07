//--------------------------------------------------------------------
// 文件名:		game_record_set.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年2月17日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _GAME_RECORD_SET_H
#define _GAME_RECORD_SET_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../utils/string_pod.h"
#include "../utils/array_pod.h"

// 游戏表格数据集合

class CGameRecord;

class CGameRecordSet
{
public:
	CGameRecordSet();
	~CGameRecordSet();

	// 查找
	bool Find(const char* name) const;
	bool FindIndex(const char* name, size_t& index) const;
	// 添加
	bool Add(CGameRecord* record);
	// 清空
	void Clear();
	// 获得
	CGameRecord* Get(const char* name);
	// 获得数量
	size_t GetCount() const;
	// 获得表格
	CGameRecord* GetRecord(size_t index) const;

private:
	void ReleaseAll();

private:
	TArrayPod<CGameRecord*, 1, TCoreAlloc> m_Records;
	TStringPod<char, size_t, TStringTraits<char>, TCoreAlloc> m_Indices;
};

#endif // _GAME_RECORD_SET_H

