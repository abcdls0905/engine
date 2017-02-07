//--------------------------------------------------------------------
// 文件名:		shader_list.h
// 内  容:		
// 说  明:		
// 创建日期:	2012年9月29日
// 创建人:		
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef	_VISUAL_SHADER_LIST_H
#define	_VISUAL_SHADER_LIST_H

#include "../public/macros.h"
#include "../public/fast_str.h"
#include "../public/core_mem.h"
#include "../visual/i_resource.h"

class IRender;

IVertexShader* render_load_vs(IRender* pRender, const char* file_name,
	const char* func_name, const char* defines);
IPixelShader* render_load_ps(IRender* pRender, const char* file_name,
	const char* func_name, const char* defines);

class TShaderAlloc
{
public:
	TShaderAlloc() {}
	~TShaderAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
	void Swap(TShaderAlloc& src) {}
};

// 方便着色器管理的辅助类

template<typename SHADER_TYPE>
struct TShaderTraits
{
};

template<>
struct TShaderTraits<IVertexShader>
{
	static IVertexShader* LoadShader(IRender* pRender, 
		const char* file_name, const char* func_name, const char* defines)
	{
		return render_load_vs(pRender, file_name, func_name, defines);
	}
};

template<>
struct TShaderTraits<IPixelShader>
{
	static IPixelShader* LoadShader(IRender* pRender, 
		const char* file_name, const char* func_name, const char* defines)
	{
		return render_load_ps(pRender, file_name, func_name, defines);
	}
};

// 管理具有几个宏定义的着色器

template<typename SHADER_TYPE, size_t DEFINE_NUM, 
	typename TRAITS = TShaderTraits<SHADER_TYPE> >
class TShaderList
{
private:
	enum { SHADER_NUM = 0x1 << DEFINE_NUM };

public:
	TShaderList()
	{
		m_pRender = NULL;
		m_strFileName = NULL;
		m_strFuncName = NULL;
		memset(m_strDefines, 0, sizeof(m_strDefines));

		for (size_t i = 0; i < SHADER_NUM; ++i)
		{
			m_nShaderList[i] = NULL;
		}
	}

    ~TShaderList()
	{
	}

	// 设置管理器
	void SetRender(IRender* value)
	{
		m_pRender = value;
	}

	// 设置着色器名称
	void SetShaderName(const char* file_name, const char* func_name)
	{
		m_strFileName = file_name;
		m_strFuncName = func_name;
	}

	// 设置宏定义
	void SetDefine(size_t slot, const char* value)
	{
		Assert(slot < DEFINE_NUM);

		m_strDefines[slot] = value;
	}

	// 获得指定的着色器索引值
	SHADER_TYPE* GetShader(size_t index)
	{
		Assert(index < SHADER_NUM);

		if (NULL == m_nShaderList[index])
		{
			return LoadShader(index, "");
		}

		return m_nShaderList[index];
	}

	// 获得指定的着色器索引值
	SHADER_TYPE* GetShaderWithPrefix(size_t index, const char* prefix)
	{
		Assert(index < SHADER_NUM);
		Assert(prefix != NULL);

		if (NULL == m_nShaderList[index])
		{
			return LoadShader(index, prefix);
		}

		return m_nShaderList[index];
	}

	// 加载指定的着色器
	SHADER_TYPE* LoadShader(size_t index, const char* prefix = "")
	{
		Assert(index < SHADER_NUM);

		TFastStr<char, 256, TCharTraits<char>, TCoreAlloc> def(prefix);
		size_t count = 0;
		size_t val = index;

		while (val > 0)
		{
			if ((val & 0x1) != 0)
			{
				def += m_strDefines[count];
			}

			val = val >> 1;
			++count;
		}

		SHADER_TYPE* pShader = TRAITS::LoadShader(m_pRender, m_strFileName, 
			m_strFuncName, def.c_str());

		m_nShaderList[index] = pShader;

		return pShader;
	}

	// 加载着色器
	bool LoadAllShader()
	{
		for (size_t i = 0; i < SHADER_NUM; ++i)
		{
			TFastStr<char, 256, TCharTraits<char>, TCoreAlloc> def;
			size_t count = 0;
			size_t val = i;

			while (val > 0)
			{
				if ((val & 0x1) != 0)
				{
					def += m_strDefines[count];
				}

				val = val >> 1;
				++count;
			}

			SHADER_TYPE* pShader = TRAITS::LoadShader(m_pRender, 
				m_strFileName, m_strFuncName, def.c_str());

			if (NULL == pShader)
			{
				return false;
			}

			m_nShaderList[i] = pShader;
		}

		return true;
	}

private:
	IRender* m_pRender;
	const char* m_strFileName;
	const char* m_strFuncName;
	const char* m_strDefines[DEFINE_NUM];
	SHADER_TYPE* m_nShaderList[SHADER_NUM];
};

// 管理一个多项选择和几个宏定义的着色器

template<typename SHADER_TYPE, size_t OPTION_NUM, size_t DEFINE_NUM, 
	typename TRAITS = TShaderTraits<SHADER_TYPE> >
class TShaderOption
{
private:
	enum { DEFINE_SIZE = 0x1 << DEFINE_NUM };
	enum { SHADER_NUM = OPTION_NUM * DEFINE_SIZE };

public:
	TShaderOption()
	{
		m_pRender = NULL;
		m_strFileName = NULL;
		m_strFuncName = NULL;
		memset(m_strOption, 0, sizeof(m_strOption));
		memset(m_strDefines, 0, sizeof(m_strDefines));
		
		for (size_t i = 0; i < SHADER_NUM; ++i)
		{
			m_nShaderList[i] = NULL;
		}
	}

	~TShaderOption()
	{
	}

	// 设置管理器
	void SetRender(IRender* value)
	{
		m_pRender = value;
	}

	// 设置着色器名称
	void SetShaderName(const char* file_name, const char* func_name)
	{
		m_strFileName = file_name;
		m_strFuncName = func_name;
	}

	// 设置选项
	void SetOption(size_t option_index, const char* value)
	{
		Assert(option_index < OPTION_NUM);

		m_strOption[option_index] = value;
	}

	// 设置宏定义
	void SetDefine(size_t define_index, const char* value)
	{
		Assert(define_index < DEFINE_NUM);

		m_strDefines[define_index] = value;
	}

	// 获得指定的着色器索引值
	SHADER_TYPE* GetShader(size_t option_index, size_t define_index)
	{
		Assert(option_index < OPTION_NUM);
		Assert(define_index < DEFINE_SIZE);

		size_t index = option_index * DEFINE_SIZE + define_index;

		if (NULL == m_nShaderList[index])
		{
			return LoadShader(option_index, define_index);
		}

		return m_nShaderList[index];
	}

	// 加载指定的着色器
	SHADER_TYPE* LoadShader(size_t option_index, size_t define_index)
	{
		Assert(option_index < OPTION_NUM);
		Assert(define_index < DEFINE_SIZE);
		
		TFastStr<char, 256, TCharTraits<char>, TCoreAlloc> def = 
			m_strOption[option_index];
		size_t count = 0;
		size_t val = define_index;

		while (val > 0)
		{
			if ((val & 0x1) != 0)
			{
				def += m_strDefines[count];
			}

			val = val >> 1;
			++count;
		}

		SHADER_TYPE* pShader = TRAITS::LoadShader(m_pRender, m_strFileName, 
			m_strFuncName, def.c_str());

		m_nShaderList[option_index * DEFINE_SIZE + define_index] = pShader;

		return pShader;
	}

	// 加载着色器
	bool LoadAllShader()
	{
		for (size_t k = 0; k < OPTION_NUM; ++k)
		{
			for (size_t i = 0; i < DEFINE_SIZE; ++i)
			{
				TFastStr<char, 256, TCharTraits<char>, TCoreAlloc> def = 
					m_strOption[k];
				size_t count = 0;
				size_t val = i;

				while (val > 0)
				{
					if ((val & 0x1) != 0)
					{
						def += m_strDefines[count];
					}

					val = val >> 1;
					++count;
				}

				SHADER_TYPE* pShader = TRAITS::LoadShader(m_pRender, 
					m_strFileName, m_strFuncName, def.c_str());

				if (NULL == pShader)
				{
					return false;
				}

				m_nShaderList[k * DEFINE_SIZE + i] = pShader;
			}
		}

		return true;
	}

private:
	IRender* m_pRender;
	const char* m_strFileName;
	const char* m_strFuncName;
	const char* m_strOption[OPTION_NUM];
	const char* m_strDefines[DEFINE_NUM];
	SHADER_TYPE* m_nShaderList[SHADER_NUM];
};

// 使用哈希表管理包含很多宏定义的着色器

template<typename SHADER_TYPE, size_t DEFINE_NUM, 
	typename TRAITS = TShaderTraits<SHADER_TYPE> >
class TShaderManyDefines
{
private:
	static const int64_t   SHADER_NUM = (int64_t)0x1 << DEFINE_NUM ;

	struct shader_node_t
	{
		size_t nIndex;
		SHADER_TYPE* pShader;
		shader_node_t* pNext;
	};

public:
	TShaderManyDefines()
	{
		m_pRender = NULL;
		m_strFileName = NULL;
		m_strFuncName = NULL;
		memset(m_strDefines, 0, sizeof(m_strDefines));
		m_nSize = 0;
		m_nCount = 0;
		m_pBuckets = NULL;
	}

	~TShaderManyDefines()
	{
		Clear();
	}

	// 设置管理器
	void SetRender(IRender* value)
	{
		m_pRender = value;
	}

	// 设置着色器名称
	void SetShaderName(const char* file_name, const char* func_name)
	{
		m_strFileName = file_name;
		m_strFuncName = func_name;
	}

	// 设置宏定义
	void SetDefine(size_t slot, const char* value)
	{
		Assert(slot < DEFINE_NUM);

		m_strDefines[slot] = value;
	}

	// 获得指定的着色器索引值
	SHADER_TYPE* GetShader(size_t index)
	{
		Assert(index < SHADER_NUM);

		shader_node_t* pNode = FindNode(index);

		if (NULL == pNode)
		{
			return LoadShader(index, "");
		}

		return pNode->pShader;
	}

	SHADER_TYPE* GetShaderWithPrefix(size_t index, const char* prefix)
	{
		Assert(index < SHADER_NUM);
		Assert(prefix != NULL);

		shader_node_t* pNode = FindNode(index);

		if (NULL == pNode)
		{
			return LoadShader(index, prefix);
		}

		return pNode->pShader;
	}

	// 加载指定的着色器
	SHADER_TYPE* LoadShader(size_t index, const char* prefix = "")
	{
		Assert(index < SHADER_NUM);

		TFastStr<char, 256, TCharTraits<char>, TCoreAlloc> def(prefix);
		size_t count = 0;
		size_t val = index;

		while (val > 0)
		{
			if ((val & 0x1) != 0)
			{
				def += m_strDefines[count];
			}

			val = val >> 1;
			++count;
		}

		SHADER_TYPE* pShader = TRAITS::LoadShader(m_pRender, m_strFileName, 
			m_strFuncName, def.c_str());

		AddNode(index, pShader);

		return pShader;
	}

	// 加载着色器
	bool LoadAllShader()
	{
		for (size_t i = 0; i < SHADER_NUM; ++i)
		{
			TFastStr<char, 256, TCharTraits<char>, TCoreAlloc> def;
			size_t count = 0;
			size_t val = i;

			while (val > 0)
			{
				if ((val & 0x1) != 0)
				{
					def += m_strDefines[count];
				}

				val = val >> 1;
				++count;
			}

			SHADER_TYPE* pShader = TRAITS::LoadShader(m_pRender, 
				m_strFileName, m_strFuncName, def.c_str());

			if (NULL == pShader)
			{
				return false;
			}

			AddNode(i, pShader);
		}

		return true;
	}

	// 清空
	void Clear()
	{
		if (m_pBuckets)
		{
			for (size_t i = 0; i < m_nSize; ++i)
			{
				shader_node_t* p = m_pBuckets[i];
		
				while (p)
				{
					shader_node_t* t = p->pNext;
					CORE_FREE(p, sizeof(shader_node_t));
					p = t;
				}
			}

			CORE_FREE(m_pBuckets, sizeof(shader_node_t*) * m_nSize);
			m_pBuckets = NULL;
			m_nSize = 0;
			m_nCount = 0;
		}
	}

private:
	TShaderManyDefines(const TShaderManyDefines&);
	TShaderManyDefines& operator=(const TShaderManyDefines&);

	shader_node_t* FindNode(size_t index) const
	{
		if (0 == m_nSize)
		{
			return NULL;
		}

		size_t bucket = index % m_nSize;
		shader_node_t* p = m_pBuckets[bucket];

		while (p)
		{
			if (p->nIndex == index)
			{
				return p;
			}

			p = p->pNext;
		}

		return NULL;
	}

	bool AddNode(size_t index, SHADER_TYPE* pShader)
	{
		if (m_nCount == m_nSize)
		{
			Expand();
		}

		size_t bucket = index % m_nSize;
		shader_node_t* p = (shader_node_t*)CORE_ALLOC(sizeof(shader_node_t));

		p->pNext = m_pBuckets[bucket];
		p->nIndex = index;
		p->pShader = pShader;

		m_pBuckets[bucket] = p;
		m_nCount++;

		return true;
	}

	void Expand()
	{
		size_t new_size = m_nSize * 2 + 1;
		shader_node_t** new_buckets = (shader_node_t**)CORE_ALLOC(
			sizeof(shader_node_t*) * new_size);

		memset(new_buckets, 0, sizeof(shader_node_t*) * new_size); 

		for (size_t i = 0; i < m_nSize; ++i)
		{
			shader_node_t* p = m_pBuckets[i];

			while (p)
			{
				shader_node_t* t = p->pNext;
				size_t bucket = size_t(p->nIndex) % new_size;

				p->pNext = new_buckets[bucket];
				new_buckets[bucket] = p;
				p = t;
			}
		}

		if (m_pBuckets)
		{
			CORE_FREE(m_pBuckets, sizeof(shader_node_t*) * m_nSize);
		}

		m_pBuckets = new_buckets;
		m_nSize = new_size;
	}

private:
	IRender* m_pRender;
	const char* m_strFileName;
	const char* m_strFuncName;
	const char* m_strDefines[DEFINE_NUM];
	shader_node_t** m_pBuckets;
	size_t m_nSize;
	size_t m_nCount;
};

#endif // _VISUAL_SHADER_LIST_H
