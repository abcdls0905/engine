//--------------------------------------------------------------------
// �ļ���:		model_player.h
// ��  ��:		
// ˵  ��:		
// ��������:	2009��5��4��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _MODEL_PLAYER_H
#define _MODEL_PLAYER_H

#include "../public/macros.h"
#include "../public/fast_str.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"

//#include "../visual/dx_scene_batch.h"
#include "../visual/i_model_player.h"
#include "../visual/i_model_system.h"
#include "../utils/array_pod.h"
#include "model_data.h"
#include "../visual/i_context.h"
#include "model_vf.h"
#include "res_model.h"
#include <assert.h>
#include "../visual/i_render.h"
#include "math_3d.h"

enum
{
	c_mtxWorld,
	c_mtxWVP,
	c_fBumpScale,
	c_fReflectScale,
	c_fSaturationScale,
	c_fGlowAlpha,
	c_fBlendWeight,
	c_fAlphaRefValue,
	c_fTime,
	c_fGlowSize,
	c_BoneList,
	c_mtxDiffuseTM,
	c_MaterialEmissive,
	c_MaterialSpecular,
	c_fSpecularPower,
	c_MaterialDiffuse,
	c_AppendColor,
	c_MaterialAmbient,
	c_MaterialAmbientEx,
	c_vLightDir,
	c_mtxViewInverse,
	c_LightAmbient,
	c_vViewPos,
	c_vReferPos,
	c_ClipPlane,
	c_LightDiffuse,
	c_FogColor,
	c_FogParam,
	c_FogExpParam,
	c_FallOffParam,
	c_SubSurfaceParam,
	c_LightMapParam,
	c_vLightMapColorScale,
	c_PointLightDiffuse,
	c_PointLightPosition,
	c_FilterParam,
	c_FilterMatrix,
	c_mtxShadowViewProj,
	c_HeightFogColor,
	c_HeightFogParam,
	tex_Diffuse,
	tex_Bump,
	tex_SpecularMap,
	tex_EmissiveMap,
	tex_ReflectionMap,
	tex_LightMap,
	tex_FilterMap,
	tex_KeepoutMap,
	tex_Shadow,
	c_max,
};

struct ShaderKey
{
	void * vs;
	void* ps;
};

extern const char* const_value_name[c_max];

#include "../utils/pod_hash_map.h" 

template<>
class TPodTraits<ShaderKey>
{
public:
	static size_t GetHash(const ShaderKey& value)
	{
		return size_t(value.ps) + size_t(value.vs);
	}

	static bool Equal(const ShaderKey& s1, const ShaderKey& s2)
	{
		return s1.ps == s2.ps && s1.vs == s2.vs;
	}
};

extern IRender* g_pRender;
class TModelAlloc
{
public:
	TModelAlloc() {}
	~TModelAlloc() {}

	// �����ڴ�
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// ����
	void Swap(TModelAlloc& src) {}
};

class ShaderManager
{
	struct Item
	{
		IShaderProgram* pShader;
		int* pConstValues;
		int MaxConstValue;
	};
	
	TPodHashMap<ShaderKey, Item, TPodTraits<ShaderKey>, TModelAlloc> m_ShaderPrograms;
	static ShaderManager* s_inst;
	Item m_current;
	IShaderParamOp* m_currentShader;
public:
	ShaderManager()
	{
		m_current.pShader = NULL;
		m_current.pConstValues = NULL;
		m_current.MaxConstValue = 0;
	}

	void Destroy()
	{
		if(s_inst == 0)
			return;
		for(TPodHashMap<ShaderKey, Item, TPodTraits<ShaderKey>, TModelAlloc> ::iterator it = m_ShaderPrograms.Begin(); it!= m_ShaderPrograms.End(); it++)
		{
			if(it.GetData().pConstValues)
			{
				CORE_FREE(it.GetData().pConstValues, it.GetData().MaxConstValue*sizeof(int));
			}
		}
		if(s_inst)
		{
			delete(s_inst);
			s_inst = 0;
		}
	}

	IShaderProgram* GetShader(size_t nVS,size_t nPS,IVertexShader* pVS, IPixelShader* pPS, node_material_t* pMat, model_node_t* pNode,  const char** const_value_list = 0, int const_value_count = 0)
	{
		//����Ľ���߲�ѯ�ٶ�
		ShaderKey key;
		key.vs = pVS;
		key.ps = pPS;
	
		if(m_ShaderPrograms.Exists(key))
		{
			m_current = m_ShaderPrograms.Find(key).GetData();
			m_currentShader = m_current.pShader->GetParamOp();
			return m_current.pShader;
		}


		const char* ptr[ModelVF::MAX];
		int len;
		pMat->vf.GetNames(ptr, len);
	
		return GetShader(nVS,nPS,pVS, pPS, ptr, len, key, const_value_list, const_value_count);
	}

	IShaderProgram* GetShader(size_t nVS,size_t nPS,IVertexShader* pVS, IPixelShader* pPS, const char** name, int len, ShaderKey& key, const char** const_value_list = 0, int const_value_count = 0)
	{
		if(key.ps  == 0 && key.vs == 0)
		{
			key.ps = pPS;
			key.vs = pVS;
		}

		if(m_ShaderPrograms.Exists(key))
		{
			m_current = m_ShaderPrograms.Find(key).GetData();
			m_currentShader = m_current.pShader->GetParamOp();
			return m_current.pShader;
		}

		IShaderProgram* pShader = g_pRender->CreateShaderProgram(pVS, pPS, name, len);
		Item item;
		item.pShader = pShader;
		item.pConstValues = 0;
		item.MaxConstValue = 0;
		if(const_value_list && const_value_count)
		{
			item.pConstValues = (int*) CORE_ALLOC(const_value_count*sizeof(fm_uint));
			item.MaxConstValue = const_value_count;
			for(int i = 0; i < item.MaxConstValue; i++)
			{
				item.pConstValues[i] = item.pShader->GetParamOp()->FindParamIdByName(const_value_list[i]);
			}
		}
		//TestErr("ShaderManager::GetShader");
		m_ShaderPrograms.Add(key, item);
		m_current = item;
		m_currentShader = m_current.pShader->GetParamOp();
		return pShader;
	}

	fm_uint GetConstValueId(int index)
	{
		return m_current.pConstValues[index];
	}

	static ShaderManager& Inst()
	{
		if(s_inst == NULL)
		{
			s_inst = new ShaderManager;
		}
		return *s_inst;
	}

	//��Shader���ñ���
	inline void SetShaderValue1f(int index, float value)
	{
		m_currentShader->SetParamValue(m_current.pConstValues[index], value);
	}

	inline void SetShaderValue1i(int index, int value)
	{
		m_currentShader->SetParamValue(m_current.pConstValues[index], value);
	}
	
	inline void SetShaderValue4fv(int index, int count, float* value)
	{
		m_currentShader->SetParamValue(m_current.pConstValues[index], count, (FmVec4*)value);
	}

	inline void SetShaderValue4f(int index, float v1, float v2, float v3, float v4)
	{
		m_currentShader->SetParamValue(m_current.pConstValues[index], v1, v2, v3, v4);
	}

	inline void SetShaderValue4f(int index, const FmVec4& value)
	{
		SetShaderValue4f(index, value.x, value.y, value.z, value.w);
	}

	inline void SetShaderValue4f(const char* name, const FmVec4& value)
	{
		fm_int id = m_currentShader->FindParamIdByName(name);
		m_currentShader->SetParamValue(id, value.x, value.y, value.z, value.w);
	}

	inline void SetShaderValue3f(int index, float v1, float v2, float v3)
	{
		m_currentShader->SetParamValue(m_current.pConstValues[index], v1, v2, v3);
	}

	inline void SetShaderValue3f(int index, const FmVec3& value)
	{
		SetShaderValue3f(index, value.x, value.y, value.z);
	}

	inline void SetShaderValueMat4(int index, const FmMat4& value)
	{
		m_currentShader->SetParamValue(m_current.pConstValues[index], (FmMat4&)value);
	}

	inline void SetTexture2D(int index, fm_int TexID)
	{
		if(m_current.pConstValues[index] < 0)
		{
			return;
		}
		m_currentShader->SetTexture2D(m_current.pConstValues[index], TexID);
	}
	
	inline void SetTexture3D(int index, fm_int TexID)
	{
		if(m_current.pConstValues[index] < 0)
		{
			return;
		}
		m_currentShader->SetTexture3D(m_current.pConstValues[index], TexID);
	}

	inline void SetTextureCube(int index, fm_int TexID)
	{
		if(m_current.pConstValues[index] < 0)
		{
			return;
		}
		m_currentShader->SetTextureCube(m_current.pConstValues[index], TexID);
	}
};

class TModelPlayerAlloc
{
public:
	TModelPlayerAlloc() {}
	~TModelPlayerAlloc() {}

	// �����ڴ�
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// ����
	void Swap(TModelPlayerAlloc& src) {}
};

class ShaderUserConstValue
{
	struct Value4f
	{
		char name[256];
		int id;
		FmVec4 value;
	};

	TArrayPod<Value4f, 1> m_values4f;
public:
	inline void SetValue(const char* name, const FmVec4& value)
	{
		for(size_t i = 0; i < m_values4f.size(); i++)
		{
			if(strcmp(m_values4f[i].name, name) == 0)
			{
				m_values4f[i].value = value;
				return;
			}
		}
		Value4f v;
		strcpy(v.name, name);
		v.value = value;
		m_values4f.push_back(v);
	}

	inline void UploadGPU()
	{
		for(size_t i = 0; i < m_values4f.size(); i++)
		{
			ShaderManager::Inst().SetShaderValue4f(m_values4f[i].name, m_values4f[i].value);
		}
	}
};

// ģ�Ͳ���

class ModelSystem;
class IRender;
class ITexture;
class IIniFile;
class CResModel;
class CActionPlayer;
class CMaterialLoader;

class CModelPlayer;
struct MatInfo
{
public:
	MatInfo()
		:pMat(0)
		,pNode(0)
		,bone_matrices(0)
		,pThis(0)
		,nColor(0xffffffff)
	{}

	node_material_t* pMat;
	model_node_t* pNode; 
	const FmVec4* bone_matrices;
	CModelPlayer* pThis;
	FmMat4 mtxCurrentTM;
	camera_t camera;
	FmVec3 refer_pos;
	material_info_t* pMatInfo;
	unsigned int nColor;
};

class MatInfoPool
{
	TArrayPod<MatInfo*, 1, TModelPlayerAlloc> m_items;
	int m_index;
public:
	MatInfoPool()
		:m_index(0)
	{}

	~MatInfoPool()
	{
		for(size_t i = 0; i < m_items.size(); i++)
		{
			delete(m_items[i]);
		}
	}

	MatInfo* Alloc()
	{
		if(m_index < (int)m_items.size())
		{
			return m_items[m_index++];
		}

		MatInfo * p = new MatInfo;
		m_items.push_back(p);
		m_index++;
		return p;
	}

	void RemoveOne()
	{
		Assert(m_index >0);
		m_index --;
	}
};

class CModelPlayer: public IModelPlayer
{
public:
	MatInfoPool m_MatInfoPools;
private:

	MatInfo* m_MatList;
	//MatInfo* m_MatListReflect;
	//MatInfo* m_MatListDynamicShadow;
	ShaderUserConstValue m_ShaderUserConstValue;

	//��SHADER���ó���
	void SetShaderConstValue(IShaderProgram* pShader, const MatInfo* info);
	static void DrawMaterialSolidBatch(void* pdata);
	static void DrawMaterialSolidColorPassBatch(void* pdata);
	static void DrawMaterialSolidOnlyZPassBatch(void* pdata);
	static void DrawMaterialBlendBatch(void* pdata);
	static void DrawMaterialAlphaTestBatch(void* pdata);
	static void DrawMaterialEarlyZBatch(void* pdata);
	static void DrawMaterialKeepOutBatch(void* pdata);
	static void DrawMaterialShadowMapBatch(void* pdata);

	void AddDrawMaterialSolidBatch(node_material_t* pMat, model_node_t* pNode, const FmVec4* bone_matrices, MatInfo& info);
	void AddDrawMaterialAlphaTestBatch(node_material_t* pMat, model_node_t* pNode, const FmVec4* bone_matrices, MatInfo& info);
	void AddDrawMaterialBlendBatch(node_material_t* pMat, model_node_t* pNode, const FmVec4* bone_matrices, MatInfo& info);
	void AddDrawMaterialKeepOut(node_material_t* pMat, model_node_t* pNode, const FmVec4* bone_matrices, MatInfo& info);
	void AddDrawMaterialShadowMapBatch(node_material_t* pMat, material_info_t* pMatInfo, model_node_t* pNode);
	void SetAniVertexData(int offset, int stride, node_material_t* pMat, int vf1, int vf2);

	inline void NodeRayTraceFunc_DoChild(model_t* pModel, model_node_t* pNode, const FmVec3* start, const FmVec3* direction, const FmMat4& mtxOld, trace_info_t* result)
	{
		for (unsigned int i = 0; i < pNode->nChildNodeCount; ++i)
		{
			model_node_t* child = &pNode->ChildNodes[i];

			Assert(child != NULL);

			if (child->nType == FXNODE_TYPE_MESH)
			{
				NodeRayTrace(pModel, child, start, direction, result);
			}
		}

		m_mtxCurrentTM = mtxOld;
	}

	inline void NodeSphereTrace_DoChild(model_t* pModel, model_node_t* pNode, const FmVec3* position, const FmMat4& mtxOld, float radius, bool& result)
	{
		for (unsigned int i = 0; i < pNode->nChildNodeCount; ++i)
		{
			model_node_t* child = &pNode->ChildNodes[i];

			Assert(child != NULL);

			if (NodeSphereTrace(pModel, child, position, radius))
			{
				result = true;
				break;
			}
		}

		m_mtxCurrentTM = mtxOld;
	}

    inline void NodeTriangleTrace_DoChild(model_t* pModel, model_node_t* pNode, const FmMat4& mtxOld, 
        const FmVec3* v0, const FmVec3* v1, const FmVec3* v2, bool& result)
    {
        for (unsigned int i = 0; i < pNode->nChildNodeCount; ++i)
        {
            model_node_t* child = &pNode->ChildNodes[i];

            Assert(child != NULL);

            if (NodeTriangleTrace(pModel, child, v0, v1, v2))
            {
                result = true;
                break;
            }
        }

        m_mtxCurrentTM = mtxOld;
    }

	inline void NodeCull_DoChild(model_t* pModel, model_node_t* pNode, 
		const FmPlane* planesTemp, unsigned int plane_num, 
		const FmVec3* center, float radius, void* cull_func, 
		void* cull_context, const FmMat4* cull_matrix, const FmMat4& mtxOld)
		
	{
		for (unsigned int i = 0; i < pNode->nChildNodeCount; ++i)
		{
			model_node_t* child = &pNode->ChildNodes[i];

			Assert(child != NULL);

			NodeCull(pModel, child, planesTemp, plane_num, center, 
				radius, cull_func, cull_context, cull_matrix);
		}

		m_mtxCurrentTM = mtxOld;
	}

	inline void DrawNodeNormal_DoChild(model_t* pModel, model_node_t* pNode, const FmMat4& mtxOld)
	{
		// Child node
		for (unsigned int i = 0; i < pNode->nChildNodeCount; ++i)
		{
			model_node_t* child = &pNode->ChildNodes[i];

			Assert(child != NULL);

			if (child->nType == FXNODE_TYPE_MESH)
			{
				DrawNodeNormal(pModel, child);
			}
		}
		m_mtxCurrentTM = mtxOld;
	}

	inline void UseLightMap(node_material_t* pMat, bool & dynamic_lighting, ITexture* &pTexture, float * light_map_param)
	{
		pTexture = 0;
		// ����ͼ
		bool use_light_map = GetUseLightMap();

		if (use_light_map && g_pRender->GetUseLightMap())
		{
			int iCurrentMatID = pMat->nIndexInModel;
			ITexture* pTex = NULL;
			//float light_map_param[4] = { 1.0F, 1.0F, 0.0F, 0.0F };

			if ((size_t)iCurrentMatID < GetLightMapCount())
			{
				ITexture* pDxTex = GetLightMapTex(iCurrentMatID);
				if (pDxTex)
				{
					pTex = pDxTex;
				}

				if ((size_t)iCurrentMatID < GetAtlasLightMapCount())
				{
					atlas_light_map_t* pAtlas =
						GetAtlasLightMap(iCurrentMatID);

					if (pAtlas)
					{
						light_map_param[0] = pAtlas->fScaleU;
						light_map_param[1] = pAtlas->fScaleV;
						light_map_param[2] = pAtlas->fOffsetU;
						light_map_param[3] = pAtlas->fOffsetV;
					}
				}
			}

			
			if ((NULL == pTex) && (pMat->MatInfo.LightMap.pTex != NULL))
			{
				pTex = pMat->MatInfo.LightMap.pTex;
			}

			if (pTex && pTex->GetShaderTex() && pTex->IsLoadComplete())
			{
				//ShaderManager::Inst().SetShaderValue4f(c_LightMapParam, light_map_param);
				dynamic_lighting = false;
				
				//if(isready)
				//	pMat->MatInfo.LightMap.pTex = pTex;
				/*
				pBatch->SetTexture(2, pTex);
				pBatch->SetVertexShaderConstantF(CV_LIGHT_MAP_PARAM, 
					light_map_param, 1);
				// ����Ҫ��̬����
				dynamic_lighting = false;
				// ʹ�ù���ͼ�����岻��ʵ����
				use_instance = false;*/
				pTexture = pTex;
			}
			else
			{
				pTexture = 0;
			}
		}
	}

	inline void SetTexture(int name, fm_uint tex)
	{
		ShaderManager::Inst().SetTexture2D(name, tex);
	}

	filter_param GetCurrentFilterParam( const MatInfo* info, unsigned int uFrameCnt, float weight )
	{
		material_info_t* pMatInfo = info->pMatInfo;
		node_material_t* pMat = info->pMat;

		if (pMat->nMaterialInfoEx & MATERIAL_FILTER_PARAM_ANI)
		{
			filter_param ret;
			filter_param* filter_src = &pMat->pFrameFilterParam[m_nCurrentFrame];
			filter_param* filter_dst;

			if (m_nCurrentFrame == uFrameCnt - 1)
			{
				if (m_bLoop)
				{
					filter_dst = &pMat->pFrameFilterParam[0];
				}
				else
				{
					filter_dst = &pMat->pFrameFilterParam[m_nCurrentFrame];
				}
			}
			else
			{
				filter_dst = &pMat->pFrameFilterParam[m_nCurrentFrame + 1];
			}

			ret.fScale = filter_src->fScale + (filter_dst->fScale - filter_src->fScale) * weight;
			ret.fBias = filter_src->fBias + (filter_dst->fBias - filter_src->fBias) * weight;
			ret.fScaleAlpha = filter_src->fScaleAlpha + (filter_dst->fScaleAlpha - filter_src->fScaleAlpha) * weight;
			ret.fBiasAlpha = filter_src->fBiasAlpha + (filter_dst->fBiasAlpha - filter_src->fBiasAlpha) * weight;

			return ret;
		}
		else
		{
			return pMatInfo->filterParam;
		}
	}

	inline void CreateFilterTextureMatrix( const node_material_t* pMat, FmMat4* mtxTex )
	{
		unsigned int uFrameCnt = GetFrameCount();
		//FLOAT weight = ref->fCurrentFrameOffset * ref->fSpeed 
		//	* pInst->fInternalFPS;
		float weight = this->m_fCurrentFrameOffset;

		if (pMat->bNoLerpT)
		{
			uv_param_t param_src = pMat->pFrameFilterTM[m_nCurrentFrame];

			CreateTextureAniMatrix(mtxTex, 
				param_src.fOffsetU,	param_src.fOffsetV,
				param_src.fRotateU,	param_src.fRotateV,	param_src.fRotateW,	
				param_src.fTilingU,	param_src.fTilingV);
			FmMat4Transpose(mtxTex, mtxTex);
		}
		else
		{
			uv_param_t param_src = pMat->pFrameFilterTM[m_nCurrentFrame];
			uv_param_t param_dst;

			if (this->m_nCurrentFrame == uFrameCnt - 1)
			{
				if (this->m_bLoop)
				{
					param_dst = pMat->pFrameFilterTM[0];
				}
				else
				{
					param_dst = pMat->pFrameFilterTM[this->m_nCurrentFrame];
				}
			}
			else
			{
				param_dst = pMat->pFrameFilterTM[m_nCurrentFrame + 1];
			}

			param_src.fOffsetU += (param_dst.fOffsetU - param_src.fOffsetU) * weight;
			param_src.fOffsetV += (param_dst.fOffsetV - param_src.fOffsetV) * weight;
			param_src.fRotateU += (param_dst.fRotateU - param_src.fRotateU) * weight;
			param_src.fRotateV += (param_dst.fRotateV - param_src.fRotateV) * weight;
			param_src.fRotateW += (param_dst.fRotateW - param_src.fRotateW) * weight;
			param_src.fTilingU += (param_dst.fTilingU - param_src.fTilingU) * weight;
			param_src.fTilingV += (param_dst.fTilingV - param_src.fTilingV) * weight;

			CreateTextureAniMatrix(mtxTex, 
				param_src.fOffsetU,	param_src.fOffsetV,
				param_src.fRotateU,	param_src.fRotateV,	param_src.fRotateW,
				param_src.fTilingU,	param_src.fTilingV);
			FmMat4Transpose(mtxTex, mtxTex);
		}
	}

	//��������ͼ�Ĵ�������������
	inline void SetModelTexture(const MatInfo* info, IShaderProgram* pShader, const node_material_t* pMat, ITexture* pTexLight = 0)
	{
		const material_info_t* pMatInfo = info->pMatInfo;

		int texlevel = 0;
		// ��������ͼ
		if (pMat->nMaterialInfo & MATERIAL_DIFFUSE_MAP_INFO)
		{
			ITexture* pTexDiffuse = pMatInfo->DiffuseMap.pTex;
			if (pTexDiffuse)
			{
				ITextureSampler* pTexSampler = pTexDiffuse->GetCanUseShaderTex()->GetTextureSampler();
				SetTexture(tex_Diffuse, pTexDiffuse->GetCanUseShaderTex()->GetTexture());
				if(pMat->MatInfo.bSamplerClamp)
				{
					pTexSampler->SetTextureUVWrapMode( ITextureSampler::TWM_CLAMP_TO_EDGE,  ITextureSampler::TWM_CLAMP_TO_EDGE);
				}
				else
				{
					pTexSampler->SetTextureUVWrapMode( ITextureSampler::TWM_REPEAT,  ITextureSampler::TWM_REPEAT);
				}

				//SetTexture(pShader, "tex_Diffuse", texlevel++, pTexDiffuse->GetShaderTex()->GetTexture());
			}
		}

		if (pMatInfo->bBumpMapEnable)
		{
			// ������ͼ
			ITexture* pTexBumpMap = pMatInfo->BumpMap.pTex;
			if (pTexBumpMap)
			{
				SetTexture(tex_Bump, pTexBumpMap->GetCanUseShaderTex()->GetTexture());
				//SetTexture(pShader, "tex_Bump", texlevel++, pTexBumpMap->GetShaderTex()->GetTexture());
			}
			else
			{
				//assert(0);
			}
		}

		if (pMatInfo->bSpecularEnable)
		{
			// �߹���ͼ
			ITexture* pTexSpecularMap = pMatInfo->SpecularMap.pTex;

			if (pTexSpecularMap)
			{
				SetTexture(tex_SpecularMap, pTexSpecularMap->GetCanUseShaderTex()->GetTexture());
				//SetTexture(pShader, "tex_SpecularMap", texlevel++, pTexSpecularMap->GetShaderTex()->GetTexture());
			}
			else
			{
				// �߹�������ͼ 
				ITexture* pTexSpecularLevelMap = pMatInfo->SpecularLevelMap.pTex;
				if (pTexSpecularLevelMap)
				{
					SetTexture(tex_SpecularMap, pTexSpecularLevelMap->GetCanUseShaderTex()->GetTexture());
					//SetTexture(pShader, "tex_SpecularMap", texlevel++, pTexSpecularLevelMap->GetShaderTex()->GetTexture());
				}
			}
		}

		if (pMatInfo->bEmissiveEnable || pMatInfo->bSkinEffect)
		{
			// �Է�����ͼ��α�����ͼ
			ITexture* pTexEmissiveMap = pMatInfo->EmissiveMap.pTex;
			if (pTexEmissiveMap)
			{
				SetTexture(tex_EmissiveMap, pTexEmissiveMap->GetCanUseShaderTex()->GetTexture());
				//SetTexture(pShader, "tex_EmissiveMap", texlevel++, pTexEmissiveMap->GetShaderTex()->GetTexture());
			}
		}

		if (pMatInfo->bSpecularEnable && pMatInfo->bReflectEnable)
		{
			// ����������ͼ
			ITexture* pTexReflectionMap = pMatInfo->ReflectionMap.pTex;

			if (pTexReflectionMap)
			{
				SetTexture(tex_ReflectionMap, pTexReflectionMap->GetCanUseShaderTex()->GetTexture());
				//SetTexture(pShader, "tex_ReflectionMap", texlevel++, pTexReflectionMap->GetShaderTex()->GetTexture());
			}
			else
			{
				// ����ʹ��ȫ�ֻ�����ͼ
				/*
				ITexture* pGlobalEnvTex = 
					pContext->GetGlobalEnvironmentCubeTex();

				if (pGlobalEnvTex)
				{
					// Ҳ��Ҫ��٤��У��
					pBatch->SetPSTexture(4, pGlobalEnvTex->GetShaderTex());
					reflect_enable = true;
				}*/
			}
		}

		if(pTexLight)
		{
			ITextureSampler* tex_LightmapSampler = pTexLight->GetCanUseShaderTex()->GetTextureSampler();
			tex_LightmapSampler->SetTextureSamplerFilter(ITextureSampler::TS_MAG_LINEAR,ITextureSampler::TS_MIN_LINEAR);;
			SetTexture(tex_LightMap, pTexLight->GetCanUseShaderTex()->GetTexture());
		}
		else if(pMatInfo->LightMap.pTex)
		{
			ITextureSampler* tex_LightmapSampler = pMatInfo->LightMap.pTex->GetCanUseShaderTex()->GetTextureSampler();
			tex_LightmapSampler->SetTextureSamplerFilter(ITextureSampler::TS_MAG_LINEAR,ITextureSampler::TS_MIN_LINEAR);;
			SetTexture(tex_LightMap, pMatInfo->LightMap.pTex->GetCanUseShaderTex()->GetTexture());
		}

		model_t* pModel = m_pResModel->GetModelData();

		if (pModel == NULL)
		{
			CORE_TRACE_EX("WARNING: [CModelPlayer::SetModelTexture] pModel == NULL model name is %s", m_pResModel->GetName());
			return;
		}

		unsigned int frame_num = pModel->nEndFrame - pModel->nStartFrame + 1;

		// ������ͼ
		if (info->pMat->nMaterialInfoEx && MATERIAL_FILTER_MAP_INFO && info->pMatInfo->bFilterEnable)
		{
			ITexture* pTexFilterMap = pMatInfo->FilterMap.pTex;

			if (pTexFilterMap)
			{
				SetTexture(tex_FilterMap, pTexFilterMap->GetCanUseShaderTex()->GetTexture());
			}
		}

		// ��Ӱͼ
		IRenderContext* pContext = g_pRender->GetContext();

		// ʵʱ��Ӱ
		bool dynamic_shadow = (pContext->GetEnableDynamicShadow() && GetReceiveShadow());
//     if (strcmp(m_strName.c_str(), "obj\\player\\monk\\monk_equip_001\\monk_equip_001.xmod") == 0)
//       dynamic_shadow = true;
		if (dynamic_shadow)
		{
			ITextureSampler* pTexSampler = pContext->GetDynamicShadowRT()->GetTextureSampler();
			pTexSampler->SetTextureUVWrapMode(ITextureSampler::TWM_CLAMP_TO_EDGE, ITextureSampler::TWM_CLAMP_TO_EDGE);

			SetTexture(tex_Shadow, pContext->GetDynamicShadowRT()->GetTexture());
		}
	}
public:
	enum
	{
		//RES_CATEGORY_LIGHT,			// ��������
		RES_CATEGORY_MATERIAL,		// ��������
	};

	enum MODEL_STATE_ENUM
	{
		MODEL_STATE_INVALID = 0x00000000,
		MODEL_STATE_PLAYING = 0x00000001,
		MODEL_STATE_STOPPED = 0x00000002,
		MODEL_STATE_PAUSE = 0x00000003,
	};

	static CModelPlayer* NewInstance(IRender* pRender, 
		CResModel* pResModel);

	struct atlas_light_map_t
	{
		size_t nSize;
		float fScaleU;
		float fScaleV;
		float fOffsetU;
		float fOffsetV;
		char strName[1];
	};

public:
	CModelPlayer(IRender* pRender, CResModel* pResModel);
	virtual ~CModelPlayer();

	// ����
	virtual void Release();
	// �Ƿ����
	virtual bool IsReady();
	// �Ƿ������ϣ��ɹ���ʧ�ܣ�
	virtual bool IsLoadComplete();
	// �Ƿ񴴽����
	bool IsCreate();

	// ���ó���������
	virtual void SetContext(IRenderContext* value);

	// ��Ⱦ
	virtual bool Draw(const FmPlane* planes, 
		size_t plane_num);

	virtual bool DrawReflect(const FmPlane* planes, size_t plane_num);

	virtual bool DrawShadowMap(const FmPlane* planes, size_t plane_num);

	// ��ײ�����ϸ��Ϣ
	virtual bool TraceDetail(const FmVec3& src, const FmVec3& dst,
		trace_info_t& result);
	// ��ײ���
	virtual bool TraceHitted(const FmVec3& src, const FmVec3& dst);
	// ���巶Χ��ײ
    virtual bool TraceSphere(const FmVec3& center, float radius);
    // ��������ײ
    virtual bool TraceTriangle(const FmVec3& v0, const FmVec3& v1, const FmVec3& v2);
	// ��ѡ
	virtual bool Cull(const FmPlane* planes, size_t plane_num, 
		const FmVec3& center, float radius, CULL_FUNC cull_func, 
		void* context, const FmMat4& mat);
	
	// ����Ƿ������Ⱦ����
	virtual bool GetLightingEnable();
	// �Ƿ���ù���ͼ
	virtual bool GetLightMapEnable();
	// �Ƿ���÷�����ͼ
	virtual bool GetNormalMapEnable();
	// �Ƿ���ö���ɫ
	virtual bool GetVertexColorEnable();
	// ��ð�Χ�гߴ�
	virtual FmVec3 GetBoxSize();
	
	// ��ñ�ǵ�����
	virtual size_t GetLabelCount();
	// ��ñ�ǵ�����
	virtual const char* GetLabelGroup(size_t index);
	// ��ñ�ǵ�����
	virtual const char* GetLabelName(size_t index);
	// ���ұ�ǵ��������
	virtual int FindLabelIndex(const char* group, const char* name);
	// ���ұ�ǵ�����
	virtual int FindLabelNameIndex(const char* name);
	// ��ñ�ǵ����
	virtual bool GetLabelMatrix(size_t index, FmMat4& mat);
	// ��ñ�ǵ�󶨵Ĺ���
	virtual int GetLabelBoneId(size_t index);
	
	// �ڵ���Ϣ
	virtual size_t GetObjectCount();
	virtual size_t GetObjectVertexCount(size_t index);
	virtual size_t GetObjectStartVertex(size_t index);
	virtual size_t GetObjectTriangleCount(size_t index);
	virtual size_t GetObjectVerterBufferIndex(size_t index);
	virtual size_t GetObjectMaterial(size_t index);
	// ���ָ���ڵ��������������
	virtual unsigned short* GetObjectIndexData(size_t index);
	// ���ָ���ڵ�ĵ�ǰ�������
	virtual bool GetObjectMatrix(size_t index, FmMat4& mat);

	// ������Ϣ
	virtual size_t GetMaterialCount();
	// ���ָ�����ʵ���ͼ����
	virtual size_t GetMaterialTextureCount(size_t mat_index);
	// ���ָ�������Ƿ���ù���ͼ
	virtual bool GetMaterialLightmapEnable(size_t mat_index);
	// ���ָ�����ʵ���ͼ��
	virtual const char* GetMaterialTextureName(size_t mat_index, 
		size_t tex_index);
	// ���ָ�����ʵĹ���ͼ��
	virtual const char* GetMaterialLightmapName(size_t mat_index);
	// ���ָ�����ʵ�Ambient
	virtual unsigned int GetMaterialAmbient(size_t mat_index);
	// ���ָ�����ʵ�Diffuse
	virtual unsigned int GetMaterialDiffuse(size_t mat_index);
	// ���ò���ʹ�úϲ���Ĺ���ͼ
	virtual bool SetMaterialAtlasLightMap(size_t mat_index, 
		const char* atlas_light_map, float scale_u, float scale_v,
		float offset_u, float offset_v);

	// ��ȡָ�����ʹ�����ͼ��ɫ��Χ������
	virtual bool SetMaterialLigthMapColorRangeScale(size_t mat_index, float x, float y, float z);
	virtual const FmVec3& GetMaterialLigthMapColorRangeScale(size_t mat_index);

	// ���㻺������
	virtual size_t GetVertexBufferCount();
	virtual size_t GetVertexBufferSize(size_t index);
	virtual size_t GetVertexDataStride(size_t index);
	virtual size_t GetVertexStride(size_t index);
	virtual size_t GetVertexCount(size_t index);
	// ���ָ�����㻺���λ������ƫ��
	virtual int GetVertexPositionOffset(size_t index);
	// ���ָ�����㻺���ָ������Ķ�������(����й��������ڹ�������)
	virtual bool GetVertexPosition(size_t index, size_t vertex_index, FmVec3& pos, bool checkbone = true);
	// ���ָ�����㻺��ķ�������ƫ��
	virtual int GetVertexNormalOffset(size_t index);
	// ���ָ�����㻺�����ɫ����ƫ��
	virtual int GetVertexDiffuseOffset(size_t index);
	// ���ָ�����㻺���ָ���������ɫ����
	virtual unsigned int GetVertexDiffuse(size_t vb_index, 
		size_t vertex_index);
	// ���ָ�����㻺�����ͼ��UV����ƫ��
	virtual int GetVertexUVOffset(size_t index);

	// ���ָ�����㻺��Ĺ���ͼUV����ƫ��
	virtual int GetVertexLightmapUVOffset(size_t index);
	virtual vertex_data_t* GetVertexData(size_t index);
	virtual bool UpdateVertexData(size_t index, vertex_data_t* data);
	
	// ����������
	virtual void SetActionPlayer(IActionPlayer* value);
	virtual IActionPlayer* GetActionPlayer();

	// �������
	virtual void SetWorldMatrix(const FmMat4& mat);
	virtual FmMat4 GetWorldMatrix();

	// �Ƿ���ʾ��Χ��
	virtual void SetShowBoundBox(bool value);
	virtual bool GetShowBoundBox();

	// �Ƿ�ʹ�ö���ɫ
	virtual void SetUseVertexColor(bool value);
	virtual bool GetUseVertexColor();

	// �Ƿ�ʹ�ù���ͼ
	virtual void SetUseLightMap(bool value);
	virtual bool GetUseLightMap();

	// д���ٶ������־
	virtual void SetWriteVelocity(int value);
	virtual int GetWriteVelocity();

	// ��ҪͶ��Ӱ��
	virtual void SetCastShadow(bool value) { m_bCastShadow = value; }
	virtual bool GetCastShadow() const { return m_bCastShadow; }

	// ��Ҫ����Ͷ��Ӱ��
	virtual void SetReceiveShadow(bool value) { m_bReceiveShadow = value; }
	virtual bool GetReceiveShadow() const { return m_bReceiveShadow; }

	// ������Ϣ�Ƿ�ɹ�����
	virtual bool GetLightLoaded();

	// ���¶���ɫ
	virtual bool UpdateVertexColor(unsigned int* color_data, 
		size_t color_size);
	// ���¹���ͼ
	virtual bool UpdateLightMap(size_t material, const char* light_file,
		unsigned int* tex_data, size_t tex_width, size_t tex_height);

	// ���ع����ļ�
	virtual bool LoadLight(const char* light_file, bool async_load);

	// ���ز����ļ�
	virtual bool LoadMaterial(const char* material_file, bool async_load);

	// ����ָ��ģ�͵ĸ�����ɫ
	virtual	void SetColor(unsigned int cr);
	// ��ȡָ��ģ�͵ĸ�����ɫ
	virtual	unsigned int GetColor();
	// ����ָ��ģ���Ƿ���ˮ��
	virtual	void SetUnderWater(bool value);
	// ��ȡָ��ģ���Ƿ���ˮ��
	virtual	bool GetUnderWater();
	// ��ȡָ��ģ�͵�֡����������־���������Max������ģ�Ͷ���֡��
	virtual unsigned int GetFrameCount();
	// ��ȡָ��ģ�͵�ԭʼFPS�����FPS����������Max������ģ�Ͷ�����FPS
	virtual float GetInternalFPS();
	// ����ģ�͵ĵ�ǰ�ٶ�
	virtual void SetSpeed(float fSpeed);
	// ��ȡģ�͵ĵ�ǰ�ٶ�
	virtual float GetSpeed();
	// �����Ƿ����ģ�Ͷ������Զ�ѭ������
	virtual void SetLoop(bool bLoop);
	// �ж��Ƿ����ģ�Ͷ������Զ�ѭ������
	virtual bool GetLoop();
	// ����ģ�Ͷ�������
	virtual void Play();
	// ֹͣģ�Ͷ�������
	virtual void Stop();
	// ��ͣģ�Ͷ�������
	virtual void Pause();
	// �ָ�ģ�Ͷ�������
	virtual void Resume();
	// ����ģ�Ͷ����ĵ�ǰ����ʱ��
	virtual void SetCurrentTimeFloat(double time);
	// ��ȡģ�Ͷ����ĵ�ǰ����ʱ��
	virtual double GetCurrentTimeFloat();
	// ����ģ�Ͷ����ĵ�ǰ֡
	virtual void SetCurrentFrame(unsigned int nFrame);
	// ��ȡģ�Ͷ����ĵ�ǰ֡
	virtual unsigned int GetCurrentFrame();
	// ��ģ�ͽ��и�������ײ���������������Ч��ѡ��ģ���ϵĸ�����
	virtual void* RayTraceHelper(const FmVec3* start, 
		const FmVec3* direction);

	// ָ���Ƿ����ص�ǰָ����ģ��
	virtual void SetHide(bool bHide);
	// ��ȡ��ǰָ����ģ���Ƿ�����
	virtual bool GetHide();

	// �Ƿ�ɻ����
	virtual void SetLive(bool value);
	virtual bool GetLive();

	// ָ���Ƿ�ǰģ��Ϊѡ��̬����ѡ��̬��ģ�ͽ���ʾ���Χ��
	virtual void SetSelection(bool bSelected);
	// ��ȡ�Ƿ�ǰģ�ʹ���ѡ��̬
	virtual bool GetSelection();

	// ָ����ǰģ��ʹ������ģʽ������Ⱦ
	//virtual void SetWireframe(bool bWireframe);
	// ��ȡ��ǰģ���Ƿ�ʹ������ģʽ������Ⱦ
	//virtual bool GetWireframe();
	// ��ʾ��ǰģ���а����ĸ�����
	virtual void SetShowHelper(bool bShow);
	// �жϵ�ǰģ���а����ĸ������Ƿ���ʾ
	virtual bool GetShowHelper();

	// ��ȡ��ǰģ���еĸ��ڵ����
	virtual unsigned int GetRootNodeCount();
	// ���ݵ�ǰģ���еĸ��ڵ�����������ȡ���������
	virtual void* GetRootNodeFromID(unsigned int index);
	// ��ȡ��ǰģ���еĽ��ĸ���
	virtual unsigned int GetNodeCount();
	// ���ݵ�ǰģ���еĽ������������ȡ���������
	virtual void* GetNodeFromID(int iNodeID);
	// ���ݵ�ǰģ���еĽ����������ȡ���������
	virtual void* GetNodeFromName(const char* pszName);
	// ���ݵ�ǰģ���еĽ�������Ļ�ȡ����������
	virtual int GetNodeID(void* node);
	// ���ݵ�ǰģ���еĽ�������Ļ�ȡ��������
	virtual const char* GetNodeName(void* node);
	// ���ݵ�ǰģ���еĽ������������ȡ��������
	virtual FXNODE_TYPE GetNodeType(void* node);
	// ���ݵ�ǰģ���еĽ������������ȡ�������Ϣ
	virtual float GetCameraNodeFov(void* node);
	virtual float GetCameraNodeFarClip(void* node);
	virtual float GetCameraNodeNearClip(void* node);
	// �Ƿ����ڵ�
	virtual bool IsAttachedNode(void* node);
	// ��ø����ڵ�ĸ��ڵ���
	virtual const char* GetAttachedParentName(void* node);
	// ��ȡ��ǰģ���еĽ��ĵ�ǰ�任��������任����������ڵ�ǰģ������ϵ��
	virtual void GetCurrentNodeTM(void* node, FmMat4* mtxTM);
	// ��ȡ��ǰģ�͵ı任����
	virtual void GetCurrentTM(FmMat4* mtxTM);

	// ��ȡ��ǰģ���Ƿ������ģ����ģ��
	virtual	bool ExistMainModel();
	// ָ����ǰģ�ͽ�����ģ����ģ��������ײ���
	virtual	void SetTraceMainModel(bool value);
	// ��ȡ��ǰģ���Ƿ������ģ����ģ��������ײ���
	virtual	bool GetTraceMainModel();

	// ָ���Ƿ���ʾ��ǰָ����ģ�ͽ��
	virtual void SetShowNode(void* node, bool bShow);
	// ��ȡ��ǰָ���Ľ���Ƿ���ʾ
	virtual bool GetShowNode(void* node);

	// ��ȡ��ǰģ�͵Ĳ��ʸ���
	//virtual unsigned int GetMaterialCount();
	// ���ݵ�ǰģ�͵Ĳ���������ȡ����������
	virtual	void* GetMaterialFromID(int iMatIndex);
	// ���ݵ�ǰģ�͵Ĳ������ƻ�ȡ����������
	virtual	void* GetMaterialFromName(const char* pszMatName);
	// ���ݲ�������������ȡ��������
	//virtual	int GetMaterialID(void* mat);
	// ��ȡ���������Ļ�ȡ��������
	virtual	const char* GetMaterialName(void* mat);
	// ��ȡָ���Ĳ��ʵ��������
	virtual	unsigned int GetMaterialTextureCount(void* mat);
	// ��ȡָ���Ĳ��ʵ�������ͼ
	virtual const char* GetMaterialTextureName(void* mat, unsigned int index);
	// ��ȡָ�����ʵĶ��㻺��
	virtual	void* GetMaterialVertexBuffer(void* mat);
	// ��ȡָ�����ʵ���������
	virtual	void* GetMaterialIndexBuffer(void* mat);
	// ��ȡָ�����ʵĶ����С
	virtual	unsigned int GetMaterialVertexSize(void* mat);
	// ��ȡָ�����ʵĶ������
	virtual	unsigned int GetMaterialVertexCount(void* mat);
	// ��ȡָ�����ʵ�������С��һ��Ϊsizeof(unsigned short)
	virtual	unsigned int GetMaterialIndexSize(void* mat);
	// ��ȡָ�����ʵ���������
	virtual unsigned int GetMaterialIndexCount(void* mat);
	// ��ȡָ�����ʵĶ���λ����Ϣ��һ��������ƫ���˶��ٸ��ֽ�
	virtual int GetPositionOffset(void* mat);
	// ��ȡָ�����ʵĶ��㷨����Ϣ��һ��������ƫ���˶��ٸ��ֽ�
	virtual int GetNormalOffset(void* mat);
	// ��ȡָ�����ʵĶ�����ɫ��Ϣ��һ��������ƫ���˶��ٸ��ֽ�
	virtual int GetDiffuseOffset(void* mat);
	// ��ȡָ�����ʵĶ���UV��Ϣ��һ��������ƫ���˶��ٸ��ֽ�
	virtual int GetUVOffset(void* mat);
	// ��ȡָ�����ʵĶ���Lightmap����������Ϣ��һ��������ƫ���˶��ٸ��ֽ�
	virtual int GetLightmapUVOffset(void* mat);
	// ��ȡָ����Diffuse�����ļ���
	virtual	const char* GetTexDiffuseFileName(void* mat);
	// ��ȡָ����Bump�����ļ���
	virtual	const char* GetTexBumpFileName(void* mat);
	// ��ȡָ����Specularmap�����ļ���
	virtual	const char* GetTexSpecularmapFileName(void* mat);
	// ��ȡָ����Specularlevelmap�����ļ���
	virtual	const char* GetTexSpecularlevelmapFileName(void* mat);
	// ��ȡָ����Glossinessmap�����ļ���
	virtual	const char* GetTexGlossinessmapFileName(void* mat);
	// ��ȡָ����Reflectionmap�����ļ���
	virtual	const char* GetTexReflectionmapFileName(void* mat);
	// ��ȡָ����Lightmap�����ļ���
	virtual	const char* GetTexLightmapFileName(void* mat);
	// ���ָ�����ʵ�Ambient
	virtual	unsigned int GetMaterialAmbient(void* mat);
	// ���ָ�����ʵ�Diffuse
	virtual	unsigned int GetMaterialDiffuse(void* mat);

	// �����Զ���Ĳ���
	virtual bool FindCustomMaterial(const char* mat_name);
	// �����Զ����������
	virtual bool SetCustomMaterialValue(const char* mat_name,
		const char* key, const char* val);
	// ��ȡ�Զ����������
	virtual bool GetCustomMaterialValue(const char* mat_name,
		const char* key, char* val, int val_size);
	// ���¼����Զ�����ʵ�ָ����ͼ
	virtual bool ReloadCustomMaterialTexture(const char* mat_name, 
		const char* key, const char* tex_paths, bool async);
	// ���¼����Զ�����ʵ�������ͼ
	virtual bool ReloadCustomMaterialTextures(const char* tex_paths, bool async);

	// ����ģ�Ͳ�������
	virtual bool SetModelMaterialValue(const char* mat_name, 
		const char* key, const char* val);
	// ��ȡģ�Ͳ�������
	virtual bool GetModelMaterialValue(const char* mat_name,
		const char* key, char* val, int val_size);
	// ���¼���ģ�͵�������ͼ
	virtual bool ReloadMaterialTextures(const char* tex_paths, 
		bool async);

	// ���ģ�ͽڵ������Ƿ�ɱ༭
	virtual bool GetModelNodeAllowModify(const char* key);
	// ����ģ�ͽڵ�����
	virtual bool SetModelNodeValue(const char* node_name, 
		const char* key, const char* val);
	// ��ȡģ�ͽڵ�����
	virtual bool GetModelNodeValue(const char* node_name,
		const char* key, char* val, int val_size);

	// ��ȡָ��ģ�͵���Դ�ļ�����
	virtual const char* GetModelFileName();
	// ��ȡָ��ģ�͵Ĵ洢��ʹ���������
	//virtual void GetMemoryStatus(LPFXMEMORY_STATUS status);
	// ����ǰģ����ָ��������������Ƥӳ��
	virtual bool InitSkeletonMappingTable();
	// �жϵ�ǰģ���Ƿ���ָ����������������Ƥӳ��
	virtual bool IsInitSkeletonMappingTable();
	// �ͷŵ�ǰģ�͵���ƤӰ��
	virtual void ReleaseSkeletonMappingTable();
	// ���õ�ǰģ�͵�ָ��������Ϊѡ��̬
	virtual void SetSelectHelper(int nHelper);
	// ��ȡ��ǰģ���д���ѡ��̬�ĸ�����
	virtual int GetSelectHelper();
	// ��ȡ��ǰģ���а����ĸ��������
	virtual unsigned int GetHelperCount();
	// ���ݵ�ǰģ�͵ĸ�����������ȡ�����������
	virtual const char* GetHelperIDFromIndex(unsigned int nIndex);
	// ָ���Ƿ�����ǰָ��ģ�͵�FallOff(������)Ч��
	virtual void SetEnableFallOff(bool value);
	// �жϵ�ǰָ����ģ���Ƿ�����FallOff(������)Ч��
	virtual bool GetEnableFallOff();
	// ָ���Ƿ�����ǰָ��ģ�͵ĵ��Դ����Ч��
	virtual void SetEnablePointLighting(bool value);
	// �жϵ�ǰָ����ģ���Ƿ����˵��Դ����Ч��
	virtual bool GetEnablePointLighting();
	// ģ�͵�ǰLOD�ļ���
	virtual void SetCurLODLevel(unsigned int lod_level);
	virtual unsigned int GetCurLODLevel();
	// ģ���ܹ���LOD����
	virtual unsigned int GetLODLevels();
	// Warning:This API just for LOD tools use!
	// ָ��ģ�ͽ���LOD�����±���ģ��
	//virtual bool LOD(unsigned int lod_levels, float dec_ratio);
	
	// ִ�в���
	virtual void Update(float seconds);

	// ͬ�����ع���
	//bool CreateLight(const char* light);
	// ��ʼ�첽�������� 
	//bool BeginCreateLight(const char* light);
	// �����ݴ�������ͼ
	//bool BuildLightMap(const char* light, size_t material, DxTexture* pTex);

	// ͬ�����ز����ļ�
	bool CreateMaterial(const char* material);
	// ��ʼ�첽���������ļ� 
	bool BeginCreateMaterial(const char* material);
	// �����ݴ�������ͼ
	bool BuildMaterial(IIniFile* pIni);

	// �����첽���� 
	bool EndCreate(int category, bool succeed);

	// ��������
	bool Create();

	// ��������
	void SetName(const char* value)
	{ 
		Assert(value != NULL);

		m_strName = value; 
	}

	// �������
	const char* GetName() const
	{
		return m_strName.c_str();
	}

	// ������ʼʱ��
	void SetBeginTime(double value) { m_dBeginTime = value; }
	double GetBeginTime() const { return m_dBeginTime; }

	// �����Ⱦ��
	IRender* GetRender() const { return m_pRender; }

	// ��ù���ͼ����
	size_t GetLightMapCount() const { return m_LightTexs.size(); }
	
	// ���ָ������ͼ
	ITexture* GetLightMapTex(size_t index) const
	{
		Assert(index < m_LightTexs.size());

		return m_LightTexs[index];
	}

	size_t GetAtlasLightMapCount() const { return m_AtlasLightMaps.size(); }

	atlas_light_map_t* GetAtlasLightMap(size_t index) const
	{
		Assert(index < m_AtlasLightMaps.size());

		return m_AtlasLightMaps[index];
	}

	const FmMat4& GetCurrentTM() const { return m_mtxCurrentTM; }
	const FmMat4& GetWorldTM() const { return m_mtxWorldTM; }

	// ������ײ��Ϣ
	bool SetTraceInfo(trace_info_t* result, model_node_t* pNode, 
		node_material_t* pMat, int tri_id, float u, float v, float t, 
		const FmVec3& p0, const FmVec3& p1, const FmVec3& p2);

private:
	CModelPlayer();
	CModelPlayer(const CModelPlayer&);
	CModelPlayer& operator=(const CModelPlayer&);

	// �������ĵ�Ͱ뾶
	void UpdateCenterRadius();
	// ����������Ƥ
	bool CreateSkeletonMapping();
	// �ͷŹ���ͼ
	void ReleaseLightMaps();
	// ���ع���
	bool InnerLoadLight();
	// ���ز����ļ�
	bool InnerLoadMaterial();

	// ��ó�������
	//IDxDynamicCB* GetObjectCB(node_material_t* pMat, 
	//	const FmVec4* bone_matrices);
	// ������������
	FmVec4* CreateBoneMatrices(node_material_t* pMat);
	void CreateBillboard(model_node_t* pNode);
	// �������㶨��
	bool CreateVDecl(node_material_t* pMat, model_node_t* pNode, 
		bool lod);
	// �����������������
	bool CreateVBIB(node_material_t* pMat, model_node_t* pNode);
	// ������ͼ��������
	void CreateTextureMatrix(node_material_t* pMat,  
		FmMat4* mtxTex);
	// �������εļ�������Ϣ
//	bool SetBatchGeometry(DxSceneBatch* pBatch, node_material_t* pMat,
//		model_node_t* pNode);
	// �������ε��ڵ���ѯ��Ϣ
//	bool SetBatchOcclusionQuery(DxSceneBatch* pBatch, node_material_t* pMat,
//		model_node_t* pNode);
	// ��õ�ǰ�Ĳ���͸����
	float GetCurrentOpacity(node_material_t* pMat, model_node_t* pNode, 
		unsigned int frame_num, float weight);
	// ����������
	bool CheckMaterialData(node_material_t* pMat, int offset, int stride);
	// �������Ƿ������Ч����
	void CheckMaterialAllData(node_material_t* pMat);

	// ѡȡ��ɫ��
	//����
	IShaderProgram* SelectShader(const MatInfo* info, node_material_t* pMat, model_node_t* pNode);

	IShaderProgram* SelectShader(const MatInfo* info, model_node_t* pNode, 
		node_material_t* pMat, bool dynamic_lighting, bool dynamic_shadow, 
		bool fog_linear, bool fog_exp, bool height_fog, bool prelight, bool reflect_enable, 
		bool fix_shadow, bool sphere_ambient, bool point_light, 
		unsigned int model_alpha, bool use_instance, bool camera_light, 
		bool is_blend, bool gamma, bool filter, bool use_clip_plane, bool onlyzpass = false, bool onlycolor = false, bool keepout = false);

	IShaderProgram* SelectShadowMapShader(const MatInfo* info, node_material_t* pMat, model_node_t* pNode);

//	bool SelectRefractionShader(DxSceneBatch* pBatch, 
//		model_node_t* pNode, node_material_t* pMat);
//	bool SelectAppliqueShader(DxSceneBatch* pBatch, model_node_t* pNode, 
//		node_material_t* pMat, bool shadow, bool prelight, bool fog_linear, 
//		bool fog_exp, bool use_intz, bool multiply, bool stereo);
//	bool SelectGlowShader(DxSceneBatch* pBatch, model_node_t* pNode, 
//		node_material_t* pMat, bool fog_linear, bool fog_exp, bool gamma);
//	bool SelectVolumeFogDepthShader(DxSceneBatch* pBatch, model_node_t* pNode, 
//		node_material_t* pMat, bool bCW);
//	bool SelectVolumeFogColorShader(DxSceneBatch* pBatch, model_node_t* pNode, 
//		node_material_t* pMat, bool fog_linear, bool fog_exp);
//	bool SelectWaterSurfaceShader(DxSceneBatch* pBatch, model_node_t* pNode, 
//		node_material_t* pMat, bool fog_linear, bool fog_exp, bool depth_map, 
//		bool refraction, bool gamma);
//	bool SelectShader(DxSceneBatch* pBatch, model_node_t* pNode, 
//		node_material_t* pMat, bool dynamic_lighting, bool dynamic_shadow, 
//		bool fog_linear, bool fog_exp, bool prelight, bool reflect_enable, 
//		bool fix_shadow, bool sphere_ambient, bool point_light, 
//		unsigned int model_alpha, bool use_instance, bool camera_light, 
//		bool is_blend, bool gamma, bool use_clip_plane);
//	bool SelectGBufferShader(DxSceneBatch* pBatch, model_node_t* pNode,
//		node_material_t* pMat, bool gamma, bool use_tesse);
	//bool SelectDepthMapShader(DxSceneBatch* pBatch, model_node_t* pNode, 
	//	node_material_t* pMat, bool use_normal, bool use_normalmap, 
	//	bool use_instance, bool only_normal);
	//bool SelectVelocityShader(DxSceneBatch* pBatch, model_node_t* pNode, 
	//	node_material_t* pMat);
//	bool SelectDepthShader(DxSceneBatch* pBatch, model_node_t* pNode, 
//		node_material_t* pMat);
//	bool SelectShadowMapShader(DxSceneBatch* pBatch, model_node_t* pNode, 
//		node_material_t* pMat, bool use_instance, bool use_tesse);

	void DrawMaterialGBuffer(node_material_t* pMat, model_node_t* pNode, 
		const FmVec4* bone_matrices);
	void DrawMaterialSolid(const MatInfo* info, bool onlyzpass = false, bool onlycolor = false);
	void DrawMaterialKeepOut(const MatInfo* info);
	void DrawMaterialBlend(const MatInfo* info);
	void DrawMaterialAlphaTest(const MatInfo* info);
	void DrawMaterialEarlyZ(const MatInfo* info);
	void DrawMaterialWater(node_material_t* pMat, model_node_t* pNode, 
		const FmVec4* bone_matrices);
	void DrawMaterialWaterClip(node_material_t* pMat, model_node_t* pNode, 
		const FmVec4* bone_matrices);
	void DrawMaterialApplique(node_material_t* pMat, model_node_t* pNode, 
		const FmVec4* bone_matrices);
	void DrawMaterialVolumeFog(node_material_t* pMat, model_node_t* pNode, 
		const FmVec4* bone_matrices);
	void DrawMaterialRefraction(node_material_t* pMat, model_node_t* pNode, 
		const FmVec4* bone_matrices);

	void GetNodeLocationBoundingBox(FmVec3& bbDstMin, 
		FmVec3& bbDstMax, FmVec3& bbDstCenter, 
		const FmVec3& bbSrcMin, const FmVec3& bbSrcMax, 
		const FmVec3& bbSrcCenter);

	void GetNodeLocationBoundingBox(FmVec3& bbDstMin, 
		FmVec3& bbDstMax, FmVec3& bbDstCenter);

	void GetNodeLocationBoundingBox(FmVec3& bbDstMin, 
		FmVec3& bbDstMax, const FmVec3& bbSrcMin, 
		const FmVec3& bbSrcMax);
	// ��Ⱦģ������
	//void DrawApplique(DxSceneBatch* pBatch, DxSceneBatch* pBatchStencil,
	//	node_material_t* pMat, model_node_t* pNode);
	// ��Ⱦ�����
	//void DrawVolumeFog(DxSceneBatch* pBatch, node_material_t* pMat, 
	//	model_node_t* pNode);
	// ��Ⱦˮ����ʱ���
	//void DrawWaterSurface(DxSceneBatch* pBatch, node_material_t* pMat, 
	//	model_node_t* pNode);
	// �ύ��Ⱦ�������
	//void CommitDepthBatch(node_material_t* pMat, model_node_t* pNode, 
	//	IDxDynamicCB* pCB);
	// �ύ��������
	//void CommitGlowBatch(node_material_t* pMat, model_node_t* pNode,
	//	bool fog_linear, bool fog_exp, DxSceneBatch* pSolidBatch, 
	//	IDxDynamicCB* pCB);
	// �ύ��Ⱦ�ٶ�����
	//void CommitVelocityBatch(node_material_t* pMat, model_node_t* pNode, 
	//	IDxDynamicCB* pCB);
	// ��ģ�Ͳ���
	void DrawMaterial(node_material_t* pMat, model_node_t* pNode, MatInfo* matinfo);
	// ��ģ�ͽڵ�
	void DrawNode(model_node_t* pNode, bool bCull, MatInfo* matinfo);
	// ���ڵ㷨��
	void DrawNodeNormal(model_t* pModel, model_node_t* pNode);
	// ��Ⱦģ��
	void DrawModel(bool bCull, MatInfo* MatList);
	// ��������Ӱͼ
	void DrawMaterialShadowMap(const MatInfo* info);
	// ���ڵ����Ӱͼ
	void DrawNodeShadowMap(model_node_t* pNode, bool bCull);
	// ��ģ����Ӱͼ
	void DrawModelShadowMap(bool bCull);

	// ����ģ�����õ�ָ�������ļ�
	bool LoadCustomMaterialFile(IIniFile* pIni);
	// ɾ��ģ�����õ�ָ������
	bool UnloadCustomMaterials();
	// �����Զ������
	material_info_t* InnerFindCustomMaterial(node_material_t* pMat);
	// �����Զ������
	material_info_t* InnerAddCustomMaterial(node_material_t* pMat);

	// ����ģ�͵Ĳ���
	node_material_t* FindModelMaterial(model_t* pModel,
		const char* mat_name);
	// ģ�����õ���ͼ�Ƿ�������
	bool CustomTextureLoadComplete();
	// �����Զ������
	material_info_t* FindCustomMaterial(node_material_t* pMat);
	// ����ģ�����õ�������ͼ
	bool LoadCustomMaterialTexture(const char* mat_name, const char* key, 
		const char* tex_paths, bool async);
	// ����ģ�����õ�������ͼ
	bool LoadCustomTextures(const char* tex_paths, bool async);
	// ж��ģ�����õ�������ͼ
	bool UnloadCustomTextures();

	// ���õ�ǰʹ�õ��������
	void SetCurrentMatrix(model_node_t* pNode);
	// ���ɹ�������ģ�Ͷ�̬���㻺��
	FmVec3* AllocCpuSkeletonAnimationVB(node_material_t* pMat);
	// �Խڵ��������׷��
	void NodeRayTrace(model_t* pModel, model_node_t* pNode, 
		const FmVec3* start, const FmVec3* direction, 
		trace_info_t* result);
	// �Խڵ����������ײ����
	bool NodeSphereTrace(model_t* pModel, model_node_t* pNode, 
		const FmVec3* position, float radius);
	// �Խڵ����������ײ����
	bool NodeTriangleTrace(model_t* pModel, model_node_t* pNode, 
		const FmVec3* v0, const FmVec3* v1, const FmVec3* v2);
	// �Խڵ������ѡȡ
	void NodeCull(model_t* pModel, model_node_t* pNode, 
		const FmPlane* planes, unsigned int plane_num, 
		const FmVec3* center, float radius, void* cull_func, 
		void* cull_context, const FmMat4* cull_matrix);
	// ѡȡ�ڵ�ĸ�����
	bool NodeRayTraceHelper(model_t* pModel, model_node_t* pNode, 
		const FmVec3* start, const FmVec3* direction, 
		void** result_node, float* result_distance);
	// �Թ��������ڵ��������׷��
	void NodeRayTraceSkin(model_t* pModel, model_node_t* pNode, 
		const FmVec3* start, const FmVec3* direction, 
        trace_info_t* result);
    // �Թ��������ڵ��������ײ
    bool NodeSphereTraceSkin(model_t* pModel, model_node_t* pNode, 
        const FmVec3* position, float radius);
    // �Թ��������ڵ������������ײ
    bool NodeTriangleTraceSkin(model_t* pModel, model_node_t* pNode, 
        const FmVec3* v0, const FmVec3* v1, const FmVec3* v2);
	// ���ݾ����ж��Ƿ����ײ����Χ��
	bool OnlyTraceBoundBoxByDistance(const FmVec3* start, 
		const FmVec3* direction, float dis, trace_info_t* result);
	// ģ������׷��
	bool InnerModelRayTrace(bool can_trace_box, const FmVec3* start, 
		const FmVec3* direction, trace_info_t* result);

	bool ModelRayTrace(bool can_trace_box, const FmVec3* start, 
		const FmVec3* direction, trace_info_t* result);
	bool ModelRayTraceAlpha(bool can_trace_box, const FmVec3* start, 
		const FmVec3* direction, trace_info_t* result);
	bool ModelRayTraceFast(const FmVec3* start, 
		const FmVec3* direction); 
	bool ModelRayTraceFastAlpha(const FmVec3* start, 
		const FmVec3* direction); 
    bool ModelSphereTrace(const FmVec3* position, float radius);
    bool ModelTriangleTrace(const FmVec3* v0, const FmVec3* v1, const FmVec3* v2);
	bool ModelCull(const FmPlane* planes, unsigned int plane_num, 
		const FmVec3* center, float fRadius, void* cull_func, 
		void* cull_context, const FmMat4* cull_matrix);

	// ���½ڵ�
	void UpdateModelNode(model_node_t* pNode, model_t* pModel, bool bCull);
	// ���¶���ʱ��
	void UpdateAniTime(double dTime);
	// ����ģ��
	void UpdateModel(bool bCull);
	
	//�����û�����Sahder�ĳ���
	void SetUserVsConstValue(const char* name, const FmVec4& value);

	void EnableKeepOut(bool keepout)
	{ 
		m_bKeepOut = keepout;
	}

	bool IsEnableKeepOut()
	{
		return m_bKeepOut; 
	}

	void SetKeepOutTexture(const char* filename);
#ifdef CPU_COMPUTE_BONE
	void CpuComputeBone(node_material_t* pMat, const FmVec4* pBones);
#endif
private:
	IRender* m_pRender;
	IRenderContext* m_pContext;
	CResModel* m_pResModel;
	CActionPlayer* m_pActionPlayer;
	core_string m_strName;
	double m_dBeginTime;
	double m_dCurrentTime;
	double m_dLastTime;
	FmVec3 m_vScale;
	FmVec3 m_vBoxSize;
	FmVec3 m_Vec3DefaultValve;
	//FmVec3 m_vCenter;
	//float m_fRadius;
//	TArrayPod<IDxDynamicCB*, 1, TModelPlayerAlloc> m_MaterialCBs;
	TArrayPod<bool, 1, TModelPlayerAlloc> m_UpdateFlags;
	TArrayPod<ITexture*, 1, TModelPlayerAlloc> m_LightTexs;
	TArrayPod<atlas_light_map_t*, 1, TModelPlayerAlloc> m_AtlasLightMaps;
	TArrayPod<FmVec3, 1, TModelPlayerAlloc> m_LightMapConfig;
	core_string m_strLightFile;
	core_string m_strMaterialFile;
	CMaterialLoader* m_pMaterialLoader;
	unsigned int m_nCustomMaterialNum;
	custom_material_t* m_pCustomMaterials;
	material_info_t* m_pCurMatInfo;
	unsigned int m_nCurrentFrame;
	float m_fCurrentFrameOffset;
	float m_fSpeed;
	unsigned int m_nRefId;
	unsigned int m_nColor;
	int m_nState;
	int m_nSelectHelper;
	int m_nWriteVelocity;
	bool m_bAsyncLoad;
	bool m_bShowBoundBox;
	bool m_bSkeletonMapped;
	bool m_bUseVertexColor;
	bool m_bUseLightMap;
	bool m_bLoop;
	bool m_bSelected;
	bool m_bTraceMainModel;
	bool m_bHide;
	bool m_bLive;
	bool m_bShowHelper;
	bool m_bUnderWater;
	bool m_bFallOff;
	bool m_bPointLighting;
	bool m_bNeedDraw;
	bool m_bKeepOut;
	bool m_bCastShadow;
	bool m_bReceiveShadow;
	FmMat4 m_mtxWorldTM;
	FmMat4 m_mtxPrevWorldTM;
	FmMat4 m_mtxCurrentTM;
	FmMat4* m_mtxNodeCurrentTM;
	cache_skin_t* m_pCacheSkins;
	cache_bone_t* m_pCacheBones;
	core_string m_KeepOutName;
	ITexture* m_KeepOutTexture;
};



#endif // _MODEL_PLAYER_H
