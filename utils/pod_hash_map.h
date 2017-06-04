
#ifndef _UTILS_POD_HASH_MAP_H
#define _UTILS_POD_HASH_MAP_H

#include "../public/macros.h"
#include "pod_traits.h"

class TPodHashMapAlloc
{
public:
	TPodHashMapAlloc() {}
	~TPodHashMapAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return NEW char[size]; }
	// 释放内存
	void Free(void* ptr, size_t size) { delete[] (char*)ptr; }
	// 交换
	void Swap(TPodHashMapAlloc& src) {}
};

template<
	typename TYPE, typename DATA, 
	typename TRAITS = TPodTraits<TYPE>,
	typename ALLOC = TPodHashMapAlloc>
class TPodHashMap;

// TPodHashMapNode

template<typename TYPE, typename DATA>
class TPodHashMapNode
{
public:
	TPodHashMapNode* pNext;
	size_t nHash;
	TYPE Key;
	DATA Data;
};

// TPodHashMapIterator

template<typename TYPE, typename DATA, typename TRAITS, typename ALLOC>
class TPodHashMapIterator
{
private:
	typedef TPodHashMap<TYPE, DATA, TRAITS, ALLOC> hash_type;
	typedef TPodHashMapNode<TYPE, DATA> node_type;

public:
	TPodHashMapIterator()
	{
		m_pSelf = NULL;
		m_pNode = NULL;
	}
	
	TPodHashMapIterator(const hash_type* self, node_type* node)
	{
		m_pSelf = self;
		m_pNode = node;
	}
	
	TPodHashMapIterator& operator++()
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
	
	const TPodHashMapIterator operator++(int)
	{
		TPodHashMapIterator temp(*this);
		++(*this);
		return temp;
	}
	
	bool operator==(const TPodHashMapIterator& other) const
	{
		return (m_pNode == other.m_pNode);
	}
	
	bool operator!=(const TPodHashMapIterator& other) const
	{
		return (m_pNode != other.m_pNode);
	}

	TYPE& GetKey() const
	{
		return m_pNode->Key;
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
	node_type* m_pNode;
	const hash_type* m_pSelf;
};

// TPodHashMap

template<typename TYPE, typename DATA, typename TRAITS, typename ALLOC>
class TPodHashMap
{
private:
	typedef TPodHashMap<TYPE, DATA, TRAITS, ALLOC> hash_type;
	typedef TPodHashMapNode<TYPE, DATA> node_type;
	
public:
	typedef TPodHashMapIterator<TYPE, DATA, TRAITS, ALLOC> iterator;
	typedef TPodHashMapIterator<TYPE, DATA, TRAITS, ALLOC> const_iterator;
	
public:
	explicit TPodHashMap(size_t size = 0)
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

	TPodHashMap(const hash_type& src)
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
					Add(p->Key, p->Data);
					p = p->pNext;
				}
			}
		}
		else
		{
			m_pBuckets = NULL;
		}
	}

	~TPodHashMap()
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

	bool Set(const TYPE& key, const DATA& data)
	{
		node_type* pNode = FindNode(key);
		
		if (NULL == pNode)
		{
			return Add(key, data);
		}
		
		pNode->Data = data;
		
		return true;
	}
	
	bool Add(const TYPE& key, const DATA& data)
	{
		if (m_nCount == m_nSize)
		{
			Expand();
		}
		
		size_t hash = TRAITS::GetHash(key);
		size_t bucket = GetBucket(hash);
		node_type* p = NewNode(key);
		
		p->pNext = m_pBuckets[bucket];
		p->nHash = hash;
		p->Data = data;
		
		m_pBuckets[bucket] = p;
		m_nCount++;
		
		return true;
	}
	
	bool Remove(const TYPE& key)
	{
		if (0 == m_nSize)
		{
			return false;
		}
		
		size_t hash = TRAITS::GetHash(key);
		size_t bucket = GetBucket(hash);
		node_type* p = m_pBuckets[bucket];

		while (p)
		{
			if ((p->nHash == hash) && TRAITS::Equal(p->Key, key))
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

	bool RemoveData(const TYPE& key, const DATA& data)
	{
		if (0 == m_nSize)
		{
			return false;
		}
		
		size_t hash = TRAITS::GetHash(key);
		size_t bucket = GetBucket(hash);
		node_type* p = m_pBuckets[bucket];

		while (p)
		{
			if ((p->nHash == hash) && TRAITS::Equal(p->Key, key) 
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

	bool Exists(const TYPE& key) const
	{
		return (FindNode(key) != NULL);
	}

	const_iterator Find(const TYPE& key) const
	{
		return const_iterator(this, FindNode(key));
	}

	bool GetData(const TYPE& key, DATA& data) const
	{
		node_type* p = FindNode(key);

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
				size += sizeof(node_type);
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

	node_type* NewNode(const TYPE& key)
	{
		node_type* p = (node_type*)m_Alloc.Alloc(sizeof(node_type));

		p->Key = key;
		
		return p;
	}
	
	void DeleteNode(node_type* p)
	{
		m_Alloc.Free(p, sizeof(node_type));
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

	node_type* FindNode(const TYPE& key) const
	{
		if (0 == m_nSize)
		{
			return NULL;
		}
		
		size_t hash = TRAITS::GetHash(key);
		size_t bucket = GetBucket(hash);
		node_type* p = m_pBuckets[bucket];
		
		while (p)
		{
			if ((p->nHash == hash) && TRAITS::Equal(p->Key, key))
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

	friend class TPodHashMapIterator<TYPE, DATA, TRAITS, ALLOC>;
};

#endif // _UTILS_PODHASH_MAP_H
