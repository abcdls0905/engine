//--------------------------------------------------------------------
// 文件名:		shader_program.h
// 内  容:		
// 说  明:		
// 创建日期:	2013年5月8日
// 创建人:		张俊胜
// 版权所有:	苏州蜗牛电子有限公司
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

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
	void Swap(TShaderParamAlloc& src) {}
};

/*
全局常量寄存器操作管理器
*/
// 参数类型
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

// Shader常量
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

	//数学库扩展
	CShaderParam(FmVec2& vec2,bool bAuto,Render* pRender);

	CShaderParam(FmVec3& vec3,bool bAuto,Render* pRender);

	CShaderParam(FmVec4& vec4,bool bAuto,Render* pRender);

	CShaderParam(FmMat3& mat3,bool bAuto,Render* pRender);

	CShaderParam(FmMat4& mat4,bool bAuto,Render* pRender);

	CShaderParam(fm_sizei count,const FmVec2* v,bool bAuto,Render* pRender);

	CShaderParam(fm_sizei count,const FmVec3* v,bool bAuto,Render* pRender);

	CShaderParam(fm_sizei count,const FmVec4* v,bool bAuto,Render* pRender);


	//构建纹理
	CShaderParam(ShaderParamType type,fm_int tex_id,bool bAuto,Render* pRender);

	~CShaderParam();
    //获取数据类型
	inline ShaderParamType GetShaderParamType() const;
    //获取数据地址
	inline void* GetData();
	//获取数据的大小
	inline size_t GetDataSize() const;
    // 是否需要引擎管理
	bool IsAuto();

	//设置采样器绑定序号
	inline void BindSamplerID(int index);

	inline int GetBindSamplerID();

	// 重载操作符
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

	//转换成可视化对象数据
	core_string ToString();

private:
   ShaderParamType m_Type;
   size_t          m_DataSize;
   Render*         m_pRender;
   bool            m_bAuto;
};

//常量操作类
class CShaderParamOp:public IShaderParamOp
{
public:
	CShaderParamOp(fm_uint nProgram,Render* pRender);
	virtual ~CShaderParamOp();
	//根据寄存器变量名字获取索引值
    virtual fm_int FindParamIdByName(const char* pstrName);
	// bAuto  是否需要引擎底层自动管理设置
  
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

	// 扩展引擎结合的数学库进行包装的
	virtual bool SetParamValue(fm_int Id,FmVec2& value,bool bAuto = true);

	virtual bool SetParamValue(fm_int Id,FmVec3& value,bool bAuto = true);

	virtual bool SetParamValue(fm_int Id,FmVec4& value,bool bAuto = true);

	virtual bool SetParamValue(fm_int Id,FmMat3& value,bool bAuto = true);

	virtual bool SetParamValue(fm_int Id,FmMat4& value,bool bAuto = true);

	virtual bool SetParamValue(fm_int Id,fm_sizei count,const FmVec2* value,bool bAuto = true);

	virtual bool SetParamValue(fm_int Id,fm_sizei count,const FmVec3* value,bool bAuto = true);

	virtual bool SetParamValue(fm_int Id,fm_sizei count,const FmVec4* value,bool bAuto = true);

	// 设置纹理
	// 常量ID         纹理ID
	virtual bool SetTexture2D(fm_int Id,fm_int TexID);
	virtual bool SetTexture3D(fm_int Id,fm_int TexID);
	virtual bool SetTextureCube(fm_int Id,fm_int TexID);

	inline void SetProgram(fm_uint program) { m_nProgram = program; }

	// 调试方便打印数据
	void Print();
private:
    //内部
    inline bool SetParamValue(fm_int Id,CShaderParam* sp);

	inline bool SetParamTexSampler(fm_int Id,CShaderParam* sp);
	//直接设置上传数据
	inline void UpGPUParamValue(fm_int Id,CShaderParam* sp);

	inline core_string UniformValueToString(fm_int location);

	// 根据GL_Type获取其类型字符串
    inline const char* GLTypeToGLTypeString(fm_enum type) const;
private:
    TPodHashMap<fm_int, CShaderParam*, TPodTraits<fm_int>,TShaderParamAlloc> m_ShaderParams;
	unsigned int m_nSamplerUsed;//已经使用的采样器数目
	fm_uint m_nProgram;
	Render* m_pRender;
};
