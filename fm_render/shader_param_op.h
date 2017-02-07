//--------------------------------------------------------------------
// �ļ���:		shader_program.h
// ��  ��:		
// ˵  ��:		
// ��������:	2013��5��8��
// ������:		�ſ�ʤ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------


#include "../visual/fm_define.h"
#include "../visual/i_resource.h"

#include "../utils/array_pod.h"
#include "../utils/pod_hash_map.h"
#include "render.h"

// 
class TShaderParamAlloc
{
public:
	TShaderParamAlloc() {}
	~TShaderParamAlloc() {}

	// �����ڴ�
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// ����
	void Swap(TShaderParamAlloc& src) {}
};

/*
ȫ�ֳ����Ĵ�������������
*/
// ��������
enum ShaderParamType
{
    SHADER_PRARM_NONE,
	SHADER_PRARM_1_F,
	SHADER_PRARM_1_I,
	SHADER_PRARM_2_F,
	SHADER_PRARM_2_I,
	SHADER_PRARM_3_F,
	SHADER_PRARM_3_I,
	SHADER_PRARM_4_F,
	SHADER_PRARM_4_I,
	SHADER_PRARM_3_3_F,
	SHADER_PRARM_4_4_F,
	SHADER_PRARM_N_F,
	SHADER_PRARM_N_I,
	SHADER_PRARM_N_2_F,
	SHADER_PRARM_N_3_F,
	SHADER_PRARM_N_4_F,
	SHADER_PRARM_TEX_2D,
	SHADER_PRARM_TEX_3D,
	SHADER_PRARM_TEX_CUBE,
	SHADER_PRARM_UNKNOW,
};

// Shader����
class CShaderParam
{
public:

	CShaderParam(fm_float fvalue,bool bAuto,Render* pRender);

	CShaderParam(fm_int nvalue,bool bAuto,Render* pRender);

	CShaderParam(fm_float x,fm_float y,bool bAuto,Render* pRender);

	CShaderParam(fm_int x,fm_int y,bool bAuto,Render* pRender);

	CShaderParam(fm_float x,fm_float y,fm_float z,bool bAuto,Render* pRender);

	CShaderParam(fm_int x,fm_int y,fm_int z,bool bAuto,Render* pRender);

	CShaderParam(fm_float x,fm_float y,fm_float z,fm_float w,bool bAuto,Render* pRender);

	CShaderParam(fm_int x,fm_int y,fm_int z,fm_int w,bool bAuto,Render* pRender);

	CShaderParam(fm_sizei count,const fm_float* v,bool bAuto,Render* pRender);

	CShaderParam(fm_sizei count,const fm_int* v,bool bAuto,Render* pRender);

	//��ѧ����չ
	CShaderParam(FmVec2& vec2,bool bAuto,Render* pRender);

	CShaderParam(FmVec3& vec3,bool bAuto,Render* pRender);

	CShaderParam(FmVec4& vec4,bool bAuto,Render* pRender);

	CShaderParam(FmMat3& mat3,bool bAuto,Render* pRender);

	CShaderParam(FmMat4& mat4,bool bAuto,Render* pRender);

	CShaderParam(fm_sizei count,const FmVec2* v,bool bAuto,Render* pRender);

	CShaderParam(fm_sizei count,const FmVec3* v,bool bAuto,Render* pRender);

	CShaderParam(fm_sizei count,const FmVec4* v,bool bAuto,Render* pRender);


	//��������
	CShaderParam(ShaderParamType type,fm_int tex_id,bool bAuto,Render* pRender);

	~CShaderParam();
    //��ȡ��������
	inline ShaderParamType GetShaderParamType() const;
    //��ȡ���ݵ�ַ
	inline void* GetData();
	//��ȡ���ݵĴ�С
	inline size_t GetDataSize() const;
    // �Ƿ���Ҫ�������
	bool IsAuto();

	//���ò����������
	inline void BindSamplerID(int index);

	inline int GetBindSamplerID();

	// ���ز�����
	bool operator==(const CShaderParam& sp) const;

	bool operator!=(const CShaderParam& sp) const;

	union
	{
		float*  m_pfValue;
		int*    m_pnValue;
		void*   m_pData;
		FmVec2* m_pVec2;
		FmVec3* m_pVec3;
		FmVec4* m_pVec4;
		FmMat3* m_pMat3;
		FmMat4* m_pMat4;
	};

	//ת���ɿ��ӻ���������
	core_string ToString();

private:
   ShaderParamType m_Type;
   size_t          m_DataSize;
   Render*         m_pRender;
   bool            m_bAuto;
};

//����������
class CShaderParamOp:public IShaderParamOp
{
public:
	CShaderParamOp(fm_uint nProgram,Render* pRender);
	virtual ~CShaderParamOp();
	//���ݼĴ����������ֻ�ȡ����ֵ
    virtual fm_int FindParamIdByName(const char* pstrName);
	// bAuto  �Ƿ���Ҫ����ײ��Զ���������
  
    virtual bool SetParamValue(fm_int Id,fm_float x,bool bAuto = true);

	virtual bool SetParamValue(fm_int Id,fm_int x,bool bAuto = true)  ;
	
	virtual bool SetParamValue(fm_int Id,fm_float x,fm_float y,bool bAuto = true)  ;

	virtual bool SetParamValue(fm_int Id,fm_int x,fm_int y,bool bAuto = true);

	virtual bool SetParamValue(fm_int Id,fm_float x,fm_float y,fm_float z,bool bAuto = true);

	virtual bool SetParamValue(fm_int Id,fm_int x,fm_int y,fm_int z,bool bAuto = true);

	virtual bool SetParamValue(fm_int Id,fm_float x,fm_float y,fm_float z,fm_float w,bool bAuto = true);

	virtual bool SetParamValue(fm_int Id,fm_int x,fm_int y,fm_int z,fm_int w,bool bAuto = true);

	virtual bool SetParamValue(fm_int Id,fm_sizei count,const fm_float* v,bool bAuto = true);

	virtual bool SetParamValue(fm_int Id,fm_sizei count,const fm_int* v,bool bAuto = true);

	// ��չ�����ϵ���ѧ����а�װ��
	virtual bool SetParamValue(fm_int Id,FmVec2& value,bool bAuto = true);

	virtual bool SetParamValue(fm_int Id,FmVec3& value,bool bAuto = true);

	virtual bool SetParamValue(fm_int Id,FmVec4& value,bool bAuto = true);

	virtual bool SetParamValue(fm_int Id,FmMat3& value,bool bAuto = true);

	virtual bool SetParamValue(fm_int Id,FmMat4& value,bool bAuto = true);

	virtual bool SetParamValue(fm_int Id,fm_sizei count,const FmVec2* value,bool bAuto = true);

	virtual bool SetParamValue(fm_int Id,fm_sizei count,const FmVec3* value,bool bAuto = true);

	virtual bool SetParamValue(fm_int Id,fm_sizei count,const FmVec4* value,bool bAuto = true);

	// ��������
	// ����ID         ����ID
	virtual bool SetTexture2D(fm_int Id,fm_int TexID);
	virtual bool SetTexture3D(fm_int Id,fm_int TexID);
	virtual bool SetTextureCube(fm_int Id,fm_int TexID);

	inline void SetProgram(fm_uint program) { m_nProgram = program; }

	// ���Է����ӡ����
	void Print();
private:
    //�ڲ�
    inline bool SetParamValue(fm_int Id,CShaderParam* sp);

	inline bool SetParamTexSampler(fm_int Id,CShaderParam* sp);
	//ֱ�������ϴ�����
	inline void UpGPUParamValue(fm_int Id,CShaderParam* sp);

	inline core_string UniformValueToString(fm_int location);

	// ����GL_Type��ȡ�������ַ���
    inline const char* GLTypeToGLTypeString(fm_enum type) const;
private:
    TPodHashMap<fm_int, CShaderParam*, TPodTraits<fm_int>,TShaderParamAlloc> m_ShaderParams;
	unsigned int m_nSamplerUsed;//�Ѿ�ʹ�õĲ�������Ŀ
	fm_uint m_nProgram;
	Render* m_pRender;
};
