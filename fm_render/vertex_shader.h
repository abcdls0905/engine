//--------------------------------------------------------------------
// �ļ���:		vertex_shader.h
// ��  ��:		
// ˵  ��:		
// ��������:	2012��9��26��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _VERTEX_SHADER_H
#define _VERTEX_SHADER_H

#include "../public/fast_str.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"


#include "../visual/i_resource.h"

// ������ɫ��

class Render;

class CVertexShader: public IVertexShader
{
	const void* m_shader_data;
	fm_int m_shader_size;
public:
	CVertexShader(Render* pRender);
	virtual ~CVertexShader();

	// �ͷ�
	virtual void Release();

	// �ļ���
	void SetFileName(const char* value) { m_strFileName = value; }
	const char* GetFileName() const { return m_strFileName.c_str(); }

	// ������
	void SetFuncName(const char* value) { m_strFuncName = value; }
	const char* GetFuncName() const { return m_strFuncName.c_str(); }

	// �궨��
	void SetDefine(const char* value) { m_strDefine = value; }
	const char* GetDefine() const { return m_strDefine.c_str(); }

	// ����
	bool Create(const void* shader_data, unsigned int shader_size);

	// ������Դ����
	void SetIndex(int value) { m_nIndex = value; }

private:
	CVertexShader();
	CVertexShader(const CVertexShader&);
	CVertexShader& operator=(const CVertexShader&);
	
	// ����
	virtual void Destroy();

	// �豸����ʱ�Ĵ���
	virtual bool Restore();
	// �豸��ʧʱ�Ĵ���
	virtual bool Invalidate();

private:
	Render* m_pRender;
	int m_nIndex;

	core_string m_strFileName;
	core_string m_strFuncName;
	core_string m_strDefine;

};

#endif // _VERTEX_SHADER_H