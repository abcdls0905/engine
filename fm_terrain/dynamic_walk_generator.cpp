//--------------------------------------------------------------------
// 文件名:		WalkGenerator.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2013年3月6日
// 创建人:		沈奇伟
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "dynamic_walk_generator.h"

#include "dynamic_walkable_manager.h"
#include "terrain.h"
#include "walk_height_compress.h"
#include "../public/auto_mem.h"
#include "../public/core_log.h"
#include "../visual/vis_utils.h"

extern void extern_to_2n(size_t& row, size_t& col);

// 高度是否近似
#define HEIGHT_EQUAL(v1, v2) ((v1 <= (v2 + 0.0001f)) && (v1 >= (v2 - 0.0001f)))

static inline bool height_is_null(float height)
{
	return (height > 1e7);
}

// 高度图
struct CDynamicWalkGenerator::height_map_t
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
struct CDynamicWalkGenerator::marker_map_t
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

// CDynamicWalkGenerator

CDynamicWalkGenerator::CDynamicWalkGenerator(Terrain* pTerrain,
											 IVisBase* pVisual, int precision)
{
	Assert(NULL != pTerrain);
	Assert(NULL != pVisual);

	size_t collide_per_unit = pTerrain->GetCollidePerUnit();
	//size_t scale = pTerrain->GetZoneScale() * collide_per_unit * precision;

	m_pTerrain = pTerrain;
	m_pVisual = pVisual;
	m_Angle = pVisual->GetAngle();
	pVisual->SetAngle(m_Angle.x, 0.0f, m_Angle.z);
	m_nRows = (size_t)(m_pVisual->GetBoxSizeZ() / m_pTerrain->GetUnitSize() * collide_per_unit * precision + 0.5f);
	m_nCols = (size_t)(m_pVisual->GetBoxSizeX() / m_pTerrain->GetUnitSize() * collide_per_unit * precision + 0.5f);

	extern_to_2n(m_nRows, m_nCols);

	m_fStep = pTerrain->GetUnitSize() / collide_per_unit / precision;
	m_nPrecision = precision;

	m_fRoleHeight = 1.0f;
	m_fRoleRadius = 0.52f;
	m_fStepHeight = 0.5f;
	m_fTraceHigh = 500.0f;
	m_fTraceLow = -500.0f;
	m_fFloorNormalY = 0.34f;
	m_fWallAngle = 15.0f;

	m_fLeft = pVisual->GetCenterX() - pVisual->GetBoxSizeX() / 2.0f;
	m_fTop = pVisual->GetCenterZ() - pVisual->GetBoxSizeZ() / 2.0f;

	size_t size = m_nRows * m_nCols;

	m_pDesign = (design_data_t*)CORE_ALLOC(sizeof(design_data_t) * size);
	memset(m_pDesign, 0, sizeof(design_data_t) * size);

	for (size_t k = 0; k < size; ++k)
	{
		m_pDesign[k].fHeight = HEIGHT_NULL;
	}

	m_pGroundHeight = NULL;

	for (size_t i = 0; i < MAX_FLOOR_NUM; ++i)
	{
		m_pFloorHeights[i] = NULL;
		m_pSpaceHeights[i] = NULL;
		m_pFloorMarkers[i] = NULL;
	}
}

CDynamicWalkGenerator::~CDynamicWalkGenerator()
{
	ReleaseAll();
}

void CDynamicWalkGenerator::ReleaseAll()
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

	for (size_t i = 0; i < MAX_FLOOR_NUM; ++i)
	{
		SAFE_RELEASE(m_pFloorHeights[i]);
		SAFE_RELEASE(m_pSpaceHeights[i]);
		SAFE_RELEASE(m_pFloorMarkers[i]);
	}

	FENCE_CONTAINER::iterator it = m_FencePoints.Begin();
	FENCE_CONTAINER::iterator end = m_FencePoints.End();

	for (; it != end; ++it)
	{
		CORE_DELETE(it.GetData());
	}

	m_FencePoints.Clear();

	/*for (size_t i = 0; i < MAX_FLOOR_NUM; ++i)
	{
		for (size_t j = 0; j < m_EdgeMap[i].size(); ++j)
		{
			if (m_EdgeMap[i][j])
			{
				m_EdgeMap[i][j]->clear();
				CORE_DELETE(m_EdgeMap[i][j]);
				m_EdgeMap[i][j] = NULL;
			}

			m_EdgeMap[i].clear();
		}
	}*/
}

void CDynamicWalkGenerator::SetHeight(height_map_t* pHeight, size_t row,
									  size_t col, float value)
{
	pHeight->pValues[row * pHeight->nCols + col] = value;
}

void CDynamicWalkGenerator::SetMarker(marker_map_t* pMarker, size_t row,
									  size_t col, unsigned char value)
{
	pMarker->pValues[row * pMarker->nCols + col] = value;
}

bool CDynamicWalkGenerator::RoleCollide(float x, float y, float z)
{
	FmVec3 center(x, y, z);
	float radius = m_fRoleRadius * 0.5f;

	// 上部
	center.y = y + m_fRoleHeight - 0.1f - radius * 0.5f;

	if (m_pTerrain->TraceSphere(center, radius))
	{
		return true;
	}

	// 中间
	center.y = y + m_fRoleHeight * 0.5f;

	return m_pTerrain->TraceSphere(center, m_fRoleRadius);
}

bool CDynamicWalkGenerator::IsPermitted(float x, float y, float z)
{
	if (m_PermitList.empty())
	{
		return false;
	}

	bool result = false;

	// 忽略辅助物体的碰撞
	bool old_track_mask = m_pTerrain->GetTraceMask("Helper");

	m_pTerrain->SetTraceMask("Helper", true);

	float radius = m_fStep;// * 0.5f;

	FmVec3 src(x, y + 0.001f, z);

	for (size_t i = 0; i < m_PermitList.size(); ++i)
	{
		FmVec3 dst = m_PermitList[i];
		FmVec3 temp1 = (dst - src);
		if (FmVec3LengthSq(&temp1) > (radius * radius))
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

bool CDynamicWalkGenerator::IsForbidden(float x, float y, float z)
{
	if (m_ForbidList.empty())
	{
		return false;
	}

	bool result = false;

	// 忽略辅助物体的碰撞
	bool old_track_mask = m_pTerrain->GetTraceMask("Helper");

	m_pTerrain->SetTraceMask("Helper", true);

	FmVec3 src(x, y + m_fRoleHeight * 0.5f, z);

	for (size_t i = 0; i < m_ForbidList.size(); ++i)
	{
		FmVec3 dst = m_ForbidList[i].pos;
		float radius = m_ForbidList[i].fRadius;
		FmVec3 temp1 = (dst - src);
		if (FmVec3LengthSq(&temp1) > (radius * radius))
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

bool CDynamicWalkGenerator::IsEntryPoint(float x, float y, float z)
{
	if (m_EntryPoints.empty())
	{
		return false;
	}

	bool result = false;

	// 忽略辅助物体的碰撞
	bool old_track_mask = m_pTerrain->GetTraceMask("Helper");

	m_pTerrain->SetTraceMask("Helper", true);

	float radius = m_fStep * 0.5f;// * 0.5f;

	FmVec3 src(x, y + 0.001f, z);

	for (size_t i = 0; i < m_EntryPoints.size(); ++i)
	{
		FmVec3 dst = m_EntryPoints[i];
		FmVec3 temp1 = (dst - src);
		if (FmVec3LengthSq(&temp1) > (radius * radius))
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

bool CDynamicWalkGenerator::AddPermit(float x, float y, float z)
{
	m_PermitList.push_back(FmVec3(x, y, z));

	return true;
}

bool CDynamicWalkGenerator::AddForbid(float x, float y, float z, float radius)
{
	forbid_t data;

	data.pos.x = x;
	data.pos.y = y;
	data.pos.z = z;
	data.fRadius = radius;

	m_ForbidList.push_back(data);

	return true;
}

bool CDynamicWalkGenerator::AddFencePoint(int fence_index, float x, float z)
{
	POINT_CONTAINER* points;

	if (!m_FencePoints.GetData(fence_index, points))
	{
		points = CORE_NEW(POINT_CONTAINER);
		m_FencePoints.Add(fence_index, points);
	}

	points->push_back(FmVec2(x, z));

	return true;
}

bool CDynamicWalkGenerator::AddEntryPoint(float x, float y, float z)
{
	m_EntryPoints.push_back(FmVec3(x, y, z));

	return true;
}

bool CDynamicWalkGenerator::GenerateBegin()
{
	return true;
}

bool CDynamicWalkGenerator::GenerateEnd()
{
	m_pTerrain->GetDynamicWalkableManager()->AddDynamicWalkable2(m_pVisual);

	CDynamicWalkable* pDynamicWalkable = m_pTerrain->GetDynamicWalkableManager()->GetDynamicWalkable(m_pVisual);

	if (NULL == pDynamicWalkable)
	{
		return false;
	}
	
	pDynamicWalkable->ReleaseAll();

	size_t rows = m_nRows / m_nPrecision;
	size_t cols = m_nCols / m_nPrecision;
	TAutoMem<float, 1, TCoreAlloc> auto_buf(rows * cols);

	// 主行走层标记
	TAutoMem<unsigned char, 1, TCoreAlloc> auto_buf1(rows * cols);

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

		pDynamicWalkable->UpdateFloorHeight(i, rows, cols, auto_buf.GetBuffer());
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

		pDynamicWalkable->UpdateSpaceHeight(i, rows, cols, auto_buf.GetBuffer());
	}

	// 可行走标记
	for (i = 0; i < MAX_FLOOR_NUM; ++i)
	{
		if (NULL == m_pFloorMarkers[i])
		{
			continue;
		}

		UpdateFlagMap(m_pFloorMarkers[i], auto_buf1.GetBuffer());

		pDynamicWalkable->UpdateFloorMarker(i, rows, cols, auto_buf1.GetBuffer());
	}

	pDynamicWalkable->SetHeight(m_pVisual->GetPositionY());
	pDynamicWalkable->SetAngleY(0.0f);
	pDynamicWalkable->SetReady(true);

	m_pVisual->SetAngle(m_Angle.x, m_Angle.y, m_Angle.z);

	return true;
}

bool CDynamicWalkGenerator::UpdateHeightMap(height_map_t* pHeight,
											float* pValues, int mode)
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

bool CDynamicWalkGenerator::UpdateFlagMap(marker_map_t* pMarker,
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

bool CDynamicWalkGenerator::GenerateGround()
{
	m_pGroundHeight = height_map_t::NewInstance(m_nRows, m_nCols);

	for (size_t row = 0; row < m_nRows; ++row)
	{
		for (size_t col = 0; col < m_nCols; ++col)
		{
			float x = GetLeft() + (col + 0.5f) * m_fStep;
			float z = GetTop() + (row + 0.5f) * m_fStep;
			float y = m_pTerrain->GetGroundHeight(x, z);

			SetHeight(m_pGroundHeight, row, col, y);
		}
	}

	return true;
}

bool CDynamicWalkGenerator::GenerateBalk(int row)
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
		// 消除一些较平坦表面由于立面造成的不可行走位置, 消除相邻入口点
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
						float step = m_fRoleHeight * 0.5f;

						if (GetNearHeight(r - 1, c, h, step)
							&& GetNearHeight(r + 1, c, h, step)
							&& GetNearHeight(r, c - 1, h, step)
							&& GetNearHeight(r, c + 1, h, step))
						{
							// 消除此立面
							td->bNotFlat = false;
						}
					}

					//if (td->bEntryPoint)
					//{
					//	float h = td->fTraceY;
					//	float step = m_fRoleHeight * 0.5f;

					//	if (GetNearEntry(r - 1, c - 1, h, step)
					//		|| GetNearEntry(r - 1, c, h, step)
					//		|| GetNearEntry(r - 1, c + 1, h, step)
					//		|| GetNearEntry(r, c - 1, h, step)
					//		|| GetNearEntry(r, c + 1, h, step)
					//		|| GetNearEntry(r + 1, c - 1, h, step)
					//		|| GetNearEntry(r + 1, c, h, step)
					//		|| GetNearEntry(r + 1, c + 1, h, step))
					//	{
					//		// 消除此入口点
					//		td->bEntryPoint = false;
					//	}
					//}
				}
			}
		}
	}

	return true;
}

bool CDynamicWalkGenerator::GetNearHeight(size_t row, size_t col, float height, 
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

bool CDynamicWalkGenerator::GetNearEntry(size_t row, size_t col, float height, 
										  float step)
{
	if ((row >= m_nRows) || (col >= m_nCols))
	{
		return false;
	}

	design_data_t* data = m_pDesign + row * m_nCols + col;

	for (int t = 0; t < data->nTraceCount; ++t)
	{
		trace_data_t* td = &data->pTrace[t];

		if ((height <= (td->fTraceY + step))
			&& (height >= (td->fTraceY - step)))
		{
			if (td->bEntryPoint)
			{
				return true;
			}
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

bool CDynamicWalkGenerator::TraceAllHeight(size_t row, size_t col)
{
	design_data_t* data = m_pDesign + row * m_nCols + col;

	float x = GetLeft() + (col + 0.5f) * m_fStep;
	float z = GetTop() + (row + 0.5f) * m_fStep;

	// 是否在场景栅栏之外
	bool out_fence = false;
	
	size_t fence_count = m_FencePoints.GetCount();

	if (fence_count > 0)
	{
		FENCE_CONTAINER::iterator it = m_FencePoints.Begin();
		FENCE_CONTAINER::iterator end = m_FencePoints.End();

		for (; it != end; ++it)
		{
			POINT_CONTAINER& points = *(it.GetData());
			size_t fence_point_num = points.size();

			if (fence_point_num >= 3)
			{
				out_fence = !point_in_polygon(FmVec2(x, z), &points[0],
					fence_point_num);

				if (!out_fence)
				{
					break;
				}
			}
		}
	}

	if (out_fence)
	{
		//Trace(x, 0.0f, z, -1, "是有效场景范围之外的点");
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

	m_pTerrain->SetTraceMask("Ground", true);
	if (!m_pTerrain->TraceDetail(src, dst, *ti))
	{
		m_pTerrain->GetRender()->SetTraceTextureName(false);
		return false;
	}
	m_pTerrain->SetTraceMask("Ground", false);

	m_pTerrain->GetRender()->SetTraceTextureName(false);

	// 排序碰撞点
	qsort(ti->Hits, ti->nHitCount, sizeof(trace_info_t::hit_t), 
		trace_hit_compare);

	if (32 == ti->nHitCount)
	{
		CORE_TRACE("(CDynamicWalkGenerator::TraceAllHeight)hit count may overflow!");
	}

	for (int i = 0; i < ti->nHitCount; ++i)
	{
		trace_info_t::hit_t* pHit = &ti->Hits[i];

		if (pHit->TraceID != m_pVisual->GetID())
		{
			// 已经到达地表
			continue;
		}

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
		//if (pHit->TraceID != m_pTerrain->GetID())
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
// 		else
// 		{
// 			// 更新地面高度
// 			//SetHeight(m_pGroundHeight, row, col, y);
// 		}

		// 获得碰撞面的法线
		FmVec3 normal;

		FmVec3 temp1 = v2 - v1;
		FmVec3 temp2 = (v3 - v1);
		FmVec3Cross(&normal, &temp1, &temp2);
		FmVec3Normalize(&normal, &normal);

		if (normal.y < 0.0f)
		{
			//Trace(x, y, z, -1, "存在朝下的面");
			// 存在朝下的面
			face_down = true;
		}

		if (through)
		{
			//Trace(x, y, z, -1, "可穿透物体");
			// 可穿透物体
			continue;
		}

		trace_data_t* td = NULL;

		if (data->nTraceCount > 0 && (i != ti->nHitCount - 1))
		{
			trace_data_t* prev_data = data->pTrace + data->nTraceCount - 1;

			// 是否与前一个高度近似
			if ((y <= (prev_data->fTraceY + 0.05f)) 
				&& (y >= (prev_data->fTraceY - 0.05f)))
			{
				if (normal.y < 0.0f)
				{
					//Trace(x, y, z, -1, "作为上一层的补充数据");
					// 作为上一层的补充数据
					prev_data->bFaceDown = true;
					continue;
				}
				else
				{
					if (prev_data->vNormal.y > 0.0f)
					{
						// 放弃此层高度
						//Trace(x, y, z, -1, "放弃此层高度");
						continue;
					}

					// 取代前一个高度
					td = prev_data;

					if (prev_data->bFaceDown)
					{
						//Trace(x, y, z, -1, "取代前一个高度");
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

		if (pHit->TraceID != m_pTerrain->GetID())
		{
			if (fabs(normal.y) < m_fFloorNormalY)
			{
				//Trace(x, y, z, -1, "不平坦");
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

		if ((space_height >= m_fRoleHeight) && (normal.y > 0.0f))
		{
			role_collide = RoleCollide(x, y, z);

			if (role_collide)
			{
				//Trace(x, y, z, -1, "role collide");
			}
		}
		else
		{
			if (space_height < m_fRoleHeight)
			{
				//Trace(x, y, z, -1, "role collide(space height < role height)");
			}

			if (normal.y <= 0.0f)
			{
				//Trace(x, y, z, -1, "role collide(normal <= 0.0f)");
			}
		}

		bool is_permitted = false;
		bool is_forbidden = false;
		bool is_entrypoint = false;

		if (normal.y > 0.0f)
		{
			is_permitted = IsPermitted(x, y, z);
			is_forbidden = IsForbidden(x, y, z);
			is_entrypoint = IsEntryPoint(x, y, z);
			if (is_forbidden)
			{
				//Trace(x, y, z, -1, "forbiddden");
			}
		}
		else
		{
			//Trace(x, y, z, -1, "朝下的面片不允许生成碰撞");
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
		td->bEntryPoint = is_entrypoint;

		if (padding)
		{
			// 已经到达填充物，填充物以下的物体高度将被忽略
			//Trace(x, y, z, -1, "已经到达填充物，填充物以下的物体高度将被忽略");
			break;
		}

		//if (pHit->TraceID == m_pTerrain->GetID())
		//{
		//	// 已经到达地表
		//	break;
		//}
	}

	if (data->nTraceCount > 0)
	{
		if (!data->pTrace[data->nTraceCount-1].bFaceDown)
		{
			trace_data_t* new_trace = (trace_data_t*)CORE_ALLOC(
				sizeof(trace_data_t) * (data->nTraceCount + 1));

			memcpy(new_trace, data->pTrace, 
				sizeof(trace_data_t) * data->nTraceCount);
			CORE_FREE(data->pTrace, 
				sizeof(trace_data_t) * data->nTraceCount);

			data->nTraceCount++;
			data->pTrace = new_trace;

			trace_data_t* td = data->pTrace + data->nTraceCount - 1;

			td->TraceID = m_pVisual->GetID();
			td->fTraceY = m_pVisual->GetPositionY();
			td->vNormal = FmVec3(0.0f, -1.0f, 0.0f);
			td->bFaceDown = true;
			td->bNotFlat = false;
			td->bWalkable = false;
			td->bForbidStand = false;
			td->bRoleCollide = false;
			td->bPermitted = false;
			td->bForbidden = false;
			td->bPadding = false;
			td->bEntryPoint = false;
		}
		// 重新排序数组。
		qsort(data->pTrace, data->nTraceCount, sizeof(trace_data_t), 
			HeightCompare);
	}

	return true;
}

int __cdecl CDynamicWalkGenerator::HeightCompare(const void* p1, const void* p2)
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

bool CDynamicWalkGenerator::GenerateFloor()
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

	if (dist < 0.0f)
	{
		// 只对正面做碰撞检测
		pTerrain->GetRender()->SetTraceOnlyFront(true);

		FmVec3 hit = src + (dst - src) * pHit->fDistance;
		FmVec3 dir = dst - src;

		FmVec3Normalize(&dir, &dir);

		FmVec3 src1 = hit - dir * 0.1f;
		FmVec3 dst1 = hit + dir * 0.1f;

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
	FmVec3 vTemp(1.0f, 0.0f, 0.0f);
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

bool CDynamicWalkGenerator::ProcessFloorPoint(size_t row, size_t col)
{
	design_data_t* data = m_pDesign + row * m_nCols + col;

	if (data->bOutFence)
	{
		// 层高度
		if (NULL == m_pFloorHeights[0])
		{
			m_pFloorHeights[0] = height_map_t::NewInstance(m_nRows, m_nCols);
		}

		//SetHeight(m_pFloorHeights[0], row, col, 10000.0f);
		SetHeight(m_pFloorHeights[0], row, col, OUT_SCENE_HEIGHT);

		// 无障碍空间高度
		if (NULL == m_pSpaceHeights[0])
		{
			m_pSpaceHeights[0] = height_map_t::NewInstance(m_nRows, m_nCols);
		}

		SetHeight(m_pSpaceHeights[0], row, col, 1.0f);

		// 层标记
		if (NULL == m_pFloorMarkers[0])
		{
			m_pFloorMarkers[0] = marker_map_t::NewInstance(m_nRows, m_nCols);
		}

		SetMarker(m_pFloorMarkers[0], row, col, FLOOR_MARKER_EXISTS);

		return true;
	}

	float x = GetLeft() + (col + 0.5f) * m_fStep;
	float z = GetTop() + (row + 0.5f) * m_fStep;

	unsigned char walk_marker = 0;
	unsigned char floor_markers[MAX_FLOOR_NUM] = { 0 };
	float floor_heights[MAX_FLOOR_NUM] = { 0.0f };
	float space_heights[MAX_FLOOR_NUM] = { 0.0f };
	int floor_trace_index[MAX_FLOOR_NUM] = { 0 };

	float ground_height = 0.0f;//m_pGroundHeight->pValues[row * m_nCols + col];
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

			if ((td->fTraceY - water_height) < (-m_fRoleHeight * 0.5f))
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

		//if ((td->fTraceY + 0.0001f) < ground_height)
		//{
		//	//Trace(x, td->fTraceY, z, walk_marker, "低于地表高度");
		//	// 低于地表高度
		//	continue;
		//}

		if (td->bForbidden)
		{
			//Trace(x, td->fTraceY, z, walk_marker, "此处被禁止");
			// 此处被禁止
			if (i != 0)
			{
				continue;
			}
			//continue;
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
					if (i != 0)
					{
						continue;
					}
					//continue;
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
				FmVec3 start(x, td->fTraceY + m_fRoleHeight * 0.5f, z);

				if (td->TraceID == m_pTerrain->GetID())
				{
					// 判断地表的可行走点是否在密闭的空间内
					if (in_occlusion_space(m_pTerrain, td->vNormal,	start, 
						100.0f))
					{
						//Trace(x, td->fTraceY, z, walk_marker, "地表的可行走点在密闭的空间内");
						moveable = false;
					}
				}
				else
				{
					// 判断建筑物的可行走点是否在密闭的空间内
					if (in_occlusion_space(m_pTerrain, 
						FmVec3(0.0f, 1.0f, 0.0f), start, m_fStep * 2))
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
			float offset = m_fStep * 0.35f;

			FmVec3 src1;
			FmVec3 dst1;

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

		/*if (i == 0 && td->bForbidden)
		{
			floor_marker &= ~FLOOR_MARKER_EXISTS;
		}*/

		if (i == 0 && td->bForbidden)
		{
			floor_marker = FLOOR_MARKER_EXISTS;
		}

		if (td->bEntryPoint)
		{
			floor_marker |= FLOOR_MARKER_EDGE;
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
			//Trace(x, td->fTraceY, z, walk_marker, "已达到最大层数");
			// 已达到最大层数
			break;
		}
	}

	// 消除一些不需要生成的点
	float water_height = -HEIGHT_NULL;

	if (m_pTerrain->GetWaterExists(x, z))
	{
		water_height = m_pTerrain->GetWaterBaseHeight(x, z);
	}

	int j = 1;

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

bool CDynamicWalkGenerator::GenerateWall()
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

bool CDynamicWalkGenerator::ProcessWallPoint(size_t row, size_t col)
{
	size_t index = row * m_nCols + col;
	design_data_t* dd = m_pDesign + index;

	if (dd->bOutFence)
	{
		return false;
	}

	float x = GetLeft() + (col + 0.5f) * m_fStep;
	float z = GetTop() + (row + 0.5f) * m_fStep;

	int i = 0;

	float beg_height = m_pGroundHeight->pValues[index];

	if (m_pFloorHeights[0])
	{
		float floor_height = m_pFloorHeights[0]->pValues[index];

		if (!height_is_null(floor_height))
		{
			if ((floor_height - beg_height) < (m_fRoleHeight * 0.5f))
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

		if ((end_height - beg_height) < (m_fRoleHeight * 0.5f))
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

bool CDynamicWalkGenerator::TraceWallPoint(size_t row, size_t col,
										   float beg_height, float end_height)
{
	design_data_t* data = m_pDesign + row * m_nCols + col;

	float x = GetLeft() + (col + 0.5f) * m_fStep;
	float z = GetTop() + (row + 0.5f) * m_fStep;

	int total_num = 0;
	int trace_num = 0;

	FmVec3 src;
	FmVec3 dst;

	float radius = m_fStep;
	float sin_angle = sinf(m_fWallAngle / 180.0f * FM_PI);
	float step = m_fRoleHeight * 0.25f;
	float height = beg_height;

	while ((height + step * 0.5f) < end_height)
	{
		// 随着倾斜度改变碰撞检测的范围
		float r = radius + (end_height - (height + step * 0.5f)) * sin_angle;

		// 从两个方向进行碰撞检测
		src.x = x;
		src.y = height + step * 0.5f;
		src.z = z - r;
		dst.x = x;
		dst.y = height + step * 0.5f;
		dst.z = z + r;

		if (m_pTerrain->TraceHitted(src, dst))
		{
			++trace_num;
		}
		else
		{
			src.x = x - r;
			src.y = height + step * 0.5f;
			src.z = z;
			dst.x = x + r;
			dst.y = height + step * 0.5f;
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

	return (float(trace_num) / float(total_num)) >= 0.9f;
}

bool CDynamicWalkGenerator::GenerateEdge()
{
	for (size_t floor = 0; floor < MAX_FLOOR_NUM; ++floor)
	{
		if (NULL == m_pFloorMarkers[floor])
		{
			break;
		}

		// 按行产生边缘点
		for (size_t r = 0; r < m_nRows; ++r)
		{
			ProcessEdgeByRow(floor, r);
		}

		// 按列产生边缘点
		for (size_t c = 0; c < m_nCols; ++c)
		{
			ProcessEdgeByCol(floor, c);
		}

		// 计算边缘点的相邻关系
		//LinkEdge(floor);
	}

	return true;
}

bool CDynamicWalkGenerator::ProcessEdgeByRow(size_t floor, size_t row)
{
	bool first = false;
	bool second = false;

	for (size_t col = 0; col < m_nCols; ++col)
	{
		unsigned char floor_marker = m_pFloorMarkers[floor]->pValues[row * m_nCols + col];
		second = (floor_marker & FLOOR_MARKER_EXISTS) != 0;
		if (first != second)
		{
			size_t c = col;

			if (first)
			{
				--c;
				floor_marker = m_pFloorMarkers[floor]->pValues[row * m_nCols + c];
			}

			floor_marker |= FLOOR_MARKER_EDGE;
			SetMarker(m_pFloorMarkers[floor], row, c, floor_marker);
		}

		first = second;
	}

	second = false;

	if (first)
	{
		unsigned char floor_marker = m_pFloorMarkers[floor]->pValues[row * m_nCols + m_nCols - 1] | FLOOR_MARKER_EDGE;
		SetMarker(m_pFloorMarkers[floor], row, m_nCols - 1, floor_marker);
	}

	return true;
}

bool CDynamicWalkGenerator::ProcessEdgeByCol(size_t floor, size_t col)
{
	bool first = false;
	bool second = false;
	for (size_t row = 0; row < m_nRows; ++row)
	{
		unsigned char floor_marker = m_pFloorMarkers[floor]->pValues[row * m_nCols + col];
		second = (floor_marker & FLOOR_MARKER_EXISTS) != 0;
		if (first != second)
		{
			size_t r = row;

			if (first)
			{
				--r;
				floor_marker = m_pFloorMarkers[floor]->pValues[r * m_nCols + col];
			}

			floor_marker |= FLOOR_MARKER_EDGE;
			SetMarker(m_pFloorMarkers[floor], r, col, floor_marker);
		}

		first = second;
	}

	second = false;

	if (first)
	{
		unsigned char floor_marker = m_pFloorMarkers[floor]->pValues[(m_nRows - 1) * m_nCols + col] | FLOOR_MARKER_EDGE;
		SetMarker(m_pFloorMarkers[floor], m_nRows - 1, col, floor_marker);
	}

	return true;
}

// bool CDynamicWalkGenerator::LinkEdge(size_t floor)
// {
// 	for (size_t r = 0; r < m_EdgeMap[floor].size(); ++r)
// 	{
// 		EDGE_LIST* edge_list[3];
// 
// 		edge_list[1] = m_EdgeMap[floor][r];
// 
// 		if (NULL == edge_list[1])
// 		{
// 			continue;
// 		}
// 
// 		int lr = 0;
// 		int hr = 0;
// 
// 		edge_list[0] = NULL;
// 		if (r != 0)
// 		{
// 			edge_list[0] = m_EdgeMap[floor][r-1];
// 		}
// 		
// 		edge_list[2] = NULL;
// 		if (r != m_EdgeMap[floor].size() - 1)
// 		{
// 			edge_list[2] = m_EdgeMap[floor][r+1];
// 		}
// 
// 		if (NULL != edge_list[0])
// 		{
// 			--lr;
// 		}
// 
// 		if (NULL != edge_list[2])
// 		{
// 			++hr;
// 		}
// 
// 		for (size_t i = 0; i < edge_list[1]->size(); ++i)
// 		{
// 			edge_t& edge = (*edge_list[1])[i];
// 			int row = edge.row;
// 			int col = edge.col;
// 
// 			int lc = -1;
// 			int hc = 1;
// 			if (col == 0)
// 			{
// 				++lc;
// 			}
// 
// 			if (col == m_nCols)
// 			{
// 				--hc;
// 			}
// 
// 			for (int re = lr; re <= hr; ++re)
// 			{
// 				for (int ce = lc; ce <= hc; ++ce)
// 				{
// 					unsigned char floor_marker = m_pFloorMarkers[floor]->pValues[(row+re) * m_nCols + (col+ce)];
// 					if ((floor_marker & FLOOR_MARKER_EDGE) != 0)
// 					{
// 						if (ce == 0 && re == 0)
// 						{
// 							continue;
// 						}
// 						
// 						for (size_t j = 0; j < edge_list[re+1]->size(); ++j)
// 						{
// 							edge_t& e = (*edge_list[re+1])[j];
// 							if (e.col == (col + ce))
// 							{
// 								int offset = 4;
// 								if (3 * re + ce > 0)
// 								{
// 									--offset;
// 								}
// 								edge.sibling[ 3 * re + ce + offset] = &e;
// 								break;
// 							}
// 						}
// 
// 					}
// 				}
// 			}
// 		}
// 	}
// 	return true;
// }
