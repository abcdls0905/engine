//--------------------------------------------------------------------
// 文件名:		i_texture.h
// 内  容:		
// 说  明:		
// 创建日期:	2013年1月24日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _VISUAL_I_TEXTURE_H
#define _VISUAL_I_TEXTURE_H

#include "../public/persistid.h"
#include "i_res_base.h"

// 贴图资源

class IShaderTex;

class ITexture: public IResBase
{
public:
	// 对象号
	virtual PERSISTID GetID() = 0;
	// 获得名字
	virtual const char* GetName() = 0;
	// 是否就绪
	virtual bool IsReady() = 0;
	// 是否加载结束（成功或失败）
	virtual bool IsLoadComplete() = 0;
	// 贴图宽度
	virtual unsigned int GetWidth() = 0;
	// 贴图高度
	virtual unsigned int GetHeight() = 0;
	// 贴图实际宽度
	virtual unsigned int GetOriginWidth() = 0;
	// 贴图实际高度
	virtual unsigned int GetOriginHeight() = 0;
	// 贴图格式
	virtual unsigned int GetFormat() = 0;
	// 获得贴图
	virtual IShaderTex* GetShaderTex() = 0;
	// 获取可用贴图(此接口为了模拟D3D的设置空纹理方式,采用默认1X1的一个临时纹理作为代替纹理)
	virtual IShaderTex* GetCanUseShaderTex() = 0;
};

#endif // _VISUAL_I_TEXTURE_H
