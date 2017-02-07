//--------------------------------------------------------------------
// 文件名:		blend_tex.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年7月3日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _BLEND_TEX_H
#define _BLEND_TEX_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../public/fast_str.h"
#include "../public/core_type.h"
#include "../public/persistid.h"
#include "../utils/array_pod.h"
#include "../visual/fm_define.h"

// 混合贴图

class ITexture;
class Terrain;

class CBlendTex
{
public:
	// 贴图样式
	struct format_t
	{
		unsigned int nScaleU;
		unsigned int nScaleV;
		unsigned int nAngleX;
		unsigned int nAngleY;
		unsigned int nAngleZ;
		FmMat4 mtxTex;
	};
	
public:
	CBlendTex();
	~CBlendTex();

	// 名字
	void SetName(const char* value) { m_strName = value; }
	const char* GetName() const { return m_strName.c_str(); }
	
	// 哈希值
	void SetHash(unsigned int value) { m_nHash = value; }
	unsigned int GetHash() const { return m_nHash; }
	
	// 贴图路径
	void SetPath(const char* value) { m_strPath = value; }
	const char* GetPath() const { return m_strPath.c_str(); }
	
	// 贴图资源
	void SetTex(ITexture* value) { m_pTex = value; }
	ITexture* GetTex() const { return m_pTex; }

	// 法线贴图
	void SetNormalMap(ITexture* value) { m_pNormalMap = value; }
	ITexture* GetNormalMap() const { return m_pNormalMap; }

	// 高度贴图
	void SetHeightMap(ITexture* value) { m_pHeightMap = value; }
	ITexture* GetHeightMap() const { return m_pHeightMap; }

	// 添加
	bool AddFormat(unsigned int scale_u, unsigned int scale_v, 
		unsigned int angle_x, unsigned int angle_y, unsigned int angle_z);

	// 获得样式数量
	size_t GetFormatCount() const { return m_Formats.size(); }

	// 获得指定索引的样式信息
	format_t* GetFormatByIndex(size_t index)
	{
		Assert(index < m_Formats.size());

		return &m_Formats[index];
	}

	// 更新贴图矩阵
	bool UpdateMatrix(size_t index, Terrain* pTerrain);

	// 高光亮度
	void SetSpecularLevel(float value) { m_fSpecularLevel = value; }
	float GetSpecularLevel() const { return m_fSpecularLevel; }

	// 光泽度
	void SetSpecularPower(float value) { m_fSpecularPower = value; }
	float GetSpecularPower() const { return m_fSpecularPower; }

	// 表面细分高度
	void SetHeightScale(float value) { m_fHeightScale = value; }
	float GetHeightScale() const { return m_fHeightScale; }

private:
	CBlendTex(const CBlendTex&);
	CBlendTex& operator=(const CBlendTex&);

private:
	core_string m_strName; // 名字
	unsigned int m_nHash;
	core_string m_strPath; // 贴图文件路径
	ITexture* m_pTex;
	ITexture* m_pNormalMap;
	ITexture* m_pHeightMap;
	TArrayPod<format_t, 1, TCoreAlloc> m_Formats;
	float m_fSpecularLevel;
	float m_fSpecularPower;
	float m_fHeightScale;
};

#endif // _BLEND_TEX_H
