//--------------------------------------------------------------------
// �ļ���:		DynamicWalkGenerator.h
// ��  ��:		
// ˵  ��:		
// ��������:	2013��3��4��
// ������:		����ΰ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _DYNAMICWALKGENERATOR_H
#define _DYNAMICWALKGENERATOR_H

#include "dynamic_walkable.h"
#include "../public/core_mem.h"
#include "../public/persistid.h"
#include "../utils/array_pod.h"
#include "../utils/pod_hash_map.h"
#include "../visual/fm_header.h"

class IVisBase;
class Terrain;

class CDynamicWalkGenerator
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
		bool bEntryPoint;
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

	/*struct edge_t
	{
		size_t row;
		size_t col;
		edge_t* sibling[8];
		edge_t()
		{
			for (int i = 0; i < 8; ++i)
			{
				sibling[i] = NULL;
			}
		}
	};*/

	// �߶�ͼ
	struct height_map_t;
	// ���ͼ
	struct marker_map_t;

	typedef TArrayPod<FmVec2, 1, TCoreAlloc> POINT_CONTAINER;
	typedef TPodHashMap<int, POINT_CONTAINER*, TPodTraits<int>, 
		TCoreAlloc> FENCE_CONTAINER;

	//typedef TArrayPod<edge_t, 1, TCoreAlloc> EDGE_LIST;
	//typedef TArrayPod<EDGE_LIST*, 1, TCoreAlloc> EDGE_MAP;

public:
	CDynamicWalkGenerator(Terrain* pTerrain, IVisBase* pVisual, int precesion);
	~CDynamicWalkGenerator();

	// �������ɲ���
	void SetRoleHeight(float value) { m_fRoleHeight = value; }
	void SetRoleRadius(float value) { m_fRoleRadius = value; }
	void SetStepHeight(float value) { m_fStepHeight = value; }
	void SetTraceHigh(float value) { m_fTraceHigh = value; }
	void SetTraceLow(float value) { m_fTraceLow = value; }
	void SetFloorNormalY(float value) { m_fFloorNormalY = value; }
	void SetWallAngle(float value) { m_fWallAngle = value; }

	float GetLeft() const { return m_fLeft; }
	void SetLeft(float value) { m_fLeft = value; }

	float GetTop() const { return m_fTop; }
	void SetTop(float value) { m_fTop = value; }

	// ��������������߲��
	bool AddPermit(float x, float y, float z);
	// ��ӽ�ֹ�������߲㷶Χ
	bool AddForbid(float x, float y, float z, float radius);
	// ��ӳ���դ����
	bool AddFencePoint(int fence_index, float x, float z);
	// ������߲���ڵ�
	bool AddEntryPoint(float x, float y, float z);

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
	// ���ɱ�Ե���
	bool GenerateEdge();

private:
	CDynamicWalkGenerator();
	CDynamicWalkGenerator(const CDynamicWalkGenerator&);
	CDynamicWalkGenerator& operator=(const CDynamicWalkGenerator&);

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
	// �Ƿ���ڵ�
	bool IsEntryPoint(float x, float y, float z);

	// �߶ȱȽ�
	static int __cdecl HeightCompare(const void* p1, const void* p2);

	// ���¸߶�����
	bool UpdateHeightMap(height_map_t* pHeight, float* pValues, int mode);
	// ���±������
	bool UpdateFlagMap(marker_map_t* pMarker, unsigned char* pValues);

	// ����ڽ��Ƿ������Ƶĸ߶�
	bool GetNearHeight(size_t row, size_t col, float height, float step);

	// ����ڽ��Ƿ�����ڵ�
	bool GetNearEntry(size_t row, size_t col, float height, float step);

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

	// ���ɱ�Ե��Ϣ
	bool ProcessEdgeByRow(size_t floor, size_t row);
	bool ProcessEdgeByCol(size_t floor, size_t col);
	// ���ӱ�Ե��Ϣ
	//bool LinkEdge(size_t floor);
private:
	Terrain* m_pTerrain;
	IVisBase* m_pVisual;
	size_t m_nRows;
	size_t m_nCols;
	float m_fLeft;
	float m_fTop;
	float m_fStep;
	float m_fWallAngle;
	float m_fFloorNormalY;
	float m_fRoleHeight;
	float m_fRoleRadius;
	float m_fStepHeight;
	float m_fTraceHigh;
	float m_fTraceLow;
	int m_nPrecision;
	int m_nPointCount;
	FmVec3 m_Angle;
	TArrayPod<unsigned int, 1, TCoreAlloc> m_PointList;
	design_data_t* m_pDesign;
	TArrayPod<FmVec3, 1, TCoreAlloc> m_PermitList;
	TArrayPod<forbid_t, 1, TCoreAlloc> m_ForbidList;
	FENCE_CONTAINER m_FencePoints;
	TArrayPod<FmVec3, 1, TCoreAlloc> m_EntryPoints;

	height_map_t* m_pGroundHeight;					// �ر�߶�
	height_map_t* m_pFloorHeights[MAX_FLOOR_NUM];	// ��߶�
	height_map_t* m_pSpaceHeights[MAX_FLOOR_NUM];	// ���ϰ��ռ�߶�
	marker_map_t* m_pFloorMarkers[MAX_FLOOR_NUM];	// ����

	//EDGE_MAP m_EdgeMap[MAX_FLOOR_NUM];
};

#endif // _DYNAMICWALKGENERATOR_H
