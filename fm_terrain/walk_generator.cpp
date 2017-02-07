//--------------------------------------------------------------------
// 文件名:		walk_generator.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年8月5日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "walk_generator.h"
#include "walk_height_compress.h"
#include "terrain.h"
#include "terrain_zone.h"
#include "terrain_walkable.h"
#include "../visual/vis_utils.h"
#include "../public/var.h"
#include "../public/auto_mem.h"
#include "../public/core_log.h"
#include "../public/core_file.h"

// 高度是否近似
#define HEIGHT_EQUAL(v1, v2) ((v1 <= (v2 + 0.0001F)) && (v1 >= (v2 - 0.0001F)))

// 高度图
struct CWalkGenerator::height_map_t
{
	size_t nRows;
	size_t nCols;
	float* pValues;

	height_map_t(size_t rows, size_t cols)
	{
		nRows = rows;
		nCols = cols;

		size_t size = rows * cols;

		pValues = (float*)CORE_ALLOC(sizeof(float) * size);
		
		for (size_t i = 0; i < size; ++i)
		{
			pValues[i] = HEIGHT_NULL;
		}
	}

	~height_map_t()
	{
		if (pValues)
		{
			CORE_FREE(pValues, sizeof(float) * nRows * nCols);
		}
	}

	static height_map_t* NewInstance(size_t rows, size_t cols)
	{
		height_map_t* p = (height_map_t*)CORE_ALLOC(sizeof(height_map_t));

		new (p) height_map_t(rows, cols);

		return p;
	}

	void Release()
	{
		CORE_DELETE(this);
	}

private:
	height_map_t();
	height_map_t(const height_map_t&);
	height_map_t& operator=(const height_map_t&);
};

// 标记图
struct CWalkGenerator::marker_map_t
{
	size_t nRows;
	size_t nCols;
	unsigned char* pValues;

	marker_map_t(size_t rows, size_t cols)
	{
		nRows = rows;
		nCols = cols;
		
		size_t size = rows * cols;

		pValues = (unsigned char*)CORE_ALLOC(size);
		memset(pValues, 0, size);
	}

	~marker_map_t()
	{
		if (pValues)
		{
			CORE_FREE(pValues, nRows * nCols);
		}
	}

	static marker_map_t* NewInstance(size_t rows, size_t cols)
	{
		marker_map_t* p = (marker_map_t*)CORE_ALLOC(sizeof(marker_map_t));

		new (p) marker_map_t(rows, cols);

		return p;
	}

	void Release()
	{
		CORE_DELETE(this);
	}

private:
	marker_map_t();
	marker_map_t(const marker_map_t&);
	marker_map_t& operator=(const marker_map_t&);
};

// CWalkGenerator

CWalkGenerator::CWalkGenerator(Terrain* pTerrain, CTerrainZone* pZone,
	int precision)
{
	Assert(pTerrain != NULL);
	Assert(pZone != NULL);

	size_t collide_per_unit = pTerrain->GetCollidePerUnit();
	size_t scale = pTerrain->GetZoneScale() * collide_per_unit * precision;

	m_pTerrain = pTerrain;
	m_pZone = pZone;
	m_nRows = scale;
	m_nCols = scale;
	m_fStep = pTerrain->GetUnitSize() / collide_per_unit / precision;
	m_nPrecision = precision;
	
	m_fRoleHeight = 1.0F;
	m_fRoleRadius = 0.52F;
	m_fStepHeight = 0.5F;
	m_fTraceHigh = 500.0F;
	m_fTraceLow = -500.0F;
	m_fGroundNormalY = 0.7F;
	m_fFloorNormalY = 0.34F;
	m_fWallAngle = 15.0F;
	
	size_t size = m_nRows * m_nCols;

	m_pDesign = (design_data_t*)CORE_ALLOC(sizeof(design_data_t) * size);
	memset(m_pDesign, 0, sizeof(design_data_t) * size);

	for (size_t k = 0; k < size; ++k)
	{
		m_pDesign[k].fHeight = HEIGHT_NULL;
	}

	m_pGroundHeight = NULL;
	m_pWaterHeight = NULL;
	m_pWalkMarker = NULL;

	for (size_t i = 0; i < MAX_FLOOR_NUM; ++i)
	{
		m_pFloorHeights[i] = NULL;
		m_pSpaceHeights[i] = NULL;
		m_pFloorMarkers[i] = NULL;
	}

	m_pWalkGeneratorTraceFile = NULL;
}

CWalkGenerator::~CWalkGenerator()
{
	ReleaseAll();
}

void CWalkGenerator::ReleaseAll()
{
	if (m_pDesign)
	{
		size_t size = m_nRows * m_nCols;

		for (size_t i = 0; i < size; ++i)
		{
			if (m_pDesign[i].pTrace)
			{
				CORE_FREE(m_pDesign[i].pTrace, 
					sizeof(trace_data_t) * m_pDesign[i].nTraceCount);
			}
		}
		
		CORE_FREE(m_pDesign, sizeof(design_data_t) * m_nRows * m_nCols);
		m_pDesign = NULL;
	}

	SAFE_RELEASE(m_pGroundHeight);
	SAFE_RELEASE(m_pWaterHeight);
	SAFE_RELEASE(m_pWalkMarker);

	for (size_t i = 0; i < MAX_FLOOR_NUM; ++i)
	{
		SAFE_RELEASE(m_pFloorHeights[i]);
		SAFE_RELEASE(m_pSpaceHeights[i]);
		SAFE_RELEASE(m_pFloorMarkers[i]);
	}

	if (m_pWalkGeneratorTraceFile)
	{
		core_file::fclose(m_pWalkGeneratorTraceFile);
		m_pWalkGeneratorTraceFile = NULL;
	}

	fence_container_t::iterator it = m_FencePoints.Begin();
	fence_container_t::iterator end = m_FencePoints.End();

	for (; it != end; ++it)
	{
		CORE_DELETE(it.GetData());
	}

	m_FencePoints.Clear();
}

void CWalkGenerator::SetHeight(height_map_t* pHeight, size_t row, size_t col, 
	float value)
{
	pHeight->pValues[row * pHeight->nCols + col] = value;
}

void CWalkGenerator::SetMarker(marker_map_t* pMarker, size_t row, size_t col, 
	unsigned char value)
{
	pMarker->pValues[row * pMarker->nCols + col] = value;
}

bool CWalkGenerator::RoleCollide(float x, float y, float z)
{
	//FmVec4 center(x, y + m_fRoleHeight * 0.5F, z);
	//return m_pTerrain->TraceSphere(center, m_fRoleRadius);
	FmVec3 center(x, y, z);
	float radius = m_fRoleRadius * 0.5F;

	// 上部
	center.y = y + m_fRoleHeight - 0.1F - radius * 0.5F;

	if (m_pTerrain->TraceSphere(center, radius))
	{
		return true;
	}

	// 中间
	center.y = y + m_fRoleHeight * 0.5F;

	return m_pTerrain->TraceSphere(center, m_fRoleRadius);
}

bool CWalkGenerator::IsPermitted(float x, float y, float z)
{
	if (m_PermitList.empty())
	{
		return false;
	}

	bool result = false;

	// 忽略辅助物体的碰撞
	bool old_track_mask = m_pTerrain->GetTraceMask("Helper");

	m_pTerrain->SetTraceMask("Helper", true);

	float radius = m_fStep;// * 0.5F;

	FmVec3 src(x, y + 0.001F, z);

	for (size_t i = 0; i < m_PermitList.size(); ++i)
	{
		FmVec3 dst = m_PermitList[i];
		FmVec3 vectemp = dst - src;

		if (FmVec3LengthSq(&vectemp) > (radius * radius))
		{
			// 不在范围内
			continue;
		}

		if (!m_pTerrain->TraceHitted(src, dst))
		{
			// 在允许范围之内
			result = true;
			break;
		}
	}

	m_pTerrain->SetTraceMask("Helper", old_track_mask);

	return result;
}

bool CWalkGenerator::IsForbidden(float x, float y, float z)
{
	if (m_ForbidList.empty())
	{
		return false;
	}

	bool result = false;

	// 忽略辅助物体的碰撞
	bool old_track_mask = m_pTerrain->GetTraceMask("Helper");

	m_pTerrain->SetTraceMask("Helper", true);
	
	FmVec3 src(x, y + m_fRoleHeight * 0.5F, z);

	for (size_t i = 0; i < m_ForbidList.size(); ++i)
	{
		FmVec3 dst = m_ForbidList[i].pos;
		float radius = m_ForbidList[i].fRadius;

		FmVec3 vec3Temp = dst - src;
		if (FmVec3LengthSq(&vec3Temp) > (radius * radius))
		{
			// 不在范围内
			continue;
		}

		if (!m_pTerrain->TraceHitted(src, dst))
		{
			// 在禁止范围之内
			result = true;
			break;
		}
	}
	
	m_pTerrain->SetTraceMask("Helper", old_track_mask);

	return result;
}

bool CWalkGenerator::AddPermit(float x, float y, float z)
{
	m_PermitList.push_back(FmVec3(x, y, z));
	
	return true;
}

bool CWalkGenerator::AddForbid(float x, float y, float z, float radius)
{
	forbid_t data;

	data.pos.x = x;
	data.pos.y = y;
	data.pos.z = z;
	data.fRadius = radius;

	m_ForbidList.push_back(data);
	
	return true;
}

bool CWalkGenerator::AddFencePoint(int fence_index, float x, float z)
{
	//m_FencePoints.push_back(FmVec2(x, z));

//	std::map<int, std::vector<FmVec2>>::iterator iter = 
//		m_FencePoints.find(fence_index);
//
//	if (iter == m_FencePoints.end())
//	{
//		std::vector<FmVec2> vData;
//
//		std::pair<std::map<int, std::vector<FmVec2>>::iterator, bool> 
//			pid = m_FencePoints.insert(make_pair(fence_index, vData));
//
//		if (!pid.second)
//		{
//			// 添加失败
//			return false;
//		}
//
//		iter = m_FencePoints.find(fence_index);
//
//		iter->second.push_back(FmVec2(x, z));
//	}
//	else
//	{
//		iter->second.push_back(FmVec2(x, z));
//	}
	point_container_t* points;
	
	if (!m_FencePoints.GetData(fence_index, points))
	{
		points = CORE_NEW(point_container_t);
		m_FencePoints.Add(fence_index, points);
	}

	points->push_back(FmVec2(x, z));

	return true;
}

bool CWalkGenerator::GenerateBegin()
{
	//m_ForbidList.clear();

	return true;
}

bool CWalkGenerator::GenerateEnd()
{
	CTerrainWalkable* pWalkable = m_pZone->GetWalkable();

	pWalkable->ReleaseAll();

	size_t rows = m_nRows / m_nPrecision;
	size_t cols = m_nCols / m_nPrecision;
	TAutoMem<float, 1, TCoreAlloc> auto_buf(rows * cols);

	// 地表高度
	UpdateHeightMap(m_pGroundHeight, auto_buf.GetBuffer(), 
		UPDATE_HEIGHT_AVERAGE);

	pWalkable->UpdateGroundHeight(rows, cols, auto_buf.GetBuffer());

	// 水面高度
	UpdateHeightMap(m_pWaterHeight, auto_buf.GetBuffer(), 
		UPDATE_HEIGHT_MAXIMUM);
	
	pWalkable->UpdateWaterHeight(rows, cols, auto_buf.GetBuffer());

	// 主行走层标记
	TAutoMem<unsigned char, 1, TCoreAlloc> auto_buf1(rows * cols);

	UpdateFlagMap(m_pWalkMarker, auto_buf1.GetBuffer());

	pWalkable->UpdateWalkMarker(rows, cols, auto_buf1.GetBuffer());

	// 层高度
	int i;

	for (i = 0; i < MAX_FLOOR_NUM; ++i)
	{
		if (NULL == m_pFloorHeights[i])
		{
			continue;
		}

		UpdateHeightMap(m_pFloorHeights[i], auto_buf.GetBuffer(),
			UPDATE_HEIGHT_MAXIMUM);

		pWalkable->UpdateFloorHeight(i, rows, cols, auto_buf.GetBuffer());
	}

	// 层空间高度
	for (i = 0; i < MAX_FLOOR_NUM; ++i)
	{
		if (NULL == m_pSpaceHeights[i])
		{
			continue;
		}

		UpdateHeightMap(m_pSpaceHeights[i], auto_buf.GetBuffer(), 
			UPDATE_HEIGHT_MAXIMUM);

		pWalkable->UpdateSpaceHeight(i, rows, cols, auto_buf.GetBuffer());
	}

	// 可行走标记
	for (i = 0; i < MAX_FLOOR_NUM; ++i)
	{
		if (NULL == m_pFloorMarkers[i])
		{
			continue;
		}

		UpdateFlagMap(m_pFloorMarkers[i], auto_buf1.GetBuffer());

		pWalkable->UpdateFloorMarker(i, rows, cols, auto_buf1.GetBuffer());
	}

	pWalkable->SetReady(true);
	
	return true;
}

bool CWalkGenerator::UpdateHeightMap(height_map_t* pHeight, float* pValues,
	int mode)
{
	Assert(pHeight != NULL);
	Assert(pValues != NULL);

	bool is_null = true;
	
	size_t rows = m_nRows / m_nPrecision;
	size_t cols = m_nCols / m_nPrecision;
	
	for (int row = 0; row < int(m_nRows); row += m_nPrecision)
	{
		for (int col = 0; col < int(m_nCols); col += m_nPrecision)
		{
			int walk_count = 0;
			float walk_height;

			for (int r = 0; r < m_nPrecision; ++r)
			{
				for (int c = 0; c < m_nPrecision; ++c)
				{
					float h = pHeight->pValues[
						(row + r) * m_nCols + (col + c)];
					
					if (height_is_null(h))
					{
						continue;
					}

					if (mode == UPDATE_HEIGHT_AVERAGE)
					{
						// 累加
						if (0 == walk_count)
						{
							walk_height = 0;
						}

						walk_height += h;
					}
					else if (mode == UPDATE_HEIGHT_MAXIMUM)
					{
						// 取最大值
						if (0 == walk_count)
						{
							walk_height = -1e8;
						}

						if (h > walk_height)
						{
							walk_height = h;
						}
					}
					else
					{
						// 取最小值
						if (0 == walk_count)
						{
							walk_height = 1e8;
						}

						if (h < walk_height)
						{
							walk_height = h;
						}
					}

					walk_count++;
				}
			}

			size_t r1 = row / m_nPrecision;
			size_t c1 = col / m_nPrecision;

			if (walk_count > 0)
			{
				// 可行走的平均高度
				if (mode == UPDATE_HEIGHT_AVERAGE)
				{
					pValues[r1 * cols + c1] = walk_height / walk_count;
				}
				else
				{
					pValues[r1 * cols + c1] = walk_height;
				}

				is_null = false;
			}
			else
			{
				pValues[r1 * cols + c1] = HEIGHT_NULL;
			}
		}
	}
	
	return (!is_null);
}

bool CWalkGenerator::UpdateFlagMap(marker_map_t* pMarker, 
	unsigned char* pValues)
{
	Assert(pMarker != NULL);
	Assert(pValues != NULL);

	bool is_null = true;
	
	size_t rows = m_nRows / m_nPrecision;
	size_t cols = m_nCols / m_nPrecision;

	for (int row = 0; row < int(m_nRows); row += m_nPrecision)
	{
		for (int col = 0; col < int(m_nCols); col += m_nPrecision)
		{
			unsigned int val = 0;

			for (int r = 0; r < m_nPrecision; ++r)
			{
				for (int c = 0; c < m_nPrecision; ++c)
				{
					val |= pMarker->pValues[(row + r) * m_nCols + (col + c)];
				}
			}

			if (val != 0)
			{
				is_null = false;
			}

			// 可行走的平均高度
			size_t r1 = row / m_nPrecision;
			size_t c1 = col / m_nPrecision;

			pValues[r1 * cols + c1] = val;
		}
	}
	
	return (!is_null);
}

bool CWalkGenerator::GenerateGround()
{
	m_pGroundHeight = height_map_t::NewInstance(m_nRows, m_nCols);

	for (size_t row = 0; row < m_nRows; ++row)
	{
		for (size_t col = 0; col < m_nCols; ++col)
		{
			float x = m_pZone->GetLeft() + (col + 0.5F) * m_fStep;
			float z = m_pZone->GetTop() + (row + 0.5F) * m_fStep;
			float y = m_pTerrain->GetGroundHeight(x, z);
			
			SetHeight(m_pGroundHeight, row, col, y);
		}
	}

	return true;
}

bool CWalkGenerator::GenerateBalk(int row)
{
	if (size_t(row) >= m_nRows)
	{
		return false;
	}

	for (size_t col = 0; col < m_nCols; ++col)
	{
		TraceAllHeight(row, col);
	}

	if (size_t(row) == (m_nRows - 1))
	{
		// 消除一些较平坦表面由于立面造成的不可行走位置
		for (size_t r = 0; r < m_nRows; ++r)
		{
			for (size_t c = 0; c < m_nCols; ++c)
			{
				design_data_t* data = m_pDesign + r * m_nCols + c;

				for (int t = 0; t < data->nTraceCount; ++t)
				{
					trace_data_t* td = &data->pTrace[t];

					if (td->bNotFlat)
					{
						float h = td->fTraceY;
						float step = m_fRoleHeight * 0.5F;

						if (GetNearHeight(r - 1, c, h, step)
							&& GetNearHeight(r + 1, c, h, step)
							&& GetNearHeight(r, c - 1, h, step)
							&& GetNearHeight(r, c + 1, h, step))
						{
							// 消除此立面
							td->bNotFlat = false;
						}
					}
				}
			}
		}
	}

	return true;
}

bool CWalkGenerator::GetNearHeight(size_t row, size_t col, float height, 
	float step)
{
	if ((row >= m_nRows) || (col >= m_nCols))
	{
		return true;
	}
	
	design_data_t* data = m_pDesign + row * m_nCols + col;

	for (int t = 0; t < data->nTraceCount; ++t)
	{
		trace_data_t* td = &data->pTrace[t];

		if ((height <= (td->fTraceY + step))
			&& (height >= (td->fTraceY - step)))
		{
			return true;
		}
	}

	return false;
}

// 点是否在多边形之中
static bool point_in_polygon(const FmVec2& t, const FmVec2* v, int n)
{
	bool inside = false;
	FmVec2 e0 = v[n - 1];
	FmVec2 e1 = v[0];
	bool y0 = (e0.y >= t.y);

	for (int i = 1; i <= n; ++i)
	{
		bool y1 = (e1.y >= t.y);

		if (y0 != y1)
		{
			bool y2 = ((e1.y - t.y) * (e0.x - e1.x) 
				>= (e1.x - t.x) * (e0.y - e1.y));
			
			if (y2 == y1)
			{
				inside = !inside;
			}
		}

		if (i == n)
		{
			break;
		}

		y0 = y1;
		e0 = e1;
		e1 = v[i];
	}

	return inside;
}

// 碰撞点距离比较
static int __cdecl trace_hit_compare(const void* p1, const void* p2)
{
	trace_info_t::hit_t* pVal1 = (trace_info_t::hit_t*)p1;
	trace_info_t::hit_t* pVal2 = (trace_info_t::hit_t*)p2;

	if (pVal1->fDistance > pVal2->fDistance)
	{
		return 1;
	}

	if (pVal1->fDistance < pVal2->fDistance)
	{
		return -1;
	}

	return 0;
}

bool CWalkGenerator::TraceAllHeight(size_t row, size_t col)
{
	design_data_t* data = m_pDesign + row * m_nCols + col;

	float x = m_pZone->GetLeft() + (col + 0.5F) * m_fStep;
	float z = m_pZone->GetTop() + (row + 0.5F) * m_fStep;

	// 是否在场景栅栏之外
	bool out_fence = false;
	//size_t fence_point_num = m_FencePoints.size();

	//if (fence_point_num >= 3)
	//{
	//	out_fence = !point_in_polygon(FmVec2(x, z), &m_FencePoints[0],
	//		fence_point_num);
	//}

//	size_t fence_count = m_FencePoints.size();
//
//	if (fence_count > 0)
//	{
//		std::map<int, std::vector<FmVec2>>::iterator iter = 
//			m_FencePoints.begin();
//
//		while (iter != m_FencePoints.end())
//		{
//			std::vector<FmVec2> vData = iter->second;
//			size_t fence_point_num = vData.size();
//
//			if (fence_point_num >= 3)
//			{
//				out_fence = !point_in_polygon(FmVec2(x, z), &vData[0],
//					fence_point_num);
//
//				if (!out_fence)
//				{
//					break;
//				}
//			}
//
//			iter++;
//		}
//	}
	size_t fence_count = m_FencePoints.GetCount();

	if (fence_count > 0)
	{
		fence_container_t::iterator it = m_FencePoints.Begin();
		fence_container_t::iterator end = m_FencePoints.End();

		for (; it != end; ++it)
		{
			point_container_t& points = *(it.GetData());
			size_t fence_point_num = points.size();

			if (fence_point_num >= 3)
			{
				out_fence = !point_in_polygon(FmVec2(x, z), &points[0],
					(int)fence_point_num);

				if (!out_fence)
				{
					break;
				}
			}
		}
	}

	if (out_fence)
	{
		//Trace(x, 0.0F, z, -1, "是有效场景范围之外的点");
		// 是有效场景范围之外的点
		data->bOutFence = true;
		return true;
	}

	// 获得多个碰撞点
	FmVec3 src(x, m_fTraceHigh, z);
	FmVec3 dst(x, m_fTraceLow, z);

	// 使碰撞检测可以返回多个碰撞点
	char buffer[sizeof(trace_info_t) + sizeof(trace_info_t::hit_t) * 31];

	memset(buffer, 0, sizeof(buffer));

	trace_info_t* ti = (trace_info_t*)buffer;

	ti->nHitMax = 32;
	ti->nHitCount = 0;
	ti->fDistance = 1e8;

	m_pTerrain->GetRender()->SetTraceTextureName(true);

	if (!m_pTerrain->TraceDetail(src, dst, *ti))
	{
		m_pTerrain->GetRender()->SetTraceTextureName(false);
		return false;
	}

	m_pTerrain->GetRender()->SetTraceTextureName(false);

	// 排序碰撞点
	qsort(ti->Hits, ti->nHitCount, sizeof(trace_info_t::hit_t), 
		trace_hit_compare);

	if (32 == ti->nHitCount)
	{
		CORE_TRACE("(CWalkGenerator::TraceAllHeight)hit count may overflow!");
	}

	for (int i = 0; i < ti->nHitCount; ++i)
	{
		trace_info_t::hit_t* pHit = &ti->Hits[i];

		float y = src.y + (dst.y - src.y) * pHit->fDistance;

		FmVec3 v1 = pHit->Vertex[0];
		FmVec3 v2 = pHit->Vertex[1];
		FmVec3 v3 = pHit->Vertex[2];

		bool walkable = true;
		bool through = false;
		bool padding = false;
		bool face_down = false;
		bool forbid_stand = false;

		// 是否碰到物体
		if (pHit->TraceID != m_pTerrain->GetID())
		{
			walkable = false;
			through = true;

			IVisBase* pVisBase = (IVisBase*)
				m_pTerrain->GetCore()->GetEntity(pHit->TraceID);

			if (pVisBase)
			{
				// 是否可行走
				walkable = VisUtil_GetCustomBool(pVisBase, "co_walkable");
				// 是否可穿透
				through = VisUtil_GetCustomBool(pVisBase, "co_through");

				// 是否辅助物体
				bool helper = VisUtil_GetCustomBool(pVisBase, "only_design");
				// 是否生成顶部
				bool gen_roof = VisUtil_GetCustomBool(pVisBase, "co_gen_roof");

				// 是否填充物
				padding = helper && (!gen_roof) && (!walkable);

				if (pHit->strTexName)
				{
					if (NULL != strstr(pHit->strTexName, "forbid_stand"))
					{
						forbid_stand = true;
					}
				}

				// 转换顶点坐标
				FmMat4 mat;

				pVisBase->GetWorldMatrix(mat);

				FmVec3TransformCoord(&v1, &v1, &mat);
				FmVec3TransformCoord(&v2, &v2, &mat);
				FmVec3TransformCoord(&v3, &v3, &mat);
			}
		}
		else
		{
			// 更新地面高度
			SetHeight(m_pGroundHeight, row, col, y);
		}

		// 获得碰撞面的法线
		FmVec3 normal;

		FmVec3 vec3Temp1 = v2 - v1;
		FmVec3 vec3Temp2 = v3 - v1;
		FmVec3Cross(&normal, &vec3Temp1, &vec3Temp2);
		FmVec3Normalize(&normal, &normal);

		if (normal.y < 0.0F)
		{
			Trace(x, y, z, -1, "存在朝下的面");
			// 存在朝下的面
			face_down = true;
		}

		if (through)
		{
			Trace(x, y, z, -1, "可穿透物体");
			// 可穿透物体
			continue;
		}

		trace_data_t* td = NULL;

		if (data->nTraceCount > 0)
		{
			trace_data_t* prev_data = data->pTrace + data->nTraceCount - 1;
			
			// 是否与前一个高度近似
			if ((y <= (prev_data->fTraceY + 0.05F)) 
				&& (y >= (prev_data->fTraceY - 0.05F)))
			{
				if (normal.y < 0.0F)
				{
					Trace(x, y, z, -1, "作为上一层的补充数据");
					// 作为上一层的补充数据
					prev_data->bFaceDown = true;
					continue;
				}
				else
				{
					if (prev_data->vNormal.y > 0.0F)
					{
						// 放弃此层高度
						Trace(x, y, z, -1, "放弃此层高度");
						continue;
					}

					// 取代前一个高度
					td = prev_data;

					if (prev_data->bFaceDown)
					{
						Trace(x, y, z, -1, "取代前一个高度");
						face_down = true;
					}
				}
			}
		}

		if (NULL == td)
		{
			trace_data_t* new_trace = (trace_data_t*)CORE_ALLOC(
				sizeof(trace_data_t) * (data->nTraceCount + 1));

			if (data->nTraceCount > 0)
			{
				memcpy(new_trace, data->pTrace, 
					sizeof(trace_data_t) * data->nTraceCount);
				CORE_FREE(data->pTrace, 
					sizeof(trace_data_t) * data->nTraceCount);
			}

			data->nTraceCount++;
			data->pTrace = new_trace;

			td = data->pTrace + data->nTraceCount - 1;
		}

		// 是否平坦
		bool not_flat = false;

		if (pHit->TraceID == m_pTerrain->GetID())
		{
			if (normal.y < m_fGroundNormalY)
			{
				Trace(x, y, z, -1, "不平坦");
				not_flat = true;
			}
		}
		else
		{
			if (fabs(normal.y) < m_fFloorNormalY)
			{
				Trace(x, y, z, -1, "不平坦");
				not_flat = true;
			}
		}

		// 两层之间的空间高度
		float space_height = HEIGHT_NULL;

		if (data->nTraceCount >= 2)
		{
			space_height = data->pTrace[data->nTraceCount - 2].fTraceY - y;
		}

		bool role_collide = true;

		if ((space_height >= m_fRoleHeight) && (normal.y > 0.0F))
		{
			role_collide = RoleCollide(x, y, z);

			if (role_collide)
			{
				Trace(x, y, z, -1, "role collide");
			}
		}
		else
		{
			if (space_height < m_fRoleHeight)
			{
				Trace(x, y, z, -1, "role collide(space height < role height)");
			}

			if (normal.y <= 0.0F)
			{
				Trace(x, y, z, -1, "role collide(normal <= 0.0F)");
			}
		}

		bool is_permitted = false;
		bool is_forbidden = false;

		if (normal.y > 0.0F)
		{
			is_permitted = IsPermitted(x, y, z);
			is_forbidden = IsForbidden(x, y, z);

			if (is_forbidden)
			{
				Trace(x, y, z, -1, "forbiddden");
			}
		}
		else
		{
			Trace(x, y, z, -1, "朝下的面片不允许生成碰撞");
			// 朝下的面片不允许生成碰撞
			is_forbidden = true;
		}

		td->TraceID = pHit->TraceID;
		td->fTraceY = y;
		td->vNormal = normal;
		td->bFaceDown = face_down;
		td->bNotFlat = not_flat;
		td->bWalkable = walkable;
		td->bForbidStand = forbid_stand;
		td->bRoleCollide = role_collide;
		td->bPermitted = is_permitted;
		td->bForbidden = is_forbidden;
		td->bPadding = padding;

		if (padding)
		{
			// 已经到达填充物，填充物以下的物体高度将被忽略
			Trace(x, y, z, -1, "已经到达填充物，填充物以下的物体高度将被忽略");
			break;
		}

		if (pHit->TraceID == m_pTerrain->GetID())
		{
			// 已经到达地表
			break;
		}
	}

	if (data->nTraceCount > 0)
	{
		// 重新排序数组
		qsort(data->pTrace, data->nTraceCount, sizeof(trace_data_t), 
			HeightCompare);
	}

	return true;
}

int __cdecl CWalkGenerator::HeightCompare(const void* p1, const void* p2)
{
	trace_data_t* pVal1 = (trace_data_t*)p1;
	trace_data_t* pVal2 = (trace_data_t*)p2;

	if (pVal1->fTraceY > pVal2->fTraceY)
	{
		return 1;
	}

	if (pVal1->fTraceY < pVal2->fTraceY)
	{
		return -1;
	}

	return 0;
}

bool CWalkGenerator::GenerateWalkable(float start_x, float start_y, 
	float start_z)
{
	size_t col = size_t((start_x - m_pZone->GetLeft()) / m_fStep);
	size_t row = size_t((start_z - m_pZone->GetTop()) / m_fStep);

	if ((row >= m_nRows) || (col >= m_nCols))
	{
		return false;
	}

	// 起始点可行走
	design_data_t* data = m_pDesign + row * m_nCols + col;

	if (data->bOutFence)
	{
		data->nFlags = FLAG_DISABLE;
		data->fHeight = HEIGHT_NULL;
		return true;
	}

	data->nFlags = FLAG_WALKABLE;
	data->fHeight = HEIGHT_NULL;
	data->nCheckSibling = 0;

	// 获得起始的可行走高度
	for (int i = 0; i < data->nTraceCount; ++i)
	{
		float h = data->pTrace[i].fTraceY;

		if ((h < (start_y + m_fStepHeight)) 
			&& (h > (start_y - m_fStepHeight)))
		{
			// 真正的起始可行走高度
			data->fHeight = h;
		}
	}

	if (height_is_null(data->fHeight))
	{
		// 错误的起始位置
		return false;
	}

	m_PointList.clear();
	m_PointList.push_back((unsigned int)((row << 16) + col));

	// 生成基本的可行走图和高度图
	m_nPointCount = 0;

	while (ProcessWalkable())
	{
	}

	return true;
}

bool CWalkGenerator::ProcessWalkable()
{
	if (m_PointList.empty())
	{
		return false;
	}

	size_t size = m_PointList.size();
	TAutoMem<unsigned int, 256, TCoreAlloc> auto_buf(size);
	unsigned int* points = auto_buf.GetBuffer();

	memcpy(points, &m_PointList[0], size * sizeof(unsigned int));

	m_PointList.clear();

	for (size_t i = 0; i < size; ++i)
	{
		size_t row = (points[i] >> 16) & 0xFFFF;
		size_t col = points[i] & 0xFFFF;

		design_data_t* pDesign = m_pDesign + row * m_nCols + col;

		if ((pDesign->nCheckSibling & SIBLING_LEFT) == 0)
		{
			ProcessWalkPoint(row, col, row, col - 1);

			pDesign->nCheckSibling |= SIBLING_LEFT;
		}

		if ((pDesign->nCheckSibling & SIBLING_RIGHT) == 0)
		{
			ProcessWalkPoint(row, col, row, col + 1);

			pDesign->nCheckSibling |= SIBLING_RIGHT;
		}

		if ((pDesign->nCheckSibling & SIBLING_UP) == 0)
		{
			ProcessWalkPoint(row, col, row - 1, col);

			pDesign->nCheckSibling |= SIBLING_UP;
		}

		if ((pDesign->nCheckSibling & SIBLING_DOWN) == 0)
		{
			ProcessWalkPoint(row, col, row + 1, col);

			pDesign->nCheckSibling |= SIBLING_DOWN;
		}
	}

	return true;
}

bool CWalkGenerator::ProcessWalkPoint(size_t beg_row, size_t beg_col, 
	size_t row, size_t col)
{
	if ((row >= m_nRows) || (col >= m_nCols))
	{
		return false;
	}

	design_data_t* data = m_pDesign + row * m_nCols + col;

	if (data->bOutFence)
	{
		return false;
	}

	int old_flag = data->nFlags;
	int new_flag = TraceWalkPoint(beg_row, beg_col, row, col);

	if ((new_flag == FLAG_WALKABLE && old_flag != FLAG_WALKABLE)
		|| (new_flag == FLAG_UPDATE_WALKABLE))
	{
		data->nFlags = FLAG_WALKABLE;

		m_PointList.push_back((unsigned int)((row << 16) + col));
	}

	m_nPointCount++;

	return true;
}

int CWalkGenerator::TraceWalkPoint(size_t beg_row, size_t beg_col, 
	size_t row, size_t col)
{
	float beg_x = m_pZone->GetLeft() + (beg_col + 0.5F) * m_fStep;
	float beg_z = m_pZone->GetTop() + (beg_row + 0.5F) * m_fStep;
	// 出发点的高度
	float beg_y = m_pDesign[beg_row * m_nCols + beg_col].fHeight;

	// 获得目标点的可行走标志和高度
	design_data_t* data = m_pDesign + row * m_nCols + col;

	int flags = data->nFlags;
	float height = data->fHeight;

	float x = m_pZone->GetLeft() + (col + 0.5F) * m_fStep;
	float z = m_pZone->GetTop() + (row + 0.5F) * m_fStep;

	if (0 == data->nTraceCount)
	{
		abort();
		return FLAG_DISABLE;
	}

	// 在数组中寻找可以超过人的高度的高度区间
	for (int i = 0; i < (data->nTraceCount - 1); ++i)
	{
		float h1 = data->pTrace[i].fTraceY;
		float h2 = data->pTrace[i + 1].fTraceY;

		if (fabs(h1 - h2) < m_fRoleHeight)
		{
			// 高度空间不够
			continue;
		}

		if ((h1 < (beg_y + m_fStepHeight)) && (h1 > (beg_y - m_fStepHeight)))
		{
			if (m_pTerrain->GetWaterExists(x, z))
			{
				float water_height = m_pTerrain->GetWaterBaseHeight(x, z);

				if ((h1 - water_height) < (-m_fRoleHeight * 0.5F))
				{
					// 水太深
					continue;
				}
			}

			if (!data->pTrace[i].bWalkable)
			{
				// 不可行走
				continue;
			}

			if (data->pTrace[i].bNotFlat)
			{
				// 平坦度不够
				continue;
			}

			if (data->pTrace[i].bForbidden)
			{
				// 此处被禁止
				continue;
			}

			if (data->pTrace[i].bRoleCollide)
			{
				// 空间不够
				continue;
			}

			if (!TwoPointCanWalk(beg_x, beg_y, beg_z, x, h1, z))
			{
				// 两点之间不连通
				continue;
			}

			m_pDesign[row * m_nCols + col].fHeight = h1;

			if ((flags == FLAG_WALKABLE) && (height < h1))
			{
				return FLAG_UPDATE_WALKABLE;
			}

			return FLAG_WALKABLE;
		}
	}

	float h = data->pTrace[data->nTraceCount - 1].fTraceY;

	if ((h < (beg_y + m_fStepHeight)) && (h > (beg_y - m_fStepHeight)))
	{
		if (m_pTerrain->GetWaterExists(x, z))
		{
			float water_height = m_pTerrain->GetWaterBaseHeight(x, z);

			if ((h - water_height) < (-m_fRoleHeight * 0.5F))
			{
				// 水太深
				return FLAG_DISABLE;
			}
		}

		if (!data->pTrace[data->nTraceCount - 1].bWalkable)
		{
			// 不可行走
			return FLAG_DISABLE;
		}

		if (data->pTrace[data->nTraceCount - 1].bNotFlat)
		{
			// 平坦度不够
			return FLAG_DISABLE;
		}

		if (data->pTrace[data->nTraceCount - 1].bForbidden)
		{
			// 此处被禁止
			return FLAG_DISABLE;
		}

		if (data->pTrace[data->nTraceCount - 1].bRoleCollide)
		{
			// 空间不够
			return FLAG_DISABLE;
		}

		if (!TwoPointCanWalk(beg_x, beg_y, beg_z, x, h, z))
		{
			// 两点之间不连通
			return FLAG_DISABLE;
		}

		m_pDesign[row * m_nCols + col].fHeight = h;

		if ((flags == FLAG_WALKABLE) && (height < h))
		{
			return FLAG_UPDATE_WALKABLE;
		}

		return FLAG_WALKABLE;
	}

	return FLAG_DISABLE;
}

bool CWalkGenerator::TwoPointCanWalk(float x1, float y1, float z1, 
	float x2, float y2, float z2)
{
	if (m_pTerrain->TraceHitted(
		FmVec3(x1, y1 + m_fRoleHeight * 0.5F, z1),
		FmVec3(x2, y2 + m_fRoleHeight * 0.5F, z2)))
	{
		if (m_pTerrain->TraceHitted(
			FmVec3(x1, y1 + m_fRoleHeight * 0.8F, z1),
			FmVec3(x2, y2 + m_fRoleHeight * 0.8F, z2)))
		{
			return false;
		}
	}

	return true;
}

bool CWalkGenerator::GenerateFloor()
{
	for (size_t row = 0; row < m_nRows; ++row)
	{
		for (size_t col = 0; col < m_nCols; ++col)
		{
			ProcessFloorPoint(row, col);
		}
	}

	return true;
}

// 判断是否首先碰撞到物体的反面
static bool is_hit_backface(Terrain* pTerrain, const FmVec3& src, 
	const FmVec3& dst)
{
	trace_info_t ti;

	if (!pTerrain->TraceDetail(src, dst, ti))
	{
		return false;
	}

	trace_info_t::hit_t* pHit = &ti.Hits[0];

	FmVec3 v0 = pHit->Vertex[0];
	FmVec3 v1 = pHit->Vertex[1];
	FmVec3 v2 = pHit->Vertex[2];
	
	if (pHit->TraceID != pTerrain->GetID())
	{
		// 点击到地面对象
		IVisBase* pVisBase = (IVisBase*)pTerrain->GetCore()->GetEntity(
			pHit->TraceID);

		if (pVisBase)
		{
			FmMat4 mat;

			pVisBase->GetWorldMatrix(mat);

			FmVec3TransformCoord(&v0, &v0, &mat);
			FmVec3TransformCoord(&v1, &v1, &mat);
			FmVec3TransformCoord(&v2, &v2, &mat);
		}
	}

	// 三角形对应的平面
	FmPlane plane;

	FmPlaneFromPoints(&plane, &v0, &v1, &v2);

	// 射线起点到面的距离
	float dist = FmPlaneDotCoord(&plane, &src);

	if (dist < 0.0F)
	{
		// 只对正面做碰撞检测
		pTerrain->GetRender()->SetTraceOnlyFront(true);

		FmVec3 hit = src + (dst - src) * pHit->fDistance;
		FmVec3 dir = dst - src;

		FmVec3Normalize(&dir, &dir);

		FmVec3 src1 = hit - dir * 0.1F;
		FmVec3 dst1 = hit + dir * 0.1F;
		
		if (pTerrain->TraceHitted(src1, dst1))
		{
			// 存在另一个非常接近的相反面片
			pTerrain->GetRender()->SetTraceOnlyFront(false);
			return false;
		}
		
		pTerrain->GetRender()->SetTraceOnlyFront(false);

		// 击中反面
		return true;
	}

	return false;
}

// 判断点是否在密闭空间内
static bool in_occlusion_space(Terrain* pTerrain, const FmVec3& vNormal,
	const FmVec3& vPosition, float trace_len)
{
	FmVec3 vFront;
	FmVec3 vLeft;

	FmVec3 vTemp(1.0F, 0.0F, 0.0F);
	FmVec3Cross(&vFront, &vNormal, &vTemp);
	FmVec3Cross(&vLeft, &vNormal, &vFront);
	
	if (is_hit_backface(pTerrain, vPosition, vPosition + vFront * trace_len))
	{
		return true;
	}
	
	if (is_hit_backface(pTerrain, vPosition, vPosition - vFront * trace_len))
	{
		return true;
	}

	if (is_hit_backface(pTerrain, vPosition, vPosition + vLeft * trace_len))
	{
		return true;
	}

	if (is_hit_backface(pTerrain, vPosition, vPosition - vLeft * trace_len))
	{
		return true;
	}

	return false;
}

bool CWalkGenerator::ProcessFloorPoint(size_t row, size_t col)
{
	design_data_t* data = m_pDesign + row * m_nCols + col;

	if (data->bOutFence)
	{
		// 层高度
		if (NULL == m_pFloorHeights[0])
		{
			m_pFloorHeights[0] = height_map_t::NewInstance(m_nRows, m_nCols);
		}

		//SetHeight(m_pFloorHeights[0], row, col, 10000.0F);
		SetHeight(m_pFloorHeights[0], row, col, OUT_SCENE_HEIGHT);

		// 无障碍空间高度
		if (NULL == m_pSpaceHeights[0])
		{
			m_pSpaceHeights[0] = height_map_t::NewInstance(m_nRows, m_nCols);
		}

		SetHeight(m_pSpaceHeights[0], row, col, 1.0F);

		// 层标记
		if (NULL == m_pFloorMarkers[0])
		{
			m_pFloorMarkers[0] = marker_map_t::NewInstance(m_nRows, m_nCols);
		}

		SetMarker(m_pFloorMarkers[0], row, col, FLOOR_MARKER_EXISTS);
		
		// 主行走层标记
		if (NULL == m_pWalkMarker)
		{
			m_pWalkMarker = marker_map_t::NewInstance(m_nRows, m_nCols);
		}

		SetMarker(m_pWalkMarker, row, col, 0);

		return true;
	}

	float x = m_pZone->GetLeft() + (col + 0.5F) * m_fStep;
	float z = m_pZone->GetTop() + (row + 0.5F) * m_fStep;

	unsigned char walk_marker = 0;
	unsigned char floor_markers[MAX_FLOOR_NUM] = { 0 };
	float floor_heights[MAX_FLOOR_NUM] = { 0.0F };
	float space_heights[MAX_FLOOR_NUM] = { 0.0F };
	int floor_trace_index[MAX_FLOOR_NUM] = { 0 };

	float ground_height = m_pGroundHeight->pValues[row * m_nCols + col];
	int floor = 0;

	for (int i = 0; i < data->nTraceCount; ++i)
	{
		trace_data_t* td = data->pTrace + i;

		// 层高度
		float floor_height = td->fTraceY;
		// 两个相邻碰撞点之间的高度差
		float space_height = HEIGHT_NULL;

		if ((i + 1) < data->nTraceCount)
		{
			// 两个相邻碰撞点之间的空间
			space_height = data->pTrace[i + 1].fTraceY - td->fTraceY;
		}

		// 是否需要生成顶部
		bool gen_roof = false;

		if (td->TraceID != m_pTerrain->GetID())
		{
			IVisBase* pVisBase = (IVisBase*)
				m_pTerrain->GetCore()->GetEntity(td->TraceID);

			if (pVisBase)
			{
				gen_roof = VisUtil_GetCustomBool(pVisBase, "co_gen_roof");
			}
		}
		else
		{
			// 在地表也可以生成可行走面
			gen_roof = true;
		}

		// 是否在水下
		bool under_water = false;

		if (m_pTerrain->GetWaterExists(x, z))
		{
			float water_height = m_pTerrain->GetWaterBaseHeight(x, z);

			if ((td->fTraceY - water_height) < (-m_fRoleHeight * 0.5F))
			{
				under_water = true;
			}
		}

		bool walkable = false;

		if (data->nFlags == FLAG_WALKABLE)
		{
			// 判断是不是主行走层
			if (HEIGHT_EQUAL(td->fTraceY, data->fHeight))
			{
				walkable = true;
			}
		}

		if ((td->fTraceY + 0.0001F) < ground_height)
		{
			//Trace(x, td->fTraceY, z, walk_marker, "低于地表高度");
			// 低于地表高度
			continue;
		}

		if (td->bForbidden)
		{
			//Trace(x, td->fTraceY, z, walk_marker, "此处被禁止");
			// 此处被禁止
			continue;
		}

		bool moveable = true;

		if (!walkable)
		{
			if (space_height < m_fRoleHeight)
			{
				// 空间不够
				if (td->bPermitted)
				{
					// 重新设置空间
					if (i == (data->nTraceCount - 1))
					{
						space_height = HEIGHT_NULL;
					}
					else
					{
						space_height = m_fRoleHeight;
					}
				}
				else
				{
					//Trace(x, td->fTraceY, z, walk_marker, "空间不够");
					continue;
				}
			}

			if (td->bNotFlat)
			{
				//Trace(x, td->fTraceY, z, walk_marker, "平坦度不够");
				// 平坦度不够
				moveable = false;
			}

			if (td->bRoleCollide)
			{
				//Trace(x, td->fTraceY, z, walk_marker, "空间不够");
				// 空间不够
				moveable = false;
			}

			if (td->bPadding)
			{
				//Trace(x, td->fTraceY, z, walk_marker, "填充物的顶部不可移动");
				// 填充物的顶部不可移动
				moveable = false;
			}

			if (under_water)
			{
				//Trace(x, td->fTraceY, z, walk_marker, "在水下");
				// 在水下
				moveable = false;
			}

			if (!moveable)
			{
				if (td->bPermitted)
				{
					// 指定此处可以行走
					moveable = true;
				}
			}

			//if (moveable && (td->TraceID == m_pTerrain->GetID()))
			if (moveable)
			{
				FmVec3 start(x, td->fTraceY + m_fRoleHeight * 0.5F, z);

				if (td->TraceID == m_pTerrain->GetID())
				{
					// 判断地表的可行走点是否在密闭的空间内
					if (in_occlusion_space(m_pTerrain, td->vNormal,	start, 
						100.0F))
					{
						//Trace(x, td->fTraceY, z, walk_marker, "地表的可行走点在密闭的空间内");
						moveable = false;
					}
				}
				else
				{
					// 判断建筑物的可行走点是否在密闭的空间内
					if (in_occlusion_space(m_pTerrain, 
						FmVec3(0.0F, 1.0F, 0.0F), start, m_fStep * 2))
					{
						//Trace(x, td->fTraceY, z, walk_marker, "建筑物的可行走点在密闭的空间内");
						moveable = false;
					}
				}
			}
		}

		if ((!moveable) && (!under_water) && td->bRoleCollide
			&& (space_height > m_fRoleHeight))
		{
			// 尝试提高红点的位置
			float offset = m_fStep * 0.35F;

			FmVec4 src1;
			FmVec4 dst1;

			src1.y = td->fTraceY + m_fRoleHeight;
			dst1.y = td->fTraceY;

			float y1 = td->fTraceY;
			float y2 = td->fTraceY;
			float y3 = td->fTraceY;
			float y4 = td->fTraceY;
			
			trace_info_t ti1;

			src1.x = x + offset;
			dst1.x = x + offset;
			src1.z = z + offset;
			dst1.z = z + offset;

			if (m_pTerrain->TraceDetail(src1, dst1, ti1))
			{
				y1 = src1.y + (dst1.y - src1.y) * ti1.fDistance;
			}

			src1.x = x - offset;
			dst1.x = x - offset;
			src1.z = z + offset;
			dst1.z = z + offset;

			if (m_pTerrain->TraceDetail(src1, dst1, ti1))
			{
				y2 = src1.y + (dst1.y - src1.y) * ti1.fDistance;
			}

			src1.x = x + offset;
			dst1.x = x + offset;
			src1.z = z - offset;
			dst1.z = z - offset;

			if (m_pTerrain->TraceDetail(src1, dst1, ti1))
			{
				y3 = src1.y + (dst1.y - src1.y) * ti1.fDistance;
			}

			src1.x = x - offset;
			dst1.x = x - offset;
			src1.z = z - offset;
			dst1.z = z - offset;

			if (m_pTerrain->TraceDetail(src1, dst1, ti1))
			{
				y4 = src1.y + (dst1.y - src1.y) * ti1.fDistance;
			}

			float max_y = td->fTraceY;

			if (y1 > max_y)
			{
				max_y = y1;
			}

			if (y2 > max_y)
			{
				max_y = y2;
			}

			if (y3 > max_y)
			{
				max_y = y3;
			}

			if (y4 > max_y)
			{
				max_y = y4;
			}

			if (height_is_null(space_height))
			{
				floor_height = max_y;
			}
			else
			{
				float delta_y = max_y - floor_height;

				if ((space_height - delta_y) > m_fRoleHeight)
				{
					floor_height = max_y;
					space_height -= delta_y;
				}
			}

			// 必须记录修正后的高度，在生成墙壁时需要使用这个高度
			td->fTraceY = max_y;
		}

		unsigned int floor_marker = FLOOR_MARKER_EXISTS;

		if (gen_roof)
		{
			if (moveable)
			{
				// 可行走
				floor_marker |= FLOOR_MARKER_MOVE | FLOOR_MARKER_STAND;
			}

			if (td->bForbidStand)
			{
				floor_marker &= ~FLOOR_MARKER_STAND;
			}
		}
		else
		{
			if (moveable)
			{
				// 可以站立但不能移动的地方
				floor_marker |= FLOOR_MARKER_STAND;
			}
		}

		if (walkable)
		{
			// 可行走
			floor_marker |= FLOOR_MARKER_MOVE | FLOOR_MARKER_STAND;
			
			// 主行走层标记
			walk_marker = floor + 1;
		}

		// 层高度
		floor_heights[floor] = floor_height;
		// 无障碍空间高度
		space_heights[floor] = space_height;
		// 层标记
		floor_markers[floor] = floor_marker;
		// 层对应的碰撞数据索引
		floor_trace_index[floor] = i;

		if (++floor >= MAX_FLOOR_NUM)
		{
			Trace(x, td->fTraceY, z, walk_marker, "已达到最大层数");
			// 已达到最大层数
			break;
		}
	}

	//// 必须保证至少有一层
	//if (0 == floor)
	//{
	//	// 层高度
	//	if (data->nTraceCount > 0)
	//	{
	//		// 用最高一层的高度
	//		floor_heights[floor] = data->pTrace[data->nTraceCount - 1].fTraceY;
	//	}
	//	else
	//	{
	//		// 使用地表高度
	//		floor_heights[floor] = ground_height;
	//	}

	//	// 无障碍空间高度
	//	space_heights[floor] = HEIGHT_NULL;
	//	// 层标记
	//	floor_markers[floor] = FLOOR_MARKER_EXISTS;

	//	++floor;
	//}

	// 消除一些不需要生成的点
	float water_height = -HEIGHT_NULL;
	
	if (m_pTerrain->GetWaterExists(x, z))
	{
		water_height = m_pTerrain->GetWaterBaseHeight(x, z);
	}

	int j = 0;

	while ((j < floor) && (floor > 1))
	{
		int trace_index = floor_trace_index[j];
		int above = j + 1;
		
		if ((j != (walk_marker - 1)) && (floor_heights[j] > water_height)
			&& (above < floor) && ((trace_index + 1) < data->nTraceCount)
			&& (floor_trace_index[above] == (trace_index + 1))
			&& ((floor_markers[above] & FLOOR_MARKER_EXISTS) != 0))
		{
			// 如果当前层已经设置允许可行走，则不删除此层
			trace_data_t* cur_td = data->pTrace + floor_trace_index[j];

			if (cur_td)
			{
				if (cur_td->bPermitted)
				{
					++j;
					continue;
				}
			}

			trace_data_t* td = data->pTrace + floor_trace_index[above];

			// 有没有向下的面罩住此层
			if (!td->bFaceDown)
			{
				// 删除此层
				if ((walk_marker - 1) > j)
				{
					//Trace(x, td->fTraceY, z, -1, "没有向下的面罩住此层->删除此层");
					--walk_marker;
				}

				for (int t1 = j; t1 < (floor - 1); ++t1)
				{
					floor_heights[t1] = floor_heights[t1 + 1];
					space_heights[t1] = space_heights[t1 + 1];
					floor_markers[t1] = floor_markers[t1 + 1];
					floor_trace_index[t1] = floor_trace_index[t1 + 1];
				}

				--floor;
				continue;
			}
		}

		++j;
	}

	// 必须保证至少有一层
	if (0 == floor)
	{
		// 层高度
		if (data->nTraceCount > 0)
		{
			// 用最高一层的高度
			floor_heights[floor] = data->pTrace[data->nTraceCount - 1].fTraceY;
		}
		else
		{
			// 使用地表高度
			floor_heights[floor] = ground_height;
		}

		// 无障碍空间高度
		space_heights[floor] = HEIGHT_NULL;
		// 层标记
		floor_markers[floor] = FLOOR_MARKER_EXISTS;

		++floor;
	}

	// 写入标记图
	if (NULL == m_pWalkMarker)
	{
		m_pWalkMarker = marker_map_t::NewInstance(m_nRows, m_nCols);
	}

	SetMarker(m_pWalkMarker, row, col, walk_marker);

	for (int k = 0; k < floor; ++k)
	{
		// 层高度
		if (NULL == m_pFloorHeights[k])
		{
			m_pFloorHeights[k] = height_map_t::NewInstance(m_nRows, m_nCols);
		}

		if (!HEIGHT_EQUAL(floor_heights[k], ground_height))
		{
			SetHeight(m_pFloorHeights[k], row, col, floor_heights[k]);
		}

		// 无障碍空间高度
		if (NULL == m_pSpaceHeights[k])
		{
			m_pSpaceHeights[k] = height_map_t::NewInstance(m_nRows, m_nCols);
		}

		SetHeight(m_pSpaceHeights[k], row, col, space_heights[k]);

		// 层标记
		if (NULL == m_pFloorMarkers[k])
		{
			m_pFloorMarkers[k] = marker_map_t::NewInstance(m_nRows, m_nCols);
		}

		SetMarker(m_pFloorMarkers[k], row, col, floor_markers[k]);
	}

	return true;
}

bool CWalkGenerator::GenerateWall()
{
	for (size_t row = 0; row < m_nRows; ++row)
	{
		for (size_t col = 0; col < m_nCols; ++col)
		{
			ProcessWallPoint(row, col);
		}
	}

	return true;
}

bool CWalkGenerator::ProcessWallPoint(size_t row, size_t col)
{
	size_t index = row * m_nCols + col;
	design_data_t* dd = m_pDesign + index;

	if (dd->bOutFence)
	{
		return false;
	}

	float x = m_pZone->GetLeft() + (col + 0.5F) * m_fStep;
	float z = m_pZone->GetTop() + (row + 0.5F) * m_fStep;

	int i = 0;

	float beg_height = m_pGroundHeight->pValues[index];

	if (m_pFloorHeights[0])
	{
		float floor_height = m_pFloorHeights[0]->pValues[index];

		if (!height_is_null(floor_height))
		{
			if ((floor_height - beg_height) < (m_fRoleHeight * 0.5F))
			{
				beg_height = floor_height;
				i = 1;
			}
			//beg_height = floor_height;
		}
	}

	//for (int i = 1; i < MAX_FLOOR_NUM; ++i)
	for (; i < MAX_FLOOR_NUM; ++i)
	{
		if (NULL == m_pFloorHeights[i])
		{
			continue;
		}

		if (NULL == m_pFloorMarkers[i])
		{
			continue;
		}

		float end_height = m_pFloorHeights[i]->pValues[index];

		if (height_is_null(end_height))
		{
			// 没有可行走层
			continue;
		}

		//unsigned int end_marker = m_pFloorMarkers[i]->pValues[index];

		//if ((end_marker & FLOOR_MARKER_MOVE) == 0)
		//{
		//	// 不可移动
		//	continue;
		//}

		if ((end_height - beg_height) < (m_fRoleHeight * 0.5F))
		//if ((end_height - beg_height) < m_fRoleHeight)
		{
			// 低于角色高度的不认为是墙
			continue;
		}

		// 模型是否需要生成墙面
		bool gen_wall = false;

		for (int t = 0; t < dd->nTraceCount; ++t)
		{
			if (HEIGHT_EQUAL(dd->pTrace[t].fTraceY, end_height)
				&& (dd->pTrace[t].TraceID != m_pTerrain->GetID()))
			{
				IVisBase* pVisBase = (IVisBase*)
					m_pTerrain->GetCore()->GetEntity(dd->pTrace[t].TraceID);

				if (pVisBase)
				{
					gen_wall = VisUtil_GetCustomBool(pVisBase, "co_gen_wall");
				}

				break;
			}
		}

		if (!gen_wall)
		{
			// 不需要生成墙面
			continue;
		}

		if (!TraceWallPoint(row, col, beg_height, end_height))
		{
			// 不是连续的墙壁
			continue;
		}

		unsigned int floor_marker = m_pFloorMarkers[i]->pValues[index];

		floor_marker |= FLOOR_MARKER_WALL;

		SetMarker(m_pFloorMarkers[i], row, col, floor_marker);
	}

	return true;
}

bool CWalkGenerator::TraceWallPoint(size_t row, size_t col, float beg_height, 
	float end_height)
{
	design_data_t* data = m_pDesign + row * m_nCols + col;

	float x = m_pZone->GetLeft() + (col + 0.5F) * m_fStep;
	float z = m_pZone->GetTop() + (row + 0.5F) * m_fStep;

	int total_num = 0;
	int trace_num = 0;

	FmVec4 src;
	FmVec4 dst;

	float radius = m_fStep;
	float sin_angle = sinf(m_fWallAngle / 180.0F * FLOAT_PI);
	float step = m_fRoleHeight * 0.25F;
	float height = beg_height;

	while ((height + step * 0.5F) < end_height)
	{
		// 随着倾斜度改变碰撞检测的范围
		float r = radius + (end_height - (height + step * 0.5F)) * sin_angle;
	
		// 从两个方向进行碰撞检测
		src.x = x;
		src.y = height + step * 0.5F;
		src.z = z - r;
		dst.x = x;
		dst.y = height + step * 0.5F;
		dst.z = z + r;

		if (m_pTerrain->TraceHitted(src, dst))
		{
			++trace_num;
		}
		else
		{
			src.x = x - r;
			src.y = height + step * 0.5F;
			src.z = z;
			dst.x = x + r;
			dst.y = height + step * 0.5F;
			dst.z = z;

			if (m_pTerrain->TraceHitted(src, dst))
			{
				++trace_num;
			}
		}

		height += step;

		++total_num;
	}

	if (total_num == 0)
	{
		return false;
	}

	return (float(trace_num) / float(total_num)) >= 0.9F;
}

bool CWalkGenerator::GenerateWater()
{
	m_pWaterHeight = height_map_t::NewInstance(m_nRows, m_nCols);
	
	for (size_t row = 0; row < m_nRows; ++row)
	{
		for (size_t col = 0; col < m_nCols; ++col)
		{
			SetHeight(m_pWaterHeight, row, col, HEIGHT_NULL);
			
			ProcessWaterPoint(row, col);
		}
	}

	return true;
}

bool CWalkGenerator::ProcessWaterPoint(size_t row, size_t col)
{
	design_data_t* data = m_pDesign + row * m_nCols + col;

	if (data->bOutFence)
	{
		return false;
	}

	float x = m_pZone->GetLeft() + (col + 0.5F) * m_fStep;
	float z = m_pZone->GetTop() + (row + 0.5F) * m_fStep;

	if (!m_pTerrain->GetWaterExists(x, z))
	{
		// 没有水面
		return false;
	}

	// 水面高度
	float water_height = m_pTerrain->GetWaterBaseHeight(x, z);
	// 地面高度
	float ground_height = m_pTerrain->GetGroundHeight(x, z);

	if (ground_height > water_height)
	{
		// 水面低于地面
		return false;
	}

	// 添加水面行走空间
	SetHeight(m_pWaterHeight, row, col, water_height);

	return true;
}

