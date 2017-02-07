//--------------------------------------------------------------------
// 文件名:		light_loader.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月15日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _LIGHT_LOADER_H
#define _LIGHT_LOADER_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../public/fast_str.h"
#include "../public/core_type.h"


#include "../public/i_interface.h"
#include "../visual/i_render.h"
#include "../visual/i_res_loader.h"

// 地面光照加载器

class CTerrainZone;
class Render;

#pragma pack(push, 1)

struct tga_header_t
{
	unsigned char byte1;		// = 0
	unsigned char byte2;		// = 0
	unsigned char type;
	unsigned char byte4_9[9];	// = 0
	unsigned short width;
	unsigned short height;
	unsigned char bpp;			// bit per pixel
	union
	{
		unsigned char attr8;
		struct
		{
			unsigned char attr: 4;
			unsigned char rez: 1;
			unsigned char origin: 1;
			unsigned char storage: 2;
		};
	};
};
	
#pragma pack(pop)


class CLightLoader: public IResLoader
{
public:
	static CLightLoader* NewInstance();
	
public:
	CLightLoader();
	virtual ~CLightLoader();

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
	// 位图文件名
	void SetBitmap(const char* file_name);
	// 光贴图文件名
	void SetTexFile(const char* file_name);
	// 混合颜色文件名
	void SetBlendFile(const char* file_name);

	// 保存地表混合颜色
	static bool SaveBlendColors(const char* file_name, CTerrainZone* pZone);

private:
	CLightLoader(const CLightLoader&);
	CLightLoader& operator=(const CLightLoader&);

	// 加载光照位图文件
	bool LoadBitmapFile(const char* file_name);
	// 加载光贴图文件
	bool LoadDDSFile(const char* file_name);
	// 加载混合颜色文件
	bool LoadBlendFile(const char* file_name);
	// 加载Lightmap配置文件
	bool LoadConfigFile(const char* file_name);
private:
	CTerrainZone* m_pOwner;
	IRender* m_pRender;
	core_string m_strBitmap;
	core_string m_strTexFile;
	core_string m_strBlendFile;
	size_t m_nRows;
	size_t m_nCols;
	size_t m_nBmpWidth;
	size_t m_nBmpHeight;
	unsigned int* m_pBmpColors;
	size_t m_nBlendRows;
	size_t m_nBlendCols;
	unsigned int* m_pBlendColors;
	ITexture* m_pTex;
};

#endif // _LIGHT_LOADER_H
