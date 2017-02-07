//--------------------------------------------------------------------
// 文件名:		tex_alpha_map.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _TEX_ALPHA_MAP_H
#define _TEX_ALPHA_MAP_H

#include "../public/macros.h"



// 用于地表贴图融合的ALPHA贴图数据

class CTexAlphaMap
{
public:
	CTexAlphaMap();
	~CTexAlphaMap();

	// 尺寸
	void SetScale(size_t value) { m_nScale = value; }
	// ALPHA数据的字节数
	size_t GetSize() const { return m_nScale * m_nScale; }

	// 建立ALPHA数据，并全部为0
	void CreateAlpha();

	// 设置Alpha值
	bool SetAlpha(size_t row, size_t col, size_t value);
	// 获得Alpha值
	size_t GetAlpha(size_t row, size_t col) const;

	// 是否为空白
	bool IsEmpty() const;

	// 填充ALPHA数据到贴图
	bool WriteMapData(const unsigned char* data, size_t size);

	// 获得ALPHA数据
	unsigned char* GetMapData() const { return m_pAlpha; }
	
private:
	CTexAlphaMap(const CTexAlphaMap&);
	CTexAlphaMap& operator=(const CTexAlphaMap&);

	// 清除ALPHA数据
	void ReleaseAlpha();

private:
	size_t m_nScale;
	unsigned char* m_pAlpha;
	size_t m_nSize;
};

#endif // _TEX_ALPHA_MAP_H

