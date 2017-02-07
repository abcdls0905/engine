//--------------------------------------------------------------------
// 文件名:		block_alpha_map.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年7月3日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "block_alpha_map.h"
#include "terrain.h"
#include "terrain_zone.h"
#include "terrain_chunk.h"
#include "alpha_map_compress.h"
#include "tex_alpha_map.h"
#include "blend_tex.h"
#include "blend_tex_set.h"
#include "name_list.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/auto_mem.h"
#include "../public/core_file.h"

#pragma pack(push, 1)

// 混合贴图信息文件头
struct map_file_header_t
{
	unsigned int nChunkRows;	// 分块行数
	unsigned int nChunkCols;	// 分块列数
	unsigned int nMapTexNum;	// 混合贴图数量
	unsigned int nMapNameSize;	// 贴图名字缓冲的总长度
	unsigned int nBlendFormatNum;	// 贴图样式数量
	unsigned int nAlphaMapNum;	// 最终混合贴图数量
	unsigned int nAlphaMapScale;// 最终混合贴图的尺寸
};

#pragma pack(pop)

// CBlockAlphaMapReader

IBlockReader* CBlockAlphaMapReader::NewInstance(CAlphaLoader::map_all_t* p,
	size_t chunk_rows, size_t chunk_cols, size_t alpha_map_scale)
{
	CBlockAlphaMapReader* ptr = (CBlockAlphaMapReader*)CORE_ALLOC(
		sizeof(CBlockAlphaMapReader));
	
	new (ptr) CBlockAlphaMapReader(p, chunk_rows, chunk_cols, 
		alpha_map_scale);

	return ptr;
}

CBlockAlphaMapReader::CBlockAlphaMapReader(CAlphaLoader::map_all_t* p,
	size_t chunk_rows, size_t chunk_cols, size_t alpha_map_scale)
{
	Assert(p != NULL);

	m_pData = p;
	m_nChunkRows = chunk_rows;
	m_nChunkCols = chunk_cols;
	m_nAlphaMapScale = alpha_map_scale;
}

CBlockAlphaMapReader::~CBlockAlphaMapReader()
{
}

void CBlockAlphaMapReader::Release()
{
	CORE_DELETE(this);
}

int CBlockAlphaMapReader::GetSize() const
{
	return 0;
}
	
static size_t get_buffer_names(const char* buffer, size_t size, 
	CNameList& names)
{
	size_t count = 0;
	const char* s = NULL;
	
	for (size_t i = 0; i < size; ++i)
	{
		if (buffer[i] == 0)
		{
			if (s != NULL)
			{
				names.AddName(s);
				s = NULL;
				count++;
			}
		}
		else if (NULL == s)
		{
			s = buffer + i;
		}
	}
	
	return count;
}

int CBlockAlphaMapReader::Read(void* handle)
{
	int read_size = 0;
	FILE* fp = (FILE*)handle;
	map_file_header_t header;

	if (core_file::fread(&header, sizeof(header), 1, fp) != 1)
	{
		CORE_TRACE("(CBlockAlphaMapReader::Read)read header failed");
		return -1;
	}

	read_size += sizeof(header);

	if ((header.nChunkRows != m_nChunkRows)
		|| (header.nChunkCols != m_nChunkCols))
	{
		CORE_TRACE("(CBlockAlphaMapReader::Read)chunk size error");
		return -1;
	}

	if (header.nAlphaMapScale != m_nAlphaMapScale)
	{
		CORE_TRACE("(CBlockAlphaMapReader::Read)alpha map size error");
		return -1;
	}

	if (header.nMapTexNum > Terrain::MAX_TEX_NUM)
	{
		CORE_TRACE("(CBlockAlphaMapReader::Read)blend tex number error");
		return -1;
	}

	if (header.nMapNameSize > 0x10000)
	{
		CORE_TRACE("(CBlockAlphaMapReader::Read)blend name size error");
		return -1;
	}

	// 所有贴图的名字
	size_t name_size = header.nMapNameSize;
	TAutoMem<char, 256, TCoreAlloc> name_auto_buf(name_size + 1);
	char* name_buffer = name_auto_buf.GetBuffer();
	
	if (core_file::fread(name_buffer, sizeof(char), name_size, fp) 
		!= name_size)
	{
		CORE_TRACE("(CBlockAlphaMapReader::Read)read map names failed");
		return -1;
	}

	read_size += (int)name_size;
	// 防止没有结束符
	name_buffer[name_size] = 0;
	
	if (get_buffer_names(name_buffer, name_size, m_pData->MapNames) 
		!= header.nMapTexNum)
	{
		CORE_TRACE("(CBlockAlphaMapReader::Read)get map names failed");
		return -1;
	}

	// 贴图样式
	size_t format_num = header.nBlendFormatNum;
	CAlphaLoader::blend_format_t* blend_format = 
		(CAlphaLoader::blend_format_t*)CORE_ALLOC(
		sizeof(CAlphaLoader::blend_format_t) * format_num);
	
	if (core_file::fread(blend_format, sizeof(CAlphaLoader::blend_format_t),
		format_num, fp) != format_num)
	{
		CORE_TRACE("(CBlockAlphaMapReader::Read)read blend format failed");
		CORE_FREE(blend_format, 
			sizeof(CAlphaLoader::blend_format_t) * format_num);
		return -1;
	}
	
	read_size += (int)(format_num * sizeof(CAlphaLoader::blend_format_t));

	for (size_t j = 0; j < format_num; ++j)
	{
		CAlphaLoader::blend_format_t* p = blend_format + j;
		
		if (p->nTexIndex >= header.nMapTexNum)
		{
			CORE_TRACE("(CBlockAlphaMapReader::Read)blend format error");
			CORE_FREE(blend_format, 
				sizeof(CAlphaLoader::blend_format_t) * format_num);
			return -1;
		}
	}
	
	// 混合贴图设计值
	size_t map_num = header.nAlphaMapNum;
	CAlphaLoader::map_alpha_t* map_alpha = (CAlphaLoader::map_alpha_t*)
		CORE_ALLOC(sizeof(CAlphaLoader::map_alpha_t) * map_num);

	if (core_file::fread(map_alpha, sizeof(CAlphaLoader::map_alpha_t), 
		map_num, fp) != map_num)
	{
		CORE_TRACE("(CBlockAlphaMapReader::Read)read map alpha failed");
		CORE_FREE(blend_format, 
			sizeof(CAlphaLoader::blend_format_t) * format_num);
		CORE_FREE(map_alpha, sizeof(CAlphaLoader::map_alpha_t) * map_num);
		return -1;
	}

	read_size += (int)(map_num * sizeof(CAlphaLoader::map_alpha_t));
	
	for (size_t i = 0; i < map_num; ++i)
	{
		CAlphaLoader::map_alpha_t* p = map_alpha + i;
		
		if ((p->nChunkRow >= m_nChunkRows)
			|| (p->nChunkCol >= m_nChunkCols)
			|| (p->nTexIndex >= header.nMapTexNum))
		{
			CORE_TRACE("(CBlockAlphaMapReader::Read)map alpha error");
			CORE_FREE(blend_format, 
				sizeof(CAlphaLoader::blend_format_t) * format_num);
			CORE_FREE(map_alpha, sizeof(CAlphaLoader::map_alpha_t) * map_num);
			return -1;
		}
	}

	size_t alpha_map_size = m_nAlphaMapScale * m_nAlphaMapScale / 2;
	// 贴图数据
	size_t map_data_size = map_num * alpha_map_size;
	unsigned char* map_data = (unsigned char*)CORE_ALLOC(map_data_size);

	// 压缩格式
	for (size_t k = 0; k < map_num; ++k)
	{
		unsigned int data_size;

		if (core_file::fread(&data_size, sizeof(data_size), 1, fp) != 1)
		{
			CORE_TRACE("(CBlockAlphaMapReader::Read)"
				"read compress data size failed");
			CORE_FREE(blend_format, 
				sizeof(CAlphaLoader::blend_format_t) * format_num);
			CORE_FREE(map_alpha, sizeof(CAlphaLoader::map_alpha_t) * map_num);
			CORE_FREE(map_data, map_data_size);
			return -1;
		}

		read_size += sizeof(data_size);
		
		if (data_size > 0x100000)
		{
			CORE_TRACE("(CBlockAlphaMapReader::Read)"
				"compress data size error");
			CORE_FREE(blend_format, 
				sizeof(CAlphaLoader::blend_format_t) * format_num);
			CORE_FREE(map_alpha, sizeof(CAlphaLoader::map_alpha_t) * map_num);
			CORE_FREE(map_data, map_data_size);
			return -1;
		}

		TAutoMem<unsigned char, 1024, TCoreAlloc> auto_buf(data_size);
		unsigned char* compress = auto_buf.GetBuffer();

		if (core_file::fread(compress, sizeof(unsigned char), data_size, fp) 
			!= data_size)
		{
			CORE_TRACE("(CBlockAlphaMapReader::Read)"
				"read compress data failed");
			CORE_FREE(blend_format, 
				sizeof(CAlphaLoader::blend_format_t) * format_num);
			CORE_FREE(map_alpha, sizeof(CAlphaLoader::map_alpha_t) * map_num);
			CORE_FREE(map_data, map_data_size);
			return -1;
		}

		read_size += data_size;
		
		CAlphaMapCompress amc;

		if (!amc.Decompress(m_nAlphaMapScale, compress, data_size))
		{
			CORE_TRACE("(CBlockAlphaMapReader::Read)"
				"decompress alpha map data failed");
			CORE_FREE(blend_format, 
				sizeof(CAlphaLoader::blend_format_t) * format_num);
			CORE_FREE(map_alpha, sizeof(CAlphaLoader::map_alpha_t) * map_num);
			CORE_FREE(map_data, map_data_size);
			return -1;
		}

		memcpy(map_data + k * alpha_map_size, amc.GetResultData(), 
			amc.GetResultSize());
	}

	m_pData->pBlendFormat = blend_format;
	m_pData->nFormatNum = header.nBlendFormatNum;
	m_pData->pMapAlpha = map_alpha;
	m_pData->pMapData = map_data;
	m_pData->nMapDataSize = map_data_size;
	m_pData->nMapTexNum = header.nMapTexNum;
	m_pData->nMapNum = header.nAlphaMapNum;
	
	return read_size;
}

// CBlockAlphaMapWriter

CBlockAlphaMapWriter::CBlockAlphaMapWriter(CTerrainZone* pZone)
{
	Assert(pZone != NULL);
	
	m_pZone = pZone;
}

CBlockAlphaMapWriter::~CBlockAlphaMapWriter()
{
}

int CBlockAlphaMapWriter::GetSize() const
{
	return 0;
}

int CBlockAlphaMapWriter::Write(void* handle)
{
	int write_size = 0;
	FILE* fp = (FILE*)handle;
	size_t map_num = m_pZone->GetChunkAlphaMapCount();
	Terrain* pTerrain = m_pZone->GetTerrain();
	size_t alpha_map_scale = pTerrain->GetAlphaMapScale();
	size_t alpha_map_size = alpha_map_scale * alpha_map_scale / 2;
	size_t chunk_rows = m_pZone->GetChunkDims();
	size_t chunk_cols = m_pZone->GetChunkDims();
	CBlendTexSet* pBlendTexSet = pTerrain->GetBlendTexSet();
	size_t tex_num = pBlendTexSet->GetCount();
	size_t name_size = 0;
	
	for (size_t k1 = 0; k1 < tex_num; ++k1)
	{
		name_size += strlen(pBlendTexSet->GetByIndex(k1)->GetName()) + 1;
	}
	
	size_t format_num = 0;
	
	for (size_t k2 = 0; k2 < tex_num; ++k2)
	{
		format_num += pBlendTexSet->GetByIndex(k2)->GetFormatCount();
	}
	
	map_file_header_t header;
	
	header.nChunkRows = (unsigned int)chunk_rows;
	header.nChunkCols = (unsigned int)chunk_cols;
	header.nMapTexNum = (unsigned int)tex_num;
	header.nMapNameSize = (unsigned int)name_size;
	header.nBlendFormatNum = (unsigned int)format_num;
	header.nAlphaMapNum = (unsigned int)map_num;
	header.nAlphaMapScale = (unsigned int)alpha_map_scale;
	
	if (core_file::fwrite(&header, sizeof(header), 1, fp) != 1)
	{
		CORE_TRACE("(CBlockAlphaMapWriter::Write)write header failed");
		return -1;
	}
	
	write_size += sizeof(header);
	
	for (size_t j = 0; j < tex_num; ++j)
	{
		const char* tex_name = pBlendTexSet->GetByIndex(j)->GetName();
		size_t name_len = strlen(tex_name) + 1;
		
		if (core_file::fwrite(tex_name, sizeof(char), name_len, fp) 
			!= name_len)
		{
			CORE_TRACE("(CBlockAlphaMapWriter::Write)write tex name failed");
			return -1;
		}

		write_size += (int)name_len;
	}
	
	CAlphaLoader::blend_format_t* save_format =
		(CAlphaLoader::blend_format_t*)CORE_ALLOC(
		sizeof(CAlphaLoader::blend_format_t) * format_num);
	size_t format_index = 0;
	
	for (size_t i = 0; i < tex_num; ++i)
	{
		CBlendTex* pBlend = pBlendTexSet->GetByIndex(i);
		size_t num = pBlend->GetFormatCount();
		
		for (size_t k = 0; k < num; ++k)
		{
			CBlendTex::format_t* format = pBlend->GetFormatByIndex(k);
			
			save_format[format_index].nScaleU = format->nScaleU;
			save_format[format_index].nScaleV = format->nScaleV;
			save_format[format_index].nAngleX = format->nAngleX;
			save_format[format_index].nAngleY = format->nAngleY;
			save_format[format_index].nAngleZ = format->nAngleZ;
			save_format[format_index].nTexIndex = (unsigned char)i;
			save_format[format_index].nFmtIndex = (unsigned char)k;
			format_index++;
		}
	}
	
	Assert(format_index == format_num);
	
	if (core_file::fwrite(save_format, sizeof(CAlphaLoader::blend_format_t), 
		format_num, fp) != format_num)
	{
		CORE_TRACE("(CBlockAlphaMapWriter::Write)write blend format failed");
		CORE_FREE(save_format, 
			sizeof(CAlphaLoader::blend_format_t) * format_num);
		return -1;
	}
	
	write_size += (int)(format_num * sizeof(CAlphaLoader::blend_format_t));
	CORE_FREE(save_format, sizeof(CAlphaLoader::blend_format_t) * format_num);

	for (size_t r1 = 0; r1 < chunk_rows; ++r1)
	{
		for (size_t c = 0; c < chunk_cols; ++c)
		{
			CTerrainChunk* pChunk = m_pZone->GetChunk(r1, c);
			size_t blend_num = pChunk->GetBlendTexCount();
			
			if (0 == blend_num)
			{
				continue;
			}
			
			const CTerrainChunk::blend_tex_t* blend = pChunk->GetBlendTexs();
			
			for (size_t k = 0; k < blend_num; ++k)
			{
				CAlphaLoader::map_alpha_t alpha;
				
				alpha.nChunkRow = (unsigned short)r1;
				alpha.nChunkCol = (unsigned short)c;
				alpha.nTexIndex = (unsigned char)blend[k].nTexIndex;
				alpha.nFormat = (unsigned char)blend[k].nFormat;
				
				if (core_file::fwrite(&alpha, sizeof(alpha), 1, fp) != 1)
				{
					CORE_TRACE("(CBlockAlphaMapWriter::Write)"
						"write map info failed");
					return -1;
				}

				write_size += sizeof(alpha);
			}
		}
	}
	
	for (size_t r2 = 0; r2 < chunk_rows; ++r2)
	{
		for (size_t c = 0; c < chunk_cols; ++c)
		{
			CTerrainChunk* pChunk = m_pZone->GetChunk(r2, c);
			size_t blend_num = pChunk->GetBlendTexCount();
			
			if (0 == blend_num)
			{
				continue;
			}
			
			const CTerrainChunk::blend_tex_t* blend = pChunk->GetBlendTexs();
			
			for (size_t k = 0; k < blend_num; ++k)
			{
				// 读取贴图数据
				CTexAlphaMap* pAlphaMap = blend[k].pAlphaMap;
				// 压缩数据
				CAlphaMapCompress amc;

				if (!amc.Compress(alpha_map_scale, pAlphaMap->GetMapData(), 
					alpha_map_size))
				{
					CORE_TRACE("(CBlockAlphaMapWriter::Write)"
						"compress map data failed");
					return -1;
				}

				unsigned int data_size = (unsigned int)amc.GetResultSize();

				if (core_file::fwrite(&data_size, sizeof(data_size), 1, fp) 
					!= 1)
				{
					CORE_TRACE("(CBlockAlphaMapWriter::Write)"
						"write compress data size failed");
					return -1;
				}

				write_size += sizeof(data_size);
				
				if (core_file::fwrite(amc.GetResultData(), 
					sizeof(unsigned char), data_size, fp) != data_size)
				{
					CORE_TRACE("(CBlockAlphaMapWriter::Write)"
						"write compress data failed");
					return -1;
				}

				write_size += data_size;
			}
		}
	}
	
	return write_size;
}
