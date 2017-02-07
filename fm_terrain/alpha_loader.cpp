//--------------------------------------------------------------------
// 文件名:		alpha_loader.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "alpha_loader.h"
#include "alpha_map_compress.h"
#include "terrain.h"
#include "terrain_zone.h"
#include "terrain_chunk.h"
#include "tex_alpha_map.h"
#include "blend_tex.h"
#include "blend_tex_set.h"
#include "block_file.h"
#include "block_base.h"
#include "block_alpha_map.h"
#include "../public/auto_mem.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/inlines.h"
#include "../public/i_file_sys.h"

#define BLOCK_ID_BASE_0 '0SAB'
#define BLOCK_ID_ALPHAMAP_1 '1PAM'

// CAlphaLoader

CAlphaLoader* CAlphaLoader::NewInstance()
{
	return CORE_NEW(CAlphaLoader);
}

IBlockReader* CAlphaLoader::CreateBlockReader(int block_id, void* context)
{
	CAlphaLoader* pthis = (CAlphaLoader*)context;
	
	if (block_id == BLOCK_ID_BASE_0)
	{
		return CBlockBaseReader::NewInstance(&pthis->m_Base,
			pthis->m_nChunkRows, pthis->m_nChunkCols);
	}
	else if (block_id == BLOCK_ID_ALPHAMAP_1)
	{
		return CBlockAlphaMapReader::NewInstance(&pthis->m_Map, 
			pthis->m_nChunkRows, pthis->m_nChunkCols, 
			pthis->m_nAlphaMapScale);
	}
	
	return NULL;
}

CAlphaLoader::CAlphaLoader()
{
	m_pOwner = NULL;
	m_nChunkRows = 0;
	m_nChunkCols = 0;
	m_nAlphaRows = 0;
	m_nAlphaCols = 0;
	m_Base.nBaseTexNum = 0;
	m_Base.pBaseTexIndex = NULL;
	m_Map.nMapTexNum = 0;
	m_Map.nFormatNum = 0;
	m_Map.pBlendFormat = NULL;
	m_Map.nMapNum = 0;
	m_Map.pMapAlpha = NULL;
	m_Map.pMapData = NULL;
}

CAlphaLoader::~CAlphaLoader()
{
	if (m_Base.pBaseTexIndex)
	{
		CORE_FREE(m_Base.pBaseTexIndex, m_nChunkRows * m_nChunkCols);
	}

	if (m_Map.pBlendFormat)
	{
		CORE_FREE(m_Map.pBlendFormat, 
			sizeof(blend_format_t) * m_Map.nFormatNum);
	}
	
	if (m_Map.pMapAlpha)
	{
		CORE_FREE(m_Map.pMapAlpha, sizeof(map_alpha_t) * m_Map.nMapNum);
	}

	if (m_Map.pMapData)
	{
		CORE_FREE(m_Map.pMapData, m_Map.nMapDataSize);
	}
}

void CAlphaLoader::Release()
{
	if (this->DecRefs() == 0)
	{
		CORE_DELETE(this);
	}
}

void CAlphaLoader::SetOwner(CTerrainZone* pOwner)
{
	Assert(pOwner != NULL);

	Terrain* pTerrain = pOwner->GetTerrain();
	
	m_pOwner = pOwner;
	m_nChunkRows = pOwner->GetChunkDims();
	m_nChunkCols = pOwner->GetChunkDims();
	m_nAlphaRows = pTerrain->GetAlphaScale() + 1;
	m_nAlphaCols = pTerrain->GetAlphaScale() + 1;
	m_nAlphaMapScale = pTerrain->GetAlphaMapScale();
}

void CAlphaLoader::SetBaseFile(const char* file_name)
{
	Assert(file_name != NULL);
	
	m_strBaseFile = file_name;
}

void CAlphaLoader::SetMapFile(const char* file_name)
{
	Assert(file_name != NULL);

	m_strMapFile = file_name;
}

const char* CAlphaLoader::GetName()
{
	return m_strMapFile.c_str();
}

bool CAlphaLoader::LoadBlockBase(const char* file_name)
{
	Assert(file_name != NULL);
	
	CBlockFile block_file(TERRAIN_FILE_SIGNATURE, CreateBlockReader, this);
	
	return block_file.Load(file_name);
}

bool CAlphaLoader::LoadBlockAlphaMap(const char* file_name)
{
	Assert(file_name != NULL);
	
	CBlockFile block_file(TERRAIN_FILE_SIGNATURE, CreateBlockReader, this);
	
	return block_file.Load(file_name);
}

bool CAlphaLoader::Load(bool async)
{
	if (!m_strBaseFile.empty())
	{
		LoadBlockBase(m_strBaseFile.c_str());
	}

	if (!m_strMapFile.empty())
	{
		LoadBlockAlphaMap(m_strMapFile.c_str());
	}
	
	return true;
}

bool CAlphaLoader::Create(bool async)
{
	CBlendTexSet* pBaseTexSet = m_pOwner->GetTerrain()->GetBaseTexSet();
	
	// 获得基础贴图的索引值
	int tex_index[Terrain::MAX_TEX_NUM];
	size_t i;

	for (i = 0; i < m_Base.nBaseTexNum; ++i)
	{
		const char* tex_name = m_Base.BaseNames.GetByIndex(i);
		size_t index;

		if (!pBaseTexSet->FindIndex(tex_name, &index))
		{
			tex_index[i] = -1;

#ifdef _DEBUG
			CORE_TRACE("(CAlphaLoader::Create)base tex not found");
			CORE_TRACE(tex_name);
			CORE_TRACE(m_strBaseFile.c_str());
#endif // _DEBUG
		}
		else
		{
			tex_index[i] = (int)index;
		}
	}

	// 设置分块的基础贴图
	if (m_Base.pBaseTexIndex)
	{
		size_t chunk_num = m_pOwner->GetChunkCount();
		CTerrainChunk* zone_chunks = m_pOwner->GetChunks();
		
		for (size_t k = 0; k < chunk_num; ++k)
		{
			CTerrainChunk* pChunk = zone_chunks + k;
			size_t index = m_Base.pBaseTexIndex[k];

			if (index >= m_Base.nBaseTexNum)
			{
				CORE_TRACE("(CAlphaLoader::Create)base tex index error");
				CORE_TRACE(m_strBaseFile.c_str());
				continue;
			}
			
			int base_tex = tex_index[index];

			if (base_tex < 0)
			{
				continue;
			}

			pChunk->SetBaseTexIndex(base_tex);
		}
	}

	// 获得混合贴图的索引值
	CBlendTexSet* pBlendTexSet = m_pOwner->GetTerrain()->GetBlendTexSet();
	
	for (i = 0; i < m_Map.nMapTexNum; ++i)
	{
		const char* tex_name = m_Map.MapNames.GetByIndex(i);
		size_t index;
		
		if (!pBlendTexSet->FindIndex(tex_name, &index))
		{
			tex_index[i] = -1;
			
#ifdef _DEBUG
			CORE_TRACE("(CAlphaLoader::Create)blend tex not found");
			CORE_TRACE(tex_name);
			CORE_TRACE(m_strMapFile.c_str());
#endif // _DEBUG
		}
		else
		{
			tex_index[i] = (int)index;
		}
	}

	// 贴图样式
	size_t blend_tex_num = pBlendTexSet->GetCount();
	size_t format_num = m_Map.nFormatNum;
	blend_format_t* pBlendFormat = m_Map.pBlendFormat;
	TAutoMem<unsigned char, 1, TCoreAlloc> auto_buf(blend_tex_num * 256);
	unsigned char* fmt_index = auto_buf.GetBuffer();

	for (i = 0; i < blend_tex_num; ++i)
	{
		fmt_index[i * 256] = 0;
	}

	if (pBlendFormat)
	{
		for (size_t k = 0; k < format_num; ++k)
		{
			blend_format_t* format = pBlendFormat + k;

			if (size_t(format->nTexIndex) >= m_Map.nMapTexNum)
			{
				CORE_TRACE("(CAlphaLoader::Create)blend tex index error");
				CORE_TRACE(m_strMapFile.c_str());
				continue;
			}
			
			size_t ti = tex_index[format->nTexIndex];

			if (ti >= pBlendTexSet->GetCount())
			{
				continue;
			}

			CBlendTex* pBlend = pBlendTexSet->GetByIndex(ti);

			if (size_t(format->nFmtIndex) < pBlend->GetFormatCount())
			{
				fmt_index[ti * 256 + format->nFmtIndex] = format->nFmtIndex;
			}
			else
			{
				pBlend->AddFormat(format->nScaleU, format->nScaleV,
					format->nAngleX, format->nAngleY, format->nAngleZ);
				pBlend->UpdateMatrix(pBlend->GetFormatCount() - 1, 
					m_pOwner->GetTerrain());
				fmt_index[ti * 256 + format->nFmtIndex] = 
					(unsigned char)(pBlend->GetFormatCount() - 1);
			}
		}
	}

	// 最终混合贴图
	size_t alpha_map_scale = m_pOwner->GetTerrain()->GetAlphaMapScale();
	size_t alpha_map_size = alpha_map_scale * alpha_map_scale / 2;
	
	if (m_Map.pMapAlpha)
	{
		// 释放此区域内所有的ALPHAMAP
		m_pOwner->ReleaseChunkAlphaMap();
		
		unsigned char* pData = m_Map.pMapData;
		
		for (size_t k = 0; k < m_Map.nMapNum; ++k)
		{
			map_alpha_t* pAlpha = m_Map.pMapAlpha + k;

			if (size_t(pAlpha->nTexIndex) >= m_Map.nMapTexNum)
			{
				pData += alpha_map_size;
				CORE_TRACE("(CAlphaLoader::Create)blend tex index exceed");
				CORE_TRACE(m_strMapFile.c_str());
				continue;
			}

			size_t tex = tex_index[pAlpha->nTexIndex];
			
			if (tex >= pBlendTexSet->GetCount())
			{
				pData += alpha_map_size;
#ifdef _DEBUG
				CORE_TRACE("(CAlphaLoader::Create)texture not found");
				CORE_TRACE(m_strMapFile.c_str());
#endif // _DEBUG
				continue;
			}

			CBlendTex* pBlend = pBlendTexSet->GetByIndex(tex);
			size_t fmt = fmt_index[tex * 256 + pAlpha->nFormat];
			
			if (fmt >= pBlend->GetFormatCount())
			{
				pData += alpha_map_size;
				CORE_TRACE("(CAlphaLoader::Create)format not found");
				CORE_TRACE(m_strMapFile.c_str());
				continue;
			}
			
			CTerrainChunk* pChunk = m_pOwner->GetChunk(pAlpha->nChunkRow, 
				pAlpha->nChunkCol);

			if (NULL == pChunk)
			{
				pData += alpha_map_size;
				CORE_TRACE("(CAlphaLoader::Create)chunk not found");
				CORE_TRACE(m_strMapFile.c_str());
				continue;
			}

			CTexAlphaMap* pAlphaMap = pChunk->AddAlphaMap(tex, fmt);

			if (pAlphaMap)
			{
				pAlphaMap->WriteMapData(pData, alpha_map_size);
			}
			
			pData += alpha_map_size;
		}
	}
	
	m_pOwner->SetAlphaLoaded(true);
	
	return true;
}

bool CAlphaLoader::EndCreate(int category, bool succeed)
{
	return m_pOwner->EndCreate(category, succeed);
}

bool CAlphaLoader::SaveBlockBase(const char* file_name, CTerrainZone* pZone)
{
	Assert(file_name != NULL);
	Assert(pZone != NULL);
	
	size_t base_num = 0;
	size_t chunk_num = pZone->GetChunkCount();

	for (size_t k = 0; k < chunk_num; ++k)
	{
		base_num += pZone->GetChunkByIndex(k)->GetBaseTexIndex();
	}

	if (0 == base_num)
	{
		// 全部是缺省贴图
		//DeleteFile(file_name);
		g_pCore->GetFileSys()->FileDelete(file_name);
		return true;
	}

	CBlockFile block_file(TERRAIN_FILE_SIGNATURE, NULL, NULL);
	
	void* handle = block_file.SaveBegin(file_name);
	
	if (NULL == handle)
	{
		return false;
	}
	
	CBlockBaseWriter writer(pZone);
	
	if (!block_file.SaveBlock(handle, BLOCK_ID_BASE_0, &writer))
	{
		return false;
	}
	
	return block_file.SaveEnd(handle);
}

bool CAlphaLoader::SaveBlockAlphaMap(const char* file_name, 
	CTerrainZone* pZone)
{
	Assert(file_name != NULL);
	Assert(pZone != NULL);
	
	size_t map_num = pZone->GetChunkAlphaMapCount();
	
	if (0 == map_num)
	{
		//DeleteFile(file_name);
		g_pCore->GetFileSys()->FileDelete(file_name);
		return true;
	}
	
	CBlockFile block_file(TERRAIN_FILE_SIGNATURE, NULL, NULL);
	
	void* handle = block_file.SaveBegin(file_name);
	
	if (NULL == handle)
	{
		return false;
	}
	
	CBlockAlphaMapWriter writer(pZone);
	
	if (!block_file.SaveBlock(handle, BLOCK_ID_ALPHAMAP_1, &writer))
	{
		return false;
	}
	
	return block_file.SaveEnd(handle);
}
