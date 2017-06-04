//--------------------------------------------------------------------
// 文件名:		model_player.h
// 内  容:		
// 说  明:		
// 创建日期:	2009年5月4日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
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

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
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
		//这里改进提高查询速度
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

	//给Shader设置变量
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

	// 申请内存
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// 释放内存
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// 交换
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

// 模型播放

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

	//给SHADER设置常量
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
		// 光贴图
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
				// 不需要动态光照
				dynamic_lighting = false;
				// 使用光贴图的物体不可实例化
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

	//把设置贴图的代码抽离出来公用
	inline void SetModelTexture(const MatInfo* info, IShaderProgram* pShader, const node_material_t* pMat, ITexture* pTexLight = 0)
	{
		const material_info_t* pMatInfo = info->pMatInfo;

		int texlevel = 0;
		// 漫反射贴图
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
			// 法线贴图
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
			// 高光贴图
			ITexture* pTexSpecularMap = pMatInfo->SpecularMap.pTex;

			if (pTexSpecularMap)
			{
				SetTexture(tex_SpecularMap, pTexSpecularMap->GetCanUseShaderTex()->GetTexture());
				//SetTexture(pShader, "tex_SpecularMap", texlevel++, pTexSpecularMap->GetShaderTex()->GetTexture());
			}
			else
			{
				// 高光亮度贴图 
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
			// 自发光贴图或次表面贴图
			ITexture* pTexEmissiveMap = pMatInfo->EmissiveMap.pTex;
			if (pTexEmissiveMap)
			{
				SetTexture(tex_EmissiveMap, pTexEmissiveMap->GetCanUseShaderTex()->GetTexture());
				//SetTexture(pShader, "tex_EmissiveMap", texlevel++, pTexEmissiveMap->GetShaderTex()->GetTexture());
			}
		}

		if (pMatInfo->bSpecularEnable && pMatInfo->bReflectEnable)
		{
			// 环境反射贴图
			ITexture* pTexReflectionMap = pMatInfo->ReflectionMap.pTex;

			if (pTexReflectionMap)
			{
				SetTexture(tex_ReflectionMap, pTexReflectionMap->GetCanUseShaderTex()->GetTexture());
				//SetTexture(pShader, "tex_ReflectionMap", texlevel++, pTexReflectionMap->GetShaderTex()->GetTexture());
			}
			else
			{
				// 尝试使用全局环境贴图
				/*
				ITexture* pGlobalEnvTex = 
					pContext->GetGlobalEnvironmentCubeTex();

				if (pGlobalEnvTex)
				{
					// 也需要做伽玛校正
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

		// 遮罩贴图
		if (info->pMat->nMaterialInfoEx && MATERIAL_FILTER_MAP_INFO && info->pMatInfo->bFilterEnable)
		{
			ITexture* pTexFilterMap = pMatInfo->FilterMap.pTex;

			if (pTexFilterMap)
			{
				SetTexture(tex_FilterMap, pTexFilterMap->GetCanUseShaderTex()->GetTexture());
			}
		}

		// 阴影图
		IRenderContext* pContext = g_pRender->GetContext();

		// 实时阴影
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
		//RES_CATEGORY_LIGHT,			// 光照数据
		RES_CATEGORY_MATERIAL,		// 材质数据
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

	// 销毁
	virtual void Release();
	// 是否就绪
	virtual bool IsReady();
	// 是否加载完毕（成功或失败）
	virtual bool IsLoadComplete();
	// 是否创建完成
	bool IsCreate();

	// 设置场景上下文
	virtual void SetContext(IRenderContext* value);

	// 渲染
	virtual bool Draw(const FmPlane* planes, 
		size_t plane_num);

	virtual bool DrawReflect(const FmPlane* planes, size_t plane_num);

	virtual bool DrawShadowMap(const FmPlane* planes, size_t plane_num);

	// 碰撞检测详细信息
	virtual bool TraceDetail(const FmVec3& src, const FmVec3& dst,
		trace_info_t& result);
	// 碰撞检测
	virtual bool TraceHitted(const FmVec3& src, const FmVec3& dst);
	// 球体范围碰撞
    virtual bool TraceSphere(const FmVec3& center, float radius);
    // 三角形碰撞
    virtual bool TraceTriangle(const FmVec3& v0, const FmVec3& v1, const FmVec3& v2);
	// 拣选
	virtual bool Cull(const FmPlane* planes, size_t plane_num, 
		const FmVec3& center, float radius, CULL_FUNC cull_func, 
		void* context, const FmMat4& mat);
	
	// 获得是否可以渲染光照
	virtual bool GetLightingEnable();
	// 是否可用光贴图
	virtual bool GetLightMapEnable();
	// 是否可用法线贴图
	virtual bool GetNormalMapEnable();
	// 是否可用顶点色
	virtual bool GetVertexColorEnable();
	// 获得包围盒尺寸
	virtual FmVec3 GetBoxSize();
	
	// 获得标记点数量
	virtual size_t GetLabelCount();
	// 获得标记点组名
	virtual const char* GetLabelGroup(size_t index);
	// 获得标记点名称
	virtual const char* GetLabelName(size_t index);
	// 查找标记点组和名称
	virtual int FindLabelIndex(const char* group, const char* name);
	// 查找标记点名称
	virtual int FindLabelNameIndex(const char* name);
	// 获得标记点矩阵
	virtual bool GetLabelMatrix(size_t index, FmMat4& mat);
	// 获得标记点绑定的骨骼
	virtual int GetLabelBoneId(size_t index);
	
	// 节点信息
	virtual size_t GetObjectCount();
	virtual size_t GetObjectVertexCount(size_t index);
	virtual size_t GetObjectStartVertex(size_t index);
	virtual size_t GetObjectTriangleCount(size_t index);
	virtual size_t GetObjectVerterBufferIndex(size_t index);
	virtual size_t GetObjectMaterial(size_t index);
	// 获得指定节点的索引缓冲数据
	virtual unsigned short* GetObjectIndexData(size_t index);
	// 获得指定节点的当前世界矩阵
	virtual bool GetObjectMatrix(size_t index, FmMat4& mat);

	// 材质信息
	virtual size_t GetMaterialCount();
	// 获得指定材质的贴图层数
	virtual size_t GetMaterialTextureCount(size_t mat_index);
	// 获得指定材质是否可用光贴图
	virtual bool GetMaterialLightmapEnable(size_t mat_index);
	// 获得指定材质的贴图名
	virtual const char* GetMaterialTextureName(size_t mat_index, 
		size_t tex_index);
	// 获得指定材质的光贴图名
	virtual const char* GetMaterialLightmapName(size_t mat_index);
	// 获得指定材质的Ambient
	virtual unsigned int GetMaterialAmbient(size_t mat_index);
	// 获得指定材质的Diffuse
	virtual unsigned int GetMaterialDiffuse(size_t mat_index);
	// 设置材质使用合并后的光贴图
	virtual bool SetMaterialAtlasLightMap(size_t mat_index, 
		const char* atlas_light_map, float scale_u, float scale_v,
		float offset_u, float offset_v);

	// 获取指定材质光照贴图颜色范围的缩放
	virtual bool SetMaterialLigthMapColorRangeScale(size_t mat_index, float x, float y, float z);
	virtual const FmVec3& GetMaterialLigthMapColorRangeScale(size_t mat_index);

	// 顶点缓冲数据
	virtual size_t GetVertexBufferCount();
	virtual size_t GetVertexBufferSize(size_t index);
	virtual size_t GetVertexDataStride(size_t index);
	virtual size_t GetVertexStride(size_t index);
	virtual size_t GetVertexCount(size_t index);
	// 获得指定顶点缓冲的位置数据偏移
	virtual int GetVertexPositionOffset(size_t index);
	// 获得指定顶点缓冲的指定顶点的顶点数据(如果有骨骼，参于骨骼计算)
	virtual bool GetVertexPosition(size_t index, size_t vertex_index, FmVec3& pos, bool checkbone = true);
	// 获得指定顶点缓冲的法线数据偏移
	virtual int GetVertexNormalOffset(size_t index);
	// 获得指定顶点缓冲的颜色数据偏移
	virtual int GetVertexDiffuseOffset(size_t index);
	// 获得指定顶点缓冲的指定顶点的颜色数据
	virtual unsigned int GetVertexDiffuse(size_t vb_index, 
		size_t vertex_index);
	// 获得指定顶点缓冲的贴图的UV数据偏移
	virtual int GetVertexUVOffset(size_t index);

	// 获得指定顶点缓冲的光贴图UV数据偏移
	virtual int GetVertexLightmapUVOffset(size_t index);
	virtual vertex_data_t* GetVertexData(size_t index);
	virtual bool UpdateVertexData(size_t index, vertex_data_t* data);
	
	// 动画播放器
	virtual void SetActionPlayer(IActionPlayer* value);
	virtual IActionPlayer* GetActionPlayer();

	// 世界矩阵
	virtual void SetWorldMatrix(const FmMat4& mat);
	virtual FmMat4 GetWorldMatrix();

	// 是否显示包围盒
	virtual void SetShowBoundBox(bool value);
	virtual bool GetShowBoundBox();

	// 是否使用顶点色
	virtual void SetUseVertexColor(bool value);
	virtual bool GetUseVertexColor();

	// 是否使用光贴图
	virtual void SetUseLightMap(bool value);
	virtual bool GetUseLightMap();

	// 写入速度纹理标志
	virtual void SetWriteVelocity(int value);
	virtual int GetWriteVelocity();

	// 需要投射影子
	virtual void SetCastShadow(bool value) { m_bCastShadow = value; }
	virtual bool GetCastShadow() const { return m_bCastShadow; }

	// 需要接受投射影子
	virtual void SetReceiveShadow(bool value) { m_bReceiveShadow = value; }
	virtual bool GetReceiveShadow() const { return m_bReceiveShadow; }

	// 光照信息是否成功加载
	virtual bool GetLightLoaded();

	// 更新顶点色
	virtual bool UpdateVertexColor(unsigned int* color_data, 
		size_t color_size);
	// 更新光贴图
	virtual bool UpdateLightMap(size_t material, const char* light_file,
		unsigned int* tex_data, size_t tex_width, size_t tex_height);

	// 加载光照文件
	virtual bool LoadLight(const char* light_file, bool async_load);

	// 加载材质文件
	virtual bool LoadMaterial(const char* material_file, bool async_load);

	// 设置指定模型的附加颜色
	virtual	void SetColor(unsigned int cr);
	// 获取指定模型的附加颜色
	virtual	unsigned int GetColor();
	// 设置指定模型是否在水下
	virtual	void SetUnderWater(bool value);
	// 获取指定模型是否在水下
	virtual	bool GetUnderWater();
	// 获取指定模型的帧数，这个数字就是美术在Max制作的模型动画帧数
	virtual unsigned int GetFrameCount();
	// 获取指定模型的原始FPS，这个FPS就是美术在Max制作的模型动画的FPS
	virtual float GetInternalFPS();
	// 设置模型的当前速度
	virtual void SetSpeed(float fSpeed);
	// 获取模型的当前速度
	virtual float GetSpeed();
	// 设置是否进行模型动画的自动循环播放
	virtual void SetLoop(bool bLoop);
	// 判断是否进行模型动画的自动循环播放
	virtual bool GetLoop();
	// 启动模型动画播放
	virtual void Play();
	// 停止模型动画播放
	virtual void Stop();
	// 暂停模型动画播放
	virtual void Pause();
	// 恢复模型动画播放
	virtual void Resume();
	// 设置模型动画的当前播放时间
	virtual void SetCurrentTimeFloat(double time);
	// 获取模型动画的当前播放时间
	virtual double GetCurrentTimeFloat();
	// 设置模型动画的当前帧
	virtual void SetCurrentFrame(unsigned int nFrame);
	// 获取模型动画的当前帧
	virtual unsigned int GetCurrentFrame();
	// 对模型进行辅助点碰撞，这个函数可以有效的选择模型上的辅助点
	virtual void* RayTraceHelper(const FmVec3* start, 
		const FmVec3* direction);

	// 指定是否隐藏当前指定的模型
	virtual void SetHide(bool bHide);
	// 获取当前指定的模型是否被隐藏
	virtual bool GetHide();

	// 是否可活动物体
	virtual void SetLive(bool value);
	virtual bool GetLive();

	// 指定是否当前模型为选择态，在选择态的模型将显示其包围盒
	virtual void SetSelection(bool bSelected);
	// 获取是否当前模型处于选择态
	virtual bool GetSelection();

	// 指定当前模型使用网格模式进行渲染
	//virtual void SetWireframe(bool bWireframe);
	// 获取当前模型是否使用网格模式进行渲染
	//virtual bool GetWireframe();
	// 显示当前模型中包含的辅助点
	virtual void SetShowHelper(bool bShow);
	// 判断当前模型中包含的辅助点是否被显示
	virtual bool GetShowHelper();

	// 获取当前模型中的根节点个数
	virtual unsigned int GetRootNodeCount();
	// 根据当前模型中的根节点索引号来获取结点上下文
	virtual void* GetRootNodeFromID(unsigned int index);
	// 获取当前模型中的结点的个数
	virtual unsigned int GetNodeCount();
	// 根据当前模型中的结点索引号来获取结点上下文
	virtual void* GetNodeFromID(int iNodeID);
	// 根据当前模型中的结点名称来获取结点上下文
	virtual void* GetNodeFromName(const char* pszName);
	// 根据当前模型中的结点上下文获取结点的索引号
	virtual int GetNodeID(void* node);
	// 根据当前模型中的结点上下文获取结点的名称
	virtual const char* GetNodeName(void* node);
	// 根据当前模型中的结点上下文来获取结点的类型
	virtual FXNODE_TYPE GetNodeType(void* node);
	// 根据当前模型中的结点索引号来获取摄像机信息
	virtual float GetCameraNodeFov(void* node);
	virtual float GetCameraNodeFarClip(void* node);
	virtual float GetCameraNodeNearClip(void* node);
	// 是否附属节点
	virtual bool IsAttachedNode(void* node);
	// 获得附属节点的父节点名
	virtual const char* GetAttachedParentName(void* node);
	// 获取当前模型中的结点的当前变换矩阵，这个变换矩阵是相对于当前模型坐标系的
	virtual void GetCurrentNodeTM(void* node, FmMat4* mtxTM);
	// 获取当前模型的变换矩阵
	virtual void GetCurrentTM(FmMat4* mtxTM);

	// 获取当前模型是否存在主模（裸模）
	virtual	bool ExistMainModel();
	// 指定当前模型仅对主模（裸模）进行碰撞检测
	virtual	void SetTraceMainModel(bool value);
	// 获取当前模型是否仅对主模（裸模）进行碰撞检测
	virtual	bool GetTraceMainModel();

	// 指定是否显示当前指定的模型结点
	virtual void SetShowNode(void* node, bool bShow);
	// 获取当前指定的结点是否被显示
	virtual bool GetShowNode(void* node);

	// 获取当前模型的材质个数
	//virtual unsigned int GetMaterialCount();
	// 根据当前模型的材质索引获取材质上下文
	virtual	void* GetMaterialFromID(int iMatIndex);
	// 根据当前模型的材质名称获取材质上下文
	virtual	void* GetMaterialFromName(const char* pszMatName);
	// 根据材质上下文来获取材质索引
	//virtual	int GetMaterialID(void* mat);
	// 获取材质上下文获取材质名称
	virtual	const char* GetMaterialName(void* mat);
	// 获取指定的材质的文理个数
	virtual	unsigned int GetMaterialTextureCount(void* mat);
	// 获取指定的材质的纹理贴图
	virtual const char* GetMaterialTextureName(void* mat, unsigned int index);
	// 获取指定材质的顶点缓冲
	virtual	void* GetMaterialVertexBuffer(void* mat);
	// 获取指定材质的索引缓冲
	virtual	void* GetMaterialIndexBuffer(void* mat);
	// 获取指定材质的顶点大小
	virtual	unsigned int GetMaterialVertexSize(void* mat);
	// 获取指定材质的顶点个数
	virtual	unsigned int GetMaterialVertexCount(void* mat);
	// 获取指定材质的索引大小，一般为sizeof(unsigned short)
	virtual	unsigned int GetMaterialIndexSize(void* mat);
	// 获取指定材质的索引个数
	virtual unsigned int GetMaterialIndexCount(void* mat);
	// 获取指定材质的顶点位置信息在一个顶点中偏移了多少个字节
	virtual int GetPositionOffset(void* mat);
	// 获取指定材质的顶点法线信息在一个顶点中偏移了多少个字节
	virtual int GetNormalOffset(void* mat);
	// 获取指定材质的顶点颜色信息在一个顶点中偏移了多少个字节
	virtual int GetDiffuseOffset(void* mat);
	// 获取指定材质的顶点UV信息在一个顶点中偏移了多少个字节
	virtual int GetUVOffset(void* mat);
	// 获取指定材质的顶点Lightmap纹理坐标信息在一个顶点中偏移了多少个字节
	virtual int GetLightmapUVOffset(void* mat);
	// 获取指定的Diffuse纹理文件名
	virtual	const char* GetTexDiffuseFileName(void* mat);
	// 获取指定的Bump纹理文件名
	virtual	const char* GetTexBumpFileName(void* mat);
	// 获取指定的Specularmap纹理文件名
	virtual	const char* GetTexSpecularmapFileName(void* mat);
	// 获取指定的Specularlevelmap纹理文件名
	virtual	const char* GetTexSpecularlevelmapFileName(void* mat);
	// 获取指定的Glossinessmap纹理文件名
	virtual	const char* GetTexGlossinessmapFileName(void* mat);
	// 获取指定的Reflectionmap纹理文件名
	virtual	const char* GetTexReflectionmapFileName(void* mat);
	// 获取指定的Lightmap纹理文件名
	virtual	const char* GetTexLightmapFileName(void* mat);
	// 获得指定材质的Ambient
	virtual	unsigned int GetMaterialAmbient(void* mat);
	// 获得指定材质的Diffuse
	virtual	unsigned int GetMaterialDiffuse(void* mat);

	// 查找自定义的材质
	virtual bool FindCustomMaterial(const char* mat_name);
	// 设置自定义材质数据
	virtual bool SetCustomMaterialValue(const char* mat_name,
		const char* key, const char* val);
	// 读取自定义材质数据
	virtual bool GetCustomMaterialValue(const char* mat_name,
		const char* key, char* val, int val_size);
	// 重新加载自定义材质的指定贴图
	virtual bool ReloadCustomMaterialTexture(const char* mat_name, 
		const char* key, const char* tex_paths, bool async);
	// 重新加载自定义材质的所有贴图
	virtual bool ReloadCustomMaterialTextures(const char* tex_paths, bool async);

	// 设置模型材质数据
	virtual bool SetModelMaterialValue(const char* mat_name, 
		const char* key, const char* val);
	// 读取模型材质数据
	virtual bool GetModelMaterialValue(const char* mat_name,
		const char* key, char* val, int val_size);
	// 重新加载模型的所有贴图
	virtual bool ReloadMaterialTextures(const char* tex_paths, 
		bool async);

	// 获得模型节点数据是否可编辑
	virtual bool GetModelNodeAllowModify(const char* key);
	// 设置模型节点数据
	virtual bool SetModelNodeValue(const char* node_name, 
		const char* key, const char* val);
	// 读取模型节点数据
	virtual bool GetModelNodeValue(const char* node_name,
		const char* key, char* val, int val_size);

	// 获取指定模型的资源文件名字
	virtual const char* GetModelFileName();
	// 获取指定模型的存储器使用情况描述
	//virtual void GetMemoryStatus(LPFXMEMORY_STATUS status);
	// 将当前模型与指定动作集进行蒙皮映射
	virtual bool InitSkeletonMappingTable();
	// 判断当前模型是否与指定动作集进行了蒙皮映射
	virtual bool IsInitSkeletonMappingTable();
	// 释放当前模型的蒙皮影射
	virtual void ReleaseSkeletonMappingTable();
	// 设置当前模型的指定辅助点为选择态
	virtual void SetSelectHelper(int nHelper);
	// 获取当前模型中处于选择态的辅助点
	virtual int GetSelectHelper();
	// 获取当前模型中包含的辅助点个数
	virtual unsigned int GetHelperCount();
	// 根据当前模型的辅助点索引获取辅助点的名字
	virtual const char* GetHelperIDFromIndex(unsigned int nIndex);
	// 指定是否开启当前指定模型的FallOff(轮廓光)效果
	virtual void SetEnableFallOff(bool value);
	// 判断当前指定的模型是否开启了FallOff(轮廓光)效果
	virtual bool GetEnableFallOff();
	// 指定是否开启当前指定模型的点光源光照效果
	virtual void SetEnablePointLighting(bool value);
	// 判断当前指定的模型是否开启了点光源光照效果
	virtual bool GetEnablePointLighting();
	// 模型当前LOD的级别
	virtual void SetCurLODLevel(unsigned int lod_level);
	virtual unsigned int GetCurLODLevel();
	// 模型总共的LOD级数
	virtual unsigned int GetLODLevels();
	// Warning:This API just for LOD tools use!
	// 指定模型进行LOD并重新保存模型
	//virtual bool LOD(unsigned int lod_levels, float dec_ratio);
	
	// 执行播放
	virtual void Update(float seconds);

	// 同步加载光照
	//bool CreateLight(const char* light);
	// 开始异步创建光照 
	//bool BeginCreateLight(const char* light);
	// 从数据创建光贴图
	//bool BuildLightMap(const char* light, size_t material, DxTexture* pTex);

	// 同步加载材质文件
	bool CreateMaterial(const char* material);
	// 开始异步创建材质文件 
	bool BeginCreateMaterial(const char* material);
	// 从数据创建光贴图
	bool BuildMaterial(IIniFile* pIni);

	// 结束异步创建 
	bool EndCreate(int category, bool succeed);

	// 创建引用
	bool Create();

	// 设置名称
	void SetName(const char* value)
	{ 
		Assert(value != NULL);

		m_strName = value; 
	}

	// 获得名称
	const char* GetName() const
	{
		return m_strName.c_str();
	}

	// 播放起始时间
	void SetBeginTime(double value) { m_dBeginTime = value; }
	double GetBeginTime() const { return m_dBeginTime; }

	// 获得渲染器
	IRender* GetRender() const { return m_pRender; }

	// 获得光贴图数量
	size_t GetLightMapCount() const { return m_LightTexs.size(); }
	
	// 获得指定光贴图
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

	// 设置碰撞信息
	bool SetTraceInfo(trace_info_t* result, model_node_t* pNode, 
		node_material_t* pMat, int tri_id, float u, float v, float t, 
		const FmVec3& p0, const FmVec3& p1, const FmVec3& p2);

private:
	CModelPlayer();
	CModelPlayer(const CModelPlayer&);
	CModelPlayer& operator=(const CModelPlayer&);

	// 更新中心点和半径
	void UpdateCenterRadius();
	// 创建动画蒙皮
	bool CreateSkeletonMapping();
	// 释放光贴图
	void ReleaseLightMaps();
	// 加载光照
	bool InnerLoadLight();
	// 加载材质文件
	bool InnerLoadMaterial();

	// 获得常量缓冲
	//IDxDynamicCB* GetObjectCB(node_material_t* pMat, 
	//	const FmVec4* bone_matrices);
	// 创建骨骼矩阵
	FmVec4* CreateBoneMatrices(node_material_t* pMat);
	void CreateBillboard(model_node_t* pNode);
	// 创建顶点定义
	bool CreateVDecl(node_material_t* pMat, model_node_t* pNode, 
		bool lod);
	// 创建顶点和索引缓冲
	bool CreateVBIB(node_material_t* pMat, model_node_t* pNode);
	// 创建贴图动画矩阵
	void CreateTextureMatrix(node_material_t* pMat,  
		FmMat4* mtxTex);
	// 设置批次的几何体信息
//	bool SetBatchGeometry(DxSceneBatch* pBatch, node_material_t* pMat,
//		model_node_t* pNode);
	// 设置批次的遮挡查询信息
//	bool SetBatchOcclusionQuery(DxSceneBatch* pBatch, node_material_t* pMat,
//		model_node_t* pNode);
	// 获得当前的材质透明度
	float GetCurrentOpacity(node_material_t* pMat, model_node_t* pNode, 
		unsigned int frame_num, float weight);
	// 检查材质数据
	bool CheckMaterialData(node_material_t* pMat, int offset, int stride);
	// 检查材质是否存在无效数据
	void CheckMaterialAllData(node_material_t* pMat);

	// 选取着色器
	//测试
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
	// 渲染模板贴花
	//void DrawApplique(DxSceneBatch* pBatch, DxSceneBatch* pBatchStencil,
	//	node_material_t* pMat, model_node_t* pNode);
	// 渲染体积雾
	//void DrawVolumeFog(DxSceneBatch* pBatch, node_material_t* pMat, 
	//	model_node_t* pNode);
	// 渲染水体材质表面
	//void DrawWaterSurface(DxSceneBatch* pBatch, node_material_t* pMat, 
	//	model_node_t* pNode);
	// 提交渲染深度批次
	//void CommitDepthBatch(node_material_t* pMat, model_node_t* pNode, 
	//	IDxDynamicCB* pCB);
	// 提交泛光批次
	//void CommitGlowBatch(node_material_t* pMat, model_node_t* pNode,
	//	bool fog_linear, bool fog_exp, DxSceneBatch* pSolidBatch, 
	//	IDxDynamicCB* pCB);
	// 提交渲染速度批次
	//void CommitVelocityBatch(node_material_t* pMat, model_node_t* pNode, 
	//	IDxDynamicCB* pCB);
	// 画模型材质
	void DrawMaterial(node_material_t* pMat, model_node_t* pNode, MatInfo* matinfo);
	// 画模型节点
	void DrawNode(model_node_t* pNode, bool bCull, MatInfo* matinfo);
	// 画节点法线
	void DrawNodeNormal(model_t* pModel, model_node_t* pNode);
	// 渲染模型
	void DrawModel(bool bCull, MatInfo* MatList);
	// 画材质阴影图
	void DrawMaterialShadowMap(const MatInfo* info);
	// 画节点的阴影图
	void DrawNodeShadowMap(model_node_t* pNode, bool bCull);
	// 画模型阴影图
	void DrawModelShadowMap(bool bCull);

	// 加载模型引用的指定材质文件
	bool LoadCustomMaterialFile(IIniFile* pIni);
	// 删除模型引用的指定材质
	bool UnloadCustomMaterials();
	// 查找自定义材质
	material_info_t* InnerFindCustomMaterial(node_material_t* pMat);
	// 增加自定义材质
	material_info_t* InnerAddCustomMaterial(node_material_t* pMat);

	// 查找模型的材质
	node_material_t* FindModelMaterial(model_t* pModel,
		const char* mat_name);
	// 模型引用的贴图是否加载完成
	bool CustomTextureLoadComplete();
	// 查找自定义材质
	material_info_t* FindCustomMaterial(node_material_t* pMat);
	// 加载模型引用的所有贴图
	bool LoadCustomMaterialTexture(const char* mat_name, const char* key, 
		const char* tex_paths, bool async);
	// 加载模型引用的所有贴图
	bool LoadCustomTextures(const char* tex_paths, bool async);
	// 卸载模型引用的所有贴图
	bool UnloadCustomTextures();

	// 设置当前使用的世界矩阵
	void SetCurrentMatrix(model_node_t* pNode);
	// 生成骨骼动画模型动态顶点缓冲
	FmVec3* AllocCpuSkeletonAnimationVB(node_material_t* pMat);
	// 对节点进行射线追踪
	void NodeRayTrace(model_t* pModel, model_node_t* pNode, 
		const FmVec3* start, const FmVec3* direction, 
		trace_info_t* result);
	// 对节点进行球体碰撞测试
	bool NodeSphereTrace(model_t* pModel, model_node_t* pNode, 
		const FmVec3* position, float radius);
	// 对节点进行球体碰撞测试
	bool NodeTriangleTrace(model_t* pModel, model_node_t* pNode, 
		const FmVec3* v0, const FmVec3* v1, const FmVec3* v2);
	// 对节点进行面选取
	void NodeCull(model_t* pModel, model_node_t* pNode, 
		const FmPlane* planes, unsigned int plane_num, 
		const FmVec3* center, float radius, void* cull_func, 
		void* cull_context, const FmMat4* cull_matrix);
	// 选取节点的辅助点
	bool NodeRayTraceHelper(model_t* pModel, model_node_t* pNode, 
		const FmVec3* start, const FmVec3* direction, 
		void** result_node, float* result_distance);
	// 对骨骼动画节点进行射线追踪
	void NodeRayTraceSkin(model_t* pModel, model_node_t* pNode, 
		const FmVec3* start, const FmVec3* direction, 
        trace_info_t* result);
    // 对骨骼动画节点进行球碰撞
    bool NodeSphereTraceSkin(model_t* pModel, model_node_t* pNode, 
        const FmVec3* position, float radius);
    // 对骨骼动画节点进行三角形碰撞
    bool NodeTriangleTraceSkin(model_t* pModel, model_node_t* pNode, 
        const FmVec3* v0, const FmVec3* v1, const FmVec3* v2);
	// 根据距离判断是否仅碰撞检测包围盒
	bool OnlyTraceBoundBoxByDistance(const FmVec3* start, 
		const FmVec3* direction, float dis, trace_info_t* result);
	// 模型射线追踪
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

	// 更新节点
	void UpdateModelNode(model_node_t* pNode, model_t* pModel, bool bCull);
	// 更新动画时间
	void UpdateAniTime(double dTime);
	// 更新模型
	void UpdateModel(bool bCull);
	
	//设置用户传入Sahder的常量
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
