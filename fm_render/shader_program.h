//--------------------------------------------------------------------
// �ļ���:		shader_program.h
// ��  ��:		
// ˵  ��:		
// ��������:	2013��3��26��
// ������:		�ſ�ʤ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _SHADER_PROGRAM_H
#define _SHADER_PROGRAM_H

#include "../public/fast_str.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"


#include "../visual/i_resource.h"

#include "shader_param_op.h"

// ��ɫ������

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

    //ʹ�ø�shader
	virtual void UsedShader();

	// �ͷ�
	virtual void Release();

	bool Create(IVertexShader* pVS,IPixelShader* pPS,const char* element_names[], 
		unsigned int element_num);

	// ������Դ����
	void SetIndex(int value) { m_nIndex = value; }

	virtual IVertexShader* GetVertexShader() const;

    virtual IPixelShader*  GetPixelShader() const;

	void Print();

private:
	// ����
	bool Create(fm_uint vs, fm_uint ps, const char* element_names[], 
		unsigned int element_num);

	CShaderProgram();
	CShaderProgram(const CShaderProgram&);
	CShaderProgram& operator=(const CShaderProgram&);
	
	// ����
	virtual void Destroy();

#ifdef PROCESS_LOST_DEVICE
	// �豸����ʱ�Ĵ���
	virtual bool Restore();

	// �豸��ʧʱ�Ĵ���
	virtual bool Invalidate();
#endif
private:
	CShaderParamOp* m_pshaderparamop;
	Render* m_pRender;
	int m_nIndex;
};

#endif // _SHADER_PROGRAM_H
