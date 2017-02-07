//--------------------------------------------------------------------
// 文件名:		alpha_loader.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _ALPHA_LOADER_H
#define _ALPHA_LOADER_H

#include "../public/macros.h"
#include "../public/core_type.h"
#include "../visual/i_res_loader.h"
#include "name_list.h"

// 混合贴图信息加载器

class CTerrainZone;
class CAlphaDesign;
class IBlockReader;

class CAlphaLoader: public IResLoader
{
public:
	static CAlphaLoader* NewInstance();
	static IBlockReader* CreateBlockReader(int block_id, void* context);
	
#pragma pack(push, 1)
	// 保存到文件的贴图样式
	struct blend_format_t
	{
		unsigned short nScaleU;
		unsigned short nScaleV;
		unsigned short nAngleX;
		unsigned short nAngleY;
		unsigned short nAngleZ;
		unsigned char nTexIndex;
		unsigned char nFmtIndex;
	};
	
	// 保存到文件的最终贴图信息
	struct map_alpha_t
	{
		unsigned short nChunkRow;
		unsigned short nChunkCol;
		unsigned char nTexIndex;
		unsigned char nFormat;
	};
#pragma pack(pop)
	
	struct base_all_t
	{
		size_t nBaseTexNum;
		CNameList BaseNames;
		unsigned char* pBaseTexIndex;
	};

	struct map_all_t
	{
		size_t nMapTexNum;
		CNameList MapNames;
		size_t nFormatNum;
		blend_format_t*	pBlendFormat;
		size_t nMapNum;
		map_alpha_t* pMapAlpha;
		unsigned char* pMapData;
		size_t nMapDataSize;
	};

public:
	CAlphaLoader();
	virtual ~CAlphaLoader();

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
	// 基础贴图设计数据文件
	void SetBaseFile(const char* file_name);
	// 最终混合贴图文件
	void SetMapFile(const char* file_name);

	// 保存基础贴图设计数据
	static bool SaveBlockBase(const char* file_name, CTerrainZone* pZone);
	// 保存最终混合贴图文件
	static bool SaveBlockAlphaMap(const char* file_name, 
		CTerrainZone* pZone);

private:
	CAlphaLoader(const CAlphaLoader&);
	CAlphaLoader& operator=(const CAlphaLoader&);

	bool LoadBlockBase(const char* file_name);
	bool LoadBlockAlphaMap(const char* file_name);
	
private:
	CTerrainZone* m_pOwner;
	core_string m_strBaseFile;
	core_string m_strBlendFile;
	core_string m_strMapFile;
	size_t m_nChunkRows;
	size_t m_nChunkCols;
	size_t m_nAlphaRows;
	size_t m_nAlphaCols;
	size_t m_nAlphaMapScale;
	base_all_t m_Base;
	map_all_t m_Map;
};

#endif // _ALPHA_LOADER_H
