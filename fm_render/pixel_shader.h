//--------------------------------------------------------------------
// 文件名:		pixel_shader.h
// 内  容:		
// 说  明:		
// 创建日期:	2012年9月26日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PIXEL_SHADER_H
#define _PIXEL_SHADER_H

#include "../public/fast_str.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"


#include "../visual/i_resource.h"

// 像素着色器

class Render;

class CPixelShader: public IPixelShader
{
	const void * m_shader_data;
	unsigned int m_shader_size;
public:
	CPixelShader(Render* pRender);
	virtual ~CPixelShader();

	// 释放
	virtual void Release();

	// 文件名
	void SetFileName(const char* value) { m_strFileName = value; }
	const char* GetFileName() const { return m_strFileName.c_str(); }

	// 函数名
	void SetFuncName(const char* value) { m_strFuncName = value; }
	const char* GetFuncName() const { return m_strFuncName.c_str(); }

	// 宏定义
	void SetDefine(const char* value) { m_strDefine = value; }
	const char* GetDefine() const { return m_strDefine.c_str(); }

	// 创建
	bool Create(const void* shader_data, unsigned int shader_size);

	// 设置资源索引
	void SetIndex(int value) { m_nIndex = value; }

private:
	CPixelShader();
	CPixelShader(const CPixelShader&);
	CPixelShader& operator=(const CPixelShader&);
	
	// 销毁
	virtual void Destroy();
	// 设备就绪时的处理
	virtual bool Restore();
	// 设备丢失时的处理
	virtual bool Invalidate();
private:
	Render* m_pRender;
	int m_nIndex;

	core_string m_strFileName;
	core_string m_strFuncName;
	core_string m_strDefine;
 
};

#endif // _PIXEL_SHADER_H
