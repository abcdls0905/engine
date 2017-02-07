#include "shader_param_op.h"
#include "../public/core_log.h"
#include "../visual/vis_utils.h"
#include "render_header.h"
#include "tex_sampler.h"


CShaderParam::CShaderParam(fm_float fvalue,bool bAuto,Render* pRender):m_pRender(pRender)
{
    m_DataSize = sizeof(fm_float);
	m_pfValue = (fm_float*)m_pRender->Alloc(m_DataSize);
	m_Type = SHADER_PRARM_1_F;
	*m_pfValue = fvalue;
	m_bAuto = bAuto;
}

CShaderParam::CShaderParam(fm_int nvalue,bool bAuto,Render* pRender):m_pRender(pRender)
{
	m_DataSize = sizeof(fm_int);
	m_pnValue = (fm_int*)m_pRender->Alloc(m_DataSize);
	m_Type = SHADER_PRARM_1_I;
	*m_pnValue = nvalue;
	m_bAuto = bAuto;
}

CShaderParam::CShaderParam(fm_float x,fm_float y,bool bAuto,Render* pRender):m_pRender(pRender)
{
	m_DataSize = sizeof(FmVec2);
	m_pVec2 = (FmVec2*)m_pRender->Alloc(m_DataSize);
	m_Type = SHADER_PRARM_2_F;
	m_bAuto = bAuto;
	*m_pVec2 = FmVec2(x,y);
}

CShaderParam::CShaderParam(fm_int x,fm_int y,bool bAuto,Render* pRender):m_pRender(pRender)
{
	m_DataSize = sizeof(fm_int)*2;
	m_pnValue =  (fm_int*)m_pRender->Alloc(m_DataSize);
	m_pnValue[0] = x;
    m_pnValue[1] = y;
	m_Type = SHADER_PRARM_2_I;
    m_bAuto = bAuto;
}

CShaderParam::CShaderParam(fm_float x,fm_float y,fm_float z,bool bAuto,Render* pRender):m_pRender(pRender)
{
	m_DataSize = sizeof(FmVec3);
	m_pVec3 = (FmVec3*)m_pRender->Alloc(m_DataSize);
	m_Type = SHADER_PRARM_3_F;
	m_bAuto = bAuto;
    *m_pVec3 = FmVec3(x,y,z);
}

CShaderParam::CShaderParam(fm_int x,fm_int y,fm_int z,bool bAuto,Render* pRender):m_pRender(pRender)
{
	m_DataSize = sizeof(fm_int)*3;
	m_pnValue = (fm_int*)m_pRender->Alloc(m_DataSize);
	m_pnValue[0] = x;
    m_pnValue[1] = y;
    m_pnValue[2] = z;
	m_Type = SHADER_PRARM_3_I;
    m_bAuto = bAuto;
}

CShaderParam::CShaderParam(fm_float x,fm_float y,fm_float z,fm_float w,bool bAuto,Render* pRender):m_pRender(pRender)
{
	m_DataSize = sizeof(FmVec4);
	m_pVec4 = (FmVec4*)m_pRender->Alloc(m_DataSize);
	m_Type = SHADER_PRARM_4_F;
	m_bAuto = bAuto;
	*m_pVec4 = FmVec4(x,y,z,w);
}

CShaderParam::CShaderParam(fm_int x,fm_int y,fm_int z,fm_int w,bool bAuto,Render* pRender):m_pRender(pRender)
{
	m_DataSize = sizeof(fm_int)*4;
	m_pnValue = (fm_int*)m_pRender->Alloc(m_DataSize);
	m_pnValue[0] = x;
    m_pnValue[1] = y;
    m_pnValue[2] = z;
    m_pnValue[3] = w;
	m_Type = SHADER_PRARM_4_I;
    m_bAuto = bAuto;
}

CShaderParam::CShaderParam(fm_sizei count,const fm_float* v,bool bAuto,Render* pRender):m_pRender(pRender)
{
    m_DataSize = count*sizeof(fm_float);
	m_pfValue = (fm_float*)m_pRender->Alloc(m_DataSize);
	m_Type = SHADER_PRARM_N_F;
    m_bAuto = bAuto;
	memcpy(m_pfValue,v,m_DataSize);
}

CShaderParam::CShaderParam(fm_sizei count,const fm_int* v,bool bAuto,Render* pRender):m_pRender(pRender)
{
    m_DataSize = count*sizeof(fm_int);
	m_pnValue = (fm_int*)m_pRender->Alloc(m_DataSize);
	m_Type = SHADER_PRARM_N_I;
    m_bAuto = bAuto;
	memcpy(m_pnValue,v,m_DataSize);
}

CShaderParam::CShaderParam(FmVec2& vec2,bool bAuto,Render* pRender):m_pRender(pRender)
{
	m_DataSize = sizeof(FmVec2);
	m_pVec2 = (FmVec2*)m_pRender->Alloc(m_DataSize);
	m_Type = SHADER_PRARM_2_F;
	m_bAuto = bAuto;
	*m_pVec2 = vec2;
}

CShaderParam::CShaderParam(FmVec3& vec3,bool bAuto,Render* pRender):m_pRender(pRender)
{
	m_DataSize = sizeof(FmVec3);
	m_pVec3 = (FmVec3*)m_pRender->Alloc(m_DataSize);
	m_Type = SHADER_PRARM_3_F;
	m_bAuto = bAuto;
	*m_pVec3 = vec3;
}

CShaderParam::CShaderParam(FmVec4& vec4,bool bAuto,Render* pRender):m_pRender(pRender)
{
	m_DataSize = sizeof(FmVec4);
	m_pVec4 = (FmVec4*)m_pRender->Alloc(m_DataSize);
	m_Type = SHADER_PRARM_4_F;
	m_bAuto = bAuto;
	*m_pVec4 = vec4;
}

CShaderParam::CShaderParam(FmMat3& mat3,bool bAuto,Render* pRender):m_pRender(pRender)
{	
	m_DataSize = sizeof(FmMat3);
	m_pMat3 = (FmMat3*)m_pRender->Alloc(m_DataSize);
	m_Type = SHADER_PRARM_3_3_F;
	m_bAuto = bAuto;
	*m_pMat3 = mat3;
}

CShaderParam::CShaderParam(FmMat4& mat4,bool bAuto,Render* pRender):m_pRender(pRender)
{
	m_DataSize = sizeof(FmMat4);
	m_pMat4 = (FmMat4*)m_pRender->Alloc(m_DataSize);
	m_Type = SHADER_PRARM_4_4_F;
	m_bAuto = bAuto;
	*m_pMat4 = mat4;
}

CShaderParam::CShaderParam(fm_sizei count,const FmVec2* v,bool bAuto,Render* pRender):m_pRender(pRender)
{
    m_DataSize = count*sizeof(FmVec2);
	m_pfValue = (fm_float*)m_pRender->Alloc(m_DataSize);
	m_Type = SHADER_PRARM_N_2_F;
    m_bAuto = bAuto;
	memcpy(m_pfValue,v,m_DataSize);
}

CShaderParam::CShaderParam(fm_sizei count,const FmVec3* v,bool bAuto,Render* pRender):m_pRender(pRender)
{
    m_DataSize = count*sizeof(FmVec3);
	m_pfValue = (fm_float*)m_pRender->Alloc(m_DataSize);
	m_Type = SHADER_PRARM_N_3_F;
    m_bAuto = bAuto;
	memcpy(m_pfValue,v,m_DataSize);
}

CShaderParam::CShaderParam(fm_sizei count,const FmVec4* v,bool bAuto,Render* pRender):m_pRender(pRender)
{
    m_DataSize = count*sizeof(FmVec4);
	m_pfValue = (fm_float*)m_pRender->Alloc(m_DataSize);
	m_Type = SHADER_PRARM_N_4_F;
    m_bAuto = bAuto;
	memcpy(m_pfValue,v,m_DataSize);
}


CShaderParam::CShaderParam(ShaderParamType type,fm_int tex_id,bool bAuto,Render* pRender):m_pRender(pRender)
{
	m_DataSize = sizeof(fm_int)*2;//����2�����ݿ� ��һ���TEXID ��2���������
	m_pnValue = (fm_int*)m_pRender->Alloc(m_DataSize);
	m_Type = type;
	m_bAuto = bAuto;
    *m_pnValue = tex_id;
}

CShaderParam::~CShaderParam()
{
	/*
	//������Դɾ��
	switch(m_Type)
	{
	case SHADER_PRARM_1_F:
		delete m_pfValue;
		break;
	case SHADER_PRARM_1_I:
		delete m_pnValue;
		break;
	case SHADER_PRARM_2_F:
		delete m_pVec2;
		break;
	case SHADER_PRARM_3_F:
		delete m_pVec3;
		break;
	case SHADER_PRARM_4_F:
		delete m_pVec4;
		break;
	case SHADER_PRARM_2_I:
	case SHADER_PRARM_3_I:
	case SHADER_PRARM_4_I:
	case SHADER_PRARM_N_I:
		delete [] m_pnValue;
		break;
	case SHADER_PRARM_N_F:
		delete [] m_pfValue;
		break;
	case SHADER_PRARM_3_3_F:
		delete m_pMat3;
	case SHADER_PRARM_4_4_F:
		delete m_pMat4;
		break;
	default:
		if(m_DataSize>0)
		{
			delete [] m_pData;
		}
	};
	*/
    
	if(m_DataSize)
	{
		m_pRender->Free(m_pData,m_DataSize);
	}

	/*
	m_pData = NULL;
	m_DataSize = 0;
	m_bAuto = false;
	m_Type = SHADER_PRARM_NONE;
	*/
}

    //��ȡ��������
ShaderParamType CShaderParam::GetShaderParamType() const
{
	return m_Type;
}
    //��ȡ���ݵ�ַ
void* CShaderParam::GetData()
{
	return m_pData;
}
	//��ȡ���ݵĴ�С
size_t CShaderParam::GetDataSize() const
{
	return m_DataSize;
}
    // �Ƿ���Ҫ�������
bool CShaderParam::IsAuto()
{
	return m_bAuto;
}

//���ò����������
void CShaderParam::BindSamplerID(int index)
{
	//���󶨵������������ŵ���2λ
	m_pnValue[1] = index;
}

int CShaderParam::GetBindSamplerID()
{
	return m_pnValue[1];
}

// ���ز�����
bool CShaderParam::operator==(const CShaderParam& sp) const
{
	//���Ͳ�ƥ��
	if(this->GetShaderParamType() != sp.GetShaderParamType())
	{
		return false;
	}
    //���ݳ����Ƿ����
	if(this->GetDataSize() != sp.GetDataSize() )
	{
		return false;
	}
 
	//���о�ȷ�Ƚ������Ƿ����
	switch(sp.GetShaderParamType())
	{
	case SHADER_PRARM_1_F:
		if(*this->m_pfValue == *sp.m_pfValue)
		{
			return true;
		}
		break;
	case SHADER_PRARM_1_I:
		if(*this->m_pnValue == *sp.m_pnValue)
		{
			return true;
		}
		break;
	case SHADER_PRARM_2_F:
		if( *this->m_pVec2 == *sp.m_pVec2 )
		{
			return true;
		}
		break;
	case SHADER_PRARM_3_F:
		if( *this->m_pVec3 == *sp.m_pVec3 )
		{
			return true;
		}
		break;
	case SHADER_PRARM_4_F:
		if( *this->m_pVec4 == *sp.m_pVec4 )
		{
			return true;
		}
		break;
	case SHADER_PRARM_2_I:
	case SHADER_PRARM_3_I:
	case SHADER_PRARM_4_I:
	case SHADER_PRARM_N_I:
		if( memcmp(this->m_pnValue,sp.m_pnValue,sp.GetDataSize()) == 0 )
		{
			return true;
		}
		break;
	case SHADER_PRARM_N_F:
	case SHADER_PRARM_3_3_F:
	case SHADER_PRARM_4_4_F:
		if( memcmp(this->m_pfValue,sp.m_pfValue,sp.GetDataSize()) == 0 )
		{
			return true;
		}
		break;
	case SHADER_PRARM_TEX_2D:
	case SHADER_PRARM_TEX_3D:
	case SHADER_PRARM_TEX_CUBE:
		{
			if( *(this->m_pnValue) == *sp.m_pnValue )
			{
				return true;
			}
		}
		break;
	default:
		{
			return false;
		};
	};


	return false;
}

bool CShaderParam::operator!=(const CShaderParam& sp) const
{
	//���Ͳ�ƥ��
	if(this->GetShaderParamType() != sp.GetShaderParamType())
	{
		return true;
	}
    //���ݳ����Ƿ����
	if(this->GetDataSize() != sp.GetDataSize() )
	{
		return true;
	}
 
	//���о�ȷ�Ƚ������Ƿ����
	switch(sp.GetShaderParamType())
	{
	case SHADER_PRARM_1_F:
		if(*this->m_pfValue == *sp.m_pfValue)
		{
			return false;
		}
		break;
	case SHADER_PRARM_1_I:
		if(*this->m_pnValue == *sp.m_pnValue)
		{
			return false;
		}
		break;
	case SHADER_PRARM_2_F:
		if( *this->m_pVec2 == *sp.m_pVec2 )
		{
			return false;
		}
		break;
	case SHADER_PRARM_3_F:
		if( *this->m_pVec3 == *sp.m_pVec3 )
		{
			return false;
		}
		break;
	case SHADER_PRARM_4_F:
		if( *this->m_pVec4 == *sp.m_pVec4 )
		{
			return false;
		}
		break;
	case SHADER_PRARM_2_I:
	case SHADER_PRARM_3_I:
	case SHADER_PRARM_4_I:
	case SHADER_PRARM_N_I:
		if( memcmp(this->m_pnValue,sp.m_pnValue,sp.GetDataSize()) == 0 )
		{
			return false;
		}
		break;
	case SHADER_PRARM_N_F:
	case SHADER_PRARM_N_2_F:
    case SHADER_PRARM_N_3_F:
	case SHADER_PRARM_N_4_F:
	case SHADER_PRARM_3_3_F:
	case SHADER_PRARM_4_4_F:
		if( memcmp(this->m_pfValue,sp.m_pfValue,sp.GetDataSize()) == 0 )
		{
			return false;
		}
		break;
	case SHADER_PRARM_TEX_2D:
	case SHADER_PRARM_TEX_3D:
	case SHADER_PRARM_TEX_CUBE:
		{
			if( *(this->m_pnValue) == *sp.m_pnValue )
			{
				return false;
			}
		}
		break;
	default:
		{
			return false;
		};
	};


	return true;
}

//ת���ɿ��ӻ���������
core_string CShaderParam::ToString()
{
	core_string str;

	//���о�ȷ�Ƚ������Ƿ����
	switch(m_Type)
	{
	case SHADER_PRARM_1_F:
		{
		    char mbzTemp[32];
            Port_SafeSprintf(mbzTemp,sizeof(mbzTemp),"%.3f",*m_pfValue);
			str += mbzTemp;
		}
		break;

	case SHADER_PRARM_1_I:
		{
		    char mbzTemp[32];
            Port_SafeSprintf(mbzTemp,sizeof(mbzTemp),"%d",*m_pnValue);
			str += mbzTemp;
		}
		break;

	case SHADER_PRARM_2_F:
		{
		    char mbzTemp[128];
            FmVec2 value = (*m_pVec2);
            Port_SafeSprintf(mbzTemp,sizeof(mbzTemp),"%.3f , %.3f",value.x,value.y);
			str += mbzTemp;
		}
		break;

	case SHADER_PRARM_3_F:
		{
		    char mbzTemp[128];
            FmVec3 value = (*m_pVec3);
            Port_SafeSprintf(mbzTemp,sizeof(mbzTemp),"%.3f , %.3f , %.3f",value.x,value.y,value.z);
			str += mbzTemp;
		}
		break;

	case SHADER_PRARM_4_F:
		{
		    char mbzTemp[256];
            FmVec4 value = (*m_pVec4);
            Port_SafeSprintf(mbzTemp,sizeof(mbzTemp),"%.3f , %.3f , %.3f , %.3f",value.x,value.y,value.z,value.w);
			str += mbzTemp;
		}
		break;

	case SHADER_PRARM_2_I:
	case SHADER_PRARM_3_I:
	case SHADER_PRARM_4_I:
	case SHADER_PRARM_N_I:
		{
			TypePointer p(m_pData);
			char mbzTemp[32];
			size_t size = m_DataSize / sizeof(int);
			for(int i = 0;i < size ; i++ )
			{
				Port_SafeSprintf(mbzTemp,sizeof(mbzTemp),"%d , ",*p.n);
				str += mbzTemp;
				p.n++;
			}
		}
		break;
	
	case SHADER_PRARM_N_F:
	case SHADER_PRARM_N_2_F:
    case SHADER_PRARM_N_3_F:
	case SHADER_PRARM_N_4_F:
	case SHADER_PRARM_3_3_F:
	case SHADER_PRARM_4_4_F:
		{
			TypePointer p(m_pData);
			char mbzTemp[32];
			size_t size = m_DataSize / sizeof(int);
			for(int i = 0;i < size ; i++ )
			{
				Port_SafeSprintf(mbzTemp,sizeof(mbzTemp),"%.3f , ",*p.f);
				str += mbzTemp;
				p.f++;
			}
		}
		break;

	case SHADER_PRARM_TEX_2D:
	case SHADER_PRARM_TEX_3D:
	case SHADER_PRARM_TEX_CUBE:
		{
		    char mbzTemp[32];
            Port_SafeSprintf(mbzTemp,sizeof(mbzTemp),"%d",*m_pnValue);
			str += mbzTemp;
		}
		break;
	default:
		{
 
		};
	};

	return str;
}
 

CShaderParamOp::CShaderParamOp(fm_uint nProgram,Render* pRender):m_nProgram(nProgram),m_nSamplerUsed(0),m_pRender(pRender)
{
}

CShaderParamOp::~CShaderParamOp()
{
 
#define shaderparam_map 	TPodHashMap<fm_int, CShaderParam*, TPodTraits<fm_int>,TShaderParamAlloc>
	for(shaderparam_map::iterator iter = m_ShaderParams.Begin(); iter != m_ShaderParams.End(); iter++ )
	{
		iter.GetData()->~CShaderParam();
		m_pRender->Free(iter.GetData(),sizeof(CShaderParam));
	}   
#undef shaderparam_map
 
}

//���ݼĴ����������ֻ�ȡ����ֵ
fm_int CShaderParamOp::FindParamIdByName(const char* pstrName)
{	
	fm_int local = glGetUniformLocation(m_nProgram,pstrName);

	//���﷢����������Ĵ��������õ�ʱ�򷵻�-1
	if(::TestErr("FindParamIdByName..") )
	{
		CORE_TRACE(pstrName);
	}
    
	return local;
}
// bAuto  �Ƿ���Ҫ����ײ��Զ���������

bool CShaderParamOp::SetParamValue(fm_int Id,fm_float x,bool bAuto)
{
	if(!bAuto && !m_pRender->IsCaptureFrame())
	{
#ifdef PERFORMANCE_DEBUG
		m_pRender->GetPerformance()->nGpuUniformDataUsedNum++;
#endif
       glUniform1f(Id, x);
	   return true;
	}
	CShaderParam* sp = (CShaderParam*) m_pRender->Alloc(sizeof(CShaderParam));
	new (sp) CShaderParam(x,bAuto,m_pRender);
    return SetParamValue(Id,sp);
}

bool CShaderParamOp::SetParamValue(fm_int Id,fm_int x,bool bAuto)
{
	if(!bAuto && !m_pRender->IsCaptureFrame())
	{
#ifdef PERFORMANCE_DEBUG
		m_pRender->GetPerformance()->nGpuUniformDataUsedNum++;
#endif
        glUniform1i(Id, x);
		return true;
	}
	CShaderParam* sp = (CShaderParam*) m_pRender->Alloc(sizeof(CShaderParam));
	new (sp) CShaderParam(x,bAuto,m_pRender);
    return SetParamValue(Id,sp);
}

bool CShaderParamOp::SetParamValue(fm_int Id,fm_float x,fm_float y,bool bAuto)
{
	if(!bAuto && !m_pRender->IsCaptureFrame())
	{
#ifdef PERFORMANCE_DEBUG
		m_pRender->GetPerformance()->nGpuUniformDataUsedNum++;
#endif
       glUniform2f(Id, x,y);
	   return true;
	}
	CShaderParam* sp = (CShaderParam*) m_pRender->Alloc(sizeof(CShaderParam));
	new (sp) CShaderParam(x,y,bAuto,m_pRender);
    return SetParamValue(Id,sp);
}

bool CShaderParamOp::SetParamValue(fm_int Id,fm_int x,fm_int y,bool bAuto)
{
	if(!bAuto && !m_pRender->IsCaptureFrame())
	{
#ifdef PERFORMANCE_DEBUG
		m_pRender->GetPerformance()->nGpuUniformDataUsedNum++;
#endif
       glUniform2i(Id, x,y);
	   return true;
	}
	CShaderParam* sp = (CShaderParam*) m_pRender->Alloc(sizeof(CShaderParam));
	new (sp) CShaderParam(x,y,bAuto,m_pRender);
    return SetParamValue(Id,sp);
}

bool CShaderParamOp::SetParamValue(fm_int Id,fm_float x,fm_float y,fm_float z,bool bAuto)
{
	if(!bAuto && !m_pRender->IsCaptureFrame())
	{
#ifdef PERFORMANCE_DEBUG
		m_pRender->GetPerformance()->nGpuUniformDataUsedNum++;
#endif
       glUniform3f(Id, x,y,z);
	   return true;
	}
	CShaderParam* sp = (CShaderParam*) m_pRender->Alloc(sizeof(CShaderParam));
	new (sp) CShaderParam(x,y,z,bAuto,m_pRender);
    return SetParamValue(Id,sp);
}

bool CShaderParamOp::SetParamValue(fm_int Id,fm_int x,fm_int y,fm_int z,bool bAuto)
{
	if(!bAuto && !m_pRender->IsCaptureFrame())
	{
#ifdef PERFORMANCE_DEBUG
		m_pRender->GetPerformance()->nGpuUniformDataUsedNum++;
#endif
       glUniform3i(Id, x,y,z);
	   return true;
	}
	CShaderParam* sp = (CShaderParam*) m_pRender->Alloc(sizeof(CShaderParam));
	new (sp) CShaderParam(x,y,z,bAuto,m_pRender);
    return SetParamValue(Id,sp);
}

bool CShaderParamOp::SetParamValue(fm_int Id,fm_float x,fm_float y,fm_float z,fm_float w,bool bAuto)
{
	if(!bAuto && !m_pRender->IsCaptureFrame())
	{
#ifdef PERFORMANCE_DEBUG
		m_pRender->GetPerformance()->nGpuUniformDataUsedNum++;
#endif
       glUniform4f(Id, x,y,z,w);
	   return true;
	}
	CShaderParam* sp = (CShaderParam*) m_pRender->Alloc(sizeof(CShaderParam));
	new (sp) CShaderParam(x,y,z,w,bAuto,m_pRender);
    return SetParamValue(Id,sp);
}

bool CShaderParamOp::SetParamValue(fm_int Id,fm_int x,fm_int y,fm_int z,fm_int w,bool bAuto)
{
	if(!bAuto && !m_pRender->IsCaptureFrame())
	{
#ifdef PERFORMANCE_DEBUG
		m_pRender->GetPerformance()->nGpuUniformDataUsedNum++;
#endif
       glUniform4i(Id, x,y,z,w);
	   return true;
	}
	CShaderParam* sp = (CShaderParam*) m_pRender->Alloc(sizeof(CShaderParam));
	new (sp) CShaderParam(x,y,z,w,bAuto,m_pRender);
    return SetParamValue(Id,sp);
}

bool CShaderParamOp::SetParamValue(fm_int Id,fm_sizei count,const fm_float* v,bool bAuto)
{
	if(!bAuto && !m_pRender->IsCaptureFrame())
	{
#ifdef PERFORMANCE_DEBUG
		m_pRender->GetPerformance()->nGpuUniformDataUsedNum++;
#endif
		glUniform1fv(Id,count,v);
		return true;
	}
	CShaderParam* sp = (CShaderParam*) m_pRender->Alloc(sizeof(CShaderParam));
	new (sp) CShaderParam(count,v,bAuto,m_pRender);
	return SetParamValue(Id,sp);
}

bool CShaderParamOp::SetParamValue(fm_int Id,fm_sizei count,const fm_int* v,bool bAuto)
{
	if(!bAuto && !m_pRender->IsCaptureFrame())
	{
#ifdef PERFORMANCE_DEBUG
		m_pRender->GetPerformance()->nGpuUniformDataUsedNum++;
#endif
		glUniform1iv(Id,count,v);
		return true;
	}
	CShaderParam* sp = (CShaderParam*) m_pRender->Alloc(sizeof(CShaderParam));
	new (sp) CShaderParam(count,v,bAuto,m_pRender);
	return SetParamValue(Id,sp);
}

// ��չ�����ϵ���ѧ����а�װ��
bool CShaderParamOp::SetParamValue(fm_int Id,FmVec2& value,bool bAuto)
{
	if(!bAuto && !m_pRender->IsCaptureFrame())
	{
#ifdef PERFORMANCE_DEBUG
		m_pRender->GetPerformance()->nGpuUniformDataUsedNum++;
#endif
		glUniform2fv(Id,1,(float*)&value);
		return true;
	}
	CShaderParam* sp = (CShaderParam*) m_pRender->Alloc(sizeof(CShaderParam));
	new (sp) CShaderParam(value,bAuto,m_pRender);
	return SetParamValue(Id,sp);
}

bool CShaderParamOp::SetParamValue(fm_int Id,FmVec3& value,bool bAuto)
{
	if(!bAuto && !m_pRender->IsCaptureFrame())
	{
#ifdef PERFORMANCE_DEBUG
		m_pRender->GetPerformance()->nGpuUniformDataUsedNum++;
#endif
		glUniform3fv(Id,1,(float*)&value);
		return true;
	}
	CShaderParam* sp = (CShaderParam*) m_pRender->Alloc(sizeof(CShaderParam));
	new (sp) CShaderParam(value,bAuto,m_pRender);
	return SetParamValue(Id,sp);
}

bool CShaderParamOp::SetParamValue(fm_int Id,FmVec4& value,bool bAuto)
{
	if(!bAuto && !m_pRender->IsCaptureFrame())
	{
#ifdef PERFORMANCE_DEBUG
		m_pRender->GetPerformance()->nGpuUniformDataUsedNum++;
#endif
		glUniform4fv(Id,1,(float*)&value);
		return true;
	}
	CShaderParam* sp = (CShaderParam*) m_pRender->Alloc(sizeof(CShaderParam));
	new (sp) CShaderParam(value,bAuto,m_pRender);
	return SetParamValue(Id,sp);
}

bool CShaderParamOp::SetParamValue(fm_int Id,FmMat3& value,bool bAuto)
{
	if(!bAuto && !m_pRender->IsCaptureFrame())
	{
#ifdef PERFORMANCE_DEBUG
		m_pRender->GetPerformance()->nGpuUniformDataUsedNum++;
#endif
		glUniformMatrix3fv(Id,1,GL_FALSE,(float*)&value);
		return true;
	}
	CShaderParam* sp = (CShaderParam*) m_pRender->Alloc(sizeof(CShaderParam));
	new (sp) CShaderParam(value,bAuto,m_pRender);
	return SetParamValue(Id,sp);
}

bool CShaderParamOp::SetParamValue(fm_int Id,FmMat4& value,bool bAuto)
{
	if(!bAuto && !m_pRender->IsCaptureFrame())
	{
#ifdef PERFORMANCE_DEBUG
		m_pRender->GetPerformance()->nGpuUniformDataUsedNum++;
#endif
		glUniformMatrix4fv(Id,1,GL_FALSE,(float*)&value);
		return true;
	}
	CShaderParam* sp = (CShaderParam*) m_pRender->Alloc(sizeof(CShaderParam));
	new (sp) CShaderParam(value,bAuto,m_pRender);
	return SetParamValue(Id,sp);
}

bool CShaderParamOp::SetParamValue(fm_int Id,fm_sizei count,const FmVec2* value,bool bAuto)
{
	if(!bAuto && !m_pRender->IsCaptureFrame())
	{
#ifdef PERFORMANCE_DEBUG
		m_pRender->GetPerformance()->nGpuUniformDataUsedNum++;
#endif
		glUniform2fv(Id,count,(float*)value);
		return true;
	}
	CShaderParam* sp = (CShaderParam*) m_pRender->Alloc(sizeof(CShaderParam));
	new (sp) CShaderParam(count,value,bAuto,m_pRender);
	return SetParamValue(Id,sp);
}

bool CShaderParamOp::SetParamValue(fm_int Id,fm_sizei count,const FmVec3* value,bool bAuto)
{
	if(!bAuto && !m_pRender->IsCaptureFrame())
	{
#ifdef PERFORMANCE_DEBUG
		m_pRender->GetPerformance()->nGpuUniformDataUsedNum++;
#endif
		glUniform3fv(Id,count,(float*)value);
		return true;
	}
	CShaderParam* sp = (CShaderParam*) m_pRender->Alloc(sizeof(CShaderParam));
	new (sp) CShaderParam(count,value,bAuto,m_pRender);
	return SetParamValue(Id,sp);
}
 
bool CShaderParamOp::SetParamValue(fm_int Id,fm_sizei count,const FmVec4* value,bool bAuto)
{
	if(!bAuto && !m_pRender->IsCaptureFrame())
	{
#ifdef PERFORMANCE_DEBUG
		m_pRender->GetPerformance()->nGpuUniformDataUsedNum++;
#endif
		glUniform4fv(Id,count,(float*)value);
		return true;
	}
	CShaderParam* sp = (CShaderParam*) m_pRender->Alloc(sizeof(CShaderParam));
	new (sp) CShaderParam(count,value,bAuto,m_pRender);
	return SetParamValue(Id,sp);
}


// �������� ����������������Ƿ���Ҫ�Զ�������Ҫ�浵���ݽ��в�ѯ
// ����ID         ����ID
bool CShaderParamOp::SetTexture2D(fm_int Id,fm_int TexID)
{
	CShaderParam* sp = (CShaderParam*) m_pRender->Alloc(sizeof(CShaderParam));
	new (sp) CShaderParam(SHADER_PRARM_TEX_2D,TexID,true,m_pRender);
	return SetParamTexSampler(Id,sp);
}

bool CShaderParamOp::SetTexture3D(fm_int Id,fm_int TexID)
{
	CShaderParam* sp = (CShaderParam*) m_pRender->Alloc(sizeof(CShaderParam));
	new (sp) CShaderParam(SHADER_PRARM_TEX_3D,TexID,true,m_pRender);
	return SetParamTexSampler(Id,sp);;
}

bool CShaderParamOp::SetTextureCube(fm_int Id,fm_int TexID)
{
	CShaderParam* sp = (CShaderParam*) m_pRender->Alloc(sizeof(CShaderParam));
	new (sp) CShaderParam(SHADER_PRARM_TEX_CUBE,TexID,true,m_pRender);
	return SetParamTexSampler(Id,sp);
}

//�ڲ�
bool CShaderParamOp::SetParamValue(fm_int Id,CShaderParam* sp)
{
    if(Id == -1)
	{
        sp->~CShaderParam();
		//��ɾ���ϵ�����
		m_pRender->Free(sp,sizeof(CShaderParam));
		return false;
	}


#define shaderparam_map 	TPodHashMap<fm_int, CShaderParam*, TPodTraits<fm_int>,TShaderParamAlloc>
	//����Ѿ�������Ҫ���ҳ���
	shaderparam_map::iterator iter = m_ShaderParams.Find(Id);
	//iter.GetData().IsAuto()
#undef shaderparam_map

#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nGpuUniformDataSumNum++;
#endif
   
	// ����Ҳ����Ļ����д���
	if( iter == m_ShaderParams.End() )
	{
#ifdef PERFORMANCE_DEBUG
		m_pRender->GetPerformance()->nGpuUniformDataUsedNum++;
#endif
		m_ShaderParams.Add(Id,sp);
		//ֱ������GPU����
		UpGPUParamValue(Id,sp);
		return true;
	}

	if( *iter.GetData() != *sp )
	{
#ifdef PERFORMANCE_DEBUG
		m_pRender->GetPerformance()->nGpuUniformDataUsedNum++;
#endif
		UpGPUParamValue(Id,sp);
		iter.GetData()->~CShaderParam();
		//��ɾ���ϵ�����
		m_pRender->Free(iter.GetData(),sizeof(CShaderParam));
    	iter.GetData() = sp;
		return true;
	}
	sp->~CShaderParam();
	m_pRender->Free(sp,sizeof(CShaderParam));
	return true;
}

bool CShaderParamOp::SetParamTexSampler(fm_int Id,CShaderParam* sp)
{
    if(Id == -1)
	{
        sp->~CShaderParam();
		//��ɾ���ϵ�����
		m_pRender->Free(sp,sizeof(CShaderParam));
		return false;
	}

	CTextureSamplerBindOp* pTexSamperBindOp = m_pRender->GetTextureSamplerBindOp();

#define shaderparam_map 	TPodHashMap<fm_int, CShaderParam*, TPodTraits<fm_int>,TShaderParamAlloc>
	//����Ѿ�������Ҫ���ҳ���
	shaderparam_map::iterator iter = m_ShaderParams.Find(Id);
	//iter.GetData().IsAuto()
#undef shaderparam_map

#ifdef PERFORMANCE_DEBUG
	m_pRender->GetPerformance()->nGpuUniformDataSumNum++;   
#endif
	// ����Ҳ����Ļ����д���
	if( iter == m_ShaderParams.End() )
	{
#ifdef PERFORMANCE_DEBUG
		m_pRender->GetPerformance()->nGpuUniformDataUsedNum++;
#endif
		sp->BindSamplerID(m_nSamplerUsed);//���ﱣ��������󶨺�
		m_ShaderParams.Add(Id,sp);
		//ֱ������GPU����
		glUniform1i(Id,m_nSamplerUsed);//�����������ID

		glActiveTexture(GL_TEXTURE0+m_nSamplerUsed);//����һ�����������
		pTexSamperBindOp->SetActiveSamplerID(m_nSamplerUsed);//��¼��ǰ���������
		pTexSamperBindOp->SetTextureIDBindSamplerID(m_nSamplerUsed,*(sp->m_pnValue));
		m_nSamplerUsed++;
		UpGPUParamValue(Id,sp);
		return true;
	}


	int samplerid = iter.GetData()->GetBindSamplerID();
	
	/*
	//���ﱾ����������һ���Ż�������������
 	if(pTexSamperBindOp->EqualTextureIDBindSamplerID(samplerid,*(sp->m_pnValue)))
 	{
 		return true;
 	}
	*/

	pTexSamperBindOp->SetTextureIDBindSamplerID(samplerid,*(sp->m_pnValue));
	if(!pTexSamperBindOp->EqualActiveSamplerID(samplerid))
	{
		glActiveTexture(GL_TEXTURE0+samplerid);
		pTexSamperBindOp->SetActiveSamplerID(samplerid);
	}

	// �ж��Ƿ����������������Ҫ�ϴ�����
	// ��������������Ƿ���Ҫ��ѯ�ϴ�״̬����Ҫÿ������Ŷ���������
//	if( *iter.GetData() != *sp )
	{
#ifdef PERFORMANCE_DEBUG
    	m_pRender->GetPerformance()->nGpuUniformDataUsedNum++;
#endif
		UpGPUParamValue(Id,sp);
        sp->~CShaderParam();
		//��ɾ���ϵ�����
		m_pRender->Free(sp,sizeof(CShaderParam));
//	    iter.GetData() = sp;
	}
	return true;
}

//ֱ�������ϴ�����
void CShaderParamOp::UpGPUParamValue(fm_int Id,CShaderParam* sp)
{
	switch(sp->GetShaderParamType())
	{
	case SHADER_PRARM_1_F:
		glUniform1f(Id, *(sp->m_pfValue));
		break;
	case SHADER_PRARM_1_I:
		glUniform1i(Id, *(sp->m_pnValue));
		break;
	case SHADER_PRARM_2_F:
		glUniform2fv(Id,1,(fm_float*)(sp->m_pVec2));
        break;
	case SHADER_PRARM_3_F:
		glUniform3fv(Id,1,(fm_float*)(sp->m_pVec3));
		break;
	case SHADER_PRARM_4_F:
		glUniform4fv(Id,1,(fm_float*)(sp->m_pVec4));
		break;
	case SHADER_PRARM_2_I:
        glUniform2iv(Id,1,(fm_int*)(sp->m_pnValue));
	case SHADER_PRARM_3_I:
        glUniform3iv(Id,1,(fm_int*)(sp->m_pnValue));
	case SHADER_PRARM_4_I:
        glUniform4iv(Id,1,(fm_int*)(sp->m_pnValue));
	case SHADER_PRARM_N_I:
		glUniform1iv(Id,(fm_sizei)sp->GetDataSize()/sizeof(fm_int),(fm_int*)(sp->m_pnValue));
		break;
	case SHADER_PRARM_N_F:
		glUniform1fv(Id,(fm_sizei)sp->GetDataSize()/sizeof(fm_float),(fm_float*)(sp->m_pfValue));
		break;
	case SHADER_PRARM_3_3_F:
		glUniformMatrix3fv(Id,1,GL_FALSE,(fm_float*)(sp->m_pMat3));
		break;
	case SHADER_PRARM_4_4_F:
		glUniformMatrix4fv(Id,1,GL_FALSE,(fm_float*)(sp->m_pMat4));
		break;
	case SHADER_PRARM_N_2_F:
        glUniform2fv(Id,(fm_sizei)sp->GetDataSize()/sizeof(FmVec2),(fm_float*)(sp->m_pfValue));
		break;
	case SHADER_PRARM_N_3_F:
        glUniform3fv(Id,(fm_sizei)sp->GetDataSize()/sizeof(FmVec3),(fm_float*)(sp->m_pfValue));
		break;
	case SHADER_PRARM_N_4_F:
        glUniform4fv(Id,(fm_sizei)sp->GetDataSize()/sizeof(FmVec4),(fm_float*)(sp->m_pfValue));
		break;
	case SHADER_PRARM_TEX_2D:
		{
			glBindTexture(GL_TEXTURE_2D,*(sp->m_pnValue));
		}
		break;
	case SHADER_PRARM_TEX_3D:
		{
#ifdef GL_TEXTURE_3D
           glBindTexture(GL_TEXTURE_3D,*(sp->m_pnValue));//Why not 
#endif
		}
		break;
	case SHADER_PRARM_TEX_CUBE:
		{
           glBindTexture(GL_TEXTURE_CUBE_MAP,*(sp->m_pnValue));
		}
		break;
    default:
        CORE_TRACE("CShaderParamOp UpGPUParamValue Err!");
	};

}

// ���Է����ӡ����
void CShaderParamOp::Print()
{

	// VS�������ݸ�ʽ
	fm_int numAttributes = 0;
	fm_int maxAttributeLen = 0;
	glGetProgramiv(m_nProgram,GL_ACTIVE_ATTRIBUTES,&numAttributes);
	glGetProgramiv(m_nProgram,GL_ACTIVE_ATTRIBUTE_MAX_LENGTH,&maxAttributeLen);


	CORE_TRACE(" + Attribute Name:");

	char* nattributeName = new char[maxAttributeLen];
	for(fm_int index = 0; index < numAttributes; index++ )
	{
		fm_int attribNameLen = 0;
		fm_enum type = 0;
		glGetActiveAttrib(m_nProgram,index, maxAttributeLen , NULL,&attribNameLen, &type ,nattributeName);

		fm_int location = glGetAttribLocation( m_nProgram , nattributeName );

		CORE_TRACE_EX("  %s :location:%d type:%s",nattributeName,index,GLTypeToGLTypeString(type));

	}

	delete [] nattributeName;


	fm_int maxUniformLen = 0;
	fm_int numUniforms = 0;
	
	glGetProgramiv(m_nProgram, GL_ACTIVE_UNIFORMS, &numUniforms );
	glGetProgramiv(m_nProgram, GL_ACTIVE_UNIFORM_MAX_LENGTH,&maxUniformLen );

	CORE_TRACE(" + Uniform Name:");

	char*  nuiformName = new char[maxUniformLen];

	for( fm_int index = 0; index <  numUniforms ; index++ )
	{
		fm_int size = 0;
		fm_enum type = 0;

		glGetActiveUniform(m_nProgram, index , maxUniformLen , NULL,&size, &type, nuiformName );
		fm_int location = glGetUniformLocation(m_nProgram,nuiformName);
        CORE_TRACE_EX("   %s :location:%d type:%s",nuiformName,index,GLTypeToGLTypeString(type));
		CORE_TRACE_EX("   %s",UniformValueToString(index).c_str());
	}

	delete [] nuiformName;

}


//��־����
struct GL_TYPE_Info
{
   fm_enum      gl_type;
   const char*	gl_type_string;//��־����
};

GL_TYPE_Info g_type_info_table[] = 
{
	//gl_type      gl_type_string 
	{ GL_FLOAT,        "GL_FLOAT"      },
	{ GL_FLOAT_VEC2,   "GL_FLOAT_VEC2" },
	{ GL_FLOAT_VEC3,   "GL_FLOAT_VEC3" },
	{ GL_FLOAT_VEC4,   "GL_FLOAT_VEC4" },
	{ GL_INT,          "GL_INT"        },
	{ GL_INT_VEC2,     "GL_INT_VEC2"   },
	{ GL_INT_VEC3,     "GL_INT_VEC3"   },
	{ GL_INT_VEC4,     "GL_INT_VEC4"   },
	{ GL_BOOL,         "GL_BOOL"       },
	{ GL_BOOL_VEC2,    "GL_BOOL_VEC2"  },
	{ GL_BOOL_VEC3,    "GL_BOOL_VEC3"  },
	{ GL_BOOL_VEC4,    "GL_BOOL_VEC4"  },
	{ GL_FLOAT_MAT2,   "GL_FLOAT_MAT2" },
	{ GL_FLOAT_MAT3,   "GL_FLOAT_MAT3" },
	{ GL_FLOAT_MAT4,   "GL_FLOAT_MAT4" },
	{ GL_SAMPLER_2D,   "GL_SAMPLER_2D" },
	{ GL_SAMPLER_CUBE, "GL_SAMPLER_CUBE"},
};

// ����GL_Type��ȡ�������ַ���
const char* CShaderParamOp::GLTypeToGLTypeString(fm_enum type) const
{
	size_t nMaxTable = sizeof(g_type_info_table) / sizeof(GL_TYPE_Info);

	for(int i = 0; i < nMaxTable; i++ )
	{
		if( g_type_info_table[i].gl_type == type )
			return g_type_info_table[i].gl_type_string;
	}

	return "unknow gl type";
}

core_string CShaderParamOp::UniformValueToString(fm_int location )
{
	core_string str;
#define shaderparam_map 	TPodHashMap<fm_int, CShaderParam*, TPodTraits<fm_int>,TShaderParamAlloc>
	//����Ѿ�������Ҫ���ҳ���
	shaderparam_map::iterator iter = m_ShaderParams.Find(location);

#undef shaderparam_map

	if( iter != m_ShaderParams.End() )
	{
		str = iter.GetData()->ToString();
	}

	return str;
}