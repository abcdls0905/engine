
#ifndef _TEX_LOADER_H
#define _TEX_LOADER_H

#include "../public/macros.h"
#include "../public/core_type.h"


#include "../visual/i_res_loader.h"
#include "../visual/vis_utils.h"
#include <assert.h>

/*
#define FREEIMAGE_LIB

#include "freeimage.h"
*/
#ifndef __APPLE__
	#include "libpvr.h"
#endif

// 贴图加载器

class CTexture;
class Render;

#include <math.h>
#include "tex_tools.h"

class CTexLoader: public IResLoader
{
public:
	static CTexLoader* NewInstance();
public:
	CTexLoader();
	virtual ~CTexLoader();

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
	// 是否保持原有尺寸
	void SetOriginSize(bool value);
	
private:
	CTexLoader(const CTexLoader&);
	CTexLoader& operator=(const CTexLoader&);

	// 加载DDS或其他通用格式贴图
	//bool LoadDDS(const char* file_name);
	//bool LoadDDSData(const char* file_name, const void* dds_data, 
	//	size_t dds_size);

	bool load_file(const char* name);
private:
	CTexture* m_pOwner;
	Render* m_pRender;
	core_string m_strFileName;
	bool m_bOriginSize;
	unsigned int m_nWidth;
	unsigned int m_nHeight;
	unsigned int m_nFormat;
	unsigned int m_nOriginWidth;
	unsigned int m_nOriginHeight;
	unsigned char* m_pData;
	unsigned int m_nDataSize;
	unsigned char* m_pDataMask;
	unsigned int m_nDataMaskSize;

	IMAGE_INFO m_ImageInfo;
	fm_uint m_pTexture;
};

#endif // _TEX_LOADER_H
