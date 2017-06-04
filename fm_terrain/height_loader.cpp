//--------------------------------------------------------------------
// 文件名:		height_loader.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "height_loader.h"
#include "terrain_zone.h"
#include "terrain.h"
#include "terrain_chunk.h"
#include "terrain_height.h"
#include "terrain_water.h"
#include "sea_water.h"
#include "block_file.h"
#include "block_height.h"
#include "block_height2.h"
#include "block_chunk.h"
#include "block_water.h"
#include "../visual/i_render.h"
#include "../visual/vis_utils.h"
#include "../public/auto_mem.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/core_file.h"



#if defined FX_SYSTEM_IOS || defined __ANDROID__

typedef struct tagBITMAPFILEHEADER {
        unsigned short    bfType;
        unsigned long   bfSize;
        unsigned short    bfReserved1;
        unsigned short    bfReserved2;
        unsigned long   bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER{
        unsigned long      biSize;
        long       biWidth;
        long       biHeight;
        unsigned short       biPlanes;
        unsigned short       biBitCount;
        unsigned long      biCompression;
        unsigned long      biSizeImage;
        long       biXPelsPerMeter;
        long       biYPelsPerMeter;
        unsigned long      biClrUsed;
        unsigned long      biClrImportant;
} BITMAPINFOHEADER;

#define BI_RGB        0L
#endif
// CHeightLoader

CHeightLoader* CHeightLoader::NewInstance()
{
	return CORE_NEW(CHeightLoader);
}

IBlockReader* CHeightLoader::CreateBlockReader(int block_id, void* context)
{
	CHeightLoader* pthis = (CHeightLoader*)context;

	if (block_id == BLOCK_ID_HEIGHT_0)
	{
		return CBlockHeightReader::NewInstance(&pthis->m_Height,
			pthis->m_nRows, pthis->m_nCols);
	}
	else if (block_id == BLOCK_ID_HEIGHT_2)
	{
		return CBlockHeight2Reader::NewInstance(&pthis->m_Height,
			pthis->m_nRows, pthis->m_nCols);
	}
	else if (block_id == BLOCK_ID_CHUNK_0)
	{
		return CBlockChunkReader::NewInstance(&pthis->m_Chunk, 
			pthis->m_nChunkRows, pthis->m_nChunkCols, 
			pthis->m_fLeft, pthis->m_fTop);
	}
	else if (block_id == BLOCK_ID_WATER_0)
	{
		return CBlockWaterReader::NewInstance(&pthis->m_Water, 
			pthis->m_nChunkRows, pthis->m_nChunkCols);
	}
	
	return NULL;
}

void CHeightLoader::CalcChunkErrors(const float* height_values, size_t cols,
	size_t row_offset, size_t col_offset, size_t chunk_scale, 
	size_t lod_levels, float* lod_errors)
{
	size_t height_size = sizeof(float) * (chunk_scale + 1) * (chunk_scale + 1);
	float* heights = (float*)CORE_ALLOC(height_size);
	
	for (size_t lev = 0; lev < lod_levels; lev++)
	{
		// 不同LOD等级的步长
		size_t step = 0x01 << lev;
		
		if (step == 1)
		{
			// 0级不需计算
			continue;
		}
		
		for (size_t i = 0; i < chunk_scale; i += step)
		{
			for (size_t k = 0; k < chunk_scale; k += step)
			{
				float y1 = height_values[ 
					(i + row_offset) * cols + k + col_offset];
				float y2 = height_values[ 
					(i + row_offset) * cols + k + col_offset + step];
				float y3 = height_values[ 
					(i + row_offset + step) * cols + k + col_offset];
				float y4 = height_values[ 
					(i + row_offset + step) * cols + k + col_offset + step];
				
				for (size_t ii = 0; ii < (step + 1); ii++)
				{
					for (size_t kk = 0; kk < (step + 1); kk++)
					{
						float u = float(ii) / float(step);
						float v = float(kk) / float(step);
						float h;
						
						if ((u + v) <= 1.0F)
						{
							h = y1 + (y2 - y1) * u + (y3 - y1) * v;
						}
						else
						{
							u = 1.0F - u;
							v = 1.0F - v;
							h = y4 + (y3 - y4) * u + (y2 - y4) * v;
						}
						
						heights[(i + ii) * (chunk_scale + 1) + k + kk] = h;
					}
				}
			}
		}
		
		float max_error = 0.0F;
		
		for (size_t i = 0; i < (chunk_scale + 1); i++)
		{
			for (size_t k = 0; k < (chunk_scale + 1); k++)
			{
				float y = height_values[
					(i + row_offset) * cols + k + col_offset];
				float error = y - heights[i * (chunk_scale + 1) + k];
				
				if (error < 0.0F)
				{
					error = -error;
				}
				
				if (error > max_error)
				{
					max_error = error;
				}
			}
		}
		
		lod_errors[lev] = max_error;
	}

	CORE_FREE(heights, height_size);
}

void CHeightLoader::CalcChunkBoundBox(const float* height_values, size_t cols,
	size_t row_offset, size_t col_offset, size_t chunk_scale, float left, 
	float top, float unit_size, FmVec3& v_min, FmVec3& v_max)
{
	const float* p = height_values + row_offset * cols + col_offset;
	float min_y = *p;
	float max_y = min_y;

	for (size_t r = 0; r <= chunk_scale; ++r)
	{
		for (size_t c = 0; c <= chunk_scale; ++c)
		{
			float y = p[c];
			
			if (y > max_y)
			{
				max_y = y;
			}
			
			if (y < min_y)
			{
				min_y = y;
			}
		}

		p += cols;
	}
	
	v_min.x = left + col_offset * unit_size;
	v_min.y = min_y;
	v_min.z = top + row_offset * unit_size;
	v_max.x = v_min.x + chunk_scale * unit_size;
	v_max.y = max_y;
	v_max.z = v_min.z + chunk_scale * unit_size;
}

void CHeightLoader::CalcBlockMinMaxY(const float* height_values, size_t cols, 
	size_t row, size_t col, size_t scale, float& min_y, float& max_y)
{
	Assert(size_t(row) < cols);
	Assert(size_t(col) < cols);
	Assert(size_t(row + scale) <= cols);
	Assert(size_t(col + scale) <= cols);

	const float* p = height_values + row * cols + col;
	min_y = *p;
	max_y = min_y;

	for (size_t r = 0; r < scale; ++r)
	{
		for (size_t c = 0; c < scale; ++c)
		{
			float val = p[c];
			
			if (val < min_y)
			{
				min_y = val;
			}
			else if (val > max_y)
			{
				max_y = val;
			}
		}

		p += cols;
	}
}

CHeightLoader::CHeightLoader()
{
	m_pOwner = NULL;
	m_nMirror = 0;
	m_nLodLevels = 0;
	m_nChunkScale = 0;
	m_nCollideScale = 0;
	m_fLeft = 0.0F;
	m_fTop = 0.0F;
	m_fUnitSize = 0.0F;
	m_fInitHeight = 0.0F;
	m_nRows = 0;
	m_nCols = 0;
	m_nChunkRows = 0;
	m_nChunkCols = 0;
	m_fMinY = 0.0F;
	m_fMaxY = 0.0F;

	m_Height.pValues = NULL;
	m_Height.pNormals = NULL;
	m_Chunk.pChunks = NULL;
	m_Water.nWaterNum = 0;
	m_Water.pWaterIndex = NULL;
	m_pBlockMinY = NULL;
	m_pBlockMaxY = NULL;
}

CHeightLoader::~CHeightLoader()
{
	if (m_Height.pValues)
	{
		CORE_FREE(m_Height.pValues, sizeof(float) * m_nRows * m_nCols);
	}

	if (m_Height.pNormals)
	{
		CORE_FREE(m_Height.pNormals, sizeof(unsigned int) * m_nRows * m_nCols);
	}

	if (m_Chunk.pChunks)
	{
		CORE_FREE(m_Chunk.pChunks, 
			sizeof(chunk_info_t) * m_nChunkRows * m_nChunkCols);
	}
	
	if (m_Water.pWaterIndex)
	{
		CORE_FREE(m_Water.pWaterIndex, m_nChunkRows * m_nChunkCols);
	}

	if (m_pBlockMinY)
	{
		size_t block_dims = m_nRows / m_nCollideScale;
		size_t block_num = block_dims * block_dims;
		
		CORE_FREE(m_pBlockMinY, sizeof(float) * block_num);
	}

	if (m_pBlockMaxY)
	{
		size_t block_dims = m_nRows / m_nCollideScale;
		size_t block_num = block_dims * block_dims;

		CORE_FREE(m_pBlockMaxY, sizeof(float) * block_num);
	}
}

void CHeightLoader::Release()
{
	if (this->DecRefs() == 0)
	{
		CORE_DELETE(this);
	}
}

void CHeightLoader::SetOwner(CTerrainZone* pOwner)
{
	Assert(pOwner != NULL);

	CTerrainHeight* pHeight = pOwner->GetHeight();
	Terrain* pTerrain = pOwner->GetTerrain();
	
	m_pOwner = pOwner;
	m_fLeft = pOwner->GetLeft();
	m_fTop = pOwner->GetTop();
	m_fUnitSize = pTerrain->GetUnitSize();
	m_fInitHeight = pTerrain->GetInitHeight();
	m_nLodLevels = pTerrain->GetLodLevels();
	m_nChunkScale = pTerrain->GetChunkScale();
	m_nRows = pHeight->GetRows();
	m_nCols = pHeight->GetCols();
	m_nChunkRows = m_nRows / m_nChunkScale;
	m_nChunkCols = m_nCols / m_nChunkScale;
}

void CHeightLoader::SetFileName(const char* file_name)
{
	Assert(file_name != NULL);
	
	m_strFileName = file_name;
}

void CHeightLoader::SetBitmap(const char* file_name, int mirror)
{
	Assert(file_name != NULL);

	m_strBitmap = file_name;
	m_nMirror = mirror;
}

void CHeightLoader::SetGroundFile(const char* file_name)
{
	Assert(file_name != NULL);

	m_strGroundFile = file_name;
}

void CHeightLoader::SetWaterFile(const char* file_name)
{
	Assert(file_name != NULL);

	m_strWaterFile = file_name;
}

void CHeightLoader::SetCollideScale(size_t value)
{
	m_nCollideScale = value;
}

bool CHeightLoader::LoadBitmapFile(const char* file_name, int mirror)
{
	Assert(file_name != NULL);
	Assert( (mirror >= 0) && (mirror < MIRROR_MAX) );
	
	FILE* fp = core_file::fopen(file_name, "rb");
	
	if (NULL == fp)
	{
		return false;
	}
	
	BITMAPFILEHEADER bmh;
	
	if (core_file::fread(&bmh, sizeof(bmh), 1, fp) != 1)
	{
		CORE_TRACE("(CHeightLoader::LoadBitmapFile)read header failed");
		CORE_TRACE(file_name);
		core_file::fclose(fp);
		return false;
	}
	
	BITMAPINFOHEADER bmi;
	
	if (core_file::fread(&bmi, sizeof(bmi), 1, fp) != 1)
	{
		CORE_TRACE("(CHeightLoader::LoadBitmapFile)read info header failed");
		CORE_TRACE(file_name);
		core_file::fclose(fp);
		return false;
	}
	
	if ((size_t(bmi.biWidth) < m_nCols) || (size_t(bmi.biHeight) < m_nRows))
	{
		CORE_TRACE("(CTerrainHeight::LoadBmp)file size error");
		CORE_TRACE(file_name);
		core_file::fclose(fp);
		return false;
	}
	
	const size_t SIZE1 = bmi.biWidth * bmi.biHeight * 3;
	unsigned char* buf = (unsigned char*)CORE_ALLOC(SIZE1);
	
	if (core_file::fread(buf, sizeof(unsigned char), SIZE1, fp) != SIZE1)
	{
		CORE_TRACE("(CHeightLoader::LoadBitmapFile)read data error");
		CORE_TRACE(file_name);
		CORE_FREE(buf, SIZE1);
		core_file::fclose(fp);
		return false;
	}

	core_file::fclose(fp);
	
	// 顶点的数据
	float* values = (float*)CORE_ALLOC(sizeof(float) * m_nRows * m_nCols);
	// 起始高度
	float low_y = 0.0F;
	// 单位高度
	float unit_y = 0.2F;
	
	if (mirror == MIRROR_NONE)
	{
		for (size_t row = 0; row < m_nRows; row++)
		{
			for (size_t col = 0; col < m_nCols; col++)
			{
				size_t index = row * bmi.biWidth + col;
				unsigned int v = buf[index * 3];

				values[row * m_nCols + col] = low_y + int(v) * unit_y;
			}
		}
	}
	else if (mirror == MIRROR_HORIZON)
	{
		for (size_t row = 0; row < m_nRows; row++)
		{
			for (size_t col = 0; col < m_nCols; col++)
			{
				size_t index = row * bmi.biWidth + (m_nCols - 1 - col);
				unsigned int v = buf[index * 3];
				
				values[row * m_nCols + col] = low_y + int(v) * unit_y;
			}
		}
	}
	else if (mirror == MIRROR_VERTICAL)
	{
		for (size_t row = 0; row < m_nRows; row++)
		{
			for (size_t col = 0; col < m_nCols; col++)
			{
				size_t index = (m_nRows - 1 - row) * bmi.biWidth + col;
				unsigned int v = buf[index * 3];
				
				values[row * m_nCols + col] = low_y + int(v) * unit_y;
			}
		}
	}
	else if (mirror == MIRROR_H_V)
	{
		for (size_t row = 0; row < m_nRows; row++)
		{
			for (size_t col = 0; col < m_nCols; col++)
			{
				size_t index = (m_nRows - 1 - row) * bmi.biWidth 
					+ (m_nCols - 1 - col);
				unsigned int v = buf[index * 3];
				
				values[row * m_nCols + col] = low_y + int(v) * unit_y;
			}
		}
	}

	CORE_FREE(buf, SIZE1);
	m_Height.pValues = values;

	return true;
}

bool CHeightLoader::LoadInitHeight()
{
	if (m_Height.pValues)
	{
		CORE_FREE(m_Height.pValues, sizeof(float) * m_nRows * m_nCols);
		m_Height.pValues = NULL;
	}
	
	size_t count = m_nRows * m_nCols;
	float init_height = m_fInitHeight;
	// 初始地形的法线一定是朝上的
	unsigned int up_normal = COLOR_ARGB(255, 128, 255, 128);
	float* pValues = (float*)CORE_ALLOC(sizeof(float) * count);
	unsigned int* pNormals = (unsigned int*)CORE_ALLOC(
		sizeof(unsigned int) * count);

	for (size_t i = 0; i < count; ++i)
	{
		pValues[i] = init_height;
		pNormals[i] = up_normal;
	}

	m_Height.pValues = pValues;
	m_Height.pNormals = pNormals;
	
	return true;
}

// 优化的地面法线计算
bool CHeightLoader::CalcNormals()
{
	//double t1 = performance_time();
	// 现在的算法计算一个地块约需2.5毫秒
	
	size_t rows = m_nRows;
	size_t cols = m_nCols;
	size_t count = rows * cols;
	float* heights = m_Height.pValues;
	unsigned int* normals = (unsigned int*)CORE_ALLOC(
		sizeof(unsigned int) * count);

	memset(normals, 0, sizeof(unsigned int) * count);

	// 只能计算非边缘顶点的法线
	FmVec3 n;
	FmVec3 n0;
	FmVec3 n1;
	FmVec3 n2;
	FmVec3 n3;
	size_t rows_2 = rows - 2;
	size_t cols_2 = cols - 2;

	for (size_t r = 1; r < rows_2; r += 2)
	{
		for (size_t c = 1; c < cols_2; c += 2)
		{
			// 一次循环计算四个顶点
			size_t index = r * cols + c;
			size_t index1 = index - 1;
			size_t index0 = index1 - cols;
			size_t index2 = index1 + cols;
			size_t index3 = index2 + cols;

			float y00 = heights[index0 + 0];
			float y01 = heights[index0 + 1];
			float y02 = heights[index0 + 2];
			float y03 = heights[index0 + 3];
			float y10 = heights[index1 + 0];
			float y11 = heights[index1 + 1];
			float y12 = heights[index1 + 2];
			float y13 = heights[index1 + 3];
			float y20 = heights[index2 + 0];
			float y21 = heights[index2 + 1];
			float y22 = heights[index2 + 2];
			float y23 = heights[index2 + 3];
			float y30 = heights[index3 + 0];
			float y31 = heights[index3 + 1];
			float y32 = heights[index3 + 2];
			float y33 = heights[index3 + 3];
			
			n0.x = y00 + y10 * 2 + y20 - y02 - y12 * 2 - y22;
			n0.y = 8.0F;
			n0.z = y00 + y01 * 2 + y02 - y20 - y21 * 2 - y22;

			n1.x = y01 + y11 * 2 + y21 - y03 - y13 * 2 - y23;
			n1.y = 8.0F;
			n1.z = y01 + y02 * 2 + y03 - y21 - y22 * 2 - y23;

			n2.x = y10 + y20 * 2 + y30 - y12 - y22 * 2 - y32;
			n2.y = 8.0F;
			n2.z = y10 + y11 * 2 + y12 - y30 - y31 * 2 - y32;

			n3.x = y11 + y21 * 2 + y31 - y13 - y23 * 2 - y33;
			n3.y = 8.0F;
			n3.z = y11 + y12 * 2 + y13 - y31 - y32 * 2 - y33;

			FmVec3Normalize(&n0, &n0);
			FmVec3Normalize(&n1, &n1);
			FmVec3Normalize(&n2, &n2);
			FmVec3Normalize(&n3, &n3);

			int x_part0 = VisUtil_FloatToInt((n0.x + 1.0F) * 0.5F * 255.0F);
			int y_part0 = VisUtil_FloatToInt(n0.y * 255.0F);
			int z_part0 = VisUtil_FloatToInt((n0.z + 1.0F) * 0.5F * 255.0F);

			int x_part1 = VisUtil_FloatToInt((n1.x + 1.0F) * 0.5F * 255.0F);
			int y_part1 = VisUtil_FloatToInt(n1.y * 255.0F);
			int z_part1 = VisUtil_FloatToInt((n1.z + 1.0F) * 0.5F * 255.0F);

			int x_part2 = VisUtil_FloatToInt((n2.x + 1.0F) * 0.5F * 255.0F);
			int y_part2 = VisUtil_FloatToInt(n2.y * 255.0F);
			int z_part2 = VisUtil_FloatToInt((n2.z + 1.0F) * 0.5F * 255.0F);

			int x_part3 = VisUtil_FloatToInt((n3.x + 1.0F) * 0.5F * 255.0F);
			int y_part3 = VisUtil_FloatToInt(n3.y * 255.0F);
			int z_part3 = VisUtil_FloatToInt((n3.z + 1.0F) * 0.5F * 255.0F);

			//Assert((x_part >= 0) && (x_part <= 255));
			//Assert((y_part >= 0) && (y_part <= 255));
			//Assert((z_part >= 0) && (z_part <= 255));

			normals[index] = COLOR_ARGB(255, x_part0, y_part0, z_part0);
			normals[index + 1] = COLOR_ARGB(255, x_part1, y_part1, z_part1);
			normals[index + cols] = COLOR_ARGB(255, x_part2, y_part2, z_part2);
			normals[index + cols + 1] = COLOR_ARGB(255, x_part3, y_part3, z_part3);
		}
	}

	for (size_t r1 = 1; r1 <= rows_2; ++r1)
	{
		size_t index = r1 * cols + cols_2;
		size_t index_up = index - cols;
		size_t index_down = index + cols;

		float y00 = heights[index_up - 1];
		float y01 = heights[index_up];
		float y02 = heights[index_up + 1];
		float y10 = heights[index - 1];
		float y12 = heights[index + 1];
		float y20 = heights[index_down - 1];
		float y21 = heights[index_down];
		float y22 = heights[index_down + 1];

		n.x = y00 + y10 * 2 + y20 - y02 - y12 * 2 - y22;
		n.y = 8.0F;
		n.z = y00 + y01 * 2 + y02 - y20 - y21 * 2 - y22;

		FmVec3Normalize(&n, &n);

		int x_part = VisUtil_FloatToInt((n.x + 1.0F) * 0.5F * 255.0F);
		int y_part = VisUtil_FloatToInt(n.y * 255.0F);
		int z_part = VisUtil_FloatToInt((n.z + 1.0F) * 0.5F * 255.0F);

		Assert((x_part >= 0) && (x_part <= 255));
		Assert((y_part >= 0) && (y_part <= 255));
		Assert((z_part >= 0) && (z_part <= 255));

		normals[index] = COLOR_ARGB(255, x_part, y_part, z_part);
	}

	for (size_t c1 = 1; c1 < cols_2; ++c1)
	{
		size_t index = rows_2 * cols + c1;
		size_t index_up = index - cols;
		size_t index_down = index + cols;

		float y00 = heights[index_up - 1];
		float y01 = heights[index_up];
		float y02 = heights[index_up + 1];
		float y10 = heights[index - 1];
		float y12 = heights[index + 1];
		float y20 = heights[index_down - 1];
		float y21 = heights[index_down];
		float y22 = heights[index_down + 1];

		n.x = y00 + y10 * 2 + y20 - y02 - y12 * 2 - y22;
		n.y = 8.0F;
		n.z = y00 + y01 * 2 + y02 - y20 - y21 * 2 - y22;

		FmVec3Normalize(&n, &n);

		int x_part = VisUtil_FloatToInt((n.x + 1.0F) * 0.5F * 255.0F);
		int y_part = VisUtil_FloatToInt(n.y * 255.0F);
		int z_part = VisUtil_FloatToInt((n.z + 1.0F) * 0.5F * 255.0F);

		Assert((x_part >= 0) && (x_part <= 255));
		Assert((y_part >= 0) && (y_part <= 255));
		Assert((z_part >= 0) && (z_part <= 255));

		normals[index] = COLOR_ARGB(255, x_part, y_part, z_part);
	}

	m_Height.pNormals = normals;

	//double t2 = performance_time() - t1;
	//char info[128];
	//SafeSprintf(info, sizeof(info), 
	//	"(CHeightLoader::CalcNormals)use %f seconds", t2);
	//CORE_TRACE(info);

	return true;
}

/*
bool CHeightLoader::CalcNormals()
{
	size_t rows = m_nRows;
	size_t cols = m_nCols;
	size_t count = rows * cols;

	unsigned int* normals = NEW unsigned int[count];

	memset(normals, 0, sizeof(unsigned int) * count);
	
	float* heights = m_Height.pValues;

	// 只能计算非边缘顶点的法线
	FmVec4 n;
	size_t rows_1 = rows - 1;
	size_t cols_1 = cols - 1;

	for (size_t r = 1; r < rows_1; ++r)
	{
		for (size_t c = 1; c < cols_1; ++c)
		{
			size_t index = r * cols + c;
			size_t index_up = index - cols;
			size_t index_down = index + cols;
			
			float y00 = heights[index_up - 1];
			float y01 = heights[index_up];
			float y02 = heights[index_up + 1];
			float y10 = heights[index - 1];
			float y12 = heights[index + 1];
			float y20 = heights[index_down - 1];
			float y21 = heights[index_down];
			float y22 = heights[index_down + 1];
			
			n.x = y00 + y10 * 2 + y20 - y02 - y12 * 2 - y22;
			n.y = 8.0F;
			n.z = y00 + y01 * 2 + y02 - y20 - y21 * 2 - y22;

			FmVec3Normalize(&n, &n);

			int x_part = int((n.x + 1.0F) * 0.5F * 255.0F);
			int y_part = int(n.y * 255.0F);
			int z_part = int((n.z + 1.0F) * 0.5F * 255.0F);

			Assert((x_part >= 0) && (x_part <= 255));
			Assert((y_part >= 0) && (y_part <= 255));
			Assert((z_part >= 0) && (z_part <= 255));
			
			//if (x_part < 0) x_part = 0;
			//if (x_part > 255) x_part = 255;
			//if (y_part < 0) y_part = 0;
			//if (y_part > 255) y_part = 255;
			//if (z_part < 0) z_part = 0;
			//if (z_part > 255) z_part = 255;

			normals[index] = D3DCOLOR_ARGB(255, x_part, y_part, z_part);
		}
	}

	m_Height.pNormals = normals;
	
	return true;
}
*/

bool CHeightLoader::CalcChunkInfo()
{
	// 计算每个分块的必要数据
	size_t chunk_num = m_nChunkRows * m_nChunkCols;
	chunk_info_t* chunks = (chunk_info_t*)CORE_ALLOC(
		sizeof(chunk_info_t) * chunk_num);
	
	memset(chunks, 0, sizeof(chunk_info_t) * chunk_num);
	
	for (size_t r = 0; r < m_nChunkRows; ++r)
	{
		for (size_t c = 0; c < m_nChunkCols; ++c)
		{
			chunk_info_t* chunk = chunks + r * m_nChunkCols + c;
			size_t row = r * m_nChunkScale;
			size_t col = c * m_nChunkScale;

			CalcChunkErrors(m_Height.pValues, m_nCols, row, col, 
				m_nChunkScale, m_nLodLevels, chunk->fErrors);
			CalcChunkBoundBox(m_Height.pValues, m_nCols, row, col, 
				m_nChunkScale, m_fLeft, m_fTop, m_fUnitSize, chunk->vMin, 
				chunk->vMax);
		}
	}
	
	m_Chunk.pChunks = chunks;
	
	return true;
}

bool CHeightLoader::InitChunkInfo()
{
	// 计算每个分块的必要数据
	size_t chunk_num = m_nChunkRows * m_nChunkCols;
	chunk_info_t* chunks = (chunk_info_t*)CORE_ALLOC(
		sizeof(chunk_info_t) * chunk_num);
	
	memset(chunks, 0, sizeof(chunk_info_t) * chunk_num);
	
	for (size_t r = 0; r < m_nChunkRows; ++r)
	{
		for (size_t c = 0; c < m_nChunkCols; ++c)
		{
			chunk_info_t* chunk = chunks + r * m_nChunkCols + c;
			size_t row = r * m_nChunkScale;
			size_t col = c * m_nChunkScale;
			
			chunk->vMin.x = m_fLeft + col * m_fUnitSize;
			chunk->vMin.y = m_fInitHeight;
			chunk->vMin.z = m_fTop + row * m_fUnitSize;
			chunk->vMax.x = chunk->vMin.x + m_nChunkScale * m_fUnitSize;
			chunk->vMax.y = m_fInitHeight;
			chunk->vMax.z = chunk->vMin.z + m_nChunkScale * m_fUnitSize;
		}
	}
	
	m_Chunk.pChunks = chunks;
	
	return true;
}

bool CHeightLoader::LoadBlockHeight(const char* file_name)
{
	Assert(file_name != NULL);
	
	CBlockFile block_file(TERRAIN_FILE_SIGNATURE, CreateBlockReader, this);
	
	return block_file.Load(file_name);
}

bool CHeightLoader::LoadBlockWater(const char* file_name)
{
	Assert(file_name != NULL);
	
	CBlockFile block_file(TERRAIN_FILE_SIGNATURE, CreateBlockReader, this);
	
	return block_file.Load(file_name);
}

const char* CHeightLoader::GetName()
{
	if (m_strFileName.empty())
	{
		return m_strGroundFile.c_str();
	}
	else
	{
		return m_strFileName.c_str();
	}
}

bool CHeightLoader::Load(bool async)
{
	if (!m_strFileName.empty())
	{
		// 加载高度文件
		LoadBlockHeight(m_strFileName.c_str());
	}

	if ((NULL == m_Height.pValues) && (!m_strGroundFile.empty()))
	{
		// 加载压缩高度文件
		LoadBlockHeight(m_strGroundFile.c_str());
	}

	if ((NULL == m_Height.pValues) && (!m_strBitmap.empty()))
	{
		// 加载高度位图
		LoadBitmapFile(m_strBitmap.c_str(), m_nMirror);
	}

	if (NULL == m_Height.pValues)
	{
		if (LoadInitHeight())
		{
			InitChunkInfo();
		}
	}

	if (NULL == m_Height.pValues)
	{
		return false;
	}

	if (NULL == m_Height.pNormals)
	{
		CalcNormals();
	}
	
	if (NULL == m_Chunk.pChunks)
	{
		CalcChunkInfo();
	}

	if (!m_strWaterFile.empty())
	{
		LoadBlockWater(m_strWaterFile.c_str());
	}

	/*
	// 计算最高点和最低点
	size_t num = m_nRows * m_nCols;
	float* heights = m_Height.pValues;

	m_fMinY = heights[0];
	m_fMaxY = heights[0];

	for (size_t i = 0; i < num; ++i)
	{
		if (heights[i] < m_fMinY)
		{
			m_fMinY = heights[i];
		}

		if (heights[i] > m_fMaxY)
		{
			m_fMaxY = heights[i];
		}
	}
	*/

	// 计算局部地块的最高点和最低点
	float* heights = m_Height.pValues;
	size_t height_cols = m_nRows;
	size_t block_dims = m_nRows / m_nCollideScale;
	size_t block_num = block_dims * block_dims;

	m_pBlockMinY = (float*)CORE_ALLOC(sizeof(float) * block_num);
	m_pBlockMaxY = (float*)CORE_ALLOC(sizeof(float) * block_num);

	for (size_t r = 0; r < block_dims; ++r)
	{
		for (size_t c = 0; c < block_dims; ++c)
		{
			size_t index = r * block_dims + c;

			CalcBlockMinMaxY(heights, height_cols, r * m_nCollideScale,
				c * m_nCollideScale, m_nCollideScale + 1, 
				m_pBlockMinY[index], m_pBlockMaxY[index]);
		}
	}

	// 计算最高点和最低点
	m_fMinY = m_pBlockMinY[0];
	m_fMaxY = m_pBlockMaxY[0];

	for (size_t k = 1; k < block_num; ++k)
	{
		if (m_pBlockMinY[k] < m_fMinY)
		{
			m_fMinY = m_pBlockMinY[k];
		}

		if (m_pBlockMaxY[k] > m_fMaxY)
		{
			m_fMaxY = m_pBlockMaxY[k];
		}
	}

	return true;
}

bool CHeightLoader::Create(bool async)
{
	CTerrainHeight* pHeight = m_pOwner->GetHeight();
	
	if (!pHeight->Build(m_nRows, m_nCols, m_fMinY, m_fMaxY, m_Height.pValues, 
		m_Height.pNormals))
	{
		return false;
	}

	// 数据所有权已经转移
	m_Height.pValues = NULL;
	m_Height.pNormals = NULL;

	if (!pHeight->BuildBlock(m_nCollideScale, m_nRows / m_nCollideScale,
		m_pBlockMinY, m_pBlockMaxY))
	{
		return false;
	}

	// 数据所有权已经转移
	m_pBlockMinY = NULL;
	m_pBlockMaxY = NULL;
	
	CTerrainWater* pWater = m_pOwner->GetTerrain()->GetWater();
	// 获得水面的索引值
	int water_index[256];
	
	for (size_t i = 0; i < m_Water.nWaterNum; ++i)
	{
		const char* water_name = m_Water.WaterNames.GetByIndex(i);
		size_t index;
		
		if (pWater->FindIndex(water_name, index))
		{
			water_index[i] = (int)index;
		}
		else
		{
			water_index[i] = -1;
			CORE_TRACE("(CHeightLoader::Create)water not found");
			CORE_TRACE(water_name);
			CORE_TRACE(m_strWaterFile.c_str());
		}
	}
	
	for (size_t r = 0; r < m_nChunkRows; ++r)
	{
		for (size_t c = 0; c < m_nChunkCols; ++c)
		{
			chunk_info_t* chunk = m_Chunk.pChunks + r * m_nChunkCols + c;
			CTerrainChunk* pChunk = m_pOwner->GetChunk(r, c);

      //ls test water
      //pChunk->SetWaterIndex(c);
			if (!pChunk->Build(chunk->vMin, chunk->vMax, chunk->fErrors, 
				m_nLodLevels))
			{
				return false;
			}

			if (m_Water.pWaterIndex != NULL)
			{
				unsigned char index = 
					m_Water.pWaterIndex[r * m_nChunkCols + c];
			
				pChunk->SetWaterIndex(water_index[index]);
			}
		}
	}

	m_pOwner->SetHeightLoaded(true);
	
	return true;
}

bool CHeightLoader::EndCreate(int category, bool succeed)
{
	return m_pOwner->EndCreate(category, succeed);
}

bool CHeightLoader::SaveBlockHeight(const char* file_name, 
	CTerrainZone* pZone)
{
	Assert(file_name != NULL);
	Assert(pZone != NULL);
	
	CBlockFile block_file(TERRAIN_FILE_SIGNATURE, NULL, NULL);
	void* handle = block_file.SaveBegin(file_name);
	
	if (NULL == handle)
	{
		return false;
	}
	
	CBlockHeightWriter writer(pZone);
	
	if (!block_file.SaveBlock(handle, BLOCK_ID_HEIGHT_0, &writer))
	{
		return false;
	}

	CBlockChunkWriter writer1(pZone);
	
	if (!block_file.SaveBlock(handle, BLOCK_ID_CHUNK_0, &writer1))
	{
		return false;
	}
	
	return block_file.SaveEnd(handle);
}

bool CHeightLoader::SaveBlockHeightCompress(const char* file_name, 
	CTerrainZone* pZone)
{
	Assert(file_name != NULL);
	Assert(pZone != NULL);

	CBlockFile block_file(TERRAIN_FILE_SIGNATURE, NULL, NULL);

	void* handle = block_file.SaveBegin(file_name);

	if (NULL == handle)
	{
		return false;
	}

	CBlockHeight2Writer writer(pZone);

	if (!block_file.SaveBlock(handle, BLOCK_ID_HEIGHT_2, &writer))
	{
		return false;
	}

	CBlockChunkWriter writer1(pZone);

	if (!block_file.SaveBlock(handle, BLOCK_ID_CHUNK_0, &writer1))
	{
		return false;
	}

	return block_file.SaveEnd(handle);
}

bool CHeightLoader::SaveBlockWater(const char* file_name, 
	CTerrainZone* pZone)
{
	Assert(file_name != NULL);
	Assert(pZone != NULL);
	
	size_t count = 0;
	
	size_t chunk_num = pZone->GetChunkCount();
	
	for (size_t k = 0; k < chunk_num; ++k)
	{
		CTerrainChunk* pChunk = pZone->GetChunkByIndex(k);
		
		if (pChunk->GetWaterIndex() >= 0)
		{
			count++;
		}
	}
	
	if (0 == count)
	{
		g_pCore->GetFileSys()->FileDelete(file_name);
		return true;
	}
	
	CBlockFile block_file(TERRAIN_FILE_SIGNATURE, NULL, NULL);
	
	void* handle = block_file.SaveBegin(file_name);
	
	if (NULL == handle)
	{
		return false;
	}
	
	CBlockWaterWriter writer(pZone);
	
	if (!block_file.SaveBlock(handle, BLOCK_ID_WATER_0, &writer))
	{
		return false;
	}
	
	return block_file.SaveEnd(handle);
}

