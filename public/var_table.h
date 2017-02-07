//--------------------------------------------------------------------
// 文件名:		var_table.h
// 内  容:		
// 说  明:		
// 创建日期:	2012年2月22日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_VAR_TABLE_H
#define _PUBLIC_VAR_TABLE_H

#include <new>
#include "macros.h"
#include "inlines.h"
#include "i_var_table.h"
#include "var.h"

// 可变类型命名参数表

class TVarTableAlloc
{
public:
	TVarTableAlloc() {}
	~TVarTableAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return NEW char[size]; }
	// 释放内存
	void Free(void* ptr, size_t size) { delete[] (char*)ptr; }
	// 交换
	void Swap(TVarTableAlloc& src) {}
};

template<typename ALLOC = TVarTableAlloc, size_t VAR_BUFFER_SIZE = 4>
class TVarTable: public IVarTable
{
private:
	typedef TVarTable<ALLOC, VAR_BUFFER_SIZE> self_type;
	typedef TVar<VAR_BUFFER_SIZE, ALLOC> var_type;
	
	struct var_node_t
	{
		var_node_t* pNext;
		size_t nHash;
		var_type Value;
		char sKey[1];
	};

public:
	explicit TVarTable(size_t size = 0)
	{
		m_nSize = size;
		m_nCount = 0;
		
		if (size > 0)
		{
			m_pBuckets = (var_node_t**)m_Alloc.Alloc(
				sizeof(var_node_t*) * size);
			memset(m_pBuckets, 0, sizeof(var_node_t*) * size);
		}
		else
		{
			m_pBuckets = NULL;
		}
	}

	TVarTable(const self_type& src)
	{
		const size_t size = src.m_nSize;
		
		m_nCount = 0;
		m_nSize = size;

		if (size > 0)
		{
			m_pBuckets = (var_node_t**)m_Alloc.Alloc(
				sizeof(var_node_t*) * size);
			memset(m_pBuckets, 0, sizeof(var_node_t*) * size);

			for (size_t i = 0; i < size; ++i)
			{
				var_node_t* p = src.m_pBuckets[i];
				
				while (p)
				{
					Add(p->sKey, p->Value);
					p = p->pNext;
				}
			}
		}
		else
		{
			m_pBuckets = NULL;
		}
	}

	virtual ~TVarTable()
	{
		Clear();

		if (m_pBuckets)
		{
			m_Alloc.Free(m_pBuckets, sizeof(var_node_t*) * m_nSize);
		}
	}

	self_type& operator=(const self_type& src)
	{
		self_type temp(src);
		Swap(temp);
		return *this;
	}

	void Swap(self_type& src)
	{
		var_node_t** temp_buckets = src.m_pBuckets;
		size_t temp_size = src.m_nSize;
		size_t temp_count = src.m_nCount;
		
		src.m_pBuckets = m_pBuckets;
		src.m_nSize = m_nSize;
		src.m_nCount = m_nCount;
		m_pBuckets = temp_buckets;
		m_nSize = temp_size;
		m_nCount = temp_count;
		m_Alloc.Swap(src.m_Alloc);
	}

	// 获得表项数量
	virtual size_t GetCount() const
	{
		return m_nCount;
	}

	// 获得键名列表
	virtual void GetKeyList(IVarList& result) const
	{
		result.Clear();
		
		for (size_t i = 0; i < m_nSize; ++i)
		{
			var_node_t* p = m_pBuckets[i];

			while (p)
			{
				result.AddString(p->sKey);
				p = p->pNext;
			}
		}
	}

	// 获得键值
	virtual IVar* GetValue(const char* key) const
	{
		var_node_t* pNode = FindNode(key);
		
		if (NULL == pNode)
		{
			return NULL;
		}
		
		return &pNode->Value;
	}
	
	// 键值是否存在
	virtual bool Exists(const char* key) const
	{
		return FindNode(key) != NULL;
	}
	
	// 添加键
	virtual IVar* Add(const char* key, const IVar& value)
	{
		Assert(key != NULL);
		
		if (m_nCount == m_nSize)
		{
			Expand();
		}
		
		size_t hash = GetHashValueCase(key);
		size_t bucket = GetBucket(hash);
		var_node_t* p = NewNode(key, value);
		
		p->pNext = m_pBuckets[bucket];
		p->nHash = hash;
		m_pBuckets[bucket] = p;
		m_nCount++;
		
		return &p->Value;
	}

	// 删除键
	virtual bool Remove(const char* key)
	{
		Assert(key != NULL);

		if (0 == m_nSize)
		{
			return false;
		}
		
		size_t hash = GetHashValueCase(key);
		size_t bucket = GetBucket(hash);
		var_node_t* p = m_pBuckets[bucket];

		while (p)
		{
			if ((p->nHash == hash) && (strcmp(p->sKey, key) == 0))
			{
				EraseNode(bucket, p);
				DeleteNode(p);
				m_nCount--;
				return true;
			}

			p = p->pNext;
		}
		
		return false;
	}

	// 清空
	virtual void Clear()
	{
		for (size_t i = 0; i < m_nSize; ++i)
		{
			var_node_t* p = m_pBuckets[i];

			while (p)
			{
				var_node_t* t = p->pNext;
				DeleteNode(p);
				p = t;
			}
			
			m_pBuckets[i] = NULL;
		}

		m_nCount = 0;
	}

	// 设置键值（不存在则添加）
	virtual void Assign(const char* key, const IVar& value)
	{
		var_node_t* pNode = FindNode(key);

		if (NULL == pNode)
		{
			Add(key, value);
		}
		else
		{
			pNode->Value.Assign(value);
		}
	}
	
private:
	size_t GetBucket(size_t hash) const
	{
		return (hash % m_nSize);
	}

	var_node_t* NewNode(const char* key, const IVar& value)
	{
		const size_t len = strlen(key);
		const size_t size = sizeof(var_node_t) + len;
		var_node_t* p = (var_node_t*)m_Alloc.Alloc(size);

		memcpy(p->sKey, key, len + 1);
		new (&p->Value) var_type(value);
		
		return p;
	}
	
	void DeleteNode(var_node_t* p)
	{
		p->Value.~var_type();
		m_Alloc.Free(p, sizeof(var_node_t) + strlen(p->sKey));
	}

	void EraseNode(size_t bucket, var_node_t* p)
	{
		Assert(bucket < m_nSize);
		Assert(p != NULL);
		
		var_node_t* t = m_pBuckets[bucket];
		
		if (t == p)
		{
			m_pBuckets[bucket] = p->pNext;
			return;
		}
		
		while (t)
		{
			if (t->pNext == p)
			{
				t->pNext = p->pNext;
				break;
			}

			t = t->pNext;
		}
	}

	var_node_t* FindNode(const char* key) const
	{
		Assert(key != NULL);
		
		if (0 == m_nSize)
		{
			return NULL;
		}
		
		size_t hash = GetHashValueCase(key);
		size_t bucket = GetBucket(hash);
		var_node_t* p = m_pBuckets[bucket];
		
		while (p)
		{
			if ((p->nHash == hash) && (strcmp(p->sKey, key) == 0))
			{
				return p;
			}
			
			p = p->pNext;
		}
		
		return NULL;
	}
	
	void Expand()
	{
		size_t new_size = m_nSize * 2 + 1;
		var_node_t** new_buckets = (var_node_t**)m_Alloc.Alloc(
			sizeof(var_node_t*) * new_size);
		
		memset(new_buckets, 0, sizeof(var_node_t*) * new_size); 
		
		for (size_t i = 0; i < m_nSize; ++i)
		{
			var_node_t* p = m_pBuckets[i];
			
			while (p)
			{
				var_node_t* t = p->pNext;
				size_t bucket = size_t(p->nHash) % new_size;
				
				p->pNext = new_buckets[bucket];
				new_buckets[bucket] = p;
				p = t;
			}
		}
		
		if (m_pBuckets)
		{
			m_Alloc.Free(m_pBuckets, sizeof(var_node_t*) * m_nSize);
		}
		
		m_pBuckets = new_buckets;
		m_nSize = new_size;
	}

private:
	ALLOC m_Alloc;
	var_node_t** m_pBuckets;
	size_t m_nSize;
	size_t m_nCount;
};

#endif // _PUBLIC_VAR_TABLE_H
