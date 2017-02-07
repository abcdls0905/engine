//--------------------------------------------------------------------
// 文件名:		game_prop_set.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年2月17日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _GAME_PROP_SET_H
#define _GAME_PROP_SET_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../public/var.h"
#include "../utils/string_pod.h"
#include "../utils/array_pod.h"

// 游戏属性数据集合

class CGamePropSet
{
private:
	struct prop_data_t
	{
		core_string strName;
		TVar<4, TCoreAlloc> Value;
	};

public:
	CGamePropSet();
	~CGamePropSet();

	// 查找
	bool Find(const char* name) const;
	bool FindIndex(const char* name, size_t& index) const;
	// 添加
	bool Add(const char* name, const IVar& value);
	// 清空
	void Clear();
	// 获得数值
	IVar* Get(const char* name);
	// 获得数量
	size_t GetCount() const;
	// 获得属性名
	const char* GetName(size_t index) const;
	// 获得属性值
	const IVar& GetValue(size_t index) const;
	// 设置属性值
	bool SetValue(size_t index, const IVar& value);

private:
	TArrayPod<prop_data_t*, 1, TCoreAlloc> m_Props;
	TStringPod<char, size_t, TStringTraits<char>, TCoreAlloc> m_Indices;
};

#endif // _GAME_PROP_SET_H

