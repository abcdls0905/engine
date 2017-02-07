//--------------------------------------------------------------------
// �ļ���:		walk_generator.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��8��5��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
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

// ����������Ϣ����

class Terrain;
class CTerrainZone;

class CWalkGenerator
{
private:
	// ���ڵ��־
	enum
	{
		SIBLING_LEFT = 0x1,
		SIBLING_RIGHT = 0x2,
		SIBLING_UP = 0x4,
		SIBLING_DOWN = 0x8,
	};

	// ���߱�־
	enum
	{
		FLAG_DISABLE,
		FLAG_WALKABLE,
		FLAG_UPDATE_WALKABLE,
	};

	// �ϲ���ʽ
	enum
	{
		UPDATE_HEIGHT_MINIMUM,	// ȡ��С�߶�
		UPDATE_HEIGHT_MAXIMUM,	// ȡ���߶�
		UPDATE_HEIGHT_AVERAGE,	// ȡƽ���߶�
	};

	// ��ײ��Ϣ
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

	// ��������Ϣ
	struct design_data_t
	{
		int nTraceCount;
		trace_data_t* pTrace;
		float fHeight;
		unsigned char nFlags;
		unsigned char nCheckSibling;
		bool bOutFence;
	};

	// ��ֹ���߲�����
	struct forbid_t
	{
		FmVec3 pos;
		float fRadius;
	};
	
	// �߶�ͼ
	struct height_map_t;
	// ���ͼ
	struct marker_map_t;

	typedef TArrayPod<FmVec2, 1, TCoreAlloc> point_container_t;
	typedef TPodHashMap<int, point_container_t*, TPodTraits<int>, 
		TCoreAlloc> fence_container_t;

public:
	CWalkGenerator(Terrain* pTerrain, CTerrainZone* pZone, int precision);
	~CWalkGenerator();

	// �������ɲ���
	void SetRoleHeight(float value) { m_fRoleHeight = value; }
	void SetRoleRadius(float value) { m_fRoleRadius = value; }
	void SetStepHeight(float value) { m_fStepHeight = value; }
	void SetTraceHigh(float value) { m_fTraceHigh = value; }
	void SetTraceLow(float value) { m_fTraceLow = value; }
	void SetGroundNormalY(float value) { m_fGroundNormalY = value; }
	void SetFloorNormalY(float value) { m_fFloorNormalY = value; }
	void SetWallAngle(float value) { m_fWallAngle = value; }

	// ��������������߲��
	bool AddPermit(float x, float y, float z);
	// ��ӽ�ֹ�������߲㷶Χ
	bool AddForbid(float x, float y, float z, float radius);
	// ��ӳ���դ����
	bool AddFencePoint(int fence_index, float x, float z);

	// ��ʼ����
	bool GenerateBegin();
	// ��������
	bool GenerateEnd();
	// ���ɵر�߶�
	bool GenerateGround();
	// �����ϰ���߶�
	bool GenerateBalk(int row);
	// ���ɿ���������
	bool GenerateWalkable(float start_x, float start_y, float start_z);
	// ���ɿ����߲�߶�
	bool GenerateFloor();
	// ����ǽ����
	bool GenerateWall();
	// ����ˮ����Ϣ
	bool GenerateWater();

private:
	CWalkGenerator();
	CWalkGenerator(const CWalkGenerator&);
	CWalkGenerator& operator=(const CWalkGenerator&);

	void ReleaseAll();

	// ���ø߶�
	void SetHeight(height_map_t* pHeight, size_t row, size_t col, 
		float value);
	// ���ñ��
	void SetMarker(marker_map_t* pMarker, size_t row, size_t col, 
		unsigned char value);
	
	// ָ��λ���Ƿ��ܹ���������
	bool RoleCollide(float x, float y, float z);
	// �Ƿ������������߲�ĵ�
	bool IsPermitted(float x, float y, float z);
	// �Ƿ��ֹ����
	bool IsForbidden(float x, float y, float z);

	// �߶ȱȽ�
	static int __cdecl HeightCompare(const void* p1, const void* p2);

	// ���¸߶�����
	bool UpdateHeightMap(height_map_t* pHeight, float* pValues, int mode);
	// ���±������
	bool UpdateFlagMap(marker_map_t* pMarker, unsigned char* pValues);

	// ����ڽ��Ƿ������Ƶĸ߶�
	bool GetNearHeight(size_t row, size_t col, float height, float step);

	// ������в�߶�
	bool TraceAllHeight(size_t row, size_t col);

	// ���ɿ�����������Ϣ
	bool ProcessWalkable();
	bool ProcessWalkPoint(size_t beg_row, size_t beg_col, size_t row, 
		size_t col);
	int TraceWalkPoint(size_t beg_row, size_t beg_col, size_t row, 
		size_t col);
	// ��������֮���Ƿ���ͨ
	bool TwoPointCanWalk(float x1, float y1, float z1, float x2, float y2, 
		float z2);

	// ���ɿ����߲���Ϣ
	bool ProcessFloorPoint(size_t row, size_t col);

	// ����ǽ����Ϣ
	bool ProcessWallPoint(size_t row, size_t col);
	bool TraceWallPoint(size_t row, size_t col, float beg_height, 
		float end_height);

	// ����ˮ����Ϣ
	bool ProcessWaterPoint(size_t row, size_t col);

private:
	// ���ڼ�¼��ײ���ɹ����е���Ϣ
	bool Trace(float x, float y, float z, int marker, const char* info);

private:
	Terrain* m_pTerrain;
	CTerrainZone* m_pZone;
	size_t m_nRows;
	size_t m_nCols;
	float m_fStep;
	float m_fWallAngle;			// ǽ�����б�Ƕ�
	float m_fGroundNormalY;		// �ر�Ļ�׼ƽ̹��
	float m_fFloorNormalY;		// �ݶ��Ļ�׼ƽ̹��
	float m_fRoleHeight;		// ��ɫ�ĸ߶�
	float m_fRoleRadius;		// ��ɫ�����ΰ뾶
	float m_fStepHeight;		// �����ߵĸ߶Ȳ�
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

	height_map_t* m_pGroundHeight;					// �ر�߶�
	height_map_t* m_pWaterHeight;					// ˮ��߶�
	height_map_t* m_pFloorHeights[MAX_FLOOR_NUM];	// ��߶�
	height_map_t* m_pSpaceHeights[MAX_FLOOR_NUM];	// ���ϰ��ռ�߶�
	marker_map_t* m_pWalkMarker;					// �����߲���
	marker_map_t* m_pFloorMarkers[MAX_FLOOR_NUM];	// ����

	// ���ڼ�¼��ײ���ɹ����е���Ϣ
	core_string m_strWalkGeneratorTraceFile;
	FILE* m_pWalkGeneratorTraceFile;
};

#endif // _WALK_GENERATOR_H

