//--------------------------------------------------------------------
// 文件名:		terrain_copy.h
// 内  容:		
// 说  明:		
// 创建日期:	2011年1月10日
// 创建人:		
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
/*
#ifndef _TERRAIN_COPY_H
#define _TERRAIN_COPY_H

#include "../public/module.h"
#include "../public/core_mem.h"
#include "../public/fast_str.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"
#include "../utils/string_pod.h"
#include "../visual/dx_header.h"

// 地形拷贝

class Terrain;
class TerrainEditor;

class TerrainCopy: public IEntity
{
private:
	struct blend_t
	{
		core_string strName;
		float x;
		float z;
		float value;
		int format;
	};

	struct grass_t
	{
		core_string strName;
		float x;					// 位置
		float z;
		//unsigned int nGrassIndex;	// 花草索引
		unsigned int nScale;		// 范围(1-10)
		unsigned int nAmount;		// 数量(1-15)
		unsigned int nSeed;			// 随机种子
	};

	typedef TArrayPod<blend_t*, 1, TCoreAlloc> BLEND_CONTAINER;
	typedef TArrayPod<grass_t*, 1, TCoreAlloc> GRASS_CONTAINER;

public:
	TerrainCopy();
	virtual ~TerrainCopy();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	// 地形
	bool SetTerrain(PERSISTID id);

	// 设置编辑范围(浮点参数精度为1.0F)
	bool SetRect(float start_x, float start_z, float end_x, float end_z);
	// 设置整体的编辑中心（未分组的为Rect中心，分组的为整体范围中心）
	bool SetCenter(float center_x, float center_z);

	// 设置当前文件名
	bool SetCurrentFile(const char* file_name);

	// 设置旋转角度
	bool SetAngle(float vlaue);

	// 是否保存混合贴图数据
	void SetCopyBlend(bool value);
	bool GetCopyBlend() const;

	// 是否清除指定范围内原混合贴图数据
	void SetClearBlend(bool value);
	bool GetClearBlend() const;

	// 是否保存花草数据
	void SetCopyGrass(bool value);
	bool GetCopyGrass() const;

	// 是否清除指定范围内原花草数据
	void SetClearGrass(bool value);
	bool GetClearGrass() const;

	// 保存前的处理
	bool BeginSave(const char* file_name);
	// 保存所有需要拷贝的数据
	bool Save();

	// 当前混合贴图数据数量
	int GetBlendDataCount() const;

	// 当前花草数据数量
	int GetGrassDataCount() const;

	// 使用存储数据修改指定范围的地形
	bool ModifyTerrain();
	
private:
	TerrainCopy(const TerrainCopy&);
	TerrainCopy& operator=(const TerrainCopy&);

	// 增加新的节点，如果成功，则更新当前节点
	bool InsertNewNode(const char* name);
	bool InsertNewBlendNode(const char* name);
	bool InsertNewGrassNode(const char* name);

	// 收集混合贴图数据
	bool CollectBlendData();
	// 当前指定索引的混合贴图数据
	const char* GetBlendDataName(int index);
	const float GetBlendDataX(int index);
	const float GetBlendDataZ(int index);
	const float GetBlendDataValue(int index);
	const int GetBlendDataFormat(int index);

	// 加载混合贴图数据
	int LoadBlendData(const char* file_name);
	// 保存混合贴图数据
	int SaveBlendData(const char* file_name);

	// 使用存储的混合贴图数据修改指定的地形
	bool UseBlendDataModifyTerrain();

	// 收集花草数据
	bool CollectGrassData();
	// 当前指定索引的花草数据
	const char* GetGrassDataName(int index);
	const float GetGrassDataX(int index);
	const float GetGrassDataZ(int index);
	const int GetGrassDataScale(int index);
	const int GetGrassDataAmount(int index);
	const int GetGrassDataSeed(int index);

	// 加载花草数据
	int LoadGrassData(const char* file_name);
	// 保存花草数据
	int SaveGrassData(const char* file_name);

	// 使用存储的花草数据修改指定的地形
	bool UseGrassDataModifyTerrain();

	// 清除所有数据
	bool ClearOldData(const char* name);

	// 清除指定节点的混合贴图数据
	bool ClearBlendData(const char* name);
	// 清除所有节点的混合贴图数据
	bool ClearAllBlendData();

	// 清除指定节点的花草数据
	bool ClearGrassData(const char* name);
	// 清除所有节点的花草数据
	bool ClearAllGrassData();

private:
	Terrain* m_pTerrain;
	TerrainEditor* m_pEditor;
	core_string m_strFileName;
	FmVec4 m_vStartPos;
	FmVec4 m_vEndPos;
	FmVec4 m_vCenterPos;
	float m_fAngle;
	bool m_bCopyBlend;
	bool m_bClearBlend;
	BLEND_CONTAINER* m_CurBlend;
	TStringPod<char, BLEND_CONTAINER*, TStringTraits<char>, 
		TCoreAlloc> m_BlendAll;
	bool m_bCopyGrass;
	bool m_bClearGrass;
	GRASS_CONTAINER* m_CurGrass;
	TStringPod<char, GRASS_CONTAINER*, TStringTraits<char>, 
		TCoreAlloc> m_GrassAll;
};

#endif // _TERRAIN_COPY_H
*/