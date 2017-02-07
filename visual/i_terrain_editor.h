//--------------------------------------------------------------------
// 文件名:		i_terrain_editor.h
// 内  容:		
// 说  明:		
// 创建日期:
// 创建人:
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _VISUAL_I_TERRAIN_EDITOR_H
#define _VISUAL_I_TERRAIN_EDITOR_H

#include "../public/module.h"

class Terrain;
class CTerrainChunk;
class CTerrainZone;
class CZoneManager;
class CWalkGenerator;

class ITerrainEditor: public IEntity
{
public:
	virtual void SetTerrain(Terrain* value) = 0;

	// 获得点所在的区域索引值
	virtual int GetZoneIndex(float x, float z) = 0;
	// 获得区域的行号
	virtual int GetZoneRow(int zone_index) = 0;
	// 获得区域的列号
	virtual int GetZoneCol(int zone_index) = 0;
	// 获得区域的名称
	virtual const char* GetZoneName(int zone_index) = 0;
	// 获得区域的地面最低点高度
	virtual float GetZoneMinY(int zone_index) = 0;
	// 获得区域的地面最高点高度
	virtual float GetZoneMaxY(int zone_index) = 0;
	// 获得在内存中所有区域的索引值
	virtual void GetZoneList(const IVarList& args, IVarList& result) = 0;
	// 获得指定范围内相关的区域索引值表
	virtual void GetRangeZoneList(const IVarList& args, IVarList& result) = 0;
	// 指定区域高度文件是否存在
	virtual bool GetZoneExists(int zone_index) = 0;
	// 获得存在文件的所有区域
	virtual void GetExistZoneList(const IVarList& args, IVarList& result) = 0;
	// 设置指定区域的数据改变标志
	virtual bool SetZoneChanged(int zone_index, const char* type, bool value) = 0;
	// 获得指定区域的数据改变标志
	virtual bool GetZoneChanged(int zone_index, const char* type) = 0;
	// 设置指定区域不卸载
	virtual bool SetZoneNoUnload(int zone_index, bool value) = 0;
	// 获得指定区域是否卸载
	virtual bool GetZoneNoUnload(int zone_index) = 0;
	// 指定的点是否在可编辑范围之内
	virtual bool GetInDesign(float x, float z) = 0;

	// 获得区域及其中所有物体的平面范围
	virtual void GetZoneRect(const IVarList& args, IVarList& result) = 0;
	// 获得物体的平面范围
	virtual void GetVisualRect(const IVarList& args, IVarList& result) = 0;
	// 获得平面范围中的所有点光源
	virtual void GetRectLightList(const IVarList& args, IVarList& result) = 0;

	// 设置指定点位置是否显示分块的包围盒
	virtual bool SetShowChunkBoundBox(float x, float z, bool value) = 0;

	// 设置任意位置高度
	virtual bool SetPosiY(float x, float z, float y) = 0;
	// 增加指定位置高度
	virtual bool IncPosiY(float x, float z, float y) = 0;
	// 某点的高度发生变化
	virtual bool SetHeightChanged(float x, float z) = 0;
	// 某个范围内的高度发生变化
	virtual bool RangeHeightChanged(float beg_x, float beg_z, float end_x, 
		float end_z) = 0;

	// 获得基础贴图
	virtual int GetTexBase(float x, float z) = 0;
	// 设置基础贴图
	virtual bool SetTexBase(float x, float z, int tex_index) = 0;

	// 获得混合贴图在当前位置的值
	virtual float GetTexBlend(float x, float z, int tex_index, int format) = 0;
	// 获得混合贴图在当前位置的最大当前值
	virtual void GetTexBlendPara(const IVarList& args, IVarList& result) = 0;
	// 获得当前位置最大值的混合贴图参数
	virtual void GetMaxBlendPara(const IVarList& args, IVarList& result) = 0;
	// 获得指定位置的混合贴图层数
	virtual int GetTexBlendLayerCount(float x, float z) = 0;
	// 获得指定位置的指定层的混合贴图索引
	virtual int GetTexBlendLayerTexIndex(float x, float z, int index) = 0;
	// 获得指定位置的指定层的混合贴图样式
	virtual int GetTexBlendLayerFormat(float x, float z, int index) = 0;
	// 复制指定位置的指定层的混合图到贴图
	virtual bool CopyTexBlendLayer(float x, float z, int tex_index, int format,
		const char* tex_name) = 0;
	// 擦除指定位置的指定混合层
	virtual bool EraseTexBlendLayer(float x, float z, int tex_index, int format) = 0;
	// 在指定位置查找指定混合贴图层
	virtual bool FindTexBlendLayer(float x, float z, int tex_index, int format) = 0;
	// 设置混合贴图的值
	virtual bool SetTexBlend(float x, float z, int tex_index, int format, 
		float strength) = 0;
	// 擦除指定混合贴图
	virtual bool EraseTexBlend(float x, float z, int tex_index) = 0;
	// 擦除所有混合贴图
	virtual bool EraseAllBlend(float x, float z) = 0;
	// 某点的贴图混合发生变化
	virtual bool SetBlendChanged(float x, float z) = 0;
	// 某个范围内的混合贴图发生变化
	virtual bool RangeBlendChanged(float beg_x, float beg_z, float end_x, 
		float end_z) = 0;

	// 获得指定区域内地面物体对象号
	virtual PERSISTID GetZoneVisual(int zone_index, const char* name) = 0;
	// 获得指定区域内地面物体名字
	virtual const char* GetZoneVisualName(int zone_index, const PERSISTID& id) = 0;
	// 获得指定区域内所有的地面物体
	virtual void GetZoneVisualList(const IVarList& args, IVarList& result) = 0;
	// 获得指定范围内的所有地面物体
	virtual void GetRectVisualList(const IVarList& args, IVarList& result) = 0;
	// 获得屏幕空间指定范围内的所有地面物体
	virtual void GetScreenVisualList(const IVarList& args, IVarList& result) = 0;

	// 获得水面类型
	virtual int GetWater(float x, float z) = 0;
	// 设置水面类型
	virtual bool SetWater(float x, float z, int water_index) = 0;
	// 测试是否需要放置水面
	virtual bool TestWater(float x, float z, int water_index) = 0;

	// 获得花草数据
	virtual void GetGrass(const IVarList& args, IVarList& result) = 0;
	// 添加花草
	virtual bool AddGrass(int grass_index, float x, float z, int scale, int amount, 
		int seed) = 0;
	// 删除花草
	virtual bool RemoveGrass(int grass_index, float x, float z) = 0;
	// 删除方形范围内的花草
	virtual int RemoveGrassRect(int grass_index, float x1, float z1, float x2, 
		float z2) = 0;
	// 删除圆形范围内的花草
	virtual int RemoveGrassCircle(int grass_index, float x, float z, float radius) = 0;
	// 获得方形范围内的花草数量
	virtual int GetGrassCountRect(int grass_index, float x1, float z1, float x2, 
		float z2) = 0;
	// 获得圆形范围内的花草数量
	virtual int GetGrassCountCircle(int grass_index, float x, float z, 
		float radius) = 0;
	// 获得方形范围内的花草索引
	virtual void GetGrassRect(const IVarList& args, IVarList& result) = 0;
	// 获得圆形范围内的花草索引
	virtual void GetGrassCircle(const IVarList& args, IVarList& result) = 0;

	// 设置范围标记颜色
	virtual bool SetRegionColor(const char* name, const char* color) = 0;
	// 获得范围标记颜色
	virtual result_string GetRegionColor(const char* name) = 0;
	// 设置指定位置的范围标记
	virtual bool SetRegionEnable(const char* name, float x, float z, bool value) = 0;
	// 改变范围名称（编辑器用）
	virtual bool ChangeRegionName(float x, float z, const char* ori_name, 
		const char* new_name) = 0;

	// 设置地区标记颜色
	virtual bool SetAreaColor(const char* name, const char* color) = 0;
	// 获得地区标记颜色
	virtual result_string GetAreaColor(const char* name) = 0;
	// 设置指定位置的地区标记
	virtual bool SetAreaEnable(const char* name, float x, float z, bool value) = 0;
	// 清除指定位置的所有地区标记
	virtual bool ClearAreaEnable(float x, float z) = 0;
	// 改变地区标记名称（编辑器用）
	virtual bool ChangeAreaName(float x, float z, const char* ori_name, 
		const char* new_name) = 0;
	// 改变地区标记名称表（编辑器用，全部的ChangeAreaName后调用）
	virtual bool ChangeAreaNameTable(const char* ori_name, const char* new_name) = 0;
	// 删除指定区域标记颜色信息（编辑器用）
	virtual bool RemoveAreaColor(const char* name) = 0;

	// 保存指定区域的顶点高度数据
	virtual bool SaveHeight(int zone_index, const char* file_name) = 0;
	// 保存压缩过的指定区域的顶点高度数据
	virtual bool SaveHeightCompress(int zone_index, const char* file_name) = 0;
	// 保存指定区域的水面设计信息
	virtual bool SaveWater(int zone_index, const char* file_name) = 0;
	// 保存指定区域的光贴图亮度数据图片
	//bool SaveDiffuseMap(int zone_index, const char* file_name) = 0;
	// 保存指定区域的基础贴图信息
	virtual bool SaveAlphaBase(int zone_index, const char* file_name) = 0;
	// 保存指定区域的最终混合贴图
	virtual bool SaveAlphaMap(int zone_index, const char* file_name) = 0;
	// 保存指定区域的地面物体信息
	virtual bool SaveVisuals(int zone_index, const char* file_name, 
		const char* widget_file, const char* helper_file, 
		const char* collide_file, const char* desc_file, 
		const char* sound_file) = 0;
	// 保存指定区域的花草信息
	virtual bool SaveGrass(int zone_index, const char* file_name) = 0;
	// 保存指定区域的客户端行走信息
	virtual bool SaveWalk(int zone_index, const char* file_name) = 0;
	// 保存指定区域的可行走图
	virtual bool SaveWalkMap(int zone_index, const char* file_name) = 0;
	// 保存指定区域的地表混合颜色
	//bool SaveBlendColors(int zone_index, const char* file_name) = 0;
	// 保存指定区域的光贴图
	//bool SaveLightMap(int zone_index, const char* file_name) = 0;
	// 保存指定区域的范围标记信息
	virtual bool SaveRegion(int zone_index, const char* file_name) = 0;
	// 保存指定区域的地区标记信息
	virtual bool SaveArea(int zone_index, const char* file_name) = 0;

	// 压缩指定区域的高度数据文件
	virtual bool CompressZoneHeight(const char* src_file, const char* dst_file) = 0;

	// 添加允许生成碰撞行走层点
	virtual bool GenWalkAddPermit(int zone_index, float x, float y, float z) = 0;
	// 添加禁止生成碰撞行走层的范围
	virtual bool GenWalkAddForbid(int zone_index, float x, float y, float z, 
		float radius) = 0;
	// 添加场景有效范围栅栏点
	virtual bool GenWalkAddFence(int fence_index, float x, float z) = 0;
	// 开始生成碰撞信息
	virtual void GenWalkBegin(const IVarList& args, IVarList& result) = 0;
	// 生成地表高度
	virtual bool GenWalkGround(int zone_index) = 0;
	// 生成障碍物高度
	virtual bool GenWalkBalk(int zone_index, int row) = 0;
	// 生成可行走数据
	virtual bool GenWalkable(int zone_index, float start_x, float start_y, 
		float start_z) = 0;
	// 生成可行走层高度
	virtual bool GenWalkFloor(int zone_index) = 0;
	// 生成墙面标记
	virtual bool GenWalkWall(int zone_index) = 0;
	// 生成水面信息
	virtual bool GenWalkWater(int zone_index) = 0;
	// 结束生成碰撞信息
	virtual bool GenWalkEnd(int zone_index) = 0;
};

#endif // _VISUAL_I_TERRAIN_EDITOR_H
