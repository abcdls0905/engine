//--------------------------------------------------------------------
// 文件名:		terrain.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _TERRAIN_H
#define _TERRAIN_H

#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"
#include "../visual/i_resource.h"
#include "../visual/i_terrain.h"
#include "../visual/i_render.h"

#include "name_list.h"

// 地形文件标识符
#define TERRAIN_FILE_SIGNATURE "FxTer\n\n\n"
// 编辑用的宏开关
//#define TERRAIN_EDIT

// 地形

class TerrainEditor;
class TerrainBrush;
class TerrainMaterial;
class CChunkStyle;
class CBlendTexSet;
class CTerrainHeight;
class CTerrainZone;
class CTerrainChunk;
class CTerrainCulling;
class CTerrainPainter;
class CTerrainWater;
class CTerrainGrass;
class CQuadCollide;
class CZoneManager;
class CVisualManager;
class CChunkVB;
class CLightMapConfig;
class CDynamicWalkLoader;
class CDynamicWalkableManager;

class Terrain: public ITerrain
{
public:
	// 基础贴图或混合贴图的最大数量
	enum { MAX_TEX_NUM = 256 };

	struct material_t
	{
		FmVec4 vDiffuse;
		FmVec4 vAmbient;
		FmVec4 vSpecular;
		FmVec4 vEmissive;
		float fSpecularPower;
		bool bSpecularEnable;
	};

private:
	struct area_color_t
	{
		char* pName;
		unsigned int nColor;
	};

public:
	Terrain();
	virtual ~Terrain();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// 设置场景上下文
	virtual void SetContext(IRenderContext* value);
	virtual IRenderContext* GetContext() const;

	// 获得世界转换矩阵
	virtual void GetWorldMatrix(FmMat4& mat); 

	virtual bool Load();
	
	virtual void Update(float seconds);
	virtual void Realize();
	virtual void RealizeShadowMap();

	// 是否完成加载（同步或异步）
	virtual bool GetLoadFinish();
	// 碰撞测试详细信息
	virtual bool TraceDetail(const FmVec3& src, const FmVec3& dst,
		trace_info_t& result);
	// 碰撞测试是否有阻挡
	virtual bool TraceHitted(const FmVec3& src, const FmVec3& dst);
	// 球体范围碰撞
	virtual bool TraceSphere(const FmVec3& center, float radius);
	// 拣选
	virtual bool Cull(const FmPlane* planes, size_t plane_num, 
		const FmVec3& center, float radius, CULL_FUNC cull_func, 
		void* context);
	
	// 画水面反射贴图
	void RealizeReflection(size_t water_index);

	// 获得加载百分比
	float GetLoadPercent();

	// 获得地形编辑器对象
	PERSISTID GetEditorID();
	// 获得地形刷子对象
	PERSISTID GetBrushID();
	// 获得地形材质对象
	PERSISTID GetMaterialID();

	// 当前玩家角色对象
	void SetPlayerID(const PERSISTID& value) { m_PlayerID = value; }
	PERSISTID GetPlayerID() const { return m_PlayerID; }

	// 天空对象
	void SetSkyID(const PERSISTID& value) { m_SkyID = value; }
	PERSISTID GetSkyID() const { return m_SkyID; }

	// 获得每个地形区域的顶点行列数
	size_t GetZoneScale() const { return m_nZoneScale; }
	int GetZoneScaleAttr() const { return (int)m_nZoneScale; }
	// 获得每个分块的顶点行列数
	size_t GetChunkScale() const { return m_nChunkScale; }
	int GetChunkScaleAttr() const { return (int)m_nChunkScale; }
	// 获得地面贴图的大小
	int GetTextureScale() const { return (int)m_nTextureScale; }
	// 获得贴图重复的格子范围
	int GetTextureUnits() const { return (int)m_nTextureUnits; }
	// 获得顶点之间的ALPHA值数量
	int GetAlphaPerUnit() const { return (int)m_nAlphaPerUnit; }
	// 获得每个分块的ALPHA贴图尺寸
	size_t GetAlphaMapScale() const { return m_nAlphaMapScale; }
	int GetAlphaMapScaleAttr() const { return (int)m_nAlphaMapScale; }
	// 获得每个地形区域的ALPHA值行列数
	size_t GetAlphaScale() const { return m_nAlphaScale; }
	int GetAlphaScaleAttr() const { return (int)m_nAlphaScale; }
	// 获得LOD级别数
	size_t GetLodLevels() const { return m_nLodLevels; }
	int GetLodLevelsAttr() const { return (int)m_nLodLevels; }
	// 获得碰撞分辨率
	size_t GetCollidePerUnit() const { return m_nCollidePerUnit; }
	int GetCollidePerUnitAttr() const { return (int)m_nCollidePerUnit; }
	// 获得碰撞单位格尺寸
	float GetCollideUnitSize() const { return m_fCollideSize; }
	
	int GetZoneRows() const;
	int GetZoneCols() const;
	int GetZoneOriginRow() const;
	int GetZoneOriginCol() const;
	int GetZoneRange() const;
	
	// 编辑模式
	void SetDesignMode(bool value) { m_bDesignMode = value; }
	bool GetDesignMode() const { return m_bDesignMode; }
	bool GetDesignModeAttr() const { return m_bDesignMode; }

	// 显示设计线条的方式
	void SetShowDesignLine(int value) { m_nShowDesignLine = value; }
	int GetShowDesignLine() const { return m_nShowDesignLine; }

	// 是否显示行走范围
	void SetShowWalkable(bool value) { m_bShowWalkable = value; }
	bool GetShowWalkable() const { return m_bShowWalkable; }

	// 是否显示行走空间高度
	void SetShowSpaceHeight(bool value) { m_bShowSpaceHeight = value; }
	bool GetShowSpaceHeight() const { return m_bShowSpaceHeight; }

	// 是否显示点光源范围
	void SetShowLightRange(bool value) { m_bShowLightRange = value; }
	bool GetShowLightRange() const { return m_bShowLightRange; }
	
	// 是否显示范围标记
	void SetShowRegion(bool value) { m_bShowRegion = value; }
	bool GetShowRegion() const { return m_bShowRegion; }

	// 显示指定范围标记名
	void SetShowRegionName(const char* value) { m_strShowRegionName = value; }
	const char* GetShowRegionName() const { return m_strShowRegionName.c_str(); }

	// 是否显示地区标记
	void SetShowArea(bool value) { m_bShowArea = value; }
	bool GetShowArea() const { return m_bShowArea; }

	// 是否显示图标模型
	void SetShowIcon(bool value) { m_bShowIcon = value; }
	bool GetShowIcon() const { return m_bShowIcon; }

	// 是否显示碰撞属性
	void SetShowCollide(bool value) { m_bShowCollide = value; }
	bool GetShowCollide() const { return m_bShowCollide; }

	// 是否在显示编辑用方格间不留边
	void SetShowDesignNoEdge(bool value) { m_bShowDesignNoEdge = value; }
	bool GetShowDesignNoEdge() const { return m_bShowDesignNoEdge; }

	// 是否显示树
	void SetShowTree(bool value) { m_bShowTree = value; }
	bool GetShowTree() const { return m_bShowTree; }

	// 是否显示区域的四叉树结构
	void SetShowQuadTree(bool value) { m_bShowQuadTree = value; }
	bool GetShowQuadTree() const { return m_bShowQuadTree; }

	// 远处的缺省LOD级别
	void SetFarLodLevel(int value) { m_nFarLodLevel = value; }
	int GetFarLodLevel() const { return m_nFarLodLevel; }

	// 需要计算LOD级别的范围半径
	void SetLodRadius(float value) { m_fLodRadius = value; }
	float GetLodRadius() const { return m_fLodRadius; }

	// 不需要计算LOD级别的范围半径
	void SetNoLodRadius(float value) { m_fNoLodRadius = value; }
	float GetNoLodRadius() const { return m_fNoLodRadius; }

	// 编辑半径
	void SetDesignRadius(float value) { m_fDesignRadius = value; }
	float GetDesignRadius() const { return m_fDesignRadius; }

	// 碰撞信息显示半径
	void SetCollideRadius(float value) { m_fCollideRadius = value; }
	float GetCollideRadius() const { return m_fCollideRadius; }

	// 混合图淡出半径	
	void SetAlphaFadeRadius(float value) { m_fAlphaFadeRadius = value; }
	float GetAlphaFadeRadius() const { return m_fAlphaFadeRadius; }
	float GetAlphaFadeRadiusAttr() const { return m_fAlphaFadeRadius; }
	
	// 混合图消隐半径（不需要画混合贴图）	
	void SetAlphaHideRadius(float value) { m_fAlphaHideRadius = value; }
	float GetAlphaHideRadius() const { return m_fAlphaHideRadius; }
	float GetAlphaHideRadiusAttr() const { return m_fAlphaHideRadius; }

	// 花草淡出半径
	void SetGrassRadius(float value) { m_fGrassRadius = value; }
	float GetGrassRadius() const { return m_fGrassRadius; }
	float GetGrassRadiusAttr() const { return m_fGrassRadius; }

	// 花草精度
	void SetGrassLod(float value) { m_fGrassLod = value; }
	float GetGrassLod() const { return m_fGrassLod; }
	float GetGrassLodAttr() const { return m_fGrassLod; }

	// 可见对象的LOD计算起始距离
	void SetVisualLodBegin(float value) { m_fVisualLodBegin = value; }
	float GetVisualLodBegin() const { return m_fVisualLodBegin; }
	float GetVisualLodBeginAttr() const { return m_fVisualLodBegin; }
	
	// 可见对象的LOD计算结束距离
	void SetVisualLodEnd(float value) { m_fVisualLodEnd = value; }
	float GetVisualLodEnd() const { return m_fVisualLodEnd; }
	float GetVisualLodEndAttr() const { return m_fVisualLodEnd; }

	// 是否可见
	void SetWaterVisible(bool value);// { m_bWaterVisible = value; }
	bool GetWaterVisible() const { return m_bWaterVisible; }
	void SetGroundVisible(bool value) { m_bGroundVisible = value; }
	bool GetGroundVisible() const { return m_bGroundVisible; }
	void SetVisualVisible(bool value) { m_bVisualVisible = value; }
	bool GetVisualVisible() const { return m_bVisualVisible; }
	void SetHelperVisible(bool value) { m_bHelperVisible = value; }
	bool GetHelperVisible() const { return m_bHelperVisible; }
	
	// 只显示三角形线
	void SetWaterWire(bool value) { m_bWaterWire = value; }
	bool GetWaterWire() const { return m_bWaterWire; }
	void SetGroundWire(bool value) { m_bGroundWire = value; }
	bool GetGroundWire() const { return m_bGroundWire; }
	void SetVisualWire(bool value) { m_bVisualWire = value; }
	bool GetVisualWire() const { return m_bVisualWire; }
	
	// 是否不加载光照
	void SetNoLight(bool value) { m_bNoLight = value; }
	bool GetNoLight() const { return m_bNoLight; }

	// 是否绘制简化地表
	void SetSimpleGround(bool value) { m_bSimpleGround = value; }
	bool GetSimpleGround() const { return m_bSimpleGround; }

	// 是否使用低平线剪裁
	void SetHorizontalCulling(bool value) { m_bHorizontalCulling = value; }
	bool GetHorizontalCulling() const { return m_bHorizontalCulling; }

	// 是否开启地面物体淡入
	void SetVisualFadeIn(bool value) { m_bVisualFadeIn = value; }
	bool GetVisualFadeIn() const { return m_bVisualFadeIn; }

	// 是否开启平滑地平线
	void SetSmoothHorizontal(bool value) { m_bSmoothHorizontal = value; }
	bool GetSmoothHorizontal() const { return m_bSmoothHorizontal; }

	// 地表LOD降级数量
	void SetGroundLevelDown(int value) { m_nGroundLevelDown = value; }
	int GetGroundLevelDown() const { return m_nGroundLevelDown; }

	// 地形倒影质量
	void SetGroundEnvMap(int value) { m_nGroundEnvMap = value; }
	int GetGroundEnvMap() const { return m_nGroundEnvMap; }

	// 地面物体倒影质量
	void SetVisualEnvMap(int value) { m_nVisualEnvMap = value; }
	int GetVisualEnvMap() const { return m_nVisualEnvMap; }

	// 地块排序
	void SetGroundSortOrder(int value) { m_nGroundSortOrder = value; }
	int GetGroundSortOrder() const { return m_nGroundSortOrder; }

	// 地面物体排序方式
	void SetVisualSortOrder(int value) { m_nVisualSortOrder = value; }
	int GetVisualSortOrder() const { return m_nVisualSortOrder; }

	// 最近剪裁半径
	void SetClipRadiusNear(float value) { m_fClipRadiusNear = value; }
	float GetClipRadiusNear() const { return m_fClipRadiusNear; }

	// 最远剪裁半径
	void SetClipRadiusFar(float value) { m_fClipRadiusFar = value; }
	float GetClipRadiusFar() const { return m_fClipRadiusFar; }

	// 最小物体半径
	void SetVisualRadiusSmall(float value) { m_fVisualRadiusSmall = value; }
	float GetVisualRadiusSmall() const { return m_fVisualRadiusSmall; }

	// 最大物体半径
	void SetVisualRadiusBig(float value) { m_fVisualRadiusBig = value; }
	float GetVisualRadiusBig() const { return m_fVisualRadiusBig; }

	// 物体加载的附加距离
	void SetLoadRadiusAdd(float value) { m_fLoadRadiusAdd = value; }
	float GetLoadRadiusAdd() const { return m_fLoadRadiusAdd; }
	
	// 物体卸载的附加距离
	void SetUnloadRadiusAdd(float value) { m_fUnloadRadiusAdd = value; }
	float GetUnloadRadiusAdd() const { return m_fUnloadRadiusAdd; }

	// 获得物体加载半径
	float GetLoadRadius() const { return m_fClipRadiusFar + m_fLoadRadiusAdd; }
	// 获得物体卸载半径
	float GetUnloadRadius() const { return GetLoadRadius() + m_fUnloadRadiusAdd; }

	// 资源文件所在路径
	void SetFilePath(const char* value) { m_strFilePath = value; }
	const char* GetFilePath() const { return m_strFilePath.c_str(); }

	// 地形贴图模型附加路径
	void SetAppendPath(const char* value) 
  {
    m_strAppendPath = value; 
  }
	const char* GetAppendPath() const { return m_strAppendPath.c_str(); }

	// 地形区域光照文件路径
	void SetZoneLightPath(const char* value) { m_strZoneLightPath = value; }
	const char* GetZoneLightPath() const { return m_strZoneLightPath.c_str(); }

	// 地面模型光照文件路径
	void SetModelLightPath(const char* value) { m_strModelLightPath = value; }
	const char* GetModelLightPath() const { return m_strModelLightPath.c_str(); }

	// 可行走文件路径
	void SetWalkablePath(const char* value) { m_strWalkablePath = value; }
	const char* GetWalkablePath() const { return m_strWalkablePath.c_str(); }

	// 动态可行走文件路径
	void SetDynamicWalkablePath(const char* value) { m_strDynamicWalkablePath = value; }
	const char* GetDynamicWalkablePath() const { return m_strDynamicWalkablePath.c_str(); }

	// 测试用的高度图
	void SetHeightMap(const char* value) { m_strHeightMap = value; }
	const char* GetHeightMap() const { return m_strHeightMap.c_str(); }

	// 地形初始高度
	void SetInitHeight(float value) { m_fInitHeight = value; }
	float GetInitHeight() const { return m_fInitHeight; }
	
	// 最大地表LOD像素误差
	void SetPixelError(float value) { m_fPixelError = value; }
	float GetPixelError() const { return m_fPixelError; }
	float GetPixelErrorAttr() const { return m_fPixelError; }

	// 光贴图的尺寸
	void SetLightTexScale(int value) { m_nLightTexScale = value; }
	int GetLightTexScale() const { return (int)m_nLightTexScale; }

	// 地面光照颜色分辨率
	void SetLightPerUnit(int value) { m_nLightPerUnit = value; }
	int GetLightPerUnit() const { return (int)m_nLightPerUnit; }
	
	// 单位尺寸
	void SetUnitSize(float value);
	float GetUnitSize() const { return m_fUnitSize; }
	float GetUnitSizeAttr() const { return m_fUnitSize; }

	// 灯光标志模型
	void SetLightModelID(const PERSISTID& value);
	PERSISTID GetLightModelID() const;

	// 声音标志模型
	void SetSoundModelID(const PERSISTID& value);
	PERSISTID GetSoundModelID() const;

	// 声音范围标志模型
	void SetSoundRangeModelID(const PERSISTID& value);
	PERSISTID GetSoundRangeModelID() const;

	// 粒子标志模型
	void SetParticleModelID(const PERSISTID& value);
	PERSISTID GetParticleModelID() const;

	// 特效标志模型
	void SetEffectModelID(const PERSISTID& value);
	PERSISTID GetEffectModelID() const;

	// 触发器标志模型
	void SetTriggerModelID(const PERSISTID& value);
	PERSISTID GetTriggerModelID() const;

	// 树木标志模型
	void SetTreeModelID(const PERSISTID& value);
	PERSISTID GetTreeModelID() const;

	// 鼠标点击
	void Pick(const IVarList& args, IVarList& result);
	// 鼠标点击地面
	void PickGround(const IVarList& args, IVarList& result);
	// 鼠标点选地面物体
	PERSISTID PickVisual(int x, int y, float distance); 
	
	// 碰撞测试获得高度
	virtual float TraceHeight(float x, float z, float high_y, float low_y);
	// 测试射线碰到的对象和位置
	virtual void TraceCollide(const IVarList& args, IVarList& result);
	// 测试射线碰到的对象位置和贴图名
	virtual void TraceTexture(const IVarList& args, IVarList& result);
	// 测试射线是否有碰撞
	virtual bool TraceHit(float x1, float y1, float z1, float x2, float y2, float z2);
	// 获得指定位置的高度
	virtual float GetPosiY(float x, float z);

	// 获得精确的地面高度（保证不低于地面）
	float GetGroundHeight(float x, float z);
	// 水面是否存在
	bool GetWaterExists(float x, float z);
	// 获得所在水面的索引值
	int GetWaterCurrent(float x, float z);
	// 获得水面基本高度
	float GetWaterBaseHeight(float x, float z);
	// 获得水面最大高度
	float GetWaterMaxHeightDefault(float x, float z, float default_y);
	// 获得水面波浪的精确高度
	float GetWaterWaveHeight(float x, float z);

	// 碰撞数据是否就绪
	virtual bool GetCollideEnable(float x, float z);
	// 获得指定位置的最大高度
	virtual float GetApexHeight(float x, float z);
	// 获得指定位置的最大高度所在的层
	virtual int GetApexFloor(float x, float z);
	// 获得最低可行走层的高度
	virtual float GetBottomHeight(float x, float z);
	// 获得最低可行走层
	virtual int GetBottomFloor(float x, float z);
	// 获得是否可行走
	virtual bool GetWalkEnable(float x, float z);
	// 获得可行走的高度
	virtual float GetWalkHeight(float x, float z);
	// 获得可行走层序号
	virtual int GetWalkFloor(float x, float z);
	// 水面是否存在
	virtual bool GetWalkWaterExists(float x, float z);
	// 获得精确的水面高度
	virtual float GetWalkWaterHeight(float x, float z);
	// 获得总的层数量
	virtual int GetFloorCount(float x, float z);
	// 获得层是否存在
	virtual bool GetFloorExists(float x, float z, int floor);
	// 获得层是否可移动
	virtual bool GetFloorCanMove(float x, float z, int floor);
	// 获得层是否可站立
	virtual bool GetFloorCanStand(float x, float z, int floor);
	// 获得层高度
	virtual float GetFloorHeight(float x, float z, int floor);
	// 获得层的无障碍空间高度
	virtual float GetFloorSpace(float x, float z, int floor);
	// 获得层是否有墙面
	virtual bool GetFloorHasWall(float x, float z, int floor);
	// 获得指定高度位置是否存在墙面
	virtual bool GetWallExists(float x, float y, float z);
	// 获得墙壁法线方向
	virtual void GetWalkWallAngle(const IVarList& args, IVarList& result);
	// 获得墙壁碰撞
	virtual void GetWalkWallCollide(const IVarList& args, IVarList& result);
	// 获得射线经过的所有碰撞格的中心坐标
	virtual void GetLineTraverse(const IVarList& args, IVarList& result);
	// 获得射线与碰撞格的首次相交位置
	virtual void GetLineIntersect(const IVarList& args, IVarList& result);

	// 获得指定名字的地面物体
	PERSISTID GetVisual(const char* name);
	// 检查场景是否存在指定的物体
	bool GetVisualExists(const PERSISTID& id);
	// 添加地面物体
	bool AddVisual(const char* name, const PERSISTID& id);
	// 添加人物角色
	bool AddVisualRole(const char* name, const PERSISTID& id);
	// 移除地面物体
	bool RemoveVisual(const PERSISTID& id);
	// 重新定位地面物体的位置
	bool RelocateVisual(const PERSISTID& id, float x, float y, float z);
	// 地面物体当前是否在可见范围内
	bool VisualInView(const PERSISTID& id);
	// 设置物体的剪裁距离
	bool SetVisualClipRadius(const PERSISTID& id, float clip_radius);
	// 获得本区域和附近区域指定范围内的所有组合物体
	void GetNearActorList(const IVarList& args, IVarList& result);
	// 获得球体范围内的活动物体数量
	int GetRolesInSphere(float x, float y, float z, float radius);
	
	// 地面物体贴图路径
	bool AddTexturePath(const char* value);
	bool ClearTexturePath();
	int GetTexturePathCount() const;
	void GetTexturePathList(const IVarList& args, IVarList& result) const;
	const char* GetTexturePathByIndex(size_t index) const;
	const char* GetTexturePathString() const { return m_strTexPath.c_str(); }
	
	// 基础贴图管理
	int GetBaseTexIndex(const char* name);
	bool FindBaseTex(const char* name);
	bool AddBaseTex(const char* name, const char* path);
	bool ClearBaseTex();
	int GetBaseTexCount();
	void GetBaseTexList(const IVarList& args, IVarList& result);
	const char* GetBaseTexName(int index);
	const char* GetBaseTexPath(int index);
	bool SetBaseTexSpecularLevel(int index, float value);
	float GetBaseTexSpecularLevel(int index);
	bool SetBaseTexSpecularPower(int index, float value);
	float GetBaseTexSpecularPower(int index);
	bool SetBaseTexHeightScale(int index, float value);
	float GetBaseTexHeightScale(int index);

	// 基础贴图样式调整(只有一个样式).
	int GetBaseFormatScaleU(const char* name, int index);
	int GetBaseFormatScaleV(const char* name, int index);
	int GetBaseFormatAngleX(const char* name, int index);
	int GetBaseFormatAngleY(const char* name, int index);
	int GetBaseFormatAngleZ(const char* name, int index);
	bool SetBaseFormatScaleU(const char* name, int index, int value);
	bool SetBaseFormatScaleV(const char* name, int index, int value);
	bool SetBaseFormatAngleX(const char* name, int index, int value);
	bool SetBaseFormatAngleY(const char* name, int index, int value);
	bool SetBaseFormatAngleZ(const char* name, int index, int value);
	
	// 混合贴图管理
	int GetBlendTexIndex(const char* name);
	bool FindBlendTex(const char* name);
	bool AddBlendTex(const char* name, const char* path);
	bool ClearBlendTex();
	int GetBlendTexCount();
	void GetBlendTexList(const IVarList& args, IVarList& result);
	const char* GetBlendTexName(int index);
	const char* GetBlendTexPath(int index);
	bool SetBlendTexSpecularLevel(int index, float value);
	float GetBlendTexSpecularLevel(int index);
	bool SetBlendTexSpecularPower(int index, float value);
	float GetBlendTexSpecularPower(int index);
	bool SetBlendTexHeightScale(int index, float value);
	float GetBlendTexHeightScale(int index);

	// 混合贴图样式
	bool AddBlendFormat(const char* name, int scale_u, int scale_v,
		int angle_x, int angle_y, int angle_z);
	int GetBlendFormatCount(const char* name);
	int GetBlendFormatScaleU(const char* name, int index);
	int GetBlendFormatScaleV(const char* name, int index);
	int GetBlendFormatAngleX(const char* name, int index);
	int GetBlendFormatAngleY(const char* name, int index);
	int GetBlendFormatAngleZ(const char* name, int index);
	bool SetBlendFormatScaleU(const char* name, int index, int value);
	bool SetBlendFormatScaleV(const char* name, int index, int value);
	bool SetBlendFormatAngleX(const char* name, int index, int value);
	bool SetBlendFormatAngleY(const char* name, int index, int value);
	bool SetBlendFormatAngleZ(const char* name, int index, int value);

	// 全局环境贴图
	void SetGlobalEnvironmentMap(const char* name);
	const char* GetGlobalEnvironmentMap() const;

	// 是否开启水面波动
	void SetWaterWave(bool value) { m_bWaterWave = value; }
	bool GetWaterWave() const { return m_bWaterWave; }

	// 是否开启水体深度
	void SetWaterDepth(bool value) { m_bWaterDepth = value; }
	bool GetWaterDepth() const { return m_bWaterDepth; }
	
	// 是否开启水体岸边过渡
	void SetWaterBorder(bool value) { m_bWaterBorder = value; }
	bool GetWaterBorder() const { return m_bWaterBorder; }

	// 是否开启水面反射
	void SetWaterReflect(bool value) { m_bWaterReflect = value; }
	bool GetWaterReflect() const { return m_bWaterReflect; }

	// 水面环境贴图是否可以重复使用
	void SetWaterEnvMapReusable(bool value) { m_bWaterEnvMapReusable = value; }
	bool GetWaterEnvMapReusable() const { return m_bWaterEnvMapReusable; }

	// 是否开启水面阳光反射
	void SetWaterSunReflect(bool value) { m_bWaterSunReflect = value; }
	bool GetWaterSunReflect() const { return m_bWaterSunReflect; }

	// 是否要更新摄像机焦点位置
	void SetUpdateFocus(bool value) { m_bUpdateFocus = value; }
	bool GetUpdateFocus() const { return m_bUpdateFocus; }

	// 是否仅剪裁地面
	void SetOnlyCullTerrain(bool value) { m_bOnlyCullTerrain = value; }
	bool GetOnlyCullTerrain() const { return m_bOnlyCullTerrain; }

	// 是否使用半球环境光
	void SetSphereAmbient(bool value) { m_bSphereAmbient = value; }
	bool GetSphereAmbient() const { return m_bSphereAmbient; }

	// 是否对碰撞检测的结果进行校验
	void SetVerifyTraceResult(bool value) { m_bVerifyTraceResult = value; }
	bool GetVerifyTraceResult() const { return m_bVerifyTraceResult; }

	// 生成行走数据时，是否进行记录
	void SetGenWalkTrace(bool value) { m_bGenWalkTrace = value; }
	bool GetGenWalkTrace() const { return m_bGenWalkTrace; }

	// 设置摄像机焦点位置
	bool SetFocusPosition(float x, float y, float z);
	// 设置摄像机焦点半径
	bool SetFocusRadius(float x, float y, float z);

	// 水面类型管理
	PERSISTID CreateWater(const char* name);
	PERSISTID GetWater(const char* name);
	bool DeleteWater(const char* name);
	bool ClearWater();
	const char* GetWaterName(int index);
	int GetWaterIndex(const char* name);
	int GetWaterCount() const;
	void GetWaterList(const IVarList& args, IVarList& result) const;
	bool RefreshWater();
	bool SetWaterCubeMapStatic(bool value);
	bool DumpWaterCubeMap(int index, const char* file_name);
	
	// 花草类型管理
	PERSISTID CreateGrass(const char* name);
	PERSISTID GetGrass(const char* name);
	bool DeleteGrass(const char* name);
	bool ClearGrass();
	const char* GetGrassName(int index);
	int GetGrassIndex(const char* name);
	int GetGrassCount() const;
	void GetGrassList(const IVarList& args, IVarList& result) const;
	bool RefreshGrass();

	// 动态物件管理器
	CDynamicWalkableManager* GetDynamicWalkableManager() const 
	{ return m_pDynamicWalkableManager; }

	// 刷新地面
	bool RefreshGround(); 
	// 刷新地面物体
	bool RefreshVisual(); 

	// 碰撞检测屏蔽
	virtual bool SetTraceMask(const char* type, bool value);
	virtual bool GetTraceMask(const char* type);
	virtual void SetTraceMaskValue(unsigned int value) { m_nTraceMask = value; }
	virtual unsigned int GetTraceMaskValue() const { return m_nTraceMask; }

	// 范围标记颜色
	bool SetRegionColor(const char* name, unsigned int value);
	unsigned int GetRegionColor(const char* name) const;
	
	// 获得地区名称列表
	virtual void GetRegionList(const IVarList& args, IVarList& result);
	// 获得指定位置的范围标记是否有效
	virtual bool GetRegionEnable(const char* name, float x, float z);

	// 改变范围名称（编辑器用）
	bool ChangeRegionName(const char* ori_name, const char* new_name);

	// 地区标记颜色
	bool SetAreaColor(const char* name, unsigned int value);
	unsigned int GetAreaColor(const char* name) const;

	// 获得地区名称列表
	virtual void GetAreaList(const IVarList& args, IVarList& result);
	// 获得指定位置的地区标记名称
	virtual const char* GetAreaName(float x, float z);

	// 改变地区名称（编辑器用）
	bool ChangeAreaName(const char* ori_name, const char* new_name);
	// 删除指定区域标记颜色信息（编辑器用）
	bool RemoveAreaColor(const char* name);

	// 设置地形参数
	bool SetParameter(size_t zone_scale, size_t chunk_scale, size_t tex_scale, 
		size_t tex_units, size_t alpha_per_unit, size_t collide_per_unit);
	// 初始化区域
	bool InitZones(size_t rows, size_t cols, size_t origin_row, 
		size_t origin_col, size_t range);

	// 地形删除之前删除所有地块的加载器
	bool ReleaseAllZoneLoader();
	
	// 获得渲染器
	IRender* GetRender() const { return m_pRender; }
	// 材质
	Terrain::material_t& GetMaterial() { return m_Material; }
	// 区域管理器
	CZoneManager* GetZoneManager() const { return m_pZoneManager; }
	// 地面物体管理器
	CVisualManager* GetVisualManager() const { return m_pVisualManager; }
	// 碰撞管理器
	CQuadCollide* GetCollide() const { return m_pCollide; }
	// 地形剪裁
	CTerrainCulling* GetCulling() const { return m_pCulling; }
	// 地形渲染器
	CTerrainPainter* GetPainter() const { return m_pPainter; }
	// 水面类型
	CTerrainWater* GetWater() const { return m_pWater; }
	// 花草类型
	CTerrainGrass* GetGrass() const { return m_pGrass; }

	// 灯光标志模型
	IVisBase* GetLightModel() const { return m_pLightModel; }
	// 声音标志模型
	IVisBase* GetSoundModel() const { return m_pSoundModel; }
	// 声音范围模型
	IVisBase* GetSoundRangeModel() const { return m_pSoundRangeModel; }
	// 粒子标志模型
	IVisBase* GetParticleModel() const { return m_pParticleModel; }
	// 特效标志模型
	IVisBase* GetEffectModel() const { return m_pEffectModel; }
	// 触发器标志模型
	IVisBase* GetTriggerModel() const { return m_pTriggerModel; }
	// 树木标志模型
	IVisBase* GetTreeModel() const { return m_pTreeModel; }
	
	// 碰撞对象的尺寸
	void SetCollideScale(int value);
	int GetCollideScale() const { return (int)m_nCollideScale; }
	
	// 地形区域的尺寸
	float GetZoneWidth() const { return m_nZoneScale * m_fUnitSize; }
	float GetZoneHeight() const { return m_nZoneScale * m_fUnitSize; }

	// 整个地形的尺寸
	float GetFullWidth() const;
	float GetFullHeight() const;

	// 获得全局行列顶点的高度
	float GetGlobalHeightDefault(size_t row, size_t col, float def);
	// 释放全局行列顶点的法线
	bool ReleaseGlobalNormal(size_t row, size_t col);
	
	// 地形范围
	void SetLeft(float value) { m_fLeft = value; }
	float GetLeft() const { return m_fLeft; }
	void SetTop(float value) { m_fTop = value; }
	float GetTop() const { return m_fTop; }
	float GetRight() const { return m_fLeft + GetFullWidth(); }
	float GetBottom() const { return m_fTop + GetFullHeight(); }

	// 贴图管理器
	CBlendTexSet* GetBaseTexSet() const { return m_pBaseTexSet; }
	CBlendTexSet* GetBlendTexSet() const { return m_pBlendTexSet; }

	// 获得样式
	CChunkStyle* GetChunkStyle(size_t style)
	{
		Assert(style < m_nStyleNum);

		return m_pStyles[style];
	}

	// 获得花草的顶点定义
//	IDxInputLayout* GetGrassInputLayout();
	// 获得花草的索引缓冲
	IStaticIB* GetGrassIndexBuffer(size_t grass_num);
	
	// 获得相邻的区域
	CTerrainZone* GetLeftZone(CTerrainZone* zone);
	CTerrainZone* GetRightZone(CTerrainZone* zone);
	CTerrainZone* GetUpZone(CTerrainZone* zone);
	CTerrainZone* GetDownZone(CTerrainZone* zone);
	
	// 获得坐标所在的分块
	CTerrainChunk* GetInChunk(float x, float z);
	
	// 获得指定位置亮度
	//float GetDiffuse(float x, float z);
	// 获得指定位置混合比重最大的贴图名称
	const char* GetMainTexture(float x, float z);
	// 获得指定半径内的所有地面物体
	virtual void GetCircleVisualList(const IVarList& args, IVarList& result);
	// 添加地面物体
	virtual int AddVisBase(const char* name, IVisBase* pVisBase, 
		bool is_role, float clip_radius);
	// 移除地面物体
	virtual int RemoveVisBase(IVisBase* pVisBase);
	// 重新定位位置发生改变的地面物体
	virtual int RelocateVisBase(IVisBase* pVisBase, float x, float y, 
		float z);
	
	// 是否使用地面法线贴图
	void SetEnableNormalMap(bool value) { m_bEnableNormalMap = value; }
	bool GetEnableNormalMap() const { return m_bEnableNormalMap; }

	// 是否使用表面细分
	void SetEnableTessellation(bool value) { m_bEnableTessellation = value; }
	bool GetEnableTessellation() const { return m_bEnableTessellation; }

	// 表面细分参数
	void SetTessellationFactor(float value) { m_fTessellationFactor = value; }
	float GetTessellationFactor() const { return m_fTessellationFactor; }

	// 地面法线贴图有效范围
	void SetNormalMapRadius(float value) { m_fNormalMapRadius = value; }
	float GetNormalMapRadius() const { return m_fNormalMapRadius; }

	// 包围球是否在剪裁面外
	bool IsViewClip(float x, float y, float z, float radius);
	// 是否被地平线剪裁
	bool IsHorizonClip(const FmVec3& v_min, const FmVec3& v_max);
	// 导出地平线信息到文件
	bool DumpHorizon(const char* file_name);

	// 导出内存中的区域列表
	bool DumpZones(const char* file_name);

	// 添加动态物件
	virtual bool AddDynamicWalkable(IVisBase* pVisBase);
	// 删除动态物件
	virtual bool DeleteDynamicWalkable(IVisBase* pVisBase);
	// 加载动态物件碰撞
	bool LoadDynamicWalkable(IVisBase* pVisBase);
	// 卸载动态物件碰撞
	bool UnloadDynamicWalkable(IVisBase* pVisBase);

	// 添加动态物件
	bool AddDynamicWalkable(const PERSISTID& id);
	// 删除动态物件
	bool DeleteDynamicWalkable(const PERSISTID& id);
	// 加载动态物件碰撞
	bool LoadDynamicWalkable(const PERSISTID& id);
	// 卸载动态物件碰撞
	bool UnloadDynamicWalkable(const PERSISTID& id);

//#ifdef TERRAIN_EDIT
//	// 是否保存指定点所在的Chunk的可行走点坐标
//	bool SetSaveChunkWalkPos(float x, float z, bool value);
//	bool GetSaveChunkWalkPos(float x, float z);
//	// 保存指定点所在的Chunk的可行走点坐标
//	bool SaveChunkWalkPos(float x, float z, const char* file_name);
//	// 保存所有可行走点到文本文件（文件名为绝对路径）
//	bool DumpAllWalkPosToFile(const char* file_name);
//	// 保存所有可行走点到图像文件（文件名为绝对路径）
//	bool DumpAllWalkPosToImage(const char* file_name);
//#endif

	// 获取光贴图配置
	CLightMapConfig* GetLigthMapConfig() { return m_pLightMapConfig; }

private:
	// 创建每种风格的索引缓冲
	bool CreateStyleIndexBuffer();
	// 释放每种风格的索引缓冲
	void ReleaseStyleIndexBuffer();
	
	// 鼠标点选
	bool InnerPick(int x, int y, float distance, trace_info_t& result);
	
	// 对碰撞检测的结果进行校验
	void VerifyTraceResult(const FmVec3& src, const FmVec3& dst,
		trace_info_t& result);

//#ifdef TERRAIN_EDIT
//	// 获得所有可行走文件信息
//	bool InnerBeginDumpWalkInfo();
//	// 保存所有可行走点到文件
//	bool InnerDumpAllWalkPosToFile(const char* file_name);
//#endif

	// 需要投射影子
	virtual void SetCastShadow(bool value) { m_bCastShadow = value; }
	virtual bool GetCastShadow() const { return m_bCastShadow; }

	// 需要接受投射影子
	virtual void SetReceiveShadow(bool value)  {m_bReceiveShadow = value; }
	virtual bool GetReceiveShadow() const { return m_bReceiveShadow; }

private:
	IRender* m_pRender;
	//IRenderContext* m_pContext;
	TerrainEditor* m_pEditor;
	TerrainBrush* m_pBrush;
	TerrainMaterial* m_pMaterial;
	CZoneManager* m_pZoneManager;		// 区域数据管理器
	CVisualManager* m_pVisualManager;	// 地面物体管理器
	CTerrainCulling* m_pCulling;		// 地形剪裁
	CTerrainPainter* m_pPainter;		// 地形渲染器
	CTerrainWater* m_pWater;			// 水面类型
	CTerrainGrass* m_pGrass;			// 花草类型
	CQuadCollide* m_pCollide;			
	CLightMapConfig* m_pLightMapConfig; // 保存所有beast光照图对应的曝光指数
	CDynamicWalkableManager* m_pDynamicWalkableManager; // 动态碰撞管理器

	PERSISTID m_PlayerID;
	PERSISTID m_SkyID;

	material_t m_Material;		// 材质
	CNameList m_TexPaths;
	core_string m_strTexPath;
	
	size_t m_nZoneScale;		// 每一个区域的格子行列数
	size_t m_nChunkScale;		// 每一个分块的格子行列数
	size_t m_nTextureScale;		// 地面贴图的大小
	size_t m_nTextureUnits;		// 贴图重复的格子范围
	size_t m_nAlphaPerUnit;		// 顶点之间的ALPHA值数量
	size_t m_nAlphaMapScale;	// 分块的ALPHA贴图尺寸
	size_t m_nAlphaScale;		// 每一个区域的ALPHA值行列数
	size_t m_nLodLevels;		// LOD级别数
	size_t m_nLightTexScale;	// 光贴图的尺寸
	size_t m_nLightPerUnit;		// 地面光照颜色分辨率
	size_t m_nCollidePerUnit;	// 碰撞分辨率
	size_t m_nCollideScale;		// 碰撞对象的尺寸
	
	bool m_bDesignMode;			// 是否设计状态
	int m_nShowDesignLine;		// 显示设计线条的方式
	bool m_bShowWalkable;		// 是否显示行走范围
	bool m_bShowSpaceHeight;	// 是否显示行走空间高度
	bool m_bShowLightRange;		// 是否显示点光源范围
	bool m_bShowRegion;			// 是否显示范围标记
	core_string m_strShowRegionName;
	bool m_bShowArea;			// 是否显示地区标记
	bool m_bShowDesignNoEdge;	// 是否在显示编辑用方格间不留边
	bool m_bShowIcon;			// 是否显示图标模型
	bool m_bShowCollide;		// 是否显示碰撞属性
	bool m_bShowTree;			// 是否显示树
	bool m_bShowQuadTree;		// 是否显示区域的四叉树结构
	int m_nFarLodLevel;			// 远处的缺省LOD级别
	float m_fLodRadius;			// 需要计算LOD级别的范围半径
	float m_fNoLodRadius;		// 不需要计算LOD级别的范围半径
	float m_fDesignRadius;		// 可编辑范围半径
	float m_fCollideRadius;		// 碰撞信息显示半径
	float m_fAlphaFadeRadius;	// 混合贴图淡出半径
	float m_fAlphaHideRadius;	// 混合贴图淡出半径
	float m_fGrassRadius;		// 花草淡出半径
	float m_fGrassLod;			// 花草精度
	float m_fVisualLodBegin;	// 可见对象的LOD计算起始距离
	float m_fVisualLodEnd;		// 可见对象的LOD计算结束距离
	bool m_bWaterVisible;		// 是否可见
	bool m_bGroundVisible;
	bool m_bVisualVisible;
	bool m_bHelperVisible;
	bool m_bWaterWire;			// 是否只显示格线
	bool m_bGroundWire;
	bool m_bVisualWire;
	bool m_bNoLight;			// 是否不加载光照
	bool m_bSimpleGround;		// 是否绘制简化地表
	bool m_bHorizontalCulling;	// 是否使用地平线剪裁
	bool m_bVisualFadeIn;		// 是否开启地面模型淡入
	bool m_bSmoothHorizontal;	// 是否要平滑地平线
	bool m_bWaterWave;
	bool m_bWaterDepth;
	bool m_bWaterBorder;
	bool m_bWaterReflect;
	bool m_bWaterEnvMapReusable;
	bool m_bWaterSunReflect;
	bool m_bUpdateFocus;
	bool m_bOnlyCullTerrain;
	bool m_bSphereAmbient;
	bool m_bVerifyTraceResult;	// 是否对碰撞检测的结果进行校验
	bool m_bGenWalkTrace;		// 生成行走数据时，是否进行记录
	int m_nGroundLevelDown;		// 地表LOD降级数量
	int m_nGroundEnvMap;
	int m_nVisualEnvMap;
	int m_nGroundSortOrder;		// 地块排序
	int m_nVisualSortOrder;		// 地面物体排序方式
	unsigned int m_nTraceMask;	// 碰撞屏蔽
	float m_fClipRadiusNear;	// 最近剪裁半径
	float m_fClipRadiusFar;		// 最远剪裁半径
	float m_fVisualRadiusSmall; // 最小物体半径
	float m_fVisualRadiusBig;	// 最大物体半径
	float m_fLoadRadiusAdd;		// 加载半径
	float m_fUnloadRadiusAdd;	// 卸载半径

	float m_fLeft;					// 左上角坐标
	float m_fTop;
	float m_fUnitSize;				// 单位格子大小
	float m_fInvUnitSize;
	float m_fCollideSize;		
	float m_fInvCollideSize;
	float m_fPixelError;			// 像素误差
	float m_fInitHeight;			// 初始高度
	
	core_string m_strFilePath; // 资源文件所在路径
	core_string m_strAppendPath;
	core_string m_strZoneLightPath; // 地形区域光照文件路径
	core_string m_strModelLightPath; // 地面模型光照文件路径
	core_string m_strWalkablePath; // 可行走文件路径
	core_string m_strDynamicWalkablePath; // 动态可行走文件路径
	core_string m_strHeightMap; // 测试用的高度图

	IVisBase* m_pLightModel;
	IVisBase* m_pSoundModel;
	IVisBase* m_pSoundRangeModel;
	IVisBase* m_pParticleModel;
	IVisBase* m_pEffectModel;
	IVisBase* m_pTriggerModel;
	IVisBase* m_pTreeModel;

	CChunkStyle** m_pStyles;		// 分块样式
	size_t m_nStyleNum;

	CBlendTexSet* m_pBaseTexSet;	// 基础贴图集合
	CBlendTexSet* m_pBlendTexSet;	// 混合贴图集合

	TArrayPod<area_color_t, 1, TCoreAlloc> m_RegionColors; // 范围标记颜色
	TArrayPod<area_color_t, 1, TCoreAlloc> m_AreaColors; // 地区标记颜色

//	IDxInputLayout* m_pGrassInputLayout;
	IStaticIB* m_pGrassIndexBuffer;
	
	bool m_bEnableNormalMap;
	bool m_bEnableTessellation;
	float m_fTessellationFactor;
	float m_fNormalMapRadius;
	float m_offset_seconds;

	bool m_bDynamicWalkLoaded;

	bool m_bCastShadow;
	bool m_bReceiveShadow;
};

#endif // _TERRAIN_H
