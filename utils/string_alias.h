//--------------------------------------------------------------------
// 文件名:		string_alias.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年3月5日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _UTILS_STRING_ALIAS_H
#define _UTILS_STRING_ALIAS_H

#include "../public/macros.h"
#include "string_traits.h"

class TStringAliasAlloc
{
public:
	TStringAliasAlloc() {}
	~TStringAliasAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return NEW char[size]; }
	// 释放内存
	void Free(void* ptr, size_t size) { delete[] (char*)ptr; }
	// 交换
	void Swap(TStringAliasAlloc& src) {}
};

template<
	typename TYPE, typename DATA, 
	typename TRAITS = TStringTraits<TYPE>,
	typename DATA_TRAITS = TStringTraits<DATA>,
	typename ALLOC = TStringAliasAlloc>
class TStringAlias;

// TStringAliasNode

template<typename TYPE, typename DATA>
class TStringAliasNode
{
public:
	TStringAliasNode* pNext;
	size_t nHash;
	TYPE* sName;
	DATA* sData;
};

// TStringAliasIterator

template<typename TYPE, typename DATA, typename TRAITS, typename DATA_TRAITS, 
	typename ALLOC>
class TStringAliasIterator
{
private:
	typedef TStringAlias<TYPE, DATA, TRAITS, DATA_TRAITS, ALLOC> hash_type;
	typedef TStringAliasNode<TYPE, DATA> node_type;

public:
	TStringAliasIterator(const hash_type* self, node_type* node)
	{
		m_pSelf = self;
		m_pNode = node;
	}
	
	TStringAliasIterator& operator++()
	{
		node_type* next = m_pNode->pNext;
		
		if (next != NULL)
		{
			m_pNode = next;
		}
		else
		{
			m_pNode = m_pSelf->ToNext(m_pNode);
		}
		
		return *this;
	}
	
	const TStringAliasIterator operator++(int)
	{
		TStringAliasIterator temp(*this);
		
		++(*this);
		
		return temp;
	}
	
	bool operator==(const TStringAliasIterator& other) const
	{
		return (m_pNode == other.m_pNode);
	}
	
	bool operator!=(const TStringAliasIterator& other) const
	{
		return (m_pNode != other.m_pNode);
	}

	const TYPE* GetKey() const
	{
		return m_pNode->sName;
	}
	
	const DATA* GetData() const
	{
		return m_pNode->sData;
	}
	
	node_type* GetNode() const
	{
		return m_pNode;
	}
	
private:
	TStringAliasIterator();
	
private:
	node_type* m_pNode;
	const hash_type* m_pSelf;
};

// TStringAlias

template<typename TYPE, typename DATA, typename TRAITS, typename DATA_TRAITS, 
	typename ALLOC>
class TStringAlias
{
private:
	typedef TStringAlias<TYPE, DATA, TRAITS, DATA_TRAITS, ALLOC> hash_type;
	typedef TStringAliasNode<TYPE, DATA> node_type;
	
public:
	typedef TStringAliasIterator<TYPE, DATA, TRAITS, DATA_TRAITS, 
		ALLOC> iterator;
	typedef TStringAliasIterator<TYPE, DATA, TRAITS, DATA_TRAITS, 
		ALLOC> const_iterator;
	
public:
	explicit TStringAlias(size_t size = 0)
	{
		m_nSize = size;
		m_nCount = 0;
		
		if (size > 0)
		{
			m_pBuckets = (node_type**)m_Alloc.Alloc(
				sizeof(node_type*) * size);
			memset(m_pBuckets, 0, sizeof(node_type*) * size);
		}
		else
		{
			m_pBuckets = NULL;
		}
	}

	TStringAlias(const hash_type& src)
	{
		const size_t size = src.m_nSize;
		
		m_nCount = 0;
		m_nSize = size;

		if (size > 0)
		{
			m_pBuckets = (node_type**)m_Alloc.Alloc(
				sizeof(node_type*) * size);
			memset(m_pBuckets, 0, sizeof(node_type*) * size);

			for (size_t i = 0; i < size; ++i)
			{
				node_type* p = src.m_pBuckets[i];
				
				while (p)
				{
					Add(p->sName, p->sData);
					p = p->pNext;
				}
			}
		}
		else
		{
			m_pBuckets = NULL;
		}
	}

	~TStringAlias()
	{
		Clear();

		if (m_pBuckets)
		{
			m_Alloc.Free(m_pBuckets, sizeof(node_type*) * m_nSize);
		}
	}

	hash_type& operator=(const hash_type& src)
	{
		hash_type temp(src);

		Swap(temp);

		return *this;
	}

	void Swap(hash_type& src)
	{
		node_type** temp_buckets = src.m_pBuckets;
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

	void Clear()
	{
		for (size_t i = 0; i < m_nSize; ++i)
		{
			node_type* p = m_pBuckets[i];

			while (p)
			{
				node_type* t = p->pNext;
				DeleteNode(p);
				p = t;
			}
			
			m_pBuckets[i] = NULL;
		}

		m_nCount = 0;
	}

	size_t GetCount() const
	{
		return m_nCount;
	}

	bool Add(const TYPE* name, const DATA* data)
	{
		Assert(name != NULL);
		
		if (m_nCount == m_nSize)
		{
			Expand();
		}
		
		size_t hash = TRAITS::GetHash(name);
		size_t bucket = GetBucket(hash);
		node_type* p = NewNode(name, data);
		
		p->pNext = m_pBuckets[bucket];
		p->nHash = hash;
		
		m_pBuckets[bucket] = p;
		m_nCount++;
		
		return true;
	}
	
	bool Remove(const TYPE* name)
	{
		Assert(name != NULL);

		if (0 == m_nSize)
		{
			return false;
		}
		
		size_t hash = TRAITS::GetHash(name);
		size_t bucket = GetBucket(hash);
		node_type* p = m_pBuckets[bucket];

		while (p)
		{
			if ((p->nHash == hash) && TRAITS::Equal(p->sName, name))
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

	bool RemoveData(const TYPE* name, const DATA* data)
	{
		Assert(name != NULL);

		if (0 == m_nSize)
		{
			return false;
		}
		
		size_t hash = TRAITS::GetHash(name);
		size_t bucket = GetBucket(hash);
		node_type* p = m_pBuckets[bucket];

		while (p)
		{
			if ((p->nHash == hash) && TRAITS::Equal(p->sName, name) 
				&& DATA_TRAITS::Equal(p->sData == data))
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

	bool Exists(const TYPE* name) const
	{
		return (FindNode(name) != NULL);
	}

	const_iterator Find(const TYPE* name) const
	{
		return const_iterator(this, FindNode(name));
	}

	const DATA* GetData(const TYPE* name) const
	{
		node_type* p = FindNode(name);

		if (NULL == p)
		{
			return NULL;
		}

		return p->sData;
	}

	iterator Begin()
	{
		for (size_t i = 0; i < m_nSize; ++i)
		{
			if (m_pBuckets[i])
			{
				return iterator(this, m_pBuckets[i]);
			}
		}
		
		return End();
	}
	
	iterator End()
	{
		return iterator(this, NULL);
	}
	
	const_iterator Begin() const
	{
		for (size_t i = 0; i < m_nSize; ++i)
		{
			if (m_pBuckets[i])
			{
				return const_iterator(this, m_pBuckets[i]);
			}
		}
		
		return End();
	}
	
	const_iterator End() const
	{
		return const_iterator(this, NULL);
	}
	
	iterator Erase(iterator it)
	{
		iterator temp(it);
		
		++temp;
		
		node_type* p = it.GetNode();
		
		EraseNode(GetBucket(p->nHash), p);
		DeleteNode(p);
		m_nCount--;
		
		return temp;
	}
	
	size_t GetMemoryUsage() const
	{
		size_t size = sizeof(hash_type);
		
		for (size_t i = 0; i < m_nSize; ++i)
		{
			node_type* p = m_pBuckets[i];
			
			while (p)
			{
				size += sizeof(node_type) 
					+ TRAITS::Length(p->sName) * sizeof(TYPE)
					+ DATA_TRAITS::Length(p->sData) * sizeof(DATA);
				p = p->pNext;
			}
		}
		
		size += sizeof(node_type*) * m_nSize;
		
		return size;
	}
	
private:
	size_t GetBucket(size_t hash) const
	{
		return (hash % m_nSize);
	}

	node_type* NewNode(const TYPE* name, const DATA* data)
	{
		const size_t size1 = (TRAITS::Length(name) + 1) * sizeof(TYPE);
		const size_t size2 = (DATA_TRAITS::Length(data) + 1) * sizeof(DATA);
		node_type* p = (node_type*)m_Alloc.Alloc(
			sizeof(node_type) + size1 + size2);

		p->sName = (TYPE*)((char*)p + sizeof(node_type));
		p->sData = (DATA*)((char*)p->sName + size1);
		memcpy(p->sName, name, size1);
		memcpy(p->sData, data, size2);
		
		return p;
	}
	
	void DeleteNode(node_type* p)
	{
		const size_t size1 = (TRAITS::Length(p->sName) + 1) * sizeof(TYPE);
		const size_t size2 = (DATA_TRAITS::Length(p->sData) + 1) * sizeof(DATA);
			
		m_Alloc.Free(p, sizeof(node_type) + size1 + size2);
	}

	void EraseNode(size_t bucket, node_type* p)
	{
		Assert(bucket < m_nSize);
		Assert(p != NULL);
		
		node_type* t = m_pBuckets[bucket];
		
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

	node_type* FindNode(const TYPE* name) const
	{
		Assert(name != NULL);
		
		if (0 == m_nSize)
		{
			return NULL;
		}
		
		size_t hash = TRAITS::GetHash(name);
		size_t bucket = GetBucket(hash);
		node_type* p = m_pBuckets[bucket];
		
		while (p)
		{
			if ((p->nHash == hash) && TRAITS::Equal(p->sName, name))
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
		node_type** new_buckets = (node_type**)m_Alloc.Alloc(
			sizeof(node_type*) * new_size);
		
		memset(new_buckets, 0, sizeof(node_type*) * new_size); 
		
		for (size_t i = 0; i < m_nSize; ++i)
		{
			node_type* p = m_pBuckets[i];
			
			while (p)
			{
				node_type* t = p->pNext;
				size_t bucket = size_t(p->nHash) % new_size;
				
				p->pNext = new_buckets[bucket];
				new_buckets[bucket] = p;
				p = t;
			}
		}
		
		if (m_pBuckets)
		{
			m_Alloc.Free(m_pBuckets, sizeof(node_type*) * m_nSize);
		}
		
		m_pBuckets = new_buckets;
		m_nSize = new_size;
	}

	node_type* ToNext(node_type* node) const
	{
		for (size_t i = GetBucket(node->nHash) + 1; i < m_nSize; ++i)
		{
			if (m_pBuckets[i])
			{
				return m_pBuckets[i];
			}
		}
		
		return NULL;
	}
	
private:
	ALLOC m_Alloc;
	node_type** m_pBuckets;
	size_t m_nSize;
	size_t m_nCount;

	friend class TStringAliasIterator<TYPE, DATA, TRAITS, DATA_TRAITS, ALLOC>;
};

#endif // _UTILS_STRING_ALIAS_H
