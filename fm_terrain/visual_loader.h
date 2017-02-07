//--------------------------------------------------------------------
// 文件名:		visual_loader.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年11月1日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _VISUAL_LOADER_H
#define _VISUAL_LOADER_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../public/fast_str.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"


#include "../visual/i_res_loader.h"
#include "visual_info.h"
#include "name_list.h"

// 地面物体信息加载器

class CTerrainZone;
class IBlockReader;

class CVisualLoader: public IResLoader
{
public:
	static CVisualLoader* NewInstance();
	
	static IBlockReader* CreateBlockReader(int block_id, void* context);
	
#pragma pack(push, 1)
	struct grass_info_t
	{
		unsigned short nPosiX;		// 位置
		unsigned short nPosiZ;
		unsigned char nGrassIndex;	// 花草索引
		unsigned char nScaleAmount;	// 范围(1-10),数量(1-15)
		unsigned short nSeed;		// 随机种子
	};
#pragma pack(pop)

	struct grass_all_t
	{
		size_t nGrassTypeNum;
		CNameList GrassNames;
		size_t nGrassDataNum;
		grass_info_t* pGrassData;
	};

public:
	CVisualLoader();
	virtual ~CVisualLoader();

	// 释放
	virtual void Release();
	// 获得名字
	virtual const char* GetName();
	// 加载
	virtual bool Load(bool async);
	// 创建
	virtual bool Create(bool async);
	// 结束异步创建 
	virtual bool EndCreate(int category, bool succeed);
	
	// 资源主体
	void SetOwner(CTerrainZone* pOwner);
	// 地面物体文件名
	void SetFileName(const char* file_name);
	// 装饰物文件名
	void SetWidgetFile(const char* file_name);
	// 辅助物体文件名
	void SetHelperFile(const char* file_name);
	// 模型碰撞信息属性文件名
	void SetCollideFile(const char* file_name);
	// 物体描述信息文件名
	void SetDescFile(const char* file_name);
	// 光贴图合并信息文件名
	void SetAtlasFile(const char* file_name);
	// 声音文件名
	void SetSoundFile(const char* file_name);
	// 花草文件名
	void SetGrassFile(const char* file_name);

	// 保存地面物体数据
	static bool SaveVisuals(const char* file_name, const char* widget_file,
		const char* helper_file, const char* collide_file, 
		const char* desc_file, const char* sound_file, CTerrainZone* pZone);
	static bool SaveBlockGrass(const char* file_name, CTerrainZone* pZone);
	
private:
	CVisualLoader(const CVisualLoader&);
	CVisualLoader& operator=(const CVisualLoader&);

	// 加载地面物体数据
	bool LoadVisuals(const char* file_name);
	// 加载模型碰撞相关属性
	bool LoadCollideInfo(const char* file_name);
	// 加载物体描述信息
	bool LoadDesc(const char* file_name);
	// 加载光贴图合并信息 
	bool LoadAtlas(const char* file_name);
	// 加载花草数据
	bool LoadBlockGrass(const char* file_name);
	// 创建哈希索引
	void CreateHashIndex();
	// 查找所属的模型信息
	bool FindVisualIndex(const char* name, size_t& index);

private:
	CTerrainZone* m_pOwner;
	bool m_bDesignMode;
	core_string m_strFileName;
	core_string m_strWidgetFile;
	core_string m_strHelperFile;
	core_string m_strCollideFile;
	core_string m_strDescFile;
	core_string m_strAtlasFile;
	core_string m_strSoundFile;
	core_string m_strGrassFile;
	grass_all_t m_Grass;
	TArrayPod<visual_info_t*, 1, TCoreAlloc> m_Visuals;
	int* m_pHashIndex;
	size_t m_nHashSize;
};

#endif // _VISUAL_LOADER_H
