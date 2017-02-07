//--------------------------------------------------------------------
// 文件名:		tex_light_map.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _TEX_LIGHT_MAP_H
#define _TEX_LIGHT_MAP_H

#include "../public/macros.h"


#include "../public/i_interface.h"
#include "../visual/i_render.h"
#include "../visual/i_resource.h"

// 地面光照贴图

class CTexLightMap
{
public:
	CTexLightMap();
	~CTexLightMap();

	// 宽度
	void SetRows(size_t value) { m_nRows = value; }
	size_t GetRows() const { return m_nRows; }

	// 高度
	void SetCols(size_t value) { m_nCols = value; }
	size_t GetCols() const { return m_nCols; }

	// 像素数量
	size_t GetSize() const { return m_nRows * m_nCols; }

	// 贴图资源
	void SetTexture(IRender* pRender, ITexture* value);

	ITexture* GetTexture();

	// 创建贴图
	bool CreateTex(IRender* pRender);
	// 释放贴图
	void ReleaseTex();

	// 使用贴图文件数据生成贴图
	bool BuildTex(IRender* pRender, ITexture* pTex);
	
	// 填充颜色数据到贴图
	bool WriteColor32(const unsigned int* colors, size_t rows, size_t cols);
	// 填充为相同的数值
	bool WriteValue(unsigned int value, size_t rows, size_t cols);

	// 保存贴图到文件
	bool SaveBitmap(const char* file);
	// 保存到压缩的DDS文件
	bool SaveDDS(IRender* pRender, const char* file, bool save_occlusion);

private:
	CTexLightMap(const CTexLightMap&);
	CTexLightMap& operator=(const CTexLightMap&);

private:
	size_t m_nRows;
	size_t m_nCols;
	ITexture* m_pTexture;
};

#endif // _TEX_LIGHT_MAP_H
 
