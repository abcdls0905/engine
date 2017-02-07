//--------------------------------------------------------------------
// 文件名:		model_player_texture.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2009年5月4日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "model_player.h"
#include "res_model.h"
#include "../visual/i_render.h"
#include "../visual/i_texture.h"
#include "../public/inlines.h"
#include "../public/core_log.h"
#include "../public/core_file.h"

extern IRender* g_pRender;

// 释放材质的所有的贴图
static inline void release_material_textures(material_info_t* pMatInfo)
{
	SAFE_RELEASE(pMatInfo->DiffuseMap.pTex);
	SAFE_RELEASE(pMatInfo->BumpMap.pTex);
	SAFE_RELEASE(pMatInfo->SpecularMap.pTex);
	SAFE_RELEASE(pMatInfo->SpecularLevelMap.pTex);
	SAFE_RELEASE(pMatInfo->GlossinessMap.pTex);
	SAFE_RELEASE(pMatInfo->EmissiveMap.pTex);
	SAFE_RELEASE(pMatInfo->ReflectionMap.pTex);
	SAFE_RELEASE(pMatInfo->DisplacementMap.pTex);
	SAFE_RELEASE(pMatInfo->LightMap.pTex);
	SAFE_RELEASE(pMatInfo->FilterMap.pTex);
}

// 贴图是否加载完成
static inline bool is_texture_load_complete(ITexture* pTexture)
{
	if (NULL == pTexture)
	{
		return true;
	}

	return pTexture->IsLoadComplete();
}

// 材质的所有贴图是否加载完成
static inline bool is_material_textures_load_complete(
	material_info_t* pMatInfo)
{
	if (!is_texture_load_complete(pMatInfo->DiffuseMap.pTex))
	{
		return false;
	}

	if (!is_texture_load_complete(pMatInfo->BumpMap.pTex))
	{
		return false;
	}

	if (!is_texture_load_complete(pMatInfo->SpecularMap.pTex))
	{
		return false;
	}

	if (!is_texture_load_complete(pMatInfo->SpecularLevelMap.pTex))
	{
		return false;
	}

	if (!is_texture_load_complete(pMatInfo->GlossinessMap.pTex))
	{
		return false;
	}

	if (!is_texture_load_complete(pMatInfo->EmissiveMap.pTex))
	{
		return false;
	}

	if (!is_texture_load_complete(pMatInfo->ReflectionMap.pTex))
	{
		return false;
	}

	if (!is_texture_load_complete(pMatInfo->DisplacementMap.pTex))
	{
		return false;
	}

	if (!is_texture_load_complete(pMatInfo->LightMap.pTex))
	{
		return false;
	}

	if (!is_texture_load_complete(pMatInfo->FilterMap.pTex))
	{
		return false;
	}
	return true;
}

// 模型节点的贴图是否加载完成
static bool model_node_texture_load_compelte(model_node_t* pNode)
{
	Assert(pNode != NULL);

	for (unsigned int m = 0; m < pNode->nMaterialCount; m++)
	{
		if (!is_material_textures_load_complete(&pNode->Materials[m].MatInfo))
		{
			return false;
		}
	}

	if (pNode->ChildNodes != NULL)
	{
		for (unsigned int i = 0; i < pNode->nChildNodeCount; ++i)
		{
			if (!model_node_texture_load_compelte(&pNode->ChildNodes[i]))
			{
				return false;
			}
		}
	}

	return true;
}

// 文件是否存在
static inline bool file_exists(const char* name)
{
	// 为了提高查找速度，只在打包的文件中搜索
	return g_pCore->GetFileSys()->FileInPack(name);
}

// 创建贴图
static ITexture* create_texture(const char* fname, const char* tex_name, 
	bool async, bool gamma)
{
	if ((tex_name[0] == 'v') && (tex_name[1] == 'o') && (tex_name[2] == 'l')
		&& (tex_name[3] == '_'))
	{
		return g_pRender->CreateVolumeTexture(fname, async);
	}
	else if ((tex_name[0] == 'c') && (tex_name[1] == 'u') 
		&& (tex_name[2] == 'b') && (tex_name[3] == '_'))
	{
		return g_pRender->CreateCubeTexture(fname, async);
	}
	else
	{
		return g_pRender->CreateTexture(fname, async);
	}
}

inline bool CheckFullPath(const char* path)
{
	if(path == 0)
		return false;
#ifdef _WIN32
	static char* dirv = "cdefghijklmnopqrstuvwxyz";
	for(int i=0; dirv[i]!=0; i++)
	{
		char s[20], s1[20];
		::sprintf(s, "%c:\\", dirv[i]);
		::sprintf(s1, "%c:/", dirv[i]);
		if(strnicmp(path, s, 3) == 0 || strnicmp(path, s1, 3) == 0)
			return true;
	}
#else
	if(path[0] == '/')
		return true;
#endif
	return false;
}


// 加载贴图
static void load_tex(texture_info_t* pTexInfo, const char* tex_base, 
	const char* tex_paths, bool async, bool need_trace, bool gamma = false)
{
	Assert(NULL == pTexInfo->pTex);

	const char* tex_name = pTexInfo->pFileName;
	
	if (NULL == tex_name)
	{
		return;
	}

	char fname[256];

	// 优先使用模型所在路径查找贴图
	bool checkfullpath = false;
	if(checkfullpath = CheckFullPath(tex_name)) //如果是绝对路径就不组合
		SafeSprintf(fname, sizeof(fname), "%s", tex_name);
	else
		SafeSprintf(fname, sizeof(fname), "%s%s", tex_base, tex_name);

	if (tex_paths[0] == 0 || checkfullpath)
	{
		// 没有其他可选路径时不需要判断文件是否存在
		pTexInfo->pTex = create_texture(fname, tex_name, async, gamma);
		return;
	}

	if (file_exists(fname))
	{
		pTexInfo->pTex = create_texture(fname, tex_name, async, gamma);
		return;
	}

	// 使用其他路径
	if (tex_paths[0] != 0)
	{
		char path[256];
		const char* s = tex_paths;
		const char* split = strchr(s, '|');

		while (split != NULL)
		{
			size_t len = split - s;

			if ((len > 0) && (len < sizeof(path)))
			{
				memcpy(path, s, len);
				path[len] = 0;

				SafeSprintf(fname, sizeof(fname), "%s%s%s", 
					g_pRender->GetPathPrefix(path), path, tex_name);

				if (file_exists(fname))
				{
					pTexInfo->pTex = create_texture(fname, tex_name, async, 
						gamma);
					return;
				}
			}

			s = split + 1;
			split = strchr(s, '|');
		}

		if (strlen(s) > 0)
		{
			SafeSprintf(fname, sizeof(fname), "%s%s%s", 
				g_pRender->GetPathPrefix(s), s, tex_name);

			if (file_exists(fname))
			{
				pTexInfo->pTex = create_texture(fname, tex_name, async, gamma);
				return;
			}
		}
	}

#ifdef _DEBUG	
	// 调试版本强制输出贴图不存在的信息
	need_trace = true;
#endif // _DEBUG
	
	if (need_trace)
	{
		char info[128];
		pTexInfo->pTex = create_texture(fname, tex_name, async, gamma);
		SafeSprintf(info, sizeof(info), "(load_tex)texture %s not found",
			tex_name);
		CORE_TRACE(info);
	}
}

// 加载材质的所有贴图
static inline void load_material_textures(material_info_t* pMatInfo, 
	const char* tex_base, const char* tex_paths, bool async)
{
	load_tex(&pMatInfo->DiffuseMap, tex_base, tex_paths, async, true, true);
	load_tex(&pMatInfo->BumpMap, tex_base, tex_paths, async, true);
	load_tex(&pMatInfo->SpecularMap, tex_base, tex_paths, async, true);
	load_tex(&pMatInfo->SpecularLevelMap, tex_base, tex_paths, async, true);
	load_tex(&pMatInfo->GlossinessMap, tex_base, tex_paths, async, true);
	load_tex(&pMatInfo->EmissiveMap, tex_base, tex_paths, async, false);
	load_tex(&pMatInfo->ReflectionMap, tex_base, tex_paths, async, true, true);
	load_tex(&pMatInfo->DisplacementMap, tex_base, tex_paths, async, true, true);
	load_tex(&pMatInfo->LightMap, tex_base, tex_paths, async, false);
	load_tex(&pMatInfo->FilterMap, tex_base, tex_paths, async, true);

//	// 漫反射贴图
//	void* pTexDiffuseMap = pMatInfo->DiffuseMap.pTex;
//
//	if (pMatInfo->DiffuseMap.nFileNameLen > 0)
//	{
//		if (NULL == pTexDiffuseMap)
//		{
//			pMatInfo->nShaderInfo &= ~DIFFUSEMAP_FLAG;
//		}
//		else
//		{
//			pMatInfo->nShaderInfo |= DIFFUSEMAP_FLAG;
//		}
//	}
//
//	// 法线贴图
//	void* pTexBumpMap = pMatInfo->BumpMap.pTex;
//
//	if (pMatInfo->BumpMap.nFileNameLen > 0)
//	{
//		if (NULL == pTexBumpMap)
//		{
//			pMatInfo->nShaderInfo &= ~NORMALMAP_FLAG;
//		}
//	}
//
//	// 高光贴图
//	void* pTexSpecularMap = pMatInfo->SpecularMap.pTex;
//
//	if (pMatInfo->SpecularMap.nFileNameLen > 0)
//	{
//		// 高光亮度贴图
//		void* pTexSpecularLevelMap = pMatInfo->SpecularLevelMap.pTex;
//
//		if (NULL == pTexSpecularMap && NULL == pTexSpecularLevelMap)
//		{
//			pMatInfo->nShaderInfo &= ~SPECULARMAP_FLAG;
//		}
//		else
//		{
//			pMatInfo->nShaderInfo |= SPECULARMAP_FLAG;
//		}
//	}
//
//	// 自发光贴图
//	void* pTexEmissiveMap = pMatInfo->EmissiveMap.pTex;
//
//	if (pMatInfo->EmissiveMap.nFileNameLen > 0)
//	{
//		if (NULL == pTexEmissiveMap)
//		{
//			pMatInfo->nShaderInfo &= ~EMISSIVE_FLAG;
//		}
//		else
//		{
//			if (pMatInfo->bEmissiveEnable || pMatInfo->bSkinEffect)
//			{
//				pMatInfo->nShaderInfo |= EMISSIVE_FLAG;
//			}
//			else
//			{
//				pMatInfo->nShaderInfo &= ~EMISSIVE_FLAG;
//			}
//		}
//	}
}

// 加载模型节点的贴图
static void load_model_node_texture(model_node_t* pNode, const char* tex_base,
	const char* tex_paths, bool async)
{
	Assert(pNode != NULL);

	for (unsigned int m = 0; m < pNode->nMaterialCount; m++)
	{
		load_material_textures(&pNode->Materials[m].MatInfo, tex_base, 
			tex_paths, async);
	}

	if (pNode->ChildNodes != NULL)
	{
		for (unsigned int i = 0; i < pNode->nChildNodeCount; ++i)
		{
			load_model_node_texture(&pNode->ChildNodes[i], tex_base, 
				tex_paths, async);
		}
	}
}

// 卸载模型节点的贴图
static void unload_model_node_texture(model_node_t* pNode)
{
	Assert(pNode != NULL);

	for (unsigned int m = 0; m < pNode->nMaterialCount; m++)
	{
		release_material_textures(&pNode->Materials[m].MatInfo);
	}

	if (pNode->ChildNodes != NULL)
	{
		for (unsigned int i = 0; i < pNode->nChildNodeCount; ++i)
		{
			unload_model_node_texture(&pNode->ChildNodes[i]);
		}
	}
}

// 获得贴图资源指针
//DxTexture* FXGetTexture(void* pTexture)
//{
//	if (NULL == pTexture)
//	{
//		return NULL;
//	}
//
//	ITexture* pTex = (ITexture*)pTexture;
//
//	return pTex->GetDxTexture();
//}

// 获得Cube贴图资源指针
//DxCubeTexture* FXGetCubeTexture(void* pTexture)
//{
//	if (NULL == pTexture)
//	{
//		return NULL;
//	}
//
//	ITexture* pTex = (ITexture*)pTexture;
//
//	return pTex->GetCubeTexture();
//}

// 获得3D贴图资源指针
//DxVolumeTexture* FXGetVolumeTexture(void* pTexture)
//{
//	if (NULL == pTexture)
//	{
//		return NULL;
//	}
//
//	ITexture* pTex = (ITexture*)pTexture;
//
//	return pTex->GetVolumeTexture();
//}

// 获得碰撞检测用的贴图资源指针
//DxTexture* FXGetTraceTexture(void* pTexture)
//{
//	if (NULL == pTexture)
//	{
//		return NULL;
//	}
//
//	ITexture* pTex = (ITexture*)pTexture;
//
//	return pTex->GetTraceTexture();
//}

// 获得贴图占用的内存大小
//unsigned int FXGetTextureSizeInBytes(void* pTexture)
//{
//	if (NULL == pTexture)
//	{
//		return 0;
//	}
//
//	ITexture* pTex = (ITexture*)pTexture;
//
//	return pTex->GetMemSize();
//}

// 模型的贴图是否加载完成
bool model_texture_load_complete(model_t* pModel)
{
	Assert(pModel != NULL);

	if (pModel->RootNodes != NULL)
	{
		for (unsigned int i = 0; i < pModel->nRootNodeCount; ++i)
		{
			if (!model_node_texture_load_compelte(&pModel->RootNodes[i])) 
			{
				return false;
			}
		}
	}

	return true;
}

// 加载模型的所有贴图
bool load_model_texture(model_t* pModel, const char* tex_paths, bool async)
{
	Assert(pModel != NULL);
	Assert(tex_paths != NULL);

	if (pModel->RootNodes != NULL)
	{
		// 加载各个结点的贴图
		for (unsigned int i = 0; i < pModel->nRootNodeCount; ++i)
		{
			load_model_node_texture(&pModel->RootNodes[i], pModel->pszTexBase, 
				tex_paths, async);
		}
	}

	return true;
}

// 卸载模型的所有贴图
bool unload_model_texture(model_t* pInstance)
{
	Assert(pInstance != NULL);

	if (pInstance->RootNodes != NULL)
	{
		// 释放各个结点的贴图
		for (unsigned int i = 0; i < pInstance->nRootNodeCount; ++i)
		{
			unload_model_node_texture(&pInstance->RootNodes[i]);
		}
	}

	return true;
}

// 模型引用的贴图是否加载完成
bool CModelPlayer::CustomTextureLoadComplete()
{
	for (unsigned int k = 0; k < m_nCustomMaterialNum; ++k)
	{
		if (!is_material_textures_load_complete(
			&m_pCustomMaterials[k].MatInfo))
		{
			return false;
		}
	}

	return true;
}

// 查找自定义材质
material_info_t* CModelPlayer::FindCustomMaterial(node_material_t* pMat)
{
	for (size_t i = 0; i < m_nCustomMaterialNum; ++i)
	{
		if (m_pCustomMaterials[i].pMaterialName == pMat->pszName)
		{
			return &m_pCustomMaterials[i].MatInfo;
		}
	}

	return NULL;
}

// 加载模型引用的所有贴图
bool CModelPlayer::LoadCustomMaterialTexture(const char* mat_name, const char* key, 
	const char* tex_paths, bool async)
{
	Assert(mat_name != NULL);
	Assert(key != NULL);
	Assert(tex_paths != NULL);

	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::LoadCustomMaterialTexture] pModel == NULL model name is %s", m_pResModel->GetName());
		return false;
	}

	node_material_t* pMat = FindModelMaterial(pModel, mat_name);

	if (NULL == pMat)
	{
		return false;
	}

	material_info_t* pMatInfo = FindCustomMaterial(pMat);

	if (NULL == pMatInfo)
	{
		return false;
	}

	const char* tex_base = pModel->pszTexBase;

	if (stricmp(key, "DiffuseMap") == 0)
	{
		SAFE_RELEASE(pMatInfo->DiffuseMap.pTex);

		char* pFileName = pMatInfo->DiffuseMap.pFileName;

		if (pFileName)
		{
			if (g_pRender->FindTexture(pFileName) != NULL)
			{
				// 使用已经在内存中的贴图资源，有可能是动态贴图
				pMatInfo->DiffuseMap.pTex = create_texture(pFileName, 
					pFileName, true, true);
			}
			else
			{
				load_tex(&pMatInfo->DiffuseMap, tex_base, tex_paths, async, 
					true, true);
			}
		}
	}
	else if (stricmp(key, "BumpMap") == 0)
	{
		SAFE_RELEASE(pMatInfo->BumpMap.pTex);
		load_tex(&pMatInfo->BumpMap, tex_base, tex_paths, async, true);
	}
	else if (stricmp(key, "SpecularMap") == 0)
	{
		SAFE_RELEASE(pMatInfo->SpecularMap.pTex);
		load_tex(&pMatInfo->SpecularMap, tex_base, tex_paths, async, true);
	}
	else if (stricmp(key, "SpecularLevelMap") == 0)
	{
		SAFE_RELEASE(pMatInfo->SpecularLevelMap.pTex);
		load_tex(&pMatInfo->SpecularLevelMap, tex_base, tex_paths, async, true);
	}
	else if (stricmp(key, "GlossinessMap") == 0)
	{
		SAFE_RELEASE(pMatInfo->GlossinessMap.pTex);
		load_tex(&pMatInfo->GlossinessMap, tex_base, tex_paths, async, true);
	}
	else if (stricmp(key, "EmissiveMap") == 0)
	{
		SAFE_RELEASE(pMatInfo->EmissiveMap.pTex);
		load_tex(&pMatInfo->EmissiveMap, tex_base, tex_paths, async, true);
	}
	else if (stricmp(key, "ReflectionMap") == 0)
	{
		SAFE_RELEASE(pMatInfo->ReflectionMap.pTex);
		load_tex(&pMatInfo->ReflectionMap, tex_base, tex_paths, async, true, 
			true);
	}
	else if (stricmp(key, "DisplacementMap") == 0)
	{
		SAFE_RELEASE(pMatInfo->DisplacementMap.pTex);
		load_tex(&pMatInfo->DisplacementMap, tex_base, tex_paths, async, true, 
			true);
	}
	else if (stricmp(key, "LightMap") == 0)
	{
		SAFE_RELEASE(pMatInfo->LightMap.pTex);
		load_tex(&pMatInfo->LightMap, tex_base, tex_paths, async, true);
	}
	else if (stricmp(key, "FilterMap") == 0)
	{
		SAFE_RELEASE(pMatInfo->FilterMap.pTex);
		load_tex(&pMatInfo->FilterMap, tex_base, tex_paths, async, true);
	}
	else
	{
		return false;
	}

//	// 漫反射贴图
//	void* pTexDiffuseMap = pMatInfo->DiffuseMap.pTex;
//
//	if (pMatInfo->DiffuseMap.nFileNameLen > 0)
//	{
//		if (NULL == pTexDiffuseMap)
//		{
//			pMatInfo->nShaderInfo &= ~DIFFUSEMAP_FLAG;
//		}
//		else
//		{
//			pMatInfo->nShaderInfo |= DIFFUSEMAP_FLAG;
//		}
//	}
//
//	// 法线贴图
//	void* pTexBumpMap = pMatInfo->BumpMap.pTex;
//
//	if (pMatInfo->BumpMap.nFileNameLen > 0)
//	{
//		if (NULL == pTexBumpMap)
//		{
//			pMatInfo->nShaderInfo &= ~NORMALMAP_FLAG;
//		}
//	}
//
//	// 高光贴图
//	void* pTexSpecularMap = pMatInfo->SpecularMap.pTex;
//
//	if (pMatInfo->SpecularMap.nFileNameLen > 0)
//	{
//		// 高光亮度贴图
//		void* pTexSpecularLevelMap = pMatInfo->SpecularLevelMap.pTex;
//
//		if (NULL == pTexSpecularMap && NULL == pTexSpecularLevelMap)
//		{
//			pMatInfo->nShaderInfo &= ~SPECULARMAP_FLAG;
//		}
//		else
//		{
//			pMatInfo->nShaderInfo |= SPECULARMAP_FLAG;
//		}
//	}
//
//	// 自发光贴图
//	void* pTexEmissiveMap = pMatInfo->EmissiveMap.pTex;
//
//	if (pMatInfo->EmissiveMap.nFileNameLen > 0)
//	{
//		if (NULL == pTexEmissiveMap)
//		{
//			pMatInfo->nShaderInfo &= ~EMISSIVE_FLAG;
//		}
//		else
//		{
//			if (pMatInfo->bEmissiveEnable || pMatInfo->bSkinEffect)
//			{
//				pMatInfo->nShaderInfo |= EMISSIVE_FLAG;
//			}
//			else
//			{
//				pMatInfo->nShaderInfo &= ~EMISSIVE_FLAG;
//			}
//		}
//	}

	return true;
}

// 加载模型引用的所有贴图
bool CModelPlayer::LoadCustomTextures(const char* tex_paths, bool async)
{
	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::LoadCustomTextures] pModel == NULL model name is %s", m_pResModel->GetName());
		return false;
	}
	
	for (unsigned int k = 0; k < m_nCustomMaterialNum; ++k)
	{
		load_material_textures(&m_pCustomMaterials[k].MatInfo, 
			pModel->pszTexBase, tex_paths, async);
	}

	return true;
}

// 卸载模型引用的所有贴图
bool CModelPlayer::UnloadCustomTextures()
{
	for (unsigned int k = 0; k < m_nCustomMaterialNum; ++k)
	{
		release_material_textures(&m_pCustomMaterials[k].MatInfo);
	}
	
	return true;
}

bool CModelPlayer::ReloadCustomMaterialTexture(const char* mat_name, 
	const char* key, const char* tex_paths, bool async)
{
	return LoadCustomMaterialTexture(mat_name, key, tex_paths, async); 
}

bool CModelPlayer::ReloadCustomMaterialTextures(const char* tex_paths, 
	bool async)
{
	UnloadCustomTextures();

	return LoadCustomTextures(tex_paths, async);
}

bool CModelPlayer::ReloadMaterialTextures(const char* tex_paths, bool async)
{
	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::ReloadMaterialTextures] pModel == NULL model name is %s", m_pResModel->GetName());
		return false;
	}

	unload_model_texture(pModel);

	return load_model_texture(pModel, tex_paths, async);
}

bool CModelPlayer::GetModelNodeAllowModify(const char* key)
{
	return get_model_node_allow_modify(key);
}

bool CModelPlayer::SetModelNodeValue(const char* node_name, const char* key, 
	const char* val)
{
	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::SetModelNodeValue] pModel == NULL model name is %s", m_pResModel->GetName());
		return false;
	}

	model_node_t* pNode = find_model_node(pModel, node_name);

	if (NULL == pNode)
	{
		return false;
	}

	return set_model_node_value(pNode, key, val);
}

bool CModelPlayer::GetModelNodeValue(const char* node_name, 
	const char* key, char* val, int val_size)
{
	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::GetModelNodeValue] pModel == NULL model name is %s", m_pResModel->GetName());
		return false;
	}

	model_node_t* pNode = find_model_node(pModel, node_name);

	if (NULL == pNode)
	{
		return false;
	}

	return get_model_node_value(pNode, key, val, val_size);
}
