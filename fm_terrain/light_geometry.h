//--------------------------------------------------------------------
// �ļ���:		light_geometry.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��11��5��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------
/*
#ifndef _LIGHT_GEOMETRY_H
#define _LIGHT_GEOMETRY_H

#include "../public/macros.h"
#include "../public/persistid.h"
#include "../public/core_mem.h"
#include "../public/fast_str.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"
#include "../utils/memory_bucket.h"
#include "../visual/i_visual.h"
#include "light_types.h"

class TLightGeometryAlloc
{
public:
	TLightGeometryAlloc() {}
	~TLightGeometryAlloc() {}

	// �����ڴ�
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// ����
	void Swap(TLightGeometryAlloc& src) {}
};

// ��Ⱦ���յ�ģ����Ϣ

class IModel;
class IVisBase;
class Render;
class IImageAccess;
class Terrain;

class CLightGeometry  
{
private:
	// ��������
	struct zone_t
	{
		core_string strName;		// ��������
		core_string strLightName;	// �����ļ���
		size_t nZoneIndex;			// ��������
		size_t nDensity;			// �����ܶ�
		float fAddSize;				// ��Ե���Ӿ���
		double dUnitX;				// ������
		double dUnitZ;
		size_t nBufStart;			// ������ʼλ��
		size_t nBufSize;			// ��������
		size_t nVertexRows;			// ���򶥵�������
		size_t nVertexCols;
		size_t nHelperRows;			// ��������������
		size_t nHelperCols;
	};
	
	// ģ�Ͷ���
	struct object_t
	{
		object_t()
		{
			pModel = NULL;
			nBufSize = 0;
			bSaveLight = true;
		};

		core_string strName;		// ģ������
		core_string strLightName;	// �����ļ���
		IModel* pModel;				// ģ��ָ��
		PERSISTID ModelId;			// ģ�Ͷ���
		size_t nBufStart;			// ������ʼλ��
		size_t nBufSize;			// ģ�Ͷ�������
		bool bSaveLight;			// �Ƿ񱣴�����ļ�
		bool bLightMap;				// �Ƿ����ɹ���ͼ
		int nLightMapSize;			// ����ͼ�ߴ�
		int nLightMapFill;			// ����ͼ�����������
	};

	// ������Ⱦ��־
	enum LIGHT_MAP_PIXEL_FLAG
	{
		PIXEL_NONE,		// δ��Ⱦ
		PIXEL_RENDER,	// ����Ⱦ
		PIXEL_EXTEND,	// ��չ����
	};

	// ����ͼ������Ϣ
	struct light_map_pixel_t
	{
		int nVertexIndex;
		int nFlags;
	};

	// ģ�͹���ͼ
	struct light_map_t
	{
		core_string strName;		// ģ������
		core_string strLightName;	// �����ļ���
		size_t nObjectIndex;		// ģ�Ͷ�������
		size_t nMaterialIndex;		// ģ�Ͳ�������
		size_t nBufStart;			// ������ʼλ��
		size_t nBufSize;			// ģ�Ͷ�������
		float fFaceAreaSum;			// ���������ε����֮��
		size_t nFillOffset;			// ��䷶Χ
		size_t nPixelWidth;			// ��ͼ�ߴ�	
		size_t nPixelHeight;
		light_map_pixel_t* pPixels;	// ������Ϣ
		IImageAccess* pAOMapData;	// ��̬�����ڵ�ͼ����
	};

public:
	CLightGeometry();
	~CLightGeometry();

	void ReleaseAll();
	
	void SetTerrain(Terrain* value) { m_pTerrain = value; }
	void SetRender(Render* value) { m_pRender = value; }

	// �Ƿ�ʹ��ģ��ԭ�ж���ɫ
	void SetUseColor(bool value) { m_bUseColor = value; }
	bool GetUseColor() const { return m_bUseColor; }

	// �Ƿ�ֻ��Ⱦģ��
	void SetOnlyGround(bool value) { m_bOnlyGround = value; }
	bool GetOnlyGround() const { return m_bOnlyGround; }

	// �Ƿ񱣴��ڵ���Ϣ
	void SetSaveOcclusion(bool value) { m_bSaveOcclusion = value; }
	bool GetSaveOcclusion() const { return m_bSaveOcclusion; }

	// �Ƿ��������Զ�ѡ�����ͼ��С����
	void SetLightMapDegrade(bool value) { m_bLightMapDegrade = value; }
	bool GetLightMapDegrade() const { return m_bLightMapDegrade; }

	// �������ͼ��С
	void SetLightMapSize(int value) { m_nLightMapSize = value; }
	int GetLightMapSize() const { return m_nLightMapSize; }

	// �������ͼ��䷶Χ
	void SetLightMapFill(int value) { m_nLightMapFill = value; }
	int GetLightMapFill() const { return m_nLightMapFill; }
	
	// ���������ɫ�ֱ���
	void SetLightPerUnit(size_t value) { m_nLightPerUnit = (int)value; }
	size_t GetLightPerUnit() const { return m_nLightPerUnit; }

	// ��������ļ�·��
	void SetZonePath(const char* value);
	const char* GetZonePath() const;

	// ģ�͹����ļ�·��
	void SetModelPath(const char* value);
	const char* GetModelPath() const;
	
	// ģ�͹����ļ���׺
	void SetModelSuffix(const char* value);
	const char* GetModelSuffix() const;
	
	// ��ӵ�������
	bool AddZone(int zone_index, int density, float add_size, int tex_scale);

	// ���ģ��
	bool AddObject(const char* name, const PERSISTID& model, bool save_light, 
		const char* light_type, int light_map_size, int light_map_fill);
	
	// Ԥ����
	bool Process(int light_num);

	// ��ײ����Ƿ����赲
	bool TraceHitted(const FmVec4& src, const FmVec4& dst);
	// ��ײ������ϸ��Ϣ
	bool TraceDetail(const FmVec4& src, const FmVec4& dst,
		trace_info_t& result);
	
	// ����ģ�Ͷ���ɫ
	void UpdateModelColors();
	// �����������ͼ
	void UpdateZoneColors();
	// ����ģ�͹����ļ� 
	bool SaveModelLight();
	// ������ι����ļ�
	bool SaveZoneLight();

	// ����
	size_t GetVertexCount() const { return m_Vertices.size(); }

	vertex_t** GetVertices() 
	{ 
		if (m_Vertices.empty())
		{
			return NULL;
		}
		
		return &m_Vertices[0]; 
	}

	// ������
	size_t GetTriangleCount() const { return m_Triangles.size(); }
	
	triangle_t** GetTriangles() 
	{ 
		if (m_Triangles.empty())
		{
			return NULL;
		}
		
		return &m_Triangles[0]; 
	}

	// ��Χ��
	const FmVec4& GetMin() const { return m_vMin; }
	const FmVec4& GetMax() const { return m_vMax; }

	// �Խ��߳���
	float GetRadius() const { return m_fRadius; }

private:
	// ���������
	bool AddTriangle(size_t i1, size_t i2, size_t i3, bool trace);
	// ��ӹ���ͼ���������������
	bool AddLightMapTriangle(size_t i1);
	// Ԥ��������
	bool ProcessZones();
	// Ԥ����ģ��
	bool ProcessObjects(size_t start_index);
	// Ԥ�������ͼ
	bool ProcessLightMaps(size_t index);
	// �������ͼ�����
	bool CalcLightMapArea(light_map_t* pLightMap);
	// ��λ����ͼ���صĿռ�λ�úͷ���
	bool LocateLightMapPixels(light_map_t* pLightMap);
	// ���㶥��İ�Χ��
	bool CalcBoundBox(size_t start_index, FmVec4& v_min, 
		FmVec4& v_max);
	// ��������ڵĶ���߶�
	float GetY(size_t start, size_t rows, size_t cols, int row, int col, 
		float def);
	// ������������εķ���
	bool GetTriNormal(size_t start, size_t rows, size_t cols, int row, 
		int col, FmVec4& normal);

	// ���ģ�͵Ĺ�������
	const char* GetModelLightType(IVisBase* pModel);
	
	// ����ģ�Ͷ���ɫ
	bool SaveModelVertexColor();
	// ����ģ�͹���ͼ
	bool SaveModelLightMap();

	// ����ģ�Ͷ���ɫ
	bool UpdateModelVertexColor();
	// ����ģ�͹���ͼ
	bool UpdateModelLightMap();
	
private:
	Terrain* m_pTerrain;
	Render* m_pRender;
	FmVec4 m_vMin;
	FmVec4 m_vMax;
	float m_fRadius;
	bool m_bUseColor;
	bool m_bOnlyGround;
	bool m_bSaveOcclusion;
	bool m_bLightMapDegrade;
	int m_nLightNum;
	int m_nLightMapSize;
	int m_nLightMapFill;
	int m_nLightPerUnit;
	core_string m_strZonePath;
	core_string m_strModelPath;
	core_string m_strModelSuffix;
	TArrayPod<zone_t*, 1, TLightGeometryAlloc> m_Zones;
	TArrayPod<object_t*, 1, TLightGeometryAlloc> m_Objects;
	TArrayPod<light_map_t*, 1, TLightGeometryAlloc> m_LightMaps;
	TArrayPod<shadow_t*, 1, TLightGeometryAlloc> m_Shadows;
	TArrayPod<vertex_t*, 1, TLightGeometryAlloc> m_Vertices;
	TArrayPod<triangle_t*, 1, TLightGeometryAlloc> m_Triangles;
	TMemoryBucket<vertex_t, 256, TLightGeometryAlloc> m_VertexBucket;
	TMemoryBucket<triangle_t, 256, TLightGeometryAlloc> m_TriangleBucket;
};

#endif // _LIGHT_GEOMETRY_H
*/