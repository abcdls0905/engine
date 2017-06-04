
#ifndef _CUBE_TEX_LOADER_H
#define _CUBE_TEX_LOADER_H

#include "../public/macros.h"
#include "../public/fast_str.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../visual/i_res_loader.h"

#include "tex_tools.h"
// 立方体贴图加载器

class CTexture;
class Render;

class CCubeTexLoader: public IResLoader
{
public:
	static CCubeTexLoader* NewInstance();

public:
	CCubeTexLoader();
	virtual ~CCubeTexLoader();

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
	void SetOwner(CTexture* pOwner);
	// 贴图文件名
	void SetFileName(const char* file_name);
	
private:
	CCubeTexLoader(const CCubeTexLoader&);
	CCubeTexLoader& operator=(const CCubeTexLoader&);

	// 加载DDS或其他通用格式贴图
	//bool LoadDDS(const char* file_name);
	//bool LoadDDSData(const char* file_name, const void* dds_data, 
	//	size_t dds_size);

private:
	CTexture* m_pOwner;
	Render* m_pRender;
	core_string m_strFileName;
	unsigned int m_nWidth;
	unsigned int m_nHeight;
	unsigned int m_nFormat;
	IMAGE_INFO m_ImageInfo;
	fm_uint m_pTexture;
	unsigned char* m_pData;
	unsigned int m_nDataSize;
};

#endif // _CUBE_TEX_LOADER_H
