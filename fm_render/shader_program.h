//--------------------------------------------------------------------
// 文件名:		shader_program.h
// 内  容:		
// 说  明:		
// 创建日期:	2013年3月26日
// 创建人:		张俊胜
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SHADER_PROGRAM_H
#define _SHADER_PROGRAM_H

#include "../public/fast_str.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"


#include "../visual/i_resource.h"

#include "shader_param_op.h"

// 着色器程序

class Render;

class CShaderProgram: public IShaderProgram
{
	fm_uint m_nProgram;
	IVertexShader* m_vs;
	IPixelShader* m_ps;
	unsigned int m_element_num;
	char** m_element_names;
public:
	CShaderProgram(Render* pRender);
	virtual ~CShaderProgram();

    virtual IShaderParamOp* GetParamOp()const;

    virtual IRenderStateOp* GetRenderStateOp() const;

	virtual IRenderDrawOp*  GetRenderDrawOp() const; 

	virtual IRenderStateRecordOp* GetRenderStateRecordOp() const;

    //使用改shader
	virtual void UsedShader();

	// 释放
	virtual void Release();

	bool Create(IVertexShader* pVS,IPixelShader* pPS,const char* element_names[], 
		unsigned int element_num);

	// 设置资源索引
	void SetIndex(int value) { m_nIndex = value; }

	virtual IVertexShader* GetVertexShader() const;

    virtual IPixelShader*  GetPixelShader() const;

	void Print();

private:
	// 创建
	bool Create(fm_uint vs, fm_uint ps, const char* element_names[], 
		unsigned int element_num);

	CShaderProgram();
	CShaderProgram(const CShaderProgram&);
	CShaderProgram& operator=(const CShaderProgram&);
	
	// 销毁
	virtual void Destroy();

#ifdef PROCESS_LOST_DEVICE
	// 设备就绪时的处理
	virtual bool Restore();

	// 设备丢失时的处理
	virtual bool Invalidate();
#endif
private:
	CShaderParamOp* m_pshaderparamop;
	Render* m_pRender;
	int m_nIndex;
};

#endif // _SHADER_PROGRAM_H
