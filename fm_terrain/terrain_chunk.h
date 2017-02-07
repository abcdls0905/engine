//--------------------------------------------------------------------
// 文件名:		terrain_chunk.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _TERRAIN_CHUNK_H
#define _TERRAIN_CHUNK_H

#include "../public/macros.h"
#include "../public/core_mem.h"

#include "../public/i_interface.h"
#include "../visual/i_render.h"
#include "../visual/i_resource.h"
#include "../utils/array_pod.h"
#include "grass_data.h"
#include "terrain.h"


class TTerrainChunkAlloc
{
public:
	TTerrainChunkAlloc() {}
	~TTerrainChunkAlloc() {}

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
	void Swap(TTerrainChunkAlloc& src) {}
};

// 地形分块

//class Terrain;
class CTerrainZone;
class CTerrainHeight;
class CQuadGround;
class CTexAlphaMap;
class CChunkVB;
class IShaderProgram;


class CTerrainChunk
{
public:
	// 相邻的分块
	enum CHUNK_SIBLING
	{
		SIBLING_LEFT,
		SIBLING_RIGHT,
		SIBLING_UP,
		SIBLING_DOWN,
		SIBLING_MAX,
	};
	
	struct blend_tex_t
	{
		size_t nTexIndex;	// 混合贴图编号
		size_t nFormat;
		CTexAlphaMap* pAlphaMap;	// 贴图数据
	};

	typedef TArrayPod<grass_data_t, 1, TTerrainChunkAlloc> grass_container_t;

	struct chunk_grass_t
	{
		grass_container_t GrassData;
		IStaticVB* pVBuffer;
		size_t nActualCount;
	};

	struct chunk_cb_object_shader_handle_t
	{
		IShaderProgram* pShader;

		//UseNormalSceneCB
		fm_int           mat4mtxViewProjHandle;
        fm_int           mat4mtxViewHandle;
		fm_int           mat4mtxProjHandle;
		fm_int           mat4mtxViewProjInverseHandle;
		fm_int           mat4mtxViewInverseHandle;
        fm_int           mat4mtxProjInverseHandle;
		fm_int           vec3ViewPosHandle;
		fm_int           vec3LightDirHandle;
		fm_int           vec3LightAmbientHandle;
		fm_int           fReflectFactorHandle;
		fm_int           vec3LightDiffuseHandle;
		fm_int           vec4FogColorHandle;
		fm_int           vec4FogParamHandle;
		fm_int           vec4FogExpParamHandle;
		fm_int           vec2PixelSizeHandle;
		fm_int           fStereoConvergenceHandle;
		fm_int           fCameraNearZHandle;
		fm_int           vec2HalfPixelSizeHandle;
		fm_int           vec2BlendHalfPixeHandle;
		fm_int           vec3ReferPosHandle;
		fm_int           vec2DepthParamHandle;
        fm_int           fFarClipDistanceHandle;
		fm_int           fAlphaRefHandle;
		fm_int           vec3PointLightPosHandle;
		fm_int           fPointLightRangeHandle;
		fm_int           vec4PointLightDiffuseHandle;
		fm_int           vec4CameraLightDiffuseHandle;
		fm_int           vec4ClipPlaneHandle;
		fm_int           mat4ShadowViewProjHandle;
		fm_int           vec4HeightFogColor;
		fm_int           vec4HeightFogParam;
		//UseChunkGroupCB
		fm_int           vec4TerrainBlockSizeHandle;
		fm_int           vec2GraduallyDisthandle;
		fm_int           vec2Detail0ParamHandle;
		fm_int           vec4TessellationFactorHandle;//实际上目前GLES没有曲面细分的,但是不排除以后搞PC版本的GL
		fm_int           vec4MaterialAmbientHandle;
		fm_int           vec4MaterialDiffuseHandle;
		fm_int           vec4MaterialSpecularHandle;
		fm_int           fTerrainBlockSizeHandle;
		//UseChunkObject
		fm_int           mat4Detail1ParamHandle;
		fm_int           mat4Detail2ParamHandle;
		fm_int           mat4Detail3ParamHandle;
		fm_int           mat4Detail4ParamHandle;
		fm_int           vec3ChunkPosHandle;
		fm_int           vec4ZoneOffsetAndSizeHandle;
		fm_int           fBaseSpecularLevelHandle;
		fm_int           fBaseSpecularPowerHandle;
		fm_int           fBaseHeightScaleHandle;
		fm_int           vec4BlendSpecularLevelHandle;
		fm_int           vec4BlendSpecularPowerHandle;
		fm_int           vec4BlendHeightScaleHandle;
		fm_int           vec3LightMapColorScaleHandle;
		//UseTexure
		fm_int           tex_LightmapHandle;
		fm_int           tex_Detailmap0Handle;
        fm_int           tex_Detailmap1Handle;
        fm_int           tex_Detailmap2Handle;
		fm_int           tex_Detailmap3Handle;
		fm_int           tex_Detailmap4Handle;
        fm_int           tex_BlendmapHandle;
		fm_int           tex_ShadowHandle;
		fm_int           tex_PrelightHandle;
        fm_int           tex_DepthmapHandle;
		fm_int           tex_Bumpmap0Handle;
	    fm_int           tex_Bumpmap1Handle;
		fm_int           tex_Bumpmap2Handle;
	    fm_int           tex_Bumpmap3Handle;
		fm_int           tex_Bumpmap4Handle;
	};


	struct chunk_cb_object_t
	{
		FmMat4 Detail1Param;
        FmMat4 Detail2Param;
		FmMat4 Detail3Param;
		FmMat4 Detail4Param;

		FmVec3 vChunkPos;
		FmVec4 ZoneOffsetAndSize;

		float fBaseSpecularLevel;
		float fBaseSpecularPower;
		float fBaseHeightScale;
		FmVec4 BlendSpecularLevel;
		FmVec4 BlendSpecularPower;
		FmVec4 BlendHeightScale;
	};

	struct chunk_cb_per_scene_t
	{
		FmMat4 mtxViewProj;
		FmMat4 mtxView;
		FmMat4 mtxProj;
		FmMat4 mtxViewProjInverse;
		FmMat4 mtxViewInverse;
		FmMat4 mtxProjInverse;
		FmVec3 vViewPos;
		FmVec3 vLightDir;
		FmVec3 LightAmbient;
		float  fReflectFactor;
		FmVec3 LightDiffuse;
		FmVec4 FogColor;
		FmVec4 FogParam;
		FmVec4 FogExpParam;
		FmVec2 PixelSize;
		float  fStereoConvergence;
		float  fCameraNearZ;
		FmVec2 HalfPixelSize;
		FmVec2 BlendHalfPixel;
		FmVec3 vReferPos;
		FmVec2 DepthParam;  
		float  fFarClipDistance;
		float  fAlphaRef;
		FmVec3 vPointLightPos;
        float  fPointLightRange;
		FmVec4 PointLightDiffuse;
		FmVec4 CameraLightDiffuse;
		FmVec4 ClipPlane;
		FmMat4 mtxShadowViewProj;
		FmVec4 vHeightFogColor;
		FmVec4 vHeightFogParam;
	};

	struct chunk_cb_per_group_t
	{
		FmVec2 GraduallyDist;
		FmVec4 TerrainBlockSize;
		FmVec2 Detail0Param;
		FmVec4 MaterialDiffuse;
		FmVec4 MaterialAmbient;
		FmVec4 MaterialSpecular;
		FmVec4 TessellationFactor;
	};

	struct chunk_grass_cb_per_group_t
	{
		FmVec3 vWinDir;
		float  fWinForce;
		float  fTimer;
		FmVec2 FadeParam;
		FmVec3 vPlayerPoint;
		FmVec4 MaterialDiffuse;
		FmVec4 MaterialAmbient;
	};

	struct grass_shader_handle_t
	{
		IShaderProgram* pShader;

		//UseGrassObject
		fm_int           vec3WinDirHandle;
		fm_int           fWinForceHandle;
        fm_int           fTimerHandle;
        fm_int           vec2FadeParamHandle;
		fm_int           vec3PlayerPointHandle;
		fm_int           vec4MaterialDiffuseHandle;
		fm_int           vec4MaterialAmbientHandle;
		//UseChunkObject
		fm_int           mat4Detail1ParamHandle;
		fm_int           mat4Detail2ParamHandle;
		fm_int           mat4Detail3ParamHandle;
		fm_int           mat4Detail4ParamHandle;
		fm_int           vec3ChunkPosHandle;
		fm_int           vec4ZoneOffsetAndSizeHandle;
		fm_int           fBaseSpecularLevelHandle;
		fm_int           fBaseSpecularPowerHandle;
		fm_int           fBaseHeightScaleHandle;
		fm_int           vec4BlendSpecularLevelHandle;
		fm_int           vec4BlendSpecularPowerHandle;
		fm_int           vec4BlendHeightScaleHandle;
		fm_int           vec3LightMapColorScaleHandle;
        //UseGrassTexture
		fm_int           tex_DiffuseHandle;
		//UseGrassNormalSceneCB
		fm_int           mat4mtxViewProjHandle;
		fm_int           mat4mtxViewHandle;
		fm_int           mat4mtxProjHandle;
		fm_int           mat4mtxViewProjInverseHandle;
		fm_int           mat4mtxViewInverseHandle;
		fm_int           mat4mtxProjInverseHandle;
		fm_int           vec3ViewPosHandle;
		fm_int           vec3LightDirHandle;
		fm_int           vec3LightAmbientHandle;
		fm_int           fReflectFactorHandle;
		fm_int           vec3LightDiffuseHandle;
		fm_int           vec4FogColorHandle;
		fm_int           vec4FogParamHandle;
		fm_int           vec4FogExpParamHandle;
		fm_int           vec2PixelSizeHandle;
		fm_int           fStereoConvergenceHandle;
		fm_int           fCameraNearZHandle;
		fm_int           vec2HalfPixelSizeHandle;
		fm_int           vec2BlendHalfPixeHandle;
		fm_int           vec3ReferPosHandle;
		fm_int           vec2DepthParamHandle;
		fm_int           fFarClipDistanceHandle;
		fm_int           fAlphaRefHandle;
		fm_int           vec3PointLightPosHandle;
		fm_int           fPointLightRangeHandle;
		fm_int           vec4PointLightDiffuseHandle;
		fm_int           vec4CameraLightDiffuseHandle;
		fm_int           vec4ClipPlaneHandle;
	};

	struct chunk_grass_cb_per_render_t
	{
		CTerrainChunk*  pChunk;
		IShaderProgram* pShader;
		IShaderProgram* pEarlyZPassShader;
		ITexture*       pTexture;
		size_t          grass_num;
        IStaticIB*      pIB;
		IStaticVB*      pVB;
		size_t          nIndex;
        CTerrainChunk::grass_shader_handle_t* pShaderHandle;
		CTerrainChunk::grass_shader_handle_t* pEarlyZShaderHandle;
	};

    struct chunk_water_shader_handle_t
	{
		IShaderProgram* pShader;
		//UseWaterPerGroup
		fm_int           mat4mtxViewProjHandle;
		fm_int           fTimeHandle;
		fm_int           vec4ViewPositionHandle;
		fm_int           vec4WaveParam0Handle;
        fm_int           vec4WaveParam1Handle;
		fm_int           vec4WaveParam2Handle;
		fm_int           vec4WaveParam3Handle;
		fm_int           vec4WaveParam4Handle;
		fm_int           vec4WaveParam5Handle;
		fm_int           vec4WaveParam6Handle;
		fm_int           vec4WaveParam7Handle;
		fm_int           fNormalMapScale0Handle;
		fm_int           fNormalMapScale1Handle;
		fm_int           fNormalMapScale2Handle;
		fm_int           fNormalMapScale3Handle;
		fm_int           vec2NormalmapSpeed0Handle;
		fm_int           vec2NormalmapSpeed1Handle;
		fm_int           vec2NormalmapSpeed2Handle;
		fm_int           vec2NormalmapSpeed3Handle;
		fm_int           vec3SunDirectionHandle;
		fm_int           vec4ReferPosHandle;
		fm_int           vec3LightDirHandle;
		fm_int           vec4UnderWaterHandle;
		fm_int           vec4OceanColorHandle;
		fm_int           vec4SkyColorHandle;
		fm_int           vec4SunColorStartHandle;
		fm_int           vec4SunColorEndHandle;
		fm_int           fDistanceScaleHandle;
		fm_int           fReflectionIntenHandle;
		fm_int           fReflectionWeightHandle;
		fm_int           fRefractionIntenHandle;
		fm_int           fRefractionWeightHandle;
		fm_int           fSunLevelHandle;
        fm_int           vec4DepthParamHandle;
		fm_int           fReflectionSunIntenHandle;
		fm_int           vec4FogColorHandle;
		fm_int           vec4FogParamHandle;
		fm_int           vec4FogExpParamHandle;
		fm_int           vec2PixelSizeHandle;
		//UseChunkObject
		fm_int           mat4Detail1ParamHandle;
		fm_int           mat4Detail2ParamHandle;
		fm_int           mat4Detail3ParamHandle;
		fm_int           mat4Detail4ParamHandle;
		fm_int           vec3ChunkPosHandle;
		fm_int           vec4ZoneOffsetAndSizeHandle;
		fm_int           fBaseSpecularLevelHandle;
		fm_int           fBaseSpecularPowerHandle;
		fm_int           fBaseHeightScaleHandle;
		fm_int           vec4BlendSpecularLevelHandle;
		fm_int           vec4BlendSpecularPowerHandle;
		fm_int           vec4BlendHeightScaleHandle;
		fm_int           vec3LightMapColorScaleHandle;
		//UseWaterTexture
		fm_int           tex_SeaReflectHandle;
		fm_int           tex_WaterNormalmap1Handle;
		fm_int           tex_WaterNormalmap2Handle;
		fm_int           tex_WaterNormalmap3Handle;
		fm_int           tex_WaterNormalmap4Handle;
		fm_int           tex_SceneRefractHandle;
		fm_int           tex_DepthmapHandle;
		fm_int           tex_WhiteWaterHandle;
		fm_int           tex_ReflectDepthHandle;

		//Add
		fm_int           vec2RefractUVScaleHandle;
		fm_int           vec2DepthUVScaleHandle;
	};

	struct chunk_water_cb_per_group_t
	{
		FmMat4 mtxViewProj;
		float times;
		FmVec4 view_pos;
		FmVec4 fWaveParam0;
		FmVec4 fWaveParam1;
		FmVec4 fWaveParam2;
		FmVec4 fWaveParam3;
		FmVec4 fWaveParam4;
		FmVec4 fWaveParam5;
		FmVec4 fWaveParam6;
		FmVec4 fWaveParam7;
        float fNormalMapScale0;
        float fNormalMapScale1;
        float fNormalMapScale2;
        float fNormalMapScale3;
		FmVec2 fNormalmapSpeed0;
        FmVec2 fNormalmapSpeed1; 
		FmVec2 fNormalmapSpeed2;
        FmVec2 fNormalmapSpeed3; 
		FmVec3 sun_pos;
		FmVec4 refer_position;
        FmVec3 light_dir;
        FmVec4 water_color;
		FmVec4 sky_color;
		FmVec4 sun_color_start;
		FmVec4 sun_color_end;
		float distance_scale;
		float ref_inten;
		float ref_weight;
		float refraction_inten;
		float refraction_weight;
		float sun_level;
		FmVec4 depth_param;
		float ref_sun_inten;
		FmVec2 pixel_size;
		FmVec4 fUnderWater;
		FmVec4 fog_color;
		FmVec4 fog_param;
		FmVec4 fog_exp_param;
	};
	
private:
	struct terrain_line_t
	{
		float x;
		float y;
		float z;
		unsigned int diffuse;
	};

	struct terrain_water_tex
	{
		bool use_refraction;
		bool use_reflection;
		IColorRT* ReflectionTex;
		ITexture* WaterNormalMap1;
		ITexture* WaterNormalMap2;
		ITexture* WaterNormalMap3;
		ITexture* WaterNormalMap4;
		IColorRT* SceneRefract;
		IColorRT* DepthRT;
		ITexture* WhiteWaterMap;
		IDepthRT* ReflectDepthRT;
	};

public:
	CTerrainChunk(Terrain* pTerrain, CTerrainZone* pZone,
		CTerrainHeight* pHeight, size_t index, size_t row, size_t col);
	~CTerrainChunk();

	CTerrainZone* GetZone() const { return m_pZone; }

	// 唯一标识符
	const char* GetUid() const { return m_strUid; }
	// 包围盒
	const FmVec3& GetMin() const { return m_vMin; }
	const FmVec3& GetMax() const { return m_vMax; }
	// 中心点
	const FmVec3& GetCenter() const { return m_vCenter; };
	// 半径
	float GetRadius() const { return m_fRadius; }
	// 获得LOD误差值
	const float* GetErrors() const { return m_fErrors; }

	// 到摄像机的距离
	void SetCameraDistance(float value) { m_fCameraDistance = value; }
	float GetCameraDistance() const { return m_fCameraDistance; }

	// 在视矩阵中的距离
	void SetViewZ(float value) { m_fViewZ = value; }
	float GetViewZ() const { return m_fViewZ; }

	// 索引
	int GetIndex() const { return m_nIndex; }
	// 行
	int GetRow() const { return m_nRow; }
	// 列
	int GetCol() const { return m_nCol; }
	// LOD级别
	int GetLevel() const { return m_nNewLevel; }
	// 样式
	int GetStyle() const { return m_nStyle; }

	// 在所属区域内的顶点起始行列位置
	int GetLocalRow() const { return m_nLocalRow; }
	int GetLocalCol() const { return m_nLocalCol; }

	// 基础贴图编号
	void SetBaseTexIndex(int value) { m_nBaseTexIndex = value; }
	int GetBaseTexIndex() const { return m_nBaseTexIndex; }

	// 水面类型编号
	void SetWaterIndex(int value);
	int GetWaterIndex() const { return m_nWaterIndex; }

	// 是否可见
	void SetVisible(bool value) { m_bVisible = value; }
	bool GetVisible() const { return m_bVisible; }

	// 混合图是否淡出
	void SetAlphaFade(bool value) { m_bAlphaFade = value; }
	bool GetAlphaFade() const { return m_bAlphaFade; }

	// 混合图是否消隐（无需画混合贴图）
	void SetAlphaHide(bool value) { m_bAlphaHide = value; }
	bool GetAlphaHide() const { return m_bAlphaHide; }

	// 是否在设计范围内
	void SetInDesign(bool value) { m_bInDesign = value; }
	bool GetInDesign() const { return m_bInDesign; }

	// 是否显示包围盒
	void SetShowBoundBox(bool value) { m_bShowBoundBox = value; }
	bool GetShowBoundBox() const { return m_bShowBoundBox; }

	// 是否完全在雾中
	void SetInFog(bool value) { m_bInFog = value; }
	bool GetInFog() const { return m_bInFog; }

	// 生成分块信息
	bool Build(const FmVec3& v_min, const FmVec3& v_max, 
		float* lod_errors, size_t lod_levels);

	// 计算地形误差
	void CalcErrors();
	// 计算包围盒
	void CalcBoundBox();
	// 计算LOD级别
	void CalcLevel(IRender* pRender, const FmMat4& mtxViewProj);
	// 计算分块的样式
	void CalcStyle();
	
	// 强行设置LOD级别
	void SetLevel(int level)
	{
		m_nNewLevel = level;
		m_nStyle = (level << 4);
	}

	void SetChunkLevel(int level)
	{
        m_nChunkLevel = level;
	}

	// 获取当前LOD级别
	int GetChunkLevel()
	{
		return m_nChunkLevel;
	}

	// 设置误差最小的级别 
	void SetMinErrorLevel(int lod_levels)
	{
		int level = 0;
		
		for (int k = 1; k < lod_levels; ++k)
		{
			if (m_fErrors[k] < 0.001F)
			{
				level = k;
			}
			else
			{
				break;
			}
		}
		
		m_nNewLevel = level;
		m_nStyle = (level << 4);
	}

	// 检查LOD级别
	bool ChangeLevel();
	// 保证顶点缓冲符合当前的LOD级别
	//bool CheckVertexBuffer();
	
	// 生成地形顶点数据
	void GenTerrainVertices(void* pv, int level);
	// 生成在雾中的顶点和索引
	size_t GenInFogVertices(FmVec4* pv);
	size_t GenInFogIndices(unsigned short* pi, unsigned short start_index);
	// 生成渲染用的顶点缓冲
	bool CreateVertexBuffer(int level);
	// 释放渲染用的顶点缓冲
	void ReleaseVertexBuffer();

	/*
	// 获得常量缓冲
	IDxStaticCB* GetConstBuffer() const { return m_pChunkCB; }
	// 创建常量缓冲
	IDxStaticCB* CreateConstBuffer(const void* pdata, size_t size);
	*/
	// 释放常量缓冲
	void ReleaseConstBuffer();

	// 获得AlphaMap数量
	size_t GetAlphaMapCount() const;
	// 获得AlphaMap的贴图索引
	size_t GetAlphaMapTexIndex(size_t index) const;
	// 获得AlphaMap的贴图样式
	size_t GetAlphaMapFormat(size_t index) const;
	// 获得AlphaMap
	CTexAlphaMap* GetAlphaMapByIndex(size_t index) const;
	CTexAlphaMap* GetAlphaMap(size_t tex_index, size_t format) const;
	CTexAlphaMap* AddAlphaMap(size_t tex_index, size_t format); 
	// 删除AlphaMap
	bool RemoveAlphaMapByIndex(size_t index);
	// 清除所有为空的AlphaMap
	void ClearEmptyAlphaMap();
	// 释放所有AlphaMap
	void ReleaseAlphaMap();

	// 获得混合贴图
	IStaticTex* GetAlphaTex() const { return m_pAlphaTex; }
	// 创建混合贴图
	IStaticTex* CreateAlphaTex(IRender* pRender);
	// 释放混合贴图
	void ReleaseAlphaTex();

	// 获得混合贴图数据
	void GetBlendTexDataRGBA8888(unsigned char* pdata, size_t data_size);

	void GetBlendTexDataRGBA4444(unsigned char* pdata, size_t data_size);

	// 获得AlphaMap的数量
	size_t GetBlendTexCount() const
	{
		return m_BlendTexs.size();
	}

	// 获得所有AlphaMap
	const blend_tex_t* GetBlendTexs() const
	{
		return &m_BlendTexs[0];
	}

	// 重新计算
	bool Recalculate(); 

	// 设置渲染批次
	bool SetBatch(void* pBatch,int level,IShaderProgram* pShader);
	
	// 画地块包围盒
	bool PaintBoundBox();

	// 编辑用的线条
	bool PrepareDesignLine();
	bool PaintDesignLine();

	// 显示可行走范围
	bool PaintWalkable();
	// 显示范围标记
	bool PaintRegion(const char* region_name);
	// 显示地区标记
	bool PaintArea();
	
	// 更新所有的相邻分块
	void UpdateAllSiblings();
	void UpdateLeftSibling();
	void UpdateRightSibling();
	void UpdateUpSibling();
	void UpdateDownSibling();

	// 没有任何草的数据
	bool GetNoAnyGrass() const { return m_GrassArray.empty(); }
	// 获得花草数据数量
	size_t GetGrassCount(int grass_index) const
	{
		if (size_t(grass_index) >= m_GrassArray.size())
		{
			return 0;
		}

		chunk_grass_t* pChunkGrass = m_GrassArray[grass_index];

		if (NULL == pChunkGrass)
		{
			return 0;
		}

		return pChunkGrass->GrassData.size();
	}

	// 获得花草数据
	const grass_data_t* GetGrasses(int grass_index) const;
	// 获得指定位置的花草数据
	grass_data_t* GetGrass(int grass_index, float x, float z);
	// 添加花草数据
	bool AddGrass(const grass_data_t& data);
	// 删除指定位置的花草
	bool RemoveGrass(int grass_index, float x, float z);
	// 删除方形范围内的花草
	int RemoveGrassRect(int grass_index, float x1, float z1, float x2, 
		float z2);
	// 删除圆形范围内的花草
	int RemoveGrassCircle(int grass_index, float x, float z, float radius);
	// 获得方形范围内的花草数量
	int GetGrassCountRect(int grass_index, float x1, float z1, float x2, 
		float z2);
	// 获得圆形范围内的花草数量
	int GetGrassCountCircle(int grass_index, float x, float z, 
		float radius);
	// 获得方形范围内的花草索引
	void GetGrassRect(float x1, float z1, float x2, float z2, 
		int* grass_index, const int max_grass_count);
	// 获得圆形范围内的花草索引
	void GetGrassCircle(float x, float z, float radius, int* grass_index, 
		const int max_grass_count);
	// 设置花草的顶点缓冲
	void SetGrassVertexBuffer(int grass_index, IStaticVB* pVBuffer,
		size_t grass_num);
	// 获得花草的顶点缓冲
	IStaticVB* GetGrassVertexBuffer(int grass_index) const;
	// 获得实际生成的花草矩形数量
	size_t GetGrassActualCount(int grass_index) const;
	// 释放花草的顶点缓冲
	void ReleaseGrassVertexBuffer();
	
	// 是否保存指定点所在的Chunk的可行走点坐标
	void SetSaveChunkWalkPos(bool value);
	bool GetSaveChunkWalkPos() const;
	// 保存指定点所在的Chunk的可行走点坐标
	bool SaveChunkWalkPos(const char* file_name);

	// 设置Shader
	void SetShader(IShaderProgram* pShader)
	{
		m_pShader = pShader;
	}

	IShaderProgram* GetShader() const
	{
		return m_pShader;
	}

	void SetChunkShader(CTerrainChunk::chunk_cb_object_shader_handle_t* pShaderHandle);

    CTerrainChunk::chunk_cb_object_shader_handle_t* GetChunkShader() const;
	// 设置改材质数据
	void SetMaterial(Terrain::material_t* pMaterial);

	//设置渲染的常量数据
	void SetChunkGroupCB(FmVec4 terrain_block_size,FmVec2 gradually,FmVec2 detail0_param,FmVec4 tess_factor);

	//开始设置常量数据
	void UseChunkGroupCB();

	//设置场景的寄存器值
	void UseNormalSceneCB();

	void SetChunkNormalSceneCB();

	//设置混合数目
	void SetBlendNum(size_t blend_num)
	{ 
		m_blend_num = blend_num;
	}

	// 设置区块使用的光贴图
	void SetZoneLightMap(ITexture* pLightMap);

	//使用纹理设置
	void UseTexture();

	void SetWaterShader(IShaderProgram* pShader);
 
    IShaderProgram* GetWaterShader()const
	{
		return m_pWaterShader;
	}

	void SetWaterShaderHandle(CTerrainChunk::chunk_water_shader_handle_t* pShaderHandle);
   
	CTerrainChunk::chunk_water_shader_handle_t* GetWaterShaderHandle() const;


	void UseChunkObject();

	// 设置草渲染相关
    IShaderProgram* GetGrassShader()const
	{
		return m_pGrassShader;
	}

	void SetGrassShader(IShaderProgram* pShader,IStaticIB* pIB,IStaticVB* pVB,size_t grass_num,CTerrainChunk::grass_shader_handle_t* pShaderHandle);

	void SetGrassTex(ITexture* pTex0);

	void SetGrassPerGroup(CTerrainChunk::chunk_grass_cb_per_group_t& grass_cb_per_group);

	void UseGrassObject();

 	void UseGrassNormalSceneCB();

	void UseGrassChunkObject();

	void UseGrassTexture();

	void DrawGrass(IShaderProgram* pShader);
   
	//水体渲染相关
	void SetWaterPerGroup(CTerrainChunk::chunk_water_cb_per_group_t& cb_per);

 	void SetWaterTexture(bool use_refraction,bool use_reflection,IColorRT* ReflectionTex,
 		                  ITexture* WaterNormalMap1,ITexture* WaterNormalMap2,ITexture* WaterNormalMap3,ITexture* WaterNormalMap4,
						  IColorRT* SceneRefract,IColorRT* DepthRT,ITexture* WhiteWaterMap,IDepthRT* ReflectDepthRT);
		                  

	void SetChunkObject();

	void UseWaterPerGroup();

	void SetWaterLevel(int level);

	void UseWaterObject();

	void UseWaterTexture();
	// 设置渲染批次
	bool SetWaterBatch(void* pBatch,IShaderProgram* pShader);

private:
	CTerrainChunk();
	CTerrainChunk(const CTerrainChunk&);
	CTerrainChunk& operator=(const CTerrainChunk&);

	// 获得相邻的分块
	CTerrainChunk* GetLeftSibling();
	CTerrainChunk* GetRightSibling();
	CTerrainChunk* GetUpSibling();
	CTerrainChunk* GetDownSibling();

	// 获得顶点向量
	FmVec3 GetVector(int row, int col);

	size_t InnerRemoveGrassRect(grass_container_t& pGrass, 
		float x1, float z1, float x2, float z2);
	size_t InnerRemoveGrassCircle(grass_container_t& pGrass, 
		float x, float z, float radius);
	size_t InnerGetGrassRect(grass_container_t& pGrass, 
		float x1, float z1, float x2, float z2);
	size_t InnerGetGrassCircle(grass_container_t& pGrass, 
		float x, float z, float radius);
	void InnerGetGrassIndexRect(grass_container_t& pGrass, 
		float x1, float z1, float x2, float z2, int* grass_index, 
		const int max_grass_count);
	void InnerGetGrassIndexCircle(grass_container_t& pGrass, 
		float x, float z, float radius, int* grass_index, 
		const int max_grass_count);
	
private:
	Terrain* m_pTerrain;
	CTerrainZone* m_pZone;
	CTerrainHeight* m_pHeight;
	CTerrainChunk* m_pSiblings[SIBLING_MAX];
	
	char m_strUid[36];
	FmVec3 m_vMin;
	FmVec3 m_vMax;
	FmVec3 m_vCenter;	// 中心点
	float m_fRadius;		// 半径
	float m_fErrors[8];		// 高度误差
	float m_fCameraDistance;
	float m_fViewZ;
	int m_nIndex;			// 区域内的索引
	int m_nRow;
	int m_nCol;
	int m_nLocalRow;		// 在区域内的起始行列号
	int m_nLocalCol;
	int m_nNewLevel;		// LOD级别
	//int m_nOldLevel;
	int m_nChunkLevel;
	int m_nStyle;			// 样式编号
	bool m_bVisible;		// 是否可视
	bool m_bAlphaFade;		// 混合图是否淡出
	bool m_bAlphaHide;		// 混合图是否消隐（无需画混合贴图）
	bool m_bInDesign;		// 是否在设计范围内
	bool m_bShowBoundBox;	// 是否显示包围盒
	bool m_bInFog;

	CChunkVB* m_pChunkVBs[8];

	int m_nWaterIndex;
	int m_nBaseTexIndex;	// 基础贴图编号
	TArrayPod<blend_tex_t, 4, TTerrainChunkAlloc> m_BlendTexs;
	IStaticTex* m_pAlphaTex;

	TArrayPod<chunk_grass_t*, 4, TTerrainChunkAlloc> m_GrassArray;
	bool m_bHasGrassVertexBuffer;
	
	size_t m_nDesignLineNum;
	terrain_line_t* m_pDesignLines;
	size_t m_nDesignLineMemSize;

	bool m_bSaveChunkWalkPos;
	int m_nShowDesignLine;
	IShaderProgram* m_pShader;
	size_t m_blend_num;
	CTerrainChunk::chunk_cb_object_shader_handle_t* m_pChunkShaderHandle;


	CTerrainChunk::chunk_cb_per_scene_t m_chunk_cb_per_scene;
	CTerrainChunk::chunk_cb_per_group_t m_chunk_cb_per_group;

	IRender* m_pRender;

	//水体渲染相关
	IShaderProgram* m_pWaterShader;
	CTerrainChunk::chunk_water_cb_per_group_t m_water_cb_per_group;
	CTerrainChunk::chunk_cb_object_t    m_chunk_cb_obj;
	CTerrainChunk::chunk_water_shader_handle_t* m_pWaterShaderHandle;

	//草体渲染相关
	IShaderProgram* m_pGrassShader;
	CTerrainChunk::chunk_grass_cb_per_group_t m_grass_cb_per_group;
    IStaticIB*      m_pGrassIB;
    IStaticVB*      m_pGrassVB;
	size_t          m_grass_num;
	ITexture*       m_pGrass_Tex0;
	CTerrainChunk::grass_shader_handle_t* m_pGrassShaderHandle;


	int m_waterlevel;

	CTerrainChunk::terrain_water_tex m_water_tex;
	ITexture*      m_pZoneLightMap;
};

#endif // _TERRAIN_CHUNK_H
