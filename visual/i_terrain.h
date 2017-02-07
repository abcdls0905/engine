//--------------------------------------------------------------------
// 文件名:		i_terrain.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年8月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _VISUAL_I_TERRAIN_H
#define _VISUAL_I_TERRAIN_H

#include "i_vis_base.h"
//#include "i_dx_render.h"

// 地形对象基类

class ITerrain: public IVisBase
{
public:
	// 碰撞检测屏蔽
	enum
	{
		TRACEMASK_GROUND = 0x1,
		TRACEMASK_MODEL = 0x2,
		TRACEMASK_PARTICLE = 0x4,
		TRACEMASK_LIGHT = 0x8,
		TRACEMASK_SOUND = 0x10,
		TRACEMASK_ACTOR = 0x20,
		TRACEMASK_TRIGGER = 0x80,
		TRACEMASK_ROLE = 0x100,
		TRACEMASK_HELPER = 0x200,
		TRACEMASK_THROUGH = 0x400,
		TRACEMASK_EFFECTMODEL = 0x800,
		TRACEMASK_VISUAL = TRACEMASK_MODEL | TRACEMASK_PARTICLE
			| TRACEMASK_LIGHT | TRACEMASK_SOUND | TRACEMASK_ACTOR 
			| TRACEMASK_TRIGGER | TRACEMASK_EFFECTMODEL,
	};
	
	// 返回值
	enum 
	{ 
		RESULT_SUCCESS = 0, 
		RESULT_CONTEXT_NOT_MATCH, 
		RESULT_ZONE_NOT_EXIST, 
		RESULT_VISUAL_HAD_EXIST, 
		RESULT_VISUAL_NOT_EXIST, 

		RESULT_UNKOWN,
	};
	
public:
	// 获得加载百分比
	virtual float GetLoadPercent() = 0;

	// 获得地形编辑器对象
	virtual PERSISTID GetEditorID() = 0;
	// 获得地形刷子对象
	virtual PERSISTID GetBrushID() = 0;
	// 获得地形材质对象
	virtual PERSISTID GetMaterialID() = 0;

	// 当前玩家角色对象
	virtual void SetPlayerID(const PERSISTID& value) = 0;
	virtual PERSISTID GetPlayerID() const = 0;

	// 天空对象
	virtual void SetSkyID(const PERSISTID& value) = 0;
	virtual PERSISTID GetSkyID() const = 0;

	// 获得每个地形区域的顶点行列数
	virtual size_t GetZoneScale() const = 0;
	virtual int GetZoneScaleAttr() const = 0;
	// 获得每个分块的顶点行列数
	virtual size_t GetChunkScale() const = 0;
	virtual int GetChunkScaleAttr() const = 0;
	// 获得地面贴图的大小
	virtual int GetTextureScale() const = 0;
	// 获得贴图重复的格子范围
	virtual int GetTextureUnits() const = 0;
	// 获得顶点之间的ALPHA值数量
	virtual int GetAlphaPerUnit() const = 0;
	// 获得每个分块的ALPHA贴图尺寸
	virtual size_t GetAlphaMapScale() const = 0;
	virtual int GetAlphaMapScaleAttr() const = 0;
	// 获得每个地形区域的ALPHA值行列数
	virtual size_t GetAlphaScale() const = 0;
	virtual int GetAlphaScaleAttr() const = 0;
	// 获得LOD级别数
	virtual size_t GetLodLevels() const = 0;
	virtual int GetLodLevelsAttr() const = 0;
	// 获得碰撞分辨率
	virtual size_t GetCollidePerUnit() const = 0;
	virtual int GetCollidePerUnitAttr() const = 0;
	// 获得碰撞单位格尺寸
	virtual float GetCollideUnitSize() const = 0;
	
	virtual int GetZoneRows() const= 0;
	virtual int GetZoneCols() const= 0;
	virtual int GetZoneOriginRow() const= 0;
	virtual int GetZoneOriginCol() const= 0;
	virtual int GetZoneRange() const= 0;
	
	// 编辑模式
	virtual void SetDesignMode(bool value) = 0;
	virtual bool GetDesignMode() const = 0;
	virtual bool GetDesignModeAttr() const = 0;

	// 显示设计线条的方式
	virtual void SetShowDesignLine(int value) = 0;
	virtual int GetShowDesignLine() const = 0;

	// 是否显示行走范围
	virtual void SetShowWalkable(bool value) = 0;
	virtual bool GetShowWalkable() const = 0;

	// 是否显示行走空间高度
	virtual void SetShowSpaceHeight(bool value) = 0;
	virtual bool GetShowSpaceHeight() const = 0;
	// 是否显示点光源范围
	virtual void SetShowLightRange(bool value) = 0;
	virtual bool GetShowLightRange() const = 0;
	
	// 是否显示范围标记
	virtual void SetShowRegion(bool value) = 0;
	virtual bool GetShowRegion() const = 0;

	// 显示指定范围标记名
	virtual void SetShowRegionName(const char* value) = 0;
	virtual const char* GetShowRegionName() const = 0;

	// 是否显示地区标记
	virtual void SetShowArea(bool value) = 0;
	virtual bool GetShowArea() const = 0;

	// 是否显示图标模型
	virtual void SetShowIcon(bool value) = 0;
	virtual bool GetShowIcon() const = 0;

	// 是否显示碰撞属性
	virtual void SetShowCollide(bool value) = 0;
	virtual bool GetShowCollide() const = 0;

	// 是否在显示编辑用方格间不留边
	virtual void SetShowDesignNoEdge(bool value) = 0;
	virtual bool GetShowDesignNoEdge() const = 0;

	// 是否显示树
	virtual void SetShowTree(bool value) = 0;
	virtual bool GetShowTree() const = 0;

	// 是否显示区域的四叉树结构
	virtual void SetShowQuadTree(bool value) = 0;
	virtual bool GetShowQuadTree() const = 0;

	// 远处的缺省LOD级别
	virtual void SetFarLodLevel(int value) = 0;
	virtual int GetFarLodLevel() const = 0;

	// 需要计算LOD级别的范围半径
	virtual void SetLodRadius(float value) = 0;
	virtual float GetLodRadius() const = 0;

	// 不需要计算LOD级别的范围半径
	virtual void SetNoLodRadius(float value) = 0;
	virtual float GetNoLodRadius() const = 0;

	// 编辑半径
	virtual void SetDesignRadius(float value)  = 0;
	virtual float GetDesignRadius() const = 0;

	// 碰撞信息显示半径
	virtual void SetCollideRadius(float value) = 0;
	virtual float GetCollideRadius() const = 0;

	// 混合图淡出半径	
	virtual void SetAlphaFadeRadius(float value) = 0;
	virtual float GetAlphaFadeRadius() const = 0;
	virtual float GetAlphaFadeRadiusAttr() const = 0;
	
	// 混合图消隐半径（不需要画混合贴图）	
	virtual void SetAlphaHideRadius(float value)  = 0;
	virtual float GetAlphaHideRadius() const = 0;
	virtual float GetAlphaHideRadiusAttr() const = 0;

	// 花草淡出半径
	virtual void SetGrassRadius(float value) = 0;
	virtual float GetGrassRadius() const = 0;
	virtual float GetGrassRadiusAttr() const = 0;

	// 花草精度
	virtual void SetGrassLod(float value) = 0;
	virtual float GetGrassLod() const = 0;
	virtual float GetGrassLodAttr() const = 0;

	// 可见对象的LOD计算起始距离
	virtual void SetVisualLodBegin(float value) = 0;
	virtual float GetVisualLodBegin() const = 0;
	virtual float GetVisualLodBeginAttr() const = 0;
	
	// 可见对象的LOD计算结束距离
	virtual void SetVisualLodEnd(float value) = 0;
	virtual float GetVisualLodEnd() const = 0;
	virtual float GetVisualLodEndAttr() const = 0;

	// 是否可见
	virtual void SetWaterVisible(bool value) = 0;
	virtual bool GetWaterVisible() const = 0;
	virtual void SetGroundVisible(bool value) = 0;
	virtual bool GetGroundVisible() const = 0;
	virtual void SetVisualVisible(bool value) = 0;
	virtual bool GetVisualVisible() const = 0;
	virtual void SetHelperVisible(bool value) = 0;
	virtual bool GetHelperVisible() const = 0;
	
	// 只显示三角形线
	virtual void SetWaterWire(bool value) = 0;
	virtual bool GetWaterWire() const  = 0;
	virtual void SetGroundWire(bool value) = 0;
	virtual bool GetGroundWire() const = 0;
	virtual void SetVisualWire(bool value) = 0;
	virtual bool GetVisualWire() const = 0;
	
	// 是否不加载光照
	virtual void SetNoLight(bool value) = 0;
	virtual bool GetNoLight() const = 0;

	// 是否绘制简化地表
	virtual void SetSimpleGround(bool value) = 0;
	virtual bool GetSimpleGround() const = 0;

	// 是否使用低平线剪裁
	virtual void SetHorizontalCulling(bool value) = 0;
	virtual bool GetHorizontalCulling() const = 0;

	// 是否开启地面物体淡入
	virtual void SetVisualFadeIn(bool value) = 0;
	virtual bool GetVisualFadeIn() const = 0;

	// 是否开启平滑地平线
	virtual void SetSmoothHorizontal(bool value) = 0;
	virtual bool GetSmoothHorizontal() const = 0;

	// 地表LOD降级数量
	virtual void SetGroundLevelDown(int value) = 0;
	virtual int GetGroundLevelDown() const = 0;

	// 地形倒影质量
	virtual void SetGroundEnvMap(int value) = 0;
	virtual int GetGroundEnvMap() const = 0;

	// 地面物体倒影质量
	virtual void SetVisualEnvMap(int value) = 0;
	virtual int GetVisualEnvMap() const = 0;

	// 地块排序
	virtual void SetGroundSortOrder(int value) = 0;
	virtual int GetGroundSortOrder() const = 0;

	// 地面物体排序方式
	virtual void SetVisualSortOrder(int value) = 0;
	virtual int GetVisualSortOrder() const = 0;

	// 最近剪裁半径
	virtual void SetClipRadiusNear(float value) = 0;
	virtual float GetClipRadiusNear() const = 0;

	// 最远剪裁半径
	virtual void SetClipRadiusFar(float value) = 0;
	virtual float GetClipRadiusFar() const = 0;

	// 最小物体半径
	virtual void SetVisualRadiusSmall(float value) = 0;
	virtual float GetVisualRadiusSmall() const = 0;

	// 最大物体半径
	virtual void SetVisualRadiusBig(float value) = 0;
	virtual float GetVisualRadiusBig() const = 0;

	// 物体加载的附加距离
	virtual void SetLoadRadiusAdd(float value) = 0;
	virtual float GetLoadRadiusAdd() const = 0;
	
	// 物体卸载的附加距离
	virtual void SetUnloadRadiusAdd(float value) = 0;
	virtual float GetUnloadRadiusAdd() const = 0;

	// 获得物体加载半径
	virtual float GetLoadRadius() const = 0;
	// 获得物体卸载半径
	virtual float GetUnloadRadius() const = 0;

	// 资源文件所在路径
	virtual void SetFilePath(const char* value) = 0;
	virtual const char* GetFilePath() const = 0;

	// 地形贴图模型附加路径
	virtual void SetAppendPath(const char* value) = 0;
	virtual const char* GetAppendPath() const = 0;

	// 地形区域光照文件路径
	virtual void SetZoneLightPath(const char* value) = 0;
	virtual const char* GetZoneLightPath() const = 0;

	// 地面模型光照文件路径
	virtual void SetModelLightPath(const char* value) = 0;
	virtual const char* GetModelLightPath() const = 0;

	// 可行走文件路径
	virtual void SetWalkablePath(const char* value) = 0;
	virtual const char* GetWalkablePath() const = 0;

	// 测试用的高度图
	virtual void SetHeightMap(const char* value) = 0;
	virtual const char* GetHeightMap() const = 0;

	// 地形初始高度
	virtual void SetInitHeight(float value) = 0;
	virtual float GetInitHeight() const = 0;
	
	// 最大地表LOD像素误差
	virtual void SetPixelError(float value) = 0;
	virtual float GetPixelError() const = 0;
	virtual float GetPixelErrorAttr() const = 0;

	// 光贴图的尺寸
	virtual void SetLightTexScale(int value) = 0;
	virtual int GetLightTexScale() const = 0;

	// 地面光照颜色分辨率
	virtual void SetLightPerUnit(int value) = 0;
	virtual int GetLightPerUnit() const = 0;
	
	// 单位尺寸
	virtual void SetUnitSize(float value) = 0;
	virtual float GetUnitSize() const = 0;
	virtual float GetUnitSizeAttr() const = 0;

	// 灯光标志模型
	virtual void SetLightModelID(const PERSISTID& value) = 0;
	virtual PERSISTID GetLightModelID() const = 0;

	// 声音标志模型
	virtual void SetSoundModelID(const PERSISTID& value) = 0;
	virtual PERSISTID GetSoundModelID() const = 0;

	// 声音范围标志模型
	virtual void SetSoundRangeModelID(const PERSISTID& value) = 0;
	virtual PERSISTID GetSoundRangeModelID() const = 0;

	// 粒子标志模型
	virtual void SetParticleModelID(const PERSISTID& value) = 0;
	virtual PERSISTID GetParticleModelID() const = 0;

	// 特效标志模型
	virtual void SetEffectModelID(const PERSISTID& value) = 0;
	virtual PERSISTID GetEffectModelID() const = 0;

	// 触发器标志模型
	virtual void SetTriggerModelID(const PERSISTID& value) = 0;
	virtual PERSISTID GetTriggerModelID() const = 0;

	// 树木标志模型
	virtual void SetTreeModelID(const PERSISTID& value) = 0;
	virtual PERSISTID GetTreeModelID() const = 0;

	// 鼠标点击
	virtual void Pick(const IVarList& args, IVarList& result) = 0;
	// 鼠标点击地面
	virtual void PickGround(const IVarList& args, IVarList& result) = 0;
	// 鼠标点选地面物体
	virtual PERSISTID PickVisual(int x, int y, float distance) = 0; 
	// 鼠标点选
	virtual bool InnerPick(int x, int y, float distance, trace_info_t& result) = 0;

	// 碰撞测试获得高度
	virtual float TraceHeight(float x, float z, float high_y, float low_y) = 0;
	// 测试射线碰到的对象和位置
	virtual void TraceCollide(const IVarList& args, IVarList& result) = 0;
	// 测试射线碰到的对象位置和贴图名
	virtual void TraceTexture(const IVarList& args, IVarList& result) = 0;
	// 测试射线是否有碰撞
	virtual bool TraceHit(float x1, float y1, float z1, float x2, float y2, 
		float z2) = 0;
	// 获得指定位置的高度
	virtual float GetPosiY(float x, float z) = 0;

	// 碰撞数据是否就绪
	virtual bool GetCollideEnable(float x, float z) = 0;
	// 获得指定位置的最大高度
	virtual float GetApexHeight(float x, float z) = 0;
	// 获得指定位置的最大高度所在的层
	virtual int GetApexFloor(float x, float z) = 0;
	// 获得最低可行走层的高度
	virtual float GetBottomHeight(float x, float z) = 0;
	// 获得最低可行走层
	virtual int GetBottomFloor(float x, float z) = 0;
	// 获得是否可行走
	virtual bool GetWalkEnable(float x, float z) = 0;
	// 获得可行走的高度
	virtual float GetWalkHeight(float x, float z) = 0;
	// 获得可行走层序号
	virtual int GetWalkFloor(float x, float z) = 0;
	// 水面是否存在
	virtual bool GetWalkWaterExists(float x, float z) = 0;
	// 获得精确的水面高度
	virtual float GetWalkWaterHeight(float x, float z) = 0;
    // 获得水面波浪的精确高度
	virtual float GetWaterWaveHeight(float x, float z) = 0;
	// 获得总的层数量
	virtual int GetFloorCount(float x, float z) = 0;
	// 获得层是否存在
	virtual bool GetFloorExists(float x, float z, int floor) = 0;
	// 获得层是否可移动
	virtual bool GetFloorCanMove(float x, float z, int floor) = 0;
	// 获得层是否可站立
	virtual bool GetFloorCanStand(float x, float z, int floor) = 0;
	// 获得层高度
	virtual float GetFloorHeight(float x, float z, int floor) = 0;
	// 获得层的无障碍空间高度
	virtual float GetFloorSpace(float x, float z, int floor) = 0;
	// 获得层是否有墙面
	virtual bool GetFloorHasWall(float x, float z, int floor) = 0;
	// 获得指定高度位置是否存在墙面
	virtual bool GetWallExists(float x, float y, float z) = 0;
	// 获得墙壁法线方向
	virtual void GetWalkWallAngle(const IVarList& args, IVarList& result) = 0;
	// 获得墙壁碰撞
	virtual void GetWalkWallCollide(const IVarList& args, IVarList& result) = 0;
	// 获得射线经过的所有碰撞格的中心坐标
	virtual void GetLineTraverse(const IVarList& args, IVarList& result) = 0;
	// 获得射线与碰撞格的首次相交位置
	virtual void GetLineIntersect(const IVarList& args, IVarList& result) = 0;

	// 获得指定名字的地面物体
	virtual PERSISTID GetVisual(const char* name) = 0;
	// 检查场景是否存在指定的物体
	virtual bool GetVisualExists(const PERSISTID& id) = 0;

	// 获得指定半径内的所有地面物体
	virtual void GetCircleVisualList(const IVarList& args, IVarList& result) = 0;

	// 添加地面物体
	virtual bool AddVisual(const char* name, const PERSISTID& id) = 0;
	// 添加人物角色
	virtual bool AddVisualRole(const char* name, const PERSISTID& id) = 0;
	// 移除地面物体
	virtual bool RemoveVisual(const PERSISTID& id) = 0;
	// 重新定位地面物体的位置
	virtual bool RelocateVisual(const PERSISTID& id, float x, float y, float z)= 0;
	// 地面物体当前是否在可见范围内
	virtual bool VisualInView(const PERSISTID& id)= 0;
	// 设置物体的剪裁距离
	virtual bool SetVisualClipRadius(const PERSISTID& id, float clip_radius) = 0;
	// 获得本区域和附近区域指定范围内的所有组合物体
	virtual void GetNearActorList(const IVarList& args, IVarList& result) = 0;
	// 获得球体范围内的活动物体数量
	virtual int GetRolesInSphere(float x, float y, float z, float radius) = 0;
	
	// 地面物体贴图路径
	virtual bool AddTexturePath(const char* value) = 0;
	virtual bool ClearTexturePath() = 0;
	virtual int GetTexturePathCount() const = 0;
	virtual void GetTexturePathList(const IVarList& args, IVarList& result) const = 0;
	virtual const char* GetTexturePathByIndex(size_t index) const = 0;
	virtual const char* GetTexturePathString() const = 0;
	
	// 基础贴图管理
	virtual int GetBaseTexIndex(const char* name) = 0;
	virtual bool FindBaseTex(const char* name) = 0;
	virtual bool AddBaseTex(const char* name, const char* path) = 0;
	virtual bool ClearBaseTex() = 0;
	virtual int GetBaseTexCount() = 0;
	virtual void GetBaseTexList(const IVarList& args, IVarList& result) = 0;
	virtual const char* GetBaseTexName(int index) = 0;
	virtual const char* GetBaseTexPath(int index) = 0;
	virtual bool SetBaseTexSpecularLevel(int index, float value) = 0;
	virtual float GetBaseTexSpecularLevel(int index) = 0;
	virtual bool SetBaseTexSpecularPower(int index, float value) = 0;
	virtual float GetBaseTexSpecularPower(int index) = 0;
	virtual bool SetBaseTexHeightScale(int index, float value) = 0;
	virtual float GetBaseTexHeightScale(int index) = 0;

	// 基础贴图样式调整(只有一个样式).
	virtual int GetBaseFormatScaleU(const char* name, int index) = 0;
	virtual int GetBaseFormatScaleV(const char* name, int index) = 0;
	virtual int GetBaseFormatAngleX(const char* name, int index) = 0;
	virtual int GetBaseFormatAngleY(const char* name, int index) = 0;
	virtual int GetBaseFormatAngleZ(const char* name, int index) = 0;
	virtual bool SetBaseFormatScaleU(const char* name, int index, int value)  = 0;
	virtual bool SetBaseFormatScaleV(const char* name, int index, int value)  = 0;
	virtual bool SetBaseFormatAngleX(const char* name, int index, int value)  = 0;
	virtual bool SetBaseFormatAngleY(const char* name, int index, int value)  = 0;
	virtual bool SetBaseFormatAngleZ(const char* name, int index, int value) = 0;
	
	// 混合贴图管理
	virtual int GetBlendTexIndex(const char* name) = 0;
	virtual bool FindBlendTex(const char* name) = 0;
	virtual bool AddBlendTex(const char* name, const char* path) = 0;
	virtual bool ClearBlendTex() = 0;
	virtual int GetBlendTexCount() = 0;
	virtual void GetBlendTexList(const IVarList& args, IVarList& result) = 0;
	virtual const char* GetBlendTexName(int index) = 0;
	virtual const char* GetBlendTexPath(int index) = 0;
	virtual bool SetBlendTexSpecularLevel(int index, float value) = 0;
	virtual float GetBlendTexSpecularLevel(int index) = 0;
	virtual bool SetBlendTexSpecularPower(int index, float value) = 0;
	virtual float GetBlendTexSpecularPower(int index) = 0;
	virtual bool SetBlendTexHeightScale(int index, float value) = 0;
	virtual float GetBlendTexHeightScale(int index) = 0;

	// 混合贴图样式
	virtual bool AddBlendFormat(const char* name, int scale_u, int scale_v,
		int angle_x, int angle_y, int angle_z) = 0;
	virtual int GetBlendFormatCount(const char* name) = 0;
	virtual int GetBlendFormatScaleU(const char* name, int index) = 0;
	virtual int GetBlendFormatScaleV(const char* name, int index) = 0;
	virtual int GetBlendFormatAngleX(const char* name, int index) = 0;
	virtual int GetBlendFormatAngleY(const char* name, int index) = 0;
	virtual int GetBlendFormatAngleZ(const char* name, int index) = 0;
	virtual bool SetBlendFormatScaleU(const char* name, int index, int value) = 0;
	virtual bool SetBlendFormatScaleV(const char* name, int index, int value) = 0;
	virtual bool SetBlendFormatAngleX(const char* name, int index, int value) = 0;
	virtual bool SetBlendFormatAngleY(const char* name, int index, int value) = 0;
	virtual bool SetBlendFormatAngleZ(const char* name, int index, int value) = 0;

	// 全局环境贴图
	virtual void SetGlobalEnvironmentMap(const char* name) = 0;
	virtual const char* GetGlobalEnvironmentMap() const = 0;

	// 是否开启水面波动
	virtual void SetWaterWave(bool value) = 0;
	virtual bool GetWaterWave() const = 0;

	// 是否开启水体深度
	virtual void SetWaterDepth(bool value) = 0;
	virtual bool GetWaterDepth() const = 0;
	
	// 是否开启水体岸边过渡
	virtual void SetWaterBorder(bool value) = 0;
	virtual bool GetWaterBorder() const = 0;

	// 是否开启水面反射
	virtual void SetWaterReflect(bool value) = 0;
	virtual bool GetWaterReflect() const = 0;

	// 水面环境贴图是否可以重复使用
	virtual void SetWaterEnvMapReusable(bool value) = 0;
	virtual bool GetWaterEnvMapReusable() const = 0;

	// 是否开启水面阳光反射
	virtual void SetWaterSunReflect(bool value) = 0;
	virtual bool GetWaterSunReflect() const = 0;

	// 是否要更新摄像机焦点位置
	virtual void SetUpdateFocus(bool value) = 0;
	virtual bool GetUpdateFocus() const = 0;

	// 是否仅剪裁地面
	virtual void SetOnlyCullTerrain(bool value) = 0;
	virtual bool GetOnlyCullTerrain() const = 0;

	// 是否使用半球环境光
	virtual void SetSphereAmbient(bool value) = 0;
	virtual bool GetSphereAmbient() const = 0;

	// 是否对碰撞检测的结果进行校验
	virtual void SetVerifyTraceResult(bool value) = 0;
	virtual bool GetVerifyTraceResult() const = 0;

	// 生成行走数据时，是否进行记录
	virtual void SetGenWalkTrace(bool value) = 0;
	virtual bool GetGenWalkTrace() const = 0;

	// 设置摄像机焦点位置
	virtual bool SetFocusPosition(float x, float y, float z) = 0;
	// 设置摄像机焦点半径
	virtual bool SetFocusRadius(float x, float y, float z) = 0;

	// 水面类型管理
	virtual PERSISTID CreateWater(const char* name) = 0;
	virtual PERSISTID GetWater(const char* name) = 0;
	virtual bool DeleteWater(const char* name) = 0;
	virtual bool ClearWater() = 0;
	virtual const char* GetWaterName(int index) = 0;
	virtual int GetWaterIndex(const char* name) = 0;
	virtual int GetWaterCount() const = 0;
	virtual void GetWaterList(const IVarList& args, IVarList& result) const = 0;
	virtual bool RefreshWater() = 0;
	virtual bool SetWaterCubeMapStatic(bool value) = 0;
	virtual bool DumpWaterCubeMap(int index, const char* file_name) = 0;
	
	// 花草类型管理
	virtual PERSISTID CreateGrass(const char* name) = 0;
	virtual PERSISTID GetGrass(const char* name) = 0;
	virtual bool DeleteGrass(const char* name) = 0;
	virtual bool ClearGrass() = 0;
	virtual const char* GetGrassName(int index) = 0;
	virtual int GetGrassIndex(const char* name) = 0;
	virtual int GetGrassCount() const = 0;
	virtual void GetGrassList(const IVarList& args, IVarList& result) const = 0;
	virtual bool RefreshGrass() = 0;

	// 刷新地面
	virtual bool RefreshGround() = 0; 
	// 刷新地面物体
	virtual bool RefreshVisual() = 0; 
	
	// 碰撞检测屏蔽
	virtual bool SetTraceMask(const char* type, bool value) = 0;
	virtual bool GetTraceMask(const char* type) = 0;
	virtual void SetTraceMaskValue(unsigned int value) = 0;
	virtual unsigned int GetTraceMaskValue() const = 0;

	// 获得地区名称列表
	virtual void GetAreaList(const IVarList& args, IVarList& result) = 0;
	// 获得指定位置的地区标记名称
	virtual const char* GetAreaName(float x, float z) = 0;

	// 添加地面物体
	virtual int AddVisBase(const char* name, IVisBase* pVisBase, 
		bool is_role, float clip_radius) = 0;
	// 移除地面物体
	virtual int RemoveVisBase(IVisBase* pVisBase) = 0;
	// 重新定位位置发生改变的地面物体
	virtual int RelocateVisBase(IVisBase* pVisBase, float x, float y, 
		float z) = 0;

	// 获得地区名称列表
	virtual void GetRegionList(const IVarList& args, IVarList& result) = 0;
	// 获得指定位置的范围标记是否有效
	virtual bool GetRegionEnable(const char* name, float x, float z) = 0;

	// 添加动态物件
	virtual bool AddDynamicWalkable(IVisBase* pVisBase) = 0;
	// 删除动态物件
	virtual bool DeleteDynamicWalkable(IVisBase* pVisBase) = 0;

		// 设置地形参数
	virtual bool SetParameter(size_t zone_scale, size_t chunk_scale, size_t tex_scale, 
		size_t tex_units, size_t alpha_per_unit, size_t collide_per_unit) = 0;
	// 初始化区域
	virtual bool InitZones(size_t rows, size_t cols, size_t origin_row, 
		size_t origin_col, size_t range) = 0;

	// 地形删除之前删除所有地块的加载器
	virtual bool ReleaseAllZoneLoader()= 0;
};

#endif // _VISUAL_I_TERRAIN_H
