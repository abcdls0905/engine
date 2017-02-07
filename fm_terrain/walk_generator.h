//--------------------------------------------------------------------
// 文件名:		walk_generator.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年8月5日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _WALK_GENERATOR_H
#define _WALK_GENERATOR_H

#include "../public/macros.h"
#include "../public/persistid.h"
#include "../public/core_mem.h"
#include "../public/fast_str.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"
#include "../utils/pod_hash_map.h"


#include "../visual/fm_define.h"


#include "terrain_walkable.h"

// 地形行走信息生成

class Terrain;
class CTerrainZone;

class CWalkGenerator
{
private:
	// 相邻点标志
	enum
	{
		SIBLING_LEFT = 0x1,
		SIBLING_RIGHT = 0x2,
		SIBLING_UP = 0x4,
		SIBLING_DOWN = 0x8,
	};

	// 行走标志
	enum
	{
		FLAG_DISABLE,
		FLAG_WALKABLE,
		FLAG_UPDATE_WALKABLE,
	};

	// 合并方式
	enum
	{
		UPDATE_HEIGHT_MINIMUM,	// 取最小高度
		UPDATE_HEIGHT_MAXIMUM,	// 取最大高度
		UPDATE_HEIGHT_AVERAGE,	// 取平均高度
	};

	// 碰撞信息
	struct trace_data_t
	{
		PERSISTID TraceID;
		float fTraceY;
		FmVec3 vNormal;
		bool bFaceDown;
		bool bNotFlat;
		bool bWalkable;
		bool bForbidStand;
		bool bRoleCollide;
		bool bPermitted;
		bool bForbidden;
		bool bPadding;
	};

	// 可行走信息
	struct design_data_t
	{
		int nTraceCount;
		trace_data_t* pTrace;
		float fHeight;
		unsigned char nFlags;
		unsigned char nCheckSibling;
		bool bOutFence;
	};

	// 禁止行走层区域
	struct forbid_t
	{
		FmVec3 pos;
		float fRadius;
	};
	
	// 高度图
	struct height_map_t;
	// 标记图
	struct marker_map_t;

	typedef TArrayPod<FmVec2, 1, TCoreAlloc> point_container_t;
	typedef TPodHashMap<int, point_container_t*, TPodTraits<int>, 
		TCoreAlloc> fence_container_t;

public:
	CWalkGenerator(Terrain* pTerrain, CTerrainZone* pZone, int precision);
	~CWalkGenerator();

	// 设置生成参数
	void SetRoleHeight(float value) { m_fRoleHeight = value; }
	void SetRoleRadius(float value) { m_fRoleRadius = value; }
	void SetStepHeight(float value) { m_fStepHeight = value; }
	void SetTraceHigh(float value) { m_fTraceHigh = value; }
	void SetTraceLow(float value) { m_fTraceLow = value; }
	void SetGroundNormalY(float value) { m_fGroundNormalY = value; }
	void SetFloorNormalY(float value) { m_fFloorNormalY = value; }
	void SetWallAngle(float value) { m_fWallAngle = value; }

	// 添加允许生成行走层点
	bool AddPermit(float x, float y, float z);
	// 添加禁止生成行走层范围
	bool AddForbid(float x, float y, float z, float radius);
	// 添加场景栅栏点
	bool AddFencePoint(int fence_index, float x, float z);

	// 开始生成
	bool GenerateBegin();
	// 结束生成
	bool GenerateEnd();
	// 生成地表高度
	bool GenerateGround();
	// 生成障碍物高度
	bool GenerateBalk(int row);
	// 生成可行走数据
	bool GenerateWalkable(float start_x, float start_y, float start_z);
	// 生成可行走层高度
	bool GenerateFloor();
	// 生成墙面标记
	bool GenerateWall();
	// 生成水面信息
	bool GenerateWater();

private:
	CWalkGenerator();
	CWalkGenerator(const CWalkGenerator&);
	CWalkGenerator& operator=(const CWalkGenerator&);

	void ReleaseAll();

	// 设置高度
	void SetHeight(height_map_t* pHeight, size_t row, size_t col, 
		float value);
	// 设置标记
	void SetMarker(marker_map_t* pMarker, size_t row, size_t col, 
		unsigned char value);
	
	// 指定位置是否能够容纳人物
	bool RoleCollide(float x, float y, float z);
	// 是否允许生成行走层的点
	bool IsPermitted(float x, float y, float z);
	// 是否禁止区域
	bool IsForbidden(float x, float y, float z);

	// 高度比较
	static int __cdecl HeightCompare(const void* p1, const void* p2);

	// 更新高度数据
	bool UpdateHeightMap(height_map_t* pHeight, float* pValues, int mode);
	// 更新标记数据
	bool UpdateFlagMap(marker_map_t* pMarker, unsigned char* pValues);

	// 获得邻近是否有相似的高度
	bool GetNearHeight(size_t row, size_t col, float height, float step);

	// 获得所有层高度
	bool TraceAllHeight(size_t row, size_t col);

	// 生成可行走区域信息
	bool ProcessWalkable();
	bool ProcessWalkPoint(size_t beg_row, size_t beg_col, size_t row, 
		size_t col);
	int TraceWalkPoint(size_t beg_row, size_t beg_col, size_t row, 
		size_t col);
	// 测试两点之间是否连通
	bool TwoPointCanWalk(float x1, float y1, float z1, float x2, float y2, 
		float z2);

	// 生成可行走层信息
	bool ProcessFloorPoint(size_t row, size_t col);

	// 生成墙面信息
	bool ProcessWallPoint(size_t row, size_t col);
	bool TraceWallPoint(size_t row, size_t col, float beg_height, 
		float end_height);

	// 生成水面信息
	bool ProcessWaterPoint(size_t row, size_t col);

private:
	// 用于记录碰撞生成过程中的信息
	bool Trace(float x, float y, float z, int marker, const char* info);

private:
	Terrain* m_pTerrain;
	CTerrainZone* m_pZone;
	size_t m_nRows;
	size_t m_nCols;
	float m_fStep;
	float m_fWallAngle;			// 墙体的倾斜角度
	float m_fGroundNormalY;		// 地表的基准平坦度
	float m_fFloorNormalY;		// 屋顶的基准平坦度
	float m_fRoleHeight;		// 角色的高度
	float m_fRoleRadius;		// 角色的球形半径
	float m_fStepHeight;		// 可行走的高度差
	float m_fTraceHigh;	
	float m_fTraceLow;
	int m_nPrecision;
	int m_nPointCount;
	TArrayPod<unsigned int, 1, TCoreAlloc> m_PointList;
	design_data_t* m_pDesign;
	TArrayPod<FmVec3, 1, TCoreAlloc> m_PermitList;
	TArrayPod<forbid_t, 1, TCoreAlloc> m_ForbidList;
	//std::vector<FmVec2> m_FencePoints;
	//std::map<int, std::vector<FmVec2>> m_FencePoints;
	fence_container_t m_FencePoints;

	height_map_t* m_pGroundHeight;					// 地表高度
	height_map_t* m_pWaterHeight;					// 水面高度
	height_map_t* m_pFloorHeights[MAX_FLOOR_NUM];	// 层高度
	height_map_t* m_pSpaceHeights[MAX_FLOOR_NUM];	// 无障碍空间高度
	marker_map_t* m_pWalkMarker;					// 主行走层标记
	marker_map_t* m_pFloorMarkers[MAX_FLOOR_NUM];	// 层标记

	// 用于记录碰撞生成过程中的信息
	core_string m_strWalkGeneratorTraceFile;
	FILE* m_pWalkGeneratorTraceFile;
};

#endif // _WALK_GENERATOR_H

