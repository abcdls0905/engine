//--------------------------------------------------------------------
// 文件名:		terrain_editor.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月17日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _TERRAIN_EDITOR_H
#define _TERRAIN_EDITOR_H

#include "../visual/i_render.h"
#include "../visual/i_terrain_editor.h"
#include "height_loader.h"

// 地形编辑
class TerrainEditor: public ITerrainEditor
{
public:
	TerrainEditor();
	virtual ~TerrainEditor();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	void SetTerrain(Terrain* value);

	// 获得点所在的区域索引值
	int GetZoneIndex(float x, float z);
	// 获得区域的行号
	int GetZoneRow(int zone_index);
	// 获得区域的列号
	int GetZoneCol(int zone_index);
	// 获得区域的名称
	const char* GetZoneName(int zone_index);
	// 获得区域的地面最低点高度
	float GetZoneMinY(int zone_index);
	// 获得区域的地面最高点高度
	float GetZoneMaxY(int zone_index);
	// 获得在内存中所有区域的索引值
	void GetZoneList(const IVarList& args, IVarList& result);
	// 获得指定范围内相关的区域索引值表
	void GetRangeZoneList(const IVarList& args, IVarList& result);
	// 指定区域高度文件是否存在
	bool GetZoneExists(int zone_index);
	// 获得存在文件的所有区域
	void GetExistZoneList(const IVarList& args, IVarList& result);
	// 设置指定区域的数据改变标志
	bool SetZoneChanged(int zone_index, const char* type, bool value);
	// 获得指定区域的数据改变标志
	bool GetZoneChanged(int zone_index, const char* type);
	// 设置指定区域不卸载
	bool SetZoneNoUnload(int zone_index, bool value);
	// 获得指定区域是否卸载
	bool GetZoneNoUnload(int zone_index);
	// 指定的点是否在可编辑范围之内
	bool GetInDesign(float x, float z);

	// 获得区域及其中所有物体的平面范围
	void GetZoneRect(const IVarList& args, IVarList& result);
	// 获得物体的平面范围
	void GetVisualRect(const IVarList& args, IVarList& result);
	// 获得平面范围中的所有点光源
	void GetRectLightList(const IVarList& args, IVarList& result);

	// 设置指定点位置是否显示分块的包围盒
	bool SetShowChunkBoundBox(float x, float z, bool value);
	
	// 设置任意位置高度
	bool SetPosiY(float x, float z, float y);
	// 增加指定位置高度
	bool IncPosiY(float x, float z, float y);
	// 某点的高度发生变化
	bool SetHeightChanged(float x, float z);
	// 某个范围内的高度发生变化
	bool RangeHeightChanged(float beg_x, float beg_z, float end_x, 
		float end_z);
	
	// 获得基础贴图
	int GetTexBase(float x, float z);
	// 设置基础贴图
	bool SetTexBase(float x, float z, int tex_index);

	// 获得混合贴图在当前位置的值
	float GetTexBlend(float x, float z, int tex_index, int format);
	// 获得混合贴图在当前位置的最大当前值
	void GetTexBlendPara(const IVarList& args, IVarList& result);
	// 获得当前位置最大值的混合贴图参数
	void GetMaxBlendPara(const IVarList& args, IVarList& result);
	// 获得指定位置的混合贴图层数
	int GetTexBlendLayerCount(float x, float z);
	// 获得指定位置的指定层的混合贴图索引
	int GetTexBlendLayerTexIndex(float x, float z, int index);
	// 获得指定位置的指定层的混合贴图样式
	int GetTexBlendLayerFormat(float x, float z, int index);
	// 复制指定位置的指定层的混合图到贴图
	bool CopyTexBlendLayer(float x, float z, int tex_index, int format,
		const char* tex_name);
	// 擦除指定位置的指定混合层
	bool EraseTexBlendLayer(float x, float z, int tex_index, int format);
	// 在指定位置查找指定混合贴图层
	bool FindTexBlendLayer(float x, float z, int tex_index, int format);
	// 设置混合贴图的值
	bool SetTexBlend(float x, float z, int tex_index, int format, 
		float strength);
	// 擦除指定混合贴图
	bool EraseTexBlend(float x, float z, int tex_index);
	// 擦除所有混合贴图
	bool EraseAllBlend(float x, float z);
	// 某点的贴图混合发生变化
	bool SetBlendChanged(float x, float z);
	// 某个范围内的混合贴图发生变化
	bool RangeBlendChanged(float beg_x, float beg_z, float end_x, 
		float end_z);

	// 获得指定区域内地面物体对象号
	PERSISTID GetZoneVisual(int zone_index, const char* name);
	// 获得指定区域内地面物体名字
	const char* GetZoneVisualName(int zone_index, const PERSISTID& id);
	// 获得指定区域内所有的地面物体
	void GetZoneVisualList(const IVarList& args, IVarList& result);
	// 获得指定范围内的所有地面物体
	void GetRectVisualList(const IVarList& args, IVarList& result);
	// 获得屏幕空间指定范围内的所有地面物体
	void GetScreenVisualList(const IVarList& args, IVarList& result);

	// 获得水面类型
	int GetWater(float x, float z);
	// 设置水面类型
	bool SetWater(float x, float z, int water_index);
	// 测试是否需要放置水面
	bool TestWater(float x, float z, int water_index);
	
	// 获得花草数据
	void GetGrass(const IVarList& args, IVarList& result);
	// 添加花草
	bool AddGrass(int grass_index, float x, float z, int scale, int amount, 
		int seed);
	// 删除花草
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
	void GetGrassRect(const IVarList& args, IVarList& result);
	// 获得圆形范围内的花草索引
	void GetGrassCircle(const IVarList& args, IVarList& result);

	// 设置范围标记颜色
	bool SetRegionColor(const char* name, const char* color);
	// 获得范围标记颜色
	result_string GetRegionColor(const char* name);
	// 设置指定位置的范围标记
	bool SetRegionEnable(const char* name, float x, float z, bool value);
	// 改变范围名称（编辑器用）
	bool ChangeRegionName(float x, float z, const char* ori_name, 
		const char* new_name);

	// 设置地区标记颜色
	bool SetAreaColor(const char* name, const char* color);
	// 获得地区标记颜色
	result_string GetAreaColor(const char* name);
	// 设置指定位置的地区标记
	bool SetAreaEnable(const char* name, float x, float z, bool value);
	// 清除指定位置的所有地区标记
	bool ClearAreaEnable(float x, float z);
	// 改变地区标记名称（编辑器用）
	bool ChangeAreaName(float x, float z, const char* ori_name, 
		const char* new_name);
	// 改变地区标记名称表（编辑器用，全部的ChangeAreaName后调用）
	bool ChangeAreaNameTable(const char* ori_name, const char* new_name);
	// 删除指定区域标记颜色信息（编辑器用）
	bool RemoveAreaColor(const char* name);

	// 保存指定区域的顶点高度数据
	bool SaveHeight(int zone_index, const char* file_name);
	// 保存压缩过的指定区域的顶点高度数据
	bool SaveHeightCompress(int zone_index, const char* file_name);
	// 保存指定区域的水面设计信息
	bool SaveWater(int zone_index, const char* file_name);
	// 保存指定区域的光贴图亮度数据图片
	//bool SaveDiffuseMap(int zone_index, const char* file_name);
	// 保存指定区域的基础贴图信息
	bool SaveAlphaBase(int zone_index, const char* file_name);
	// 保存指定区域的最终混合贴图
	bool SaveAlphaMap(int zone_index, const char* file_name);
	// 保存指定区域的地面物体信息
	bool SaveVisuals(int zone_index, const char* file_name, 
		const char* widget_file, const char* helper_file, 
		const char* collide_file, const char* desc_file, 
		const char* sound_file);
	// 保存指定区域的花草信息
	bool SaveGrass(int zone_index, const char* file_name);
	// 保存指定区域的客户端行走信息
	bool SaveWalk(int zone_index, const char* file_name);
	// 保存指定区域的可行走图
	bool SaveWalkMap(int zone_index, const char* file_name);
	// 保存指定区域的地表混合颜色
	//bool SaveBlendColors(int zone_index, const char* file_name);
	// 保存指定区域的光贴图
	//bool SaveLightMap(int zone_index, const char* file_name);
	// 保存指定区域的范围标记信息
	bool SaveRegion(int zone_index, const char* file_name);
	// 保存指定区域的地区标记信息
	bool SaveArea(int zone_index, const char* file_name);

	// 创建高度数据读取
	static IBlockReader* CreateHeightReader(int block_id, void* context);
	// 压缩指定区域的高度数据文件
	bool CompressZoneHeight(const char* src_file, const char* dst_file);

	// 添加允许生成碰撞行走层点
	bool GenWalkAddPermit(int zone_index, float x, float y, float z);
	// 添加禁止生成碰撞行走层的范围
	bool GenWalkAddForbid(int zone_index, float x, float y, float z, 
		float radius);
	// 添加场景有效范围栅栏点
	bool GenWalkAddFence(int fence_index, float x, float z);
	// 开始生成碰撞信息
	void GenWalkBegin(const IVarList& args, IVarList& result);
	// 生成地表高度
	bool GenWalkGround(int zone_index);
	// 生成障碍物高度
	bool GenWalkBalk(int zone_index, int row);
	// 生成可行走数据
	bool GenWalkable(int zone_index, float start_x, float start_y, 
		float start_z);
	// 生成可行走层高度
	bool GenWalkFloor(int zone_index);
	// 生成墙面标记
	bool GenWalkWall(int zone_index);
	// 生成水面信息
	bool GenWalkWater(int zone_index);
	// 结束生成碰撞信息
	bool GenWalkEnd(int zone_index);

	// 合并指定区域的小面积模型光贴图
	//bool GenLightMapAtlas(int zone_index, int atlas_map_size, 
	//	int min_light_map_size, int small_file_size);
	
private:
	TerrainEditor(const TerrainEditor&);
	TerrainEditor& operator=(const TerrainEditor&);

	CTerrainZone* InnerGetZone(int zone_index);
	// 获得指定点在AlphaMap上的位置
	bool GetAlphaPosition(CTerrainZone* pZone, CTerrainChunk* pChunk,
		float x, float z, int& row, int& col);

private:
	Terrain* m_pTerrain;
	CZoneManager* m_pZoneManager;
	CWalkGenerator* m_pWalkGenerator;
	CHeightLoader::height_all_t m_Height;
	CHeightLoader::chunk_all_t m_Chunk;
};

#endif // _TERRAIN_EDITOR_H
