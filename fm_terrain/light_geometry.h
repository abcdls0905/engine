//--------------------------------------------------------------------
// 文件名:		light_geometry.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年11月5日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
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

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
	void Swap(TLightGeometryAlloc& src) {}
};

// 渲染光照的模型信息

class IModel;
class IVisBase;
class Render;
class IImageAccess;
class Terrain;

class CLightGeometry  
{
private:
	// 地形区域
	struct zone_t
	{
		core_string strName;		// 区域名称
		core_string strLightName;	// 光照文件名
		size_t nZoneIndex;			// 区域索引
		size_t nDensity;			// 顶点密度
		float fAddSize;				// 边缘附加距离
		double dUnitX;				// 顶点间距
		double dUnitZ;
		size_t nBufStart;			// 顶点起始位置
		size_t nBufSize;			// 顶点数量
		size_t nVertexRows;			// 区域顶点行列数
		size_t nVertexCols;
		size_t nHelperRows;			// 辅助顶点行列数
		size_t nHelperCols;
	};
	
	// 模型对象
	struct object_t
	{
		object_t()
		{
			pModel = NULL;
			nBufSize = 0;
			bSaveLight = true;
		};

		core_string strName;		// 模型名称
		core_string strLightName;	// 光照文件名
		IModel* pModel;				// 模型指针
		PERSISTID ModelId;			// 模型对象
		size_t nBufStart;			// 顶点起始位置
		size_t nBufSize;			// 模型顶点数量
		bool bSaveLight;			// 是否保存光照文件
		bool bLightMap;				// 是否生成光贴图
		int nLightMapSize;			// 光贴图尺寸
		int nLightMapFill;			// 光贴图填充像素数量
	};

	// 像素渲染标志
	enum LIGHT_MAP_PIXEL_FLAG
	{
		PIXEL_NONE,		// 未渲染
		PIXEL_RENDER,	// 已渲染
		PIXEL_EXTEND,	// 扩展像素
	};

	// 光贴图像素信息
	struct light_map_pixel_t
	{
		int nVertexIndex;
		int nFlags;
	};

	// 模型光贴图
	struct light_map_t
	{
		core_string strName;		// 模型名称
		core_string strLightName;	// 光照文件名
		size_t nObjectIndex;		// 模型对象索引
		size_t nMaterialIndex;		// 模型材质索引
		size_t nBufStart;			// 顶点起始位置
		size_t nBufSize;			// 模型顶点数量
		float fFaceAreaSum;			// 所有三角形的面积之和
		size_t nFillOffset;			// 填充范围
		size_t nPixelWidth;			// 贴图尺寸	
		size_t nPixelHeight;
		light_map_pixel_t* pPixels;	// 像素信息
		IImageAccess* pAOMapData;	// 静态环境遮挡图数据
	};

public:
	CLightGeometry();
	~CLightGeometry();

	void ReleaseAll();
	
	void SetTerrain(Terrain* value) { m_pTerrain = value; }
	void SetRender(Render* value) { m_pRender = value; }

	// 是否使用模型原有顶点色
	void SetUseColor(bool value) { m_bUseColor = value; }
	bool GetUseColor() const { return m_bUseColor; }

	// 是否只渲染模型
	void SetOnlyGround(bool value) { m_bOnlyGround = value; }
	bool GetOnlyGround() const { return m_bOnlyGround; }

	// 是否保存遮挡信息
	void SetSaveOcclusion(bool value) { m_bSaveOcclusion = value; }
	bool GetSaveOcclusion() const { return m_bSaveOcclusion; }

	// 是否根据面积自动选择光贴图大小降级
	void SetLightMapDegrade(bool value) { m_bLightMapDegrade = value; }
	bool GetLightMapDegrade() const { return m_bLightMapDegrade; }

	// 物体光贴图大小
	void SetLightMapSize(int value) { m_nLightMapSize = value; }
	int GetLightMapSize() const { return m_nLightMapSize; }

	// 物体光贴图填充范围
	void SetLightMapFill(int value) { m_nLightMapFill = value; }
	int GetLightMapFill() const { return m_nLightMapFill; }
	
	// 物体光照颜色分辨率
	void SetLightPerUnit(size_t value) { m_nLightPerUnit = (int)value; }
	size_t GetLightPerUnit() const { return m_nLightPerUnit; }

	// 区域光照文件路径
	void SetZonePath(const char* value);
	const char* GetZonePath() const;

	// 模型光照文件路径
	void SetModelPath(const char* value);
	const char* GetModelPath() const;
	
	// 模型光照文件后缀
	void SetModelSuffix(const char* value);
	const char* GetModelSuffix() const;
	
	// 添加地形区域
	bool AddZone(int zone_index, int density, float add_size, int tex_scale);

	// 添加模型
	bool AddObject(const char* name, const PERSISTID& model, bool save_light, 
		const char* light_type, int light_map_size, int light_map_fill);
	
	// 预处理
	bool Process(int light_num);

	// 碰撞检测是否有阻挡
	bool TraceHitted(const FmVec4& src, const FmVec4& dst);
	// 碰撞测试详细信息
	bool TraceDetail(const FmVec4& src, const FmVec4& dst,
		trace_info_t& result);
	
	// 更新模型顶点色
	void UpdateModelColors();
	// 更新区域光贴图
	void UpdateZoneColors();
	// 保存模型光照文件 
	bool SaveModelLight();
	// 保存地形光照文件
	bool SaveZoneLight();

	// 顶点
	size_t GetVertexCount() const { return m_Vertices.size(); }

	vertex_t** GetVertices() 
	{ 
		if (m_Vertices.empty())
		{
			return NULL;
		}
		
		return &m_Vertices[0]; 
	}

	// 三角形
	size_t GetTriangleCount() const { return m_Triangles.size(); }
	
	triangle_t** GetTriangles() 
	{ 
		if (m_Triangles.empty())
		{
			return NULL;
		}
		
		return &m_Triangles[0]; 
	}

	// 包围盒
	const FmVec4& GetMin() const { return m_vMin; }
	const FmVec4& GetMax() const { return m_vMax; }

	// 对角线长度
	float GetRadius() const { return m_fRadius; }

private:
	// 添加三角形
	bool AddTriangle(size_t i1, size_t i2, size_t i3, bool trace);
	// 添加光贴图独立顶点的三角形
	bool AddLightMapTriangle(size_t i1);
	// 预处理区域
	bool ProcessZones();
	// 预处理模型
	bool ProcessObjects(size_t start_index);
	// 预处理光贴图
	bool ProcessLightMaps(size_t index);
	// 计算光贴图的面积
	bool CalcLightMapArea(light_map_t* pLightMap);
	// 定位光贴图像素的空间位置和法线
	bool LocateLightMapPixels(light_map_t* pLightMap);
	// 计算顶点的包围盒
	bool CalcBoundBox(size_t start_index, FmVec4& v_min, 
		FmVec4& v_max);
	// 获得区域内的顶点高度
	float GetY(size_t start, size_t rows, size_t cols, int row, int col, 
		float def);
	// 获得区域三角形的法线
	bool GetTriNormal(size_t start, size_t rows, size_t cols, int row, 
		int col, FmVec4& normal);

	// 获得模型的光照类型
	const char* GetModelLightType(IVisBase* pModel);
	
	// 保存模型顶点色
	bool SaveModelVertexColor();
	// 保存模型光贴图
	bool SaveModelLightMap();

	// 更新模型顶点色
	bool UpdateModelVertexColor();
	// 更新模型光贴图
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