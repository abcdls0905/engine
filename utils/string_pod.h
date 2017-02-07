//--------------------------------------------------------------------
// 文件名:		string_pod.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年2月17日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _UTILS_STRING_POD_H
#define _UTILS_STRING_POD_H

#include "../public/macros.h"
#include "string_traits.h"

class TStringPodAlloc
{
public:
	TStringPodAlloc() {}
	~TStringPodAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return NEW char[size]; }
	// 释放内存
	void Free(void* ptr, size_t size) { delete[] (char*)ptr; }
	// 交换
	void Swap(TStringPodAlloc& src) {}
};

template<
	typename TYPE, typename DATA, 
	typename TRAITS = TStringTraits<TYPE>,
	typename ALLOC = TStringPodAlloc>
class TStringPod;

// TStringPodNode

template<typename TYPE, typename DATA>
class TStringPodNode
{
public:
	TStringPodNode* pNext;
	size_t nHash;
	DATA Data;
	TYPE sName[1];
};

// TStringPodIterator

template<typename TYPE, typename DATA, typename TRAITS, typename ALLOC>
class TStringPodIterator
{
private:
	typedef TStringPod<TYPE, DATA, TRAITS, ALLOC> hash_type;
	typedef TStringPodNode<TYPE, DATA> node_type;

public:
	TStringPodIterator(const hash_type* self, node_type* node)
	{
		m_pSelf = self;
		m_pNode = node;
	}
	
	TStringPodIterator& operator++()
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
	
	const TStringPodIterator operator++(int)
	{
		TStringPodIterator temp(*this);
		++(*this);
		return temp;
	}
	
	bool operator==(const TStringPodIterator& other) const
	{
		return (m_pNode == other.m_pNode);
	}
	
	bool operator!=(const TStringPodIterator& other) const
	{
		return (m_pNode != other.m_pNode);
	}

	const TYPE* GetKey() const
	{
		return m_pNode->sName;
	}
	
	DATA& GetData() const
	{
		return m_pNode->Data;
	}
	
	node_type* GetNode() const
	{
		return m_pNode;
	}
	
private:
	TStringPodIterator();
	
private:
	node_type* m_pNode;
	const hash_type* m_pSelf;
};

// TStringPod

template<typename TYPE, typename DATA, typename TRAITS, typename ALLOC>
class TStringPod
{
private:
	typedef TStringPod<TYPE, DATA, TRAITS, ALLOC> hash_type;
	typedef TStringPodNode<TYPE, DATA> node_type;
	
public:
	typedef TStringPodIterator<TYPE, DATA, TRAITS, ALLOC> iterator;
	typedef TStringPodIterator<TYPE, DATA, TRAITS, ALLOC> const_iterator;
	
public:
	explicit TStringPod(size_t size = 0)
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

	TStringPod(const hash_type& src)
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
					Add(p->sName, p->Data);
					p = p->pNext;
				}
			}
		}
		else
		{
			m_pBuckets = NULL;
		}
	}

	~TStringPod()
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

	bool Set(const TYPE* name, const DATA& data)
	{
		node_type* pNode = FindNode(name);

		if (NULL == pNode)
		{
			return Add(name, data);
		}

		pNode->Data = data;

		return true;
	}


	bool Add(const TYPE* name, const DATA& data)
	{
		Assert(name != NULL);
		
		if (m_nCount == m_nSize)
		{
			Expand();
		}
		
		size_t hash = TRAITS::GetHash(name);
		size_t bucket = GetBucket(hash);
		node_type* p = NewNode(name);
		
		p->pNext = m_pBuckets[bucket];
		p->nHash = hash;
		p->Data = data;
		
		m_pBuckets[bucket] = p;
		m_nCount++;
		
		return true;
	}

	bool Add(const TYPE* name, const DATA& data, TYPE*& pname)
	{
		Assert(name != NULL);

		if (m_nCount == m_nSize)
		{
			Expand();
		}

		size_t hash = TRAITS::GetHash(name);
		size_t bucket = GetBucket(hash);
		node_type* p = NewNode(name);

		p->pNext = m_pBuckets[bucket];
		p->nHash = hash;
		p->Data = data;

		m_pBuckets[bucket] = p;
		m_nCount++;

		pname = p->sName;

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

	bool RemoveData(const TYPE* name, const DATA& data)
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
				&& (p->Data == data))
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
	
	iterator Find(const TYPE* name)
	{
		return iterator(this, FindNode(name));
	}

	bool GetData(const TYPE* name, DATA& data) const
	{
		node_type* p = FindNode(name);

		if (NULL == p)
		{
			return false;
		}

		data = p->Data;

		return true;
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
				size += sizeof(node_type) + TRAITS::Length(p->sName) * sizeof(TYPE);
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

	node_type* NewNode(const TYPE* name)
	{
		const size_t len = TRAITS::Length(name);
		const size_t size = sizeof(node_type) + len * sizeof(TYPE);
		node_type* p = (node_type*)m_Alloc.Alloc(size);

		TRAITS::Copy(p->sName, name, len + 1);
		
		return p;
	}
	
	void DeleteNode(node_type* p)
	{
		m_Alloc.Free(p, 
			sizeof(node_type) + TRAITS::Length(p->sName) * sizeof(TYPE));
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

	friend class TStringPodIterator<TYPE, DATA, TRAITS, ALLOC>;
};

#endif // _UTILS_STRING_POD_H
