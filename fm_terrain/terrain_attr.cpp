//--------------------------------------------------------------------
// 文件名:		terrain_attr.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "terrain.h"

/// \file terrain_attr.cpp
/// \brief 地形

 

/// entity: Terrain
/// \brief 地形实体
/// \see 继承自IVisBase
DECLARE_ENTITY(Terrain, 0, IVisBase);

/// readonly: object Editor
/// \brief 地形编辑对象
DECLARE_PROPERTY_GET(PERSISTID, Terrain, Editor, GetEditorID);
/// readonly: object Brush
/// \brief 地形刷子对象
DECLARE_PROPERTY_GET(PERSISTID, Terrain, Brush, GetBrushID);
/// readonly: object Material
/// \brief 地形材质对象
DECLARE_PROPERTY_GET(PERSISTID, Terrain, Material, GetMaterialID);

/// readonly: int ZoneScale
/// \brief 地形区域的尺寸
DECLARE_PROPERTY_GET(int, Terrain, ZoneScale, GetZoneScaleAttr);
/// readonly: int ChunkScale
/// \brief 区域内分块的尺寸
DECLARE_PROPERTY_GET(int, Terrain, ChunkScale, GetChunkScaleAttr);
/// readonly: int TextureScale
/// \brief 地面贴图的尺寸
DECLARE_PROPERTY_GET(int, Terrain, TextureScale, GetTextureScale);
/// readonly: int TextureUnits
/// \brief 每个地面贴图覆盖的地形单位长度数量
DECLARE_PROPERTY_GET(int, Terrain, TextureUnits, GetTextureUnits);
/// readonly: int AlphaPerUnit
/// \brief 每个地形单位长度内的ALPHA值数量
DECLARE_PROPERTY_GET(int, Terrain, AlphaPerUnit, GetAlphaPerUnit);
/// readonly: int AlphaMapScale
/// \brief 混合贴图数据的尺寸
DECLARE_PROPERTY_GET(int, Terrain, AlphaMapScale, GetAlphaMapScaleAttr);
/// readonly: int AlphaScale
/// \brief 每个区域的混合数据尺寸
DECLARE_PROPERTY_GET(int, Terrain, AlphaScale, GetAlphaScaleAttr);
/// readonly: int LodLevels
/// \brief LOD级别数
DECLARE_PROPERTY_GET(int, Terrain, LodLevels, GetLodLevelsAttr);
/// readonly: int CollidePerUnit
/// \brief 碰撞分辨率
DECLARE_PROPERTY_GET(int, Terrain, CollidePerUnit, GetCollidePerUnitAttr);

/// readonly: int ZoneRows
/// \brief 区域行数
DECLARE_PROPERTY_GET(int, Terrain, ZoneRows, GetZoneRows);
/// readonly: int ZoneCols
/// \brief 区域列数
DECLARE_PROPERTY_GET(int, Terrain, ZoneCols, GetZoneCols);
/// readonly: int ZoneOriginRows
/// \brief 原点所在区域行
DECLARE_PROPERTY_GET(int, Terrain, ZoneOriginRow, GetZoneOriginRow);
/// readonly: int ZoneOriginCols
/// \brief 原点所在区域列
DECLARE_PROPERTY_GET(int, Terrain, ZoneOriginCol, GetZoneOriginCol);
/// readonly: int ZoneRange
/// \brief 可见区域范围
DECLARE_PROPERTY_GET(int, Terrain, ZoneRange, GetZoneRange);

/// proprety: object Player
/// \brief 当前玩家对象
DECLARE_PROPERTY(PERSISTID, Terrain, Player, GetPlayerID, SetPlayerID);
/// proprety: object Sky
/// \brief 天空对象
DECLARE_PROPERTY(PERSISTID, Terrain, Sky, GetSkyID, SetSkyID);

/// property: int LightTexScale
/// \brief 光贴图的尺寸
DECLARE_PROPERTY(int, Terrain, LightTexScale, GetLightTexScale, 
	SetLightTexScale);
/// property: int LightPerUnit
/// \brief 地面光照颜色分辨率
DECLARE_PROPERTY(int, Terrain, LightPerUnit, GetLightPerUnit, 
	SetLightPerUnit);
/// property: int CollideScale
/// \brief 碰撞对象的尺寸
DECLARE_PROPERTY(int, Terrain, CollideScale, GetCollideScale, 
	SetCollideScale);
/// property: bool DesignMode
/// \brief 是否设计状态
DECLARE_PROPERTY(bool, Terrain, DesignMode, GetDesignModeAttr, SetDesignMode);
/// property: int ShowDesignLine
/// \brief 显示设计线条的方式
/// \brief 0不显示；1只显示区域边缘线；2显示区域边缘线和分块边缘线；3全部显示
DECLARE_PROPERTY(int, Terrain, ShowDesignLine, GetShowDesignLine, 
	SetShowDesignLine);
/// property: bool ShowWalkable
/// \brief 是否显示行走范围
DECLARE_PROPERTY(bool, Terrain, ShowWalkable, GetShowWalkable, 
	SetShowWalkable);
/// property: bool ShowSpaceHeight
/// \brief 是否显示行走空间高度
DECLARE_PROPERTY(bool, Terrain, ShowSpaceHeight, GetShowSpaceHeight, 
	SetShowSpaceHeight);
/// property: bool ShowLightRange
/// \brief 是否显示点光源范围
DECLARE_PROPERTY(bool, Terrain, ShowLightRange, GetShowLightRange, 
	SetShowLightRange);
/// property: bool ShowRegion
/// \brief 是否显示范围标记
DECLARE_PROPERTY(bool, Terrain, ShowRegion, GetShowRegion, SetShowRegion);
/// property: string ShowRegionName
/// \brief 显示指定范围标记名
DECLARE_PROPERTY(const char*, Terrain, ShowRegionName, GetShowRegionName, 
	SetShowRegionName);
/// property: bool ShowArea
/// \brief 是否显示地区标记
DECLARE_PROPERTY(bool, Terrain, ShowArea, GetShowArea, SetShowArea);
/// property: bool ShowIcon
/// \brief 是否显示图标模型
DECLARE_PROPERTY(bool, Terrain, ShowIcon, GetShowIcon, SetShowIcon);
/// property: bool ShowCollide
/// \brief 是否显示碰撞属性
DECLARE_PROPERTY(bool, Terrain, ShowCollide, GetShowCollide, SetShowCollide);
/// property: bool ShowArea
/// \brief 是否在显示编辑用方格间不留边
DECLARE_PROPERTY(bool, Terrain, ShowDesignNoEdge, GetShowDesignNoEdge, 
	SetShowDesignNoEdge);
/// property: bool ShowTree
/// \brief 是否显示树
DECLARE_PROPERTY(bool, Terrain, ShowTree, GetShowTree, SetShowTree);
/// property: bool ShowQuadTree
/// \brief 是否显示区域的四叉树结构
DECLARE_PROPERTY(bool, Terrain, ShowQuadTree, GetShowQuadTree, SetShowQuadTree);
/// property: int FarLodLevel
/// \brief 地形远处的缺省LOD级别
DECLARE_PROPERTY(int, Terrain, FarLodLevel, GetFarLodLevel, SetFarLodLevel);
/// property: float LodRadius
/// \brief 需要计算LOD级别的范围半径，超出范围则用最大值
DECLARE_PROPERTY(float, Terrain, LodRadius, GetLodRadius, SetLodRadius);
/// property: float NoLodRadius
/// \brief 不需要计算LOD级别的范围半径，超出范围则计算LOD级别
DECLARE_PROPERTY(float, Terrain, NoLodRadius, GetNoLodRadius, SetNoLodRadius);
/// property: float DesignRadius
/// \brief 设计半径，此范围内的分块不做LOD处理
DECLARE_PROPERTY(float, Terrain, DesignRadius, GetDesignRadius, 
	SetDesignRadius);
/// property: float CollideRadius
/// \brief 碰撞信息显示半径
DECLARE_PROPERTY(float, Terrain, CollideRadius, GetCollideRadius, 
	SetCollideRadius);
/// property: float AlphaFadeRadius
/// \brief 混合图淡出半径
DECLARE_PROPERTY(float, Terrain, AlphaFadeRadius, GetAlphaFadeRadiusAttr, 
	SetAlphaFadeRadius);
/// property: float AlphaHideRadius
/// \brief 混合图消隐半径，此范围外的分块不需要画混合贴图 
DECLARE_PROPERTY(float, Terrain, AlphaHideRadius, GetAlphaHideRadiusAttr, 
	SetAlphaHideRadius);
/// property: float GrassRadius
/// \brief 花草淡出半径
DECLARE_PROPERTY(float, Terrain, GrassRadius, GetGrassRadiusAttr, 
	SetGrassRadius);
/// property: float GrassLod
/// \brief 花草精度
DECLARE_PROPERTY(float, Terrain, GrassLod, GetGrassLodAttr, SetGrassLod);
/// property: float VisualLodBegin
/// \brief 可见对象的LOD计算起始距离
DECLARE_PROPERTY(float, Terrain, VisualLodBegin, GetVisualLodBeginAttr, 
	SetVisualLodBegin);
/// property: float VisualLodEnd
/// \brief 可见对象的LOD计算结束距离
DECLARE_PROPERTY(float, Terrain, VisualLodEnd, GetVisualLodEndAttr, 
	SetVisualLodEnd);

/// property: bool StopRefresh
/// \brief 停止刷新地形，调试用
//DECLARE_PROPERTY(bool, Terrain, StopRefresh, GetStopRefresh, SetStopRefresh);

/// property: bool WaterVisible
/// \brief 水面是否可见
DECLARE_PROPERTY(bool, Terrain, WaterVisible, GetWaterVisible, 
	SetWaterVisible);
/// property: bool GroundVisible
/// \brief 地面是否可见
DECLARE_PROPERTY(bool, Terrain, GroundVisible, GetGroundVisible, 
	SetGroundVisible);
/// property: bool VisualVisible
/// \brief 地面物体是否可见
DECLARE_PROPERTY(bool, Terrain, VisualVisible, GetVisualVisible, 
	SetVisualVisible);
/// property: bool HelperVisible
/// \brief 辅助物体是否可见
DECLARE_PROPERTY(bool, Terrain, HelperVisible, GetHelperVisible, 
	SetHelperVisible);
/// property: bool WaterWire
/// \brief 水面是否只显示格线
DECLARE_PROPERTY(bool, Terrain, WaterWire, GetWaterWire, SetWaterWire);
/// property: bool GroundWire
/// \brief 地面是否只显示格线
DECLARE_PROPERTY(bool, Terrain, GroundWire, GetGroundWire, SetGroundWire);
/// property: bool VisualWire
/// \brief 地面是否只显示格线
DECLARE_PROPERTY(bool, Terrain, VisualWire, GetVisualWire, SetVisualWire);

/// property: bool NoLight
/// \brief 是否不加载光照
DECLARE_PROPERTY(bool, Terrain, NoLight, GetNoLight, SetNoLight);
/// property: bool SpecularMaterial
/// \brief 是否开启高光材质
//DECLARE_PROPERTY(bool, Terrain, SpecularMaterial, GetSpecularMaterial, 
//	SetSpecularMaterial);
/// property: bool SimpleGround
/// \brief 是否绘制简化地表
DECLARE_PROPERTY(bool, Terrain, SimpleGround, GetSimpleGround, 
	SetSimpleGround);
/// property: bool HorizontalCulling
/// \brief 是否使用地平线剪裁
DECLARE_PROPERTY(bool, Terrain, HorizontalCulling, GetHorizontalCulling, 
	SetHorizontalCulling);
/// property: bool VisualFadeIn
/// \brief 是否开启地面物体淡入
DECLARE_PROPERTY(bool, Terrain, VisualFadeIn, GetVisualFadeIn, 
	SetVisualFadeIn);
/// property: bool SmoothHorizontal
/// \brief 是否开启平滑地平线
DECLARE_PROPERTY(bool, Terrain, SmoothHorizontal, GetSmoothHorizontal, 
	SetSmoothHorizontal);

/// property: bool WaterWave
/// \brief 是否开启水面波动
DECLARE_PROPERTY(bool, Terrain, WaterWave, GetWaterWave, SetWaterWave);
/// property: bool WaterDepth
/// \brief 是否开启水体深度
DECLARE_PROPERTY(bool, Terrain, WaterDepth, GetWaterDepth, SetWaterDepth);
/// property: bool WaterBorder
/// \brief 是否开启水体岸边过渡
DECLARE_PROPERTY(bool, Terrain, WaterBorder, GetWaterBorder, SetWaterBorder);
/// property: bool WaterReflect
/// \brief 是否开启水面反射
DECLARE_PROPERTY(bool, Terrain, WaterReflect, GetWaterReflect, 
	SetWaterReflect);
/// property: bool WaterEnvMapReusable
/// \brief 水面环境贴图是否可以重复使用
DECLARE_PROPERTY(bool, Terrain, WaterEnvMapReusable, GetWaterEnvMapReusable, 
	SetWaterEnvMapReusable);
/// property: bool WaterSunReflect
/// \brief 是否开启水面阳光反射
DECLARE_PROPERTY(bool, Terrain, WaterSunReflect, GetWaterSunReflect, 
	SetWaterSunReflect);

/// property: bool UpdateFocus
/// \brief 是否要更新摄像机焦点位置和半径
DECLARE_PROPERTY(bool, Terrain, UpdateFocus, GetUpdateFocus, SetUpdateFocus);

/// property: bool OnlyCullTerrain
/// \brief 是否仅剪裁地面
DECLARE_PROPERTY(bool, Terrain, OnlyCullTerrain, GetOnlyCullTerrain, 
	SetOnlyCullTerrain);

/// property: bool SphereAmbient
/// \brief 是否使用半球环境光
DECLARE_PROPERTY(bool, Terrain, SphereAmbient, GetSphereAmbient, 
	SetSphereAmbient);

/// property: bool VerifyTraceResult
/// \brief 是否对碰撞检测的结果进行校验
DECLARE_PROPERTY(bool, Terrain, VerifyTraceResult, GetVerifyTraceResult, 
	SetVerifyTraceResult);

/// property: bool GenWalkTrace
/// \brief 生成行走数据时，是否进行记录
DECLARE_PROPERTY(bool, Terrain, GenWalkTrace, GetGenWalkTrace, SetGenWalkTrace);

/// property: int GroundLevelDown
/// \brief 地表LOD降级数量
DECLARE_PROPERTY(int, Terrain, GroundLevelDown, GetGroundLevelDown, 
	SetGroundLevelDown);
/// property: int GroundEnvMap
/// \brief 地形倒影质量(0-2)
DECLARE_PROPERTY(int, Terrain, GroundEnvMap, GetGroundEnvMap, 
	SetGroundEnvMap);
/// property: int VisualEnvMap
/// \brief 地面物体倒影质量(0-2)
DECLARE_PROPERTY(int, Terrain, VisualEnvMap, GetVisualEnvMap, 
	SetVisualEnvMap);
/// property: int GroundSortOrder
/// \brief 地块排序(0-不排序，1-从近到远排序)
DECLARE_PROPERTY(int, Terrain, GroundSortOrder, GetGroundSortOrder, 
	SetGroundSortOrder);
/// property: int VisualSortOrder
/// \brief 地面物体排序方式(0-不排序，1-从近到远排序)
DECLARE_PROPERTY(int, Terrain, VisualSortOrder, GetVisualSortOrder, 
	SetVisualSortOrder);

/// property: float ClipRadiusNear
/// \brief 最近剪裁半径
DECLARE_PROPERTY(float, Terrain, ClipRadiusNear, GetClipRadiusNear, 
	SetClipRadiusNear);
/// property: float ClipRadiusFar
/// \brief 最远剪裁半径
DECLARE_PROPERTY(float, Terrain, ClipRadiusFar, GetClipRadiusFar, 
	SetClipRadiusFar);
/// property: float VisualRadiusSmall
/// \brief 最小物体半径
DECLARE_PROPERTY(float, Terrain, VisualRadiusSmall, GetVisualRadiusSmall, 
	SetVisualRadiusSmall);
/// property: float VisualRadiusBig
/// \brief 最大物体半径
DECLARE_PROPERTY(float, Terrain, VisualRadiusBig, GetVisualRadiusBig, 
	SetVisualRadiusBig);
/// property: float LoadRadiusAdd
/// \brief 物体加载的附加距离
DECLARE_PROPERTY(float, Terrain, LoadRadiusAdd, GetLoadRadiusAdd, 
	SetLoadRadiusAdd);
/// property: float UnloadRadiusAdd
/// \brief 物体卸载的附加距离
DECLARE_PROPERTY(float, Terrain, UnloadRadiusAdd, GetUnloadRadiusAdd, 
	SetUnloadRadiusAdd);

/// property: string RoleDiffuseFactor
/// \brief 人物光照光源颜色调节因子
//DECLARE_PROPERTY(float, Terrain, RoleDiffuseFactor, GetRoleDiffuseFactor, 
//	SetRoleDiffuseFactor);
/// property: string RoleAmbientFactor
/// \brief 人物光照环境光调节因子
//DECLARE_PROPERTY(float, Terrain, RoleAmbientFactor, GetRoleAmbientFactor, 
//	SetRoleAmbientFactor);
/// property: string RoleSpecularFactor
/// \brief 人物光照反射光调节因子
//DECLARE_PROPERTY(float, Terrain, RoleSpecularFactor, GetRoleSpecularFactor, 
//	SetRoleSpecularFactor);

/// property: float PixelError
/// \brief LOD显示的允许像素误差
DECLARE_PROPERTY(float, Terrain, PixelError, GetPixelErrorAttr, SetPixelError);

/// property: string FilePath
/// \brief 地形资源文件所在路径
DECLARE_PROPERTY(const char*, Terrain, FilePath, GetFilePath, SetFilePath);
/// property: string AppendPath
/// \brief 地形贴图模型附加路径
DECLARE_PROPERTY(const char*, Terrain, AppendPath, GetAppendPath, 
	SetAppendPath);
/// property: string ZoneLightPath
/// \brief 地形区域光照文件路径
DECLARE_PROPERTY(const char*, Terrain, ZoneLightPath, GetZoneLightPath, 
	SetZoneLightPath);
/// property: string ModelLightPath
/// \brief 地面模型光照文件路径
DECLARE_PROPERTY(const char*, Terrain, ModelLightPath, GetModelLightPath, 
	SetModelLightPath);
/// property: string WalkablePath
/// \brief 可行走文件路径
DECLARE_PROPERTY(const char*, Terrain, WalkablePath, GetWalkablePath, 
	SetWalkablePath);
/// property: string HeightMap
/// \brief 调试用的高度图文件
DECLARE_PROPERTY(const char*, Terrain, HeightMap, GetHeightMap, SetHeightMap);
/// property: float InitHeight
/// \brief 地形的初始高度
DECLARE_PROPERTY(float, Terrain, InitHeight, GetInitHeight, SetInitHeight);
/// property: float UnitSize
/// \brief 单位格子大小
DECLARE_PROPERTY(float, Terrain, UnitSize, GetUnitSizeAttr, SetUnitSize);

/// property: object LightModel
/// \brief 灯光标志模型
DECLARE_PROPERTY(PERSISTID, Terrain, LightModel, GetLightModelID, 
	SetLightModelID);
/// property: object SoundModel
/// \brief 声音标志模型
DECLARE_PROPERTY(PERSISTID, Terrain, SoundModel, GetSoundModelID, 
	SetSoundModelID);
/// property: object SoundRangeModel
/// \brief 声音范围模型
DECLARE_PROPERTY(PERSISTID, Terrain, SoundRangeModel, GetSoundRangeModelID, 
	SetSoundRangeModelID);
/// property: object ParticleModel
/// \brief 粒子标志模型
DECLARE_PROPERTY(PERSISTID, Terrain, ParticleModel, GetParticleModelID, 
	SetParticleModelID);
/// property: object EffectModel
/// \brief 特效标志模型
DECLARE_PROPERTY(PERSISTID, Terrain, EffectModel, GetEffectModelID, 
	SetEffectModelID);
/// property: object TriggerModel
/// \brief 触发器标志模型
DECLARE_PROPERTY(PERSISTID, Terrain, TriggerModel, GetTriggerModelID, 
	SetTriggerModelID);
/// property: object TreeModel
/// \brief 树木标志模型
DECLARE_PROPERTY(PERSISTID, Terrain, TreeModel, GetTreeModelID, 
	SetTreeModelID);

/// property: bool EnableNormalMap
/// \brief 是否使用地面法线贴图
DECLARE_PROPERTY(bool, Terrain, EnableNormalMap, GetEnableNormalMap, 
	SetEnableNormalMap);
/// property: bool EnableTessellation
/// \brief 是否使用表面细分
DECLARE_PROPERTY(bool, Terrain, EnableTessellation, GetEnableTessellation, 
	SetEnableTessellation);
/// property: float TessellationFactor
/// \brief 表面细分参数
DECLARE_PROPERTY(float, Terrain, TessellationFactor, GetTessellationFactor, 
	SetTessellationFactor);
/// property: float NormalMapRadius
/// \brief 地面法线贴图有效范围
DECLARE_PROPERTY(float, Terrain, NormalMapRadius, GetNormalMapRadius, 
	SetNormalMapRadius);

/// property: string GlobalEnvironmentMap
/// \brief 全局环境贴图
DECLARE_PROPERTY(const char*, Terrain, GlobalEnvironmentMap, 
	GetGlobalEnvironmentMap, SetGlobalEnvironmentMap);

DECLARE_PROPERTY(const char*, Terrain, DynamicWalkablePath, GetDynamicWalkablePath, 
	SetDynamicWalkablePath); 
/*
/// method: float GetLoadPercent()
/// \brief 获得加载百分比
DECLARE_METHOD_0(float, Terrain, GetLoadPercent);				 
/// method: [bool, float, float, float] Pick(int x, int y, float distance)
/// \brief 获得鼠标点击的位置
/// \param x,y 屏幕坐标
/// \param distance 检测距离
/// \return 结果及交点坐标
DECLARE_METHOD_N(Terrain, Pick);
/// method: [bool, float, float, float] PickGround(int x, int y, float distance)
/// \brief 获得鼠标点击的地面位置
/// \param x,y 屏幕坐标
/// \param distance 检测距离
/// \return 结果及交点坐标
DECLARE_METHOD_N(Terrain, PickGround);

/// method: object PickVisual(int x, int y, float distance)
/// \brief 鼠标点选地面物体
/// \param x,y 屏幕坐标
/// \param distance 检测距离
DECLARE_METHOD_3(PERSISTID, Terrain, PickVisual, int, int, float);
/// method: float GetPosiY(float x, float z)
/// \brief 获得指定点的Y坐标（最快速）
/// \param x,y 屏幕坐标
DECLARE_METHOD_2(float, Terrain, GetPosiY, float, float);
/// method: float GetGroundHeight(float x, float z)
/// \brief 获得精确的地面高度
/// \param x,z 坐标
DECLARE_METHOD_2(float, Terrain, GetGroundHeight, float, float);
/// method: float TraceHeight(float beg_x, float beg_z, float high_y, float low_y)
/// \brief 检测高度
/// \param beg_x 起始X坐标
/// \param beg_z 起始Z坐标
/// \param high_y 最高点
/// \param low_y 最低点
/// \return 0到1.0表示有碰撞
DECLARE_METHOD_4(float, Terrain, TraceHeight, float, float, float, float);
/// method: [object,x,y,z] TraceCollide(float beg_x, float beg_y, float beg_z, 
///		float end_x, float end_y, float end_z)
/// \brief 检测射线碰撞到的对象和位置
/// \param beg_x,beg_y,beg_z 起始点坐标
/// \param end_x,end_y,end_z 终止点坐标
/// \return 返回碰到的对象ID和碰撞位置坐标
DECLARE_METHOD_N(Terrain, TraceCollide);
/// method: [tex_name,object,x,y,z] TraceTexture(float beg_x, float beg_y, float beg_z, 
///		float end_x, float end_y, float end_z)
/// \brief 检测射线碰撞到的对象位置和贴图名
/// \param beg_x,beg_y,beg_z 起始点坐标
/// \param end_x,end_y,end_z 终止点坐标
/// \return 返回碰到的贴图名,对象ID,碰撞位置坐标
DECLARE_METHOD_N(Terrain, TraceTexture);
/// method: bool TraceHit(float beg_x, float beg_y, float beg_z, 
///		float end_x, float end_y, float end_z)
/// \brief 测试射线是否有碰撞
/// \param beg_x,beg_y,beg_z 起始点坐标
/// \param end_x,end_y,end_z 终止点坐标
DECLARE_METHOD_6(bool, Terrain, TraceHit, float, float, float, float, float, 
	float);

/// method: bool GetCollideEnable(float x, float z)
/// \brief 碰撞数据是否就绪
/// \param x,z 坐标
DECLARE_METHOD_2(bool, Terrain, GetCollideEnable, float, float);
/// method: float GetApexHeight(float x, float z)
/// \brief 获得指定位置的最大高度
/// \param x,z 坐标
DECLARE_METHOD_2(float, Terrain, GetApexHeight, float, float);
/// method: int GetApexFloor(float x, float z)
/// \brief 获得指定位置的最大高度所在的层
/// \param x,z 坐标
DECLARE_METHOD_2(int, Terrain, GetApexFloor, float, float);
/// method: float GetBottomHeight(float x, float z)
/// \brief 获得最低可行走层的高度
/// \param x,z 坐标
DECLARE_METHOD_2(float, Terrain, GetBottomHeight, float, float);
/// method: int GetBottomFloor(float x, float z)
/// \brief 获得最低可行走层
/// \param x,z 坐标
DECLARE_METHOD_2(int, Terrain, GetBottomFloor, float, float);
/// method: bool GetWalkEnable(float x, float z)
/// \brief 获得是否可行走
/// \param x,z 坐标
DECLARE_METHOD_2(bool, Terrain, GetWalkEnable, float, float);
/// method: float GetWalkHeight(float x, float z)
/// \brief 获得可行走的高度
/// \param x,z 坐标
DECLARE_METHOD_2(float, Terrain, GetWalkHeight, float, float);
/// method: int GetWalkFloor(float x, float z)
/// \brief 获得可行走层的序号
/// \param x,z 坐标
DECLARE_METHOD_2(int, Terrain, GetWalkFloor, float, float);

/// method: bool GetWalkWaterExists(float x, float z)
/// \brief 获得可行走水面是否存在
/// \param x,z 坐标
DECLARE_METHOD_2(bool, Terrain, GetWalkWaterExists, float, float);
/// method: float GetWalkWaterHeight(float x, float z)
/// \brief 获得可行走水面的高度
/// \param x,z 坐标
DECLARE_METHOD_2(float, Terrain, GetWalkWaterHeight, float, float);
/// method: float GetWaterWaveHeight(float x, float z)
/// \brief 获得水面波浪的精确高度
/// \param x,z 坐标
DECLARE_METHOD_2(float, Terrain, GetWaterWaveHeight, float, float);

/// method: int GetFloorCount(float x, float z)
/// \brief 获得层数量
/// \param x,z 坐标
DECLARE_METHOD_2(int, Terrain, GetFloorCount, float, float);
/// method: bool GetFloorExists(float x, float z, int floor)
/// \brief 获得层是否存在
/// \param x,z 坐标
/// \param floor 层序号（1到GetFloorCount() - 1）
DECLARE_METHOD_3(bool, Terrain, GetFloorExists, float, float, int);
/// method: bool GetFloorCanMove(float x, float z, int floor)
/// \brief 获得层是否可移动
/// \param x,z 坐标
/// \param floor 层序号（0到GetFloorCount() - 1）
DECLARE_METHOD_3(bool, Terrain, GetFloorCanMove, float, float, int);
/// method: bool GetFloorCanStand(float x, float z, int floor)
/// \brief 获得层是否可站立
/// \param x,z 坐标
/// \param floor 层序号（0到GetFloorCount() - 1）
DECLARE_METHOD_3(bool, Terrain, GetFloorCanStand, float, float, int);
/// method: float GetFloorHeight(float x, float z, int floor)
/// \brief 获得层的的高度
/// \param x,z 坐标
/// \param floor 层序号（1到GetFloorCount() - 1）
DECLARE_METHOD_3(float, Terrain, GetFloorHeight, float, float, int);
/// method: float GetFloorSpace(float x, float z, int floor)
/// \brief 获得层的的无障碍空间高度
/// \param x,z 坐标
/// \param floor 层序号（1到GetFloorCount() - 1）
DECLARE_METHOD_3(float, Terrain, GetFloorSpace, float, float, int);
/// method: bool GetFloorHasWall(float x, float z, int floor)
/// \brief 获得层是否有墙面
/// \param x,z 坐标
/// \param floor 层序号（1到GetFloorCount() - 1）
DECLARE_METHOD_3(bool, Terrain, GetFloorHasWall, float, float, int);
/// method: bool GetWallExists(float x, float y, float z)
/// \brief 获得指定空间位置是否有墙面
/// \param x,y,z 坐标
DECLARE_METHOD_3(bool, Terrain, GetWallExists, float, float, float);

/// method: [posi_x, posi_y, posi_z, angle_x, angle_y, angle_z] GetWalkWallAngle(
///		float beg_x, float beg_y, float beg_z, float end_x, float end_y, float_z)
/// \brief 获得可行走墙面位置和角度
DECLARE_METHOD_N(Terrain, GetWalkWallAngle);
/// method: [posi_x, posi_y, posi_z, angle_y] GetWalkWallCollide(
///		float beg_x, float beg_y, float beg_z, float end_x, float end_y, float_z)
/// \brief 获得可行走墙面碰撞
DECLARE_METHOD_N(Terrain, GetWalkWallCollide);

/// method: table GetLineTraverse(float src_x, float src_z, float dst_x, float dst_z)
/// \brief 获得射线经过的所有碰撞格的中心坐标
/// \param src_x,src_z 射线起始点坐标
/// \param dst_x,dst_z 射线终止点坐标
DECLARE_METHOD_T(Terrain, GetLineTraverse);
/// method: [hit_x, hit_z] GetLineIntersect(float src_x, float src_z, 
///		float dst_x, float dst_z, float grid_x, float grid_z)
/// \brief 获得射线与碰撞格的首次相交位置
/// \param src_x,src_z 射线起始点坐标
/// \param dst_x,dst_z 射线终止点坐标
/// \param grid_x,grid_z 碰撞格坐标
/// \return hit_x,hit_z 首次相交的坐标位置
DECLARE_METHOD_N(Terrain, GetLineIntersect);

/// method: object GetVisual(string name)
/// \brief 获得指定名字的地面物体
/// \param name 物体名字
DECLARE_METHOD_1(PERSISTID, Terrain, GetVisual, const char*);
/// method: object GetVisualExists(object obj)
/// \brief 检查场景是否存在指定的物体
/// \param obj 对象号
DECLARE_METHOD_1(bool, Terrain, GetVisualExists, const PERSISTID&);
/// method: bool AddVisual(string name, object obj)
/// \brief 添加地面物体
/// \param name 对象名
/// \param obj 对象号
DECLARE_METHOD_2(bool, Terrain, AddVisual, const char*, const PERSISTID&);
/// method: bool AddVisualRole(string name, object obj)
/// \brief 添加人物
/// \param name 对象名
/// \param obj 对象号
DECLARE_METHOD_2(bool, Terrain, AddVisualRole, const char*, const PERSISTID&);
/// method: bool RemoveVisual(object obj)
/// \brief 移除地面物体
/// \param obj 对象号
DECLARE_METHOD_1(bool, Terrain, RemoveVisual, const PERSISTID&);
/// method: bool RelocateVisual(object obj, float x, float y, float z)
/// \brief 重新定位地面物体的位置
/// \param obj 对象号
/// \param x 位置X
/// \param y 位置Y
/// \param z 位置Z
DECLARE_METHOD_4(bool, Terrain, RelocateVisual, const PERSISTID&, float, 
	float, float)
/// method: bool VisualInView(object obj)
/// \brief 判断地面物体当前是否在可见范围内
/// \param obj 对象号
DECLARE_METHOD_1(bool, Terrain, VisualInView, const PERSISTID&);
/// method: bool SetVisualClipRadius(object obj, float clip_radius)
/// \brief 设置地面物体剪裁距离
/// \param obj 对象号
/// \param clip_radius 距离
DECLARE_METHOD_2(bool, Terrain, SetVisualClipRadius, const PERSISTID&, float);
/// method: [table] GetNearActorList(float beg_x, float beg_z, float end_x, float end_z)
/// \brief 获得本区域和附近区域指定范围内的所有组合物体
/// \param beg_x,beg_z,end_x,end_z 范围
DECLARE_METHOD_T(Terrain, GetNearActorList);
/// method: int GetRolesInSphere(float x, float y, float z, float radius)
/// \brief 获得球体范围内的活动物体数量
/// \param x,y,z 球体中心点坐标
/// \param radius 球体半径
DECLARE_METHOD_4(int, Terrain, GetRolesInSphere, float, float, float, float);

/// method: bool AddTexturePath(string path)
/// \brief 添加地面物体贴图文件路径
/// \param path 文件路径，多个路径之间用'|'分隔
DECLARE_METHOD_1(bool, Terrain, AddTexturePath, const char*);
/// method: bool ClearTexturePath()
/// \brief 清空地面物体贴图文件路径
DECLARE_METHOD_0(bool, Terrain, ClearTexturePath);
/// method: int GetTexturePathCount()
/// \brief 获得地面物体贴图文件路径数量
DECLARE_METHOD_0(int, Terrain, GetTexturePathCount);
/// method: table GetTexturePathList()
/// \brief 获得地面物体贴图文件路径表
DECLARE_METHOD_T(Terrain, GetTexturePathList);
/// method: string GetTexturePathString()
/// \brief 获得地面物体贴图文件路径表
DECLARE_METHOD_0(const char*, Terrain, GetTexturePathString);

/// method: int GetBaseTexIndex(string tex_name)
/// \brief 获得基础贴图名字对应的索引值
/// \param tex_name 贴图名
DECLARE_METHOD_1(int, Terrain, GetBaseTexIndex, const char*);
/// method: bool FindBaseTex(string tex_name)
/// \brief 查找指定名字的基础贴图
/// \param tex_name 贴图名
DECLARE_METHOD_1(bool, Terrain, FindBaseTex, const char*);
/// method: bool AddBaseTex(string tex_name, string tex_path)
/// \brief 添加基础贴图
/// \param tex_name 贴图名
/// \param tex_path 贴图文件路径
DECLARE_METHOD_2(bool, Terrain, AddBaseTex, const char*, const char*);
/// method: bool ClearBaseTex()
/// \brief 清空所有基础贴图
DECLARE_METHOD_0(bool, Terrain, ClearBaseTex);
/// method: int GetBaseTexCount()
/// \brief 获得基础贴图数量
DECLARE_METHOD_0(int, Terrain, GetBaseTexCount);
/// method: table GetBaseTexList()
/// \brief 获得基础贴图名称列表
DECLARE_METHOD_T(Terrain, GetBaseTexList);
/// method: string GetBaseTexName(int index)
/// \brief 获得指定索引的基础贴图名
/// \param index 索引值
DECLARE_METHOD_1(const char*, Terrain, GetBaseTexName, int);
/// method: string GetBaseTexPath(int index)
/// \brief 获得指定索引的基础贴图文件路径
/// \param index 索引值
DECLARE_METHOD_1(const char*, Terrain, GetBaseTexPath, int);
/// method: bool SetBaseTexSpecularLevel(int index, float value)
/// \brief 设置指定索引基础贴图的高光亮度 
/// \param index 索引值
/// \param value 数值
DECLARE_METHOD_2(bool, Terrain, SetBaseTexSpecularLevel, int, float);
/// method: float GetBaseTexSpecularLevel(int index)
/// \brief 获得指定索引基础贴图的高光亮度
/// \param index 索引值
DECLARE_METHOD_1(float, Terrain, GetBaseTexSpecularLevel, int);
/// method: bool SetBaseTexSpecularPower(int index, float value)
/// \brief 设置指定索引基础贴图的光泽度 
/// \param index 索引值
/// \param value 数值
DECLARE_METHOD_2(bool, Terrain, SetBaseTexSpecularPower, int, float);
/// method: float GetBaseTexSpecularPower(int index)
/// \brief 获得指定索引基础贴图的光泽度 
/// \param index 索引值
DECLARE_METHOD_1(float, Terrain, GetBaseTexSpecularPower, int);
/// method: bool SetBaseTexHeightScale(int index, float value)
/// \brief 设置指定索引基础贴图的表面细分高度 
/// \param index 索引值
/// \param value 数值
DECLARE_METHOD_2(bool, Terrain, SetBaseTexHeightScale, int, float);
/// method: float GetBaseTexHeightScale(int index)
/// \brief 获得指定索引基础贴图的表面细分高度 
/// \param index 索引值
DECLARE_METHOD_1(float, Terrain, GetBaseTexHeightScale, int);

/// method: int GetBlendTexIndex(string tex_name)
/// \brief 获得混合贴图名字对应的索引值
/// \param tex_name 贴图名
DECLARE_METHOD_1(int, Terrain, GetBlendTexIndex, const char*);
/// method: bool FindBlendTex(string tex_name)
/// \brief 查找指定名字的混合贴图
/// \param tex_name 贴图名
DECLARE_METHOD_1(bool, Terrain, FindBlendTex, const char*);
/// method: bool AddBlendTex(string tex_name, string tex_path)
/// \brief 添加混合贴图
/// \param tex_name 贴图名
/// \param tex_path 贴图文件路径
DECLARE_METHOD_2(bool, Terrain, AddBlendTex, const char*, const char*);
/// method: bool ClearBlendTex()
/// \brief 清空所有混合贴图
DECLARE_METHOD_0(bool, Terrain, ClearBlendTex);
/// method: int GetBlendTexCount()
/// \brief 获得混合贴图数量
DECLARE_METHOD_0(int, Terrain, GetBlendTexCount);
/// method: table GetBlendTexList()
/// \brief 获得混合贴图名称列表
DECLARE_METHOD_T(Terrain, GetBlendTexList);
/// method: string GetBlendTexName(int index)
/// \brief 获得指定索引的混合贴图名
/// \brief index 索引值
DECLARE_METHOD_1(const char*, Terrain, GetBlendTexName, int);
/// method: string GetBlendTexPath(int index)
/// \brief 获得指定索引的混合贴图文件路径
/// \brief index 索引值
DECLARE_METHOD_1(const char*, Terrain, GetBlendTexPath, int);
/// method: bool SetBlendTexSpecularLevel(int index, float value)
/// \brief 设置指定索引混合贴图的高光亮度 
/// \param index 索引值
/// \param value 数值
DECLARE_METHOD_2(bool, Terrain, SetBlendTexSpecularLevel, int, float);
/// method: float GetBlendTexSpecularLevel(int index)
/// \brief 获得指定索引混合贴图的高光亮度 
/// \param index 索引值
DECLARE_METHOD_1(float, Terrain, GetBlendTexSpecularLevel, int);
/// method: bool SetBlendTexSpecularPower(int index, float value)
/// \brief 设置指定索引混合贴图的光泽度 
/// \param index 索引值
/// \param value 数值
DECLARE_METHOD_2(bool, Terrain, SetBlendTexSpecularPower, int, float);
/// method: float GetBlendTexSpecularPower(int index)
/// \brief 获得指定索引混合贴图的光泽度 
/// \param index 索引值
DECLARE_METHOD_1(float, Terrain, GetBlendTexSpecularPower, int);
/// method: bool SetBlendTexHeightScale(int index, float value)
/// \brief 设置指定索引混合贴图的光泽度 
/// \param index 索引值
/// \param value 数值
DECLARE_METHOD_2(bool, Terrain, SetBlendTexHeightScale, int, float);
/// method: float GetBlendTexHeightScale(int index)
/// \brief 获得指定索引混合贴图的表面细分高度 
/// \param index 索引值
DECLARE_METHOD_1(float, Terrain, GetBlendTexHeightScale, int);

/// method: bool AddBlendFormat(string name, int scale_u, int scale_v,
///		int angle_x, int angle_y, int angle_z)
/// \brief 增加混合贴图格式
/// \param name 混合贴图名
/// \param scale_u U坐标密度
/// \param scale_v V坐标密度
/// \param angle_x X坐标角度
/// \param angle_y Y坐标角度
/// \param angle_z Z坐标角度
DECLARE_METHOD_6(bool, Terrain, AddBlendFormat, const char*, int, int, int, 
	int, int);
/// method: int GetBlendFormatCount(string name)
/// \brief 获得指定混合贴图的格式数量
/// \param name 混合贴图名
DECLARE_METHOD_1(int, Terrain, GetBlendFormatCount, const char*);
/// method: int GetBlendFormatScaleU(string name, int index)
/// \brief 获得指定混合贴图的U坐标密度
/// \param name 混合贴图名
/// \param index 格式索引
DECLARE_METHOD_2(int, Terrain, GetBlendFormatScaleU, const char*, int);
/// method: int GetBlendFormatScaleV(string name, int index)
/// \brief 获得指定混合贴图的V坐标密度
/// \param name 混合贴图名
/// \param index 格式索引
DECLARE_METHOD_2(int, Terrain, GetBlendFormatScaleV, const char*, int);
/// method: int GetBlendFormatAngleX(string name, int index)
/// \brief 获得指定混合贴图的X坐标角度
/// \param name 混合贴图名
/// \param index 格式索引
DECLARE_METHOD_2(int, Terrain, GetBlendFormatAngleX, const char*, int);
/// method: int GetBlendFormatAngleY(string name, int index)
/// \brief 获得指定混合贴图的Y坐标角度
/// \param name 混合贴图名
/// \param index 格式索引
DECLARE_METHOD_2(int, Terrain, GetBlendFormatAngleY, const char*, int);
/// method: int GetBlendFormatAngleZ(string name, int index)
/// \brief 获得指定混合贴图的Z坐标角度
/// \param name 混合贴图名
/// \param index 格式索引
DECLARE_METHOD_2(int, Terrain, GetBlendFormatAngleZ, const char*, int);
/// method: bool SetBlendFormatScaleU(string name, int index, int value)
/// \brief 设置指定混合贴图的U坐标密度
/// \param name 混合贴图名
/// \param index 格式索引
/// \param value 值
DECLARE_METHOD_3(bool, Terrain, SetBlendFormatScaleU, const char*, int, int);
/// method: bool SetBlendFormatScaleV(string name, int index, int value)
/// \brief 设置指定混合贴图的V坐标密度
/// \param name 混合贴图名
/// \param index 格式索引
/// \param value 值
DECLARE_METHOD_3(bool, Terrain, SetBlendFormatScaleV, const char*, int, int);
/// method: bool SetBlendFormatAngleX(string name, int index, int value)
/// \brief 设置指定混合贴图的X坐标角度
/// \param name 混合贴图名
/// \param index 格式索引
/// \param value 值
DECLARE_METHOD_3(bool, Terrain, SetBlendFormatAngleX, const char*, int, int);
/// method: bool SetBlendFormatAngleY(string name, int index, int value)
/// \brief 设置指定混合贴图的Y坐标角度
/// \param name 混合贴图名
/// \param index 格式索引
/// \param value 值
DECLARE_METHOD_3(bool, Terrain, SetBlendFormatAngleY, const char*, int, int);
/// method: bool SetBlendFormatAngleZ(string name, int index, int value)
/// \brief 设置指定混合贴图的Z坐标角度
/// \param name 混合贴图名
/// \param index 格式索引
/// \param value 值
DECLARE_METHOD_3(bool, Terrain, SetBlendFormatAngleZ, const char*, int, int);

/// method: bool SetFocusPosition(float x, float y, float z)
/// \brief 设置摄像机焦点位置
DECLARE_METHOD_3(bool, Terrain, SetFocusPosition, float, float, float);
/// method: bool SetFocusRadius(float x, float y, float z)
/// \brief 设置摄像机焦点半径
DECLARE_METHOD_3(bool, Terrain, SetFocusRadius, float, float, float);

/// method: object CreateWater(string name)
/// \brief 创建水面类型
/// \param name 类型名称
DECLARE_METHOD_1(PERSISTID, Terrain, CreateWater, const char*);
/// method: object GetWater(string name)
/// \brief 获得水面类型
/// \param name 类型名称
DECLARE_METHOD_1(PERSISTID, Terrain, GetWater, const char*);
/// method: bool DeleteWater(string name)
/// \brief 删除水面类型
/// \param name 类型名称
DECLARE_METHOD_1(bool, Terrain, DeleteWater, const char*);
/// method: bool ClearWater()
/// \brief 删除所有水面类型
DECLARE_METHOD_0(bool, Terrain, ClearWater);
/// method: string GetWaterName(int index)
/// \brief 获得指定索引的水面类型名称
/// \param index 索引
DECLARE_METHOD_1(const char*, Terrain, GetWaterName, int);
/// method: int GetWaterIndex(string name)
/// \brief 获得水面类型索引
/// \param name 类型名称
DECLARE_METHOD_1(int, Terrain, GetWaterIndex, const char*);
/// method: int GetWaterCount()
/// \brief 获得水面类型数量
DECLARE_METHOD_0(size_t, Terrain, GetWaterCount);
/// method: table GetWaterList()
/// \brief 获得水面类型列表
DECLARE_METHOD_T(Terrain, GetWaterList);
/// method: bool RefreshWater()
/// \brief 强制刷新水面显示
DECLARE_METHOD_0(bool, Terrain, RefreshWater);
/// method: bool SetWaterCubeMapStatic(bool value)
/// \brief 设置水面环境贴图是否静止
/// \param value 是否静止
DECLARE_METHOD_1(bool, Terrain, SetWaterCubeMapStatic, bool);
/// method: bool DumpWaterCubeMap(int index, string file_name)
/// \brief 导出指定索引的水面的环境贴图
/// \param index 索引
/// \param file_name 文件名
DECLARE_METHOD_2(bool, Terrain, DumpWaterCubeMap, int, const char*);

/// method: bool GetWaterCurrent(float x, float z)
/// \brief 获得所在水面的索引值（没有水面则返回-1）
/// \param x,z 位置
DECLARE_METHOD_2(int, Terrain, GetWaterCurrent, float, float);

/// method: object CreateGrass(string name)
/// \brief 创建花草类型
/// \param name 类型名称
DECLARE_METHOD_1(PERSISTID, Terrain, CreateGrass, const char*);
/// method: object GetGrass(string name)
/// \brief 获得花草类型
/// \param name 类型名称
DECLARE_METHOD_1(PERSISTID, Terrain, GetGrass, const char*);
/// method: bool DeleteGrass(string name)
/// \brief 删除花草型
/// \param name 类型名称
DECLARE_METHOD_1(bool, Terrain, DeleteGrass, const char*);
/// method: bool ClearGrass()
/// \brief 删除所有花草类型
DECLARE_METHOD_0(bool, Terrain, ClearGrass);
/// method: string GetGrassName(int index)
/// \brief 获得指定索引的花草类型名称
/// \param index 索引
DECLARE_METHOD_1(const char*, Terrain, GetGrassName, int);
/// method: int GetGrassIndex(string name)
/// \brief 获得花草类型索引
/// \param name 类型名称
DECLARE_METHOD_1(int, Terrain, GetGrassIndex, const char*);
/// method: int GetGrassCount()
/// \brief 获得花草类型数量
DECLARE_METHOD_0(size_t, Terrain, GetGrassCount);
/// method: table GetGrassList()
/// \brief 获得花草类型列表
DECLARE_METHOD_T(Terrain, GetGrassList);
/// method: bool RefreshGrass()
/// \brief 强制刷新花草显示
DECLARE_METHOD_0(bool, Terrain, RefreshGrass);

/// method: bool RefreshGround()
/// \brief 强制刷新地面显示
DECLARE_METHOD_0(bool, Terrain, RefreshGround);
/// method: bool RefreshVisual()
/// \brief 强制刷新地面物体显示
DECLARE_METHOD_0(bool, Terrain, RefreshVisual);

/// method: bool SetTraceMask(string type, bool value)
/// \brief 设置碰撞屏蔽状态
/// \param type 物体类型（"Ground", "Model", "Particle", "Light", "Sound", "Actor"）
/// \param value 屏蔽状态
DECLARE_METHOD_2(bool, Terrain, SetTraceMask, const char*, bool);
/// method: bool GetTraceMask(string type)
/// \brief 获得碰撞屏蔽状态
/// \param type 物体类型（"Ground", "Model", "Particle", "Light", "Sound", "Actor"）
DECLARE_METHOD_1(bool, Terrain, GetTraceMask, const char*);

/// method: table GetRegionList()
/// \brief 获得有显示颜色的范围名称列表
DECLARE_METHOD_T(Terrain, GetRegionList);
/// method: bool GetRegionEnable(string name, float x, float z)
/// \brief 获得指定位置的范围标记是否有效
/// \param name 范围名称
/// \param x,z 坐标
DECLARE_METHOD_3(bool, Terrain, GetRegionEnable, const char*, float, float);

/// method: table GetAreaList()
/// \brief 获得有显示颜色的地区名称列表
DECLARE_METHOD_T(Terrain, GetAreaList);
/// method: string GetAreaName(float x, float z)
/// \brief 获得指定位置的地区名称
/// \param x,z 坐标
DECLARE_METHOD_2(const char*, Terrain, GetAreaName, float, float);

/// method: bool SetParameter(int zone_scale, int chunk_scale, 
///		int tex_scale, int tex_units, int alpha_per_unit, int collide_per_unit)
/// \brief 设置地形参数
/// \param zone_scale 区域尺寸
/// \param chunk_scale 分块尺寸
/// \param tex_scale 地面贴图尺寸
/// \param tex_units 地面贴图覆盖的单位尺寸
/// \param alpha_per_unit 每个地形单位长度内的ALPHA值数量
/// \param collide_per_unit 每个地形单位长度内的碰撞信息数量
DECLARE_METHOD_6(bool, Terrain, SetParameter, size_t, size_t, size_t, size_t, 
	size_t, size_t);
/// method: bool InitZones(int zone_rows, int zone_cols, 
///		int origin_row, origin_col, int zone_range)
/// \brief 初始化地形区域
/// \param zone_rows 区域最大行数
/// \param zone_cols 区域最大列数
/// \param origin_row 原点所在的区域行数
/// \param origin_col 原点所在的区域列数
/// \param zone_range 视野可见的区域范围
DECLARE_METHOD_5(bool, Terrain, InitZones, int, int, int, int, int);

/// method: bool ReleaseAllZoneLoader()
/// \brief 地形删除之前删除所有地块的加载器
DECLARE_METHOD_0(bool, Terrain, ReleaseAllZoneLoader);

/// method: float GetDiffuse(float x, float z)
/// \brief 获得指定位置的光贴图亮度（返回0.0-1.0）
/// \param x,z 位置坐标
//DECLARE_METHOD_2(float, Terrain, GetDiffuse, float, float);
/// method: string GetMainTexture(float x, float z)
/// \brief 获得指定位置的混合比重最大的贴图名称
/// \param x,z 位置坐标
DECLARE_METHOD_2(const char*, Terrain, GetMainTexture, float, float);

/// method: bool IsViewClip(float x, float y, float z, float radius)
/// \brief 包围球是否在剪裁面外
/// \param x,y,z 包围球中心
/// \param radius 包围球半径
DECLARE_METHOD_4(bool, Terrain, IsViewClip, float, float, float, float);

/// method: bool DumpHorizon(string file_name)
/// \brief 导出地平线信息到文件
/// \param file_name 文件名
DECLARE_METHOD_1(bool, Terrain, DumpHorizon, const char*);

/// method: bool DumpZones(string file_name)
/// \brief 导出区域列表到文件
/// \param file_name 文件名
DECLARE_METHOD_1(bool, Terrain, DumpZones, const char*);

*/