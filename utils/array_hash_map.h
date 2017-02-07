//--------------------------------------------------------------------
// 文件名:		array_hash_map.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年1月12日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _UTILS_ARRAY_HASH_MAP_H
#define _UTILS_ARRAY_HASH_MAP_H

#include "../public/macros.h"
#include "pod_traits.h"

class TArrayHashMapAlloc
{
public:
	TArrayHashMapAlloc() {}
	~TArrayHashMapAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return NEW char[size]; }
	// 释放内存
	void Free(void* ptr, size_t size) { delete[] (char*)ptr; }
	// 交换
	void Swap(TArrayHashMapAlloc& src) {}
};

template<
	typename TYPE, typename DATA, 
	typename TRAITS = TPodTraits<TYPE>,
	typename ALLOC = TArrayHashMapAlloc>
class TArrayHashMap;

// TArrayHashMapNode

template<typename TYPE, typename DATA>
class TArrayHashMapNode
{
public:
	TYPE Key;
	DATA Data;
	size_t nHash;
	int nFlag;
};

// TArrayHashMapIterator

template<typename TYPE, typename DATA, typename TRAITS, typename ALLOC>
class TArrayHashMapIterator
{
private:
	typedef TArrayHashMap<TYPE, DATA, TRAITS, ALLOC> hash_type;
	typedef TArrayHashMapNode<TYPE, DATA> node_type;

public:
	TArrayHashMapIterator()
	{
		m_pSelf = NULL;
		m_pNode = NULL;
	}
	
	TArrayHashMapIterator(const hash_type* self, node_type* node)
	{
		m_pSelf = self;
		m_pNode = node;
	}
	
	TArrayHashMapIterator& operator++()
	{
		m_pNode = m_pSelf->ToNext(m_pNode);
		return *this;
	}
	
	const TArrayHashMapIterator operator++(int)
	{
		TArrayHashMapIterator temp(*this);
		++(*this);
		return temp;
	}
	
	bool operator==(const TArrayHashMapIterator& other) const
	{
		return (m_pNode == other.m_pNode);
	}
	
	bool operator!=(const TArrayHashMapIterator& other) const
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

// TArrayHashMap

template<typename TYPE, typename DATA, typename TRAITS, typename ALLOC>
class TArrayHashMap
{
private:
	enum
	{
		FLAG_NOTUSE,
		FLAG_INUSE,
		FLAG_REMOVED,
	};
	
	typedef TArrayHashMap<TYPE, DATA, TRAITS, ALLOC> hash_type;
	typedef TArrayHashMapNode<TYPE, DATA> node_type;
	
public:
	typedef TArrayHashMapIterator<TYPE, DATA, TRAITS, ALLOC> iterator;
	typedef TArrayHashMapIterator<TYPE, DATA, TRAITS, ALLOC> const_iterator;
	
public:
	TArrayHashMap()
	{
		m_nSize = 0;
		m_nCount = 0;
		m_pBuckets = NULL;
	}

	TArrayHashMap(const hash_type& src)
	{
		const size_t size = src.m_nSize;
		
		m_nCount = 0;
		m_nSize = size;

		if (size > 0)
		{
			m_pBuckets = (node_type*)m_Alloc.Alloc(
				sizeof(node_type) * size);
			memset(m_pBuckets, 0, sizeof(node_type) * size);

			const node_type* buckets = src.m_pBuckets;

			for (size_t i = 0; i < size; ++i)
			{
				node_type* pNode = &buckets[i];
				
				if (pNode->nFlag == FLAG_INUSE)
				{
					Add(pNode->Key, pNode->Data);
				}
			}
		}
		else
		{
			m_pBuckets = NULL;
		}
	}

	~TArrayHashMap()
	{
		if (m_pBuckets)
		{
			m_Alloc.Free(m_pBuckets, sizeof(node_type) * m_nSize);
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
		if (m_pBuckets)
		{
			memset(m_pBuckets, 0, sizeof(node_type) * m_nSize);
		}

		m_nCount = 0;
	}

	size_t GetCount() const
	{
		return m_nCount;
	}

	bool Add(const TYPE& key, const DATA& data)
	{
		if ((m_nCount * 2) == m_nSize)
		{
			Expand();
		}
		
		size_t hash = TRAITS::GetHash(key);
		size_t size_1 = m_nSize - 1;
			
		for (size_t i = 0; i < m_nSize; ++i)
		{
			size_t bucket = (hash + i) & size_1;
			node_type* pNode = &m_pBuckets[bucket];
			
			if (pNode->nFlag != FLAG_INUSE)
			{
				pNode->Key = key;
				pNode->Data = data;
				pNode->nHash = hash;
				pNode->nFlag = FLAG_INUSE;
				m_nCount++;
				return true;
			}
		}
		
		Assert(0);
		
		return false;
	}
	
	bool Remove(const TYPE& key)
	{
		if (0 == m_nSize)
		{
			return false;
		}
		
		size_t hash = TRAITS::GetHash(key);
		size_t size_1 = m_nSize - 1;
			
		for (size_t i = 0; i < m_nSize; ++i)
		{
			size_t bucket = (hash + i) & size_1;
			node_type* pNode = &m_pBuckets[bucket];
			
			if (pNode->nFlag == FLAG_NOTUSE)
			{
				return false;
			}

			if ((pNode->nHash == hash) && TRAITS::Equal(pNode->Key, key))
			{
				pNode->nFlag = FLAG_REMOVED;
				m_nCount--;
				return true;
			}
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
		size_t size_1 = m_nSize - 1;
			
		for (size_t i = 0; i < m_nSize; ++i)
		{
			size_t bucket = (hash + i) & size_1;
			node_type* pNode = &m_pBuckets[bucket];
			
			if (pNode->nFlag == FLAG_NOTUSE)
			{
				return false;
			}

			if ((pNode->nHash == hash) && TRAITS::Equal(pNode->Key, key)
				&& (pNode->Data == data))
			{
				pNode->nFlag = FLAG_REMOVED;
				m_nCount--;
				return true;
			}
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
		node_type* pNode = m_pBuckets;
		node_type* pEndNode = pNode + m_nSize;
		
		for (; pNode < pEndNode; ++pNode)
		{
			if (pNode->nFlag == FLAG_INUSE)
			{
				return iterator(this, pNode);
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
		node_type* pNode = m_pBuckets;
		node_type* pEndNode = pNode + m_nSize;
		
		for (; pNode < pEndNode; ++pNode)
		{
			if (pNode->nFlag == FLAG_INUSE)
			{
				return const_iterator(this, pNode);
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
		it.GetNode()->nFlag = FLAG_REMOVED;
		m_nCount--;
		
		return temp;
	}
	
private:
	node_type* FindNode(const TYPE& key) const
	{
		if (0 == m_nSize)
		{
			return NULL;
		}
		
		size_t hash = TRAITS::GetHash(key);
		size_t size_1 = m_nSize - 1;
			
		for (size_t i = 0; i < m_nSize; ++i)
		{
			size_t bucket = (hash + i) & size_1;
			node_type* pNode = &m_pBuckets[bucket];

			if (pNode->nFlag == FLAG_NOTUSE)
			{
				return NULL;
			}

			if ((pNode->nHash == hash) && TRAITS::Equal(pNode->Key, key))
			{
				return pNode;
			}
		}
		
		return NULL;
	}
	
	void Expand()
	{
		size_t new_size;
		
		if (0 == m_nSize)
		{
			new_size = 32;
		}
		else
		{
			new_size = m_nSize * 2;
		}
		
		size_t new_size_1 = new_size - 1;
		node_type* new_buckets = (node_type*)m_Alloc.Alloc(
			sizeof(node_type) * new_size);
		
		memset(new_buckets, 0, sizeof(node_type) * new_size); 
		
		for (size_t i = 0; i < m_nSize; ++i)
		{
			node_type* p = &m_pBuckets[i];
			
			if (p->nFlag == FLAG_INUSE)
			{
				for (size_t k = 0; k < new_size; ++k)
				{
					size_t bucket = (p->nHash + k) & new_size_1;
					
					if (new_buckets[bucket].nFlag != FLAG_INUSE)
					{
						new_buckets[bucket] = *p;
						break;
					}
				}
			}
		}
		
		if (m_pBuckets)
		{
			m_Alloc.Free(m_pBuckets, sizeof(node_type) * m_nSize);
		}
		
		m_pBuckets = new_buckets;
		m_nSize = new_size;
	}

	node_type* ToNext(node_type* pNode) const
	{
		node_type* pEndNode = m_pBuckets + m_nSize;
		
		while (++pNode < pEndNode)
		{
			if (pNode->nFlag == FLAG_INUSE)
			{
				return pNode;
			}
		}
		
		return NULL;
	}
	
private:
	ALLOC m_Alloc;
	node_type* m_pBuckets;
	size_t m_nSize;
	size_t m_nCount;

	friend class TArrayHashMapIterator<TYPE, DATA, TRAITS, ALLOC>;
};

#endif // _UTILS_ARRAY_HASH_MAP_H
