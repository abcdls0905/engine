//--------------------------------------------------------------------
// �ļ���:		model_player_texture.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2009��5��4��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "model_player.h"
#include "res_model.h"
#include "../visual/i_render.h"
#include "../visual/i_texture.h"
#include "../public/inlines.h"
#include "../public/core_log.h"
#include "../public/core_file.h"

extern IRender* g_pRender;

// �ͷŲ��ʵ����е���ͼ
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

// ��ͼ�Ƿ�������
static inline bool is_texture_load_complete(ITexture* pTexture)
{
	if (NULL == pTexture)
	{
		return true;
	}

	return pTexture->IsLoadComplete();
}

// ���ʵ�������ͼ�Ƿ�������
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

// ģ�ͽڵ����ͼ�Ƿ�������
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

// �ļ��Ƿ����
static inline bool file_exists(const char* name)
{
	// Ϊ����߲����ٶȣ�ֻ�ڴ�����ļ�������
	return g_pCore->GetFileSys()->FileInPack(name);
}

// ������ͼ
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


// ������ͼ
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

	// ����ʹ��ģ������·��������ͼ
	bool checkfullpath = false;
	if(checkfullpath = CheckFullPath(tex_name)) //����Ǿ���·���Ͳ����
		SafeSprintf(fname, sizeof(fname), "%s", tex_name);
	else
		SafeSprintf(fname, sizeof(fname), "%s%s", tex_base, tex_name);

	if (tex_paths[0] == 0 || checkfullpath)
	{
		// û��������ѡ·��ʱ����Ҫ�ж��ļ��Ƿ����
		pTexInfo->pTex = create_texture(fname, tex_name, async, gamma);
		return;
	}

	if (file_exists(fname))
	{
		pTexInfo->pTex = create_texture(fname, tex_name, async, gamma);
		return;
	}

	// ʹ������·��
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
	// ���԰汾ǿ�������ͼ�����ڵ���Ϣ
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

// ���ز��ʵ�������ͼ
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

//	// ��������ͼ
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
//	// ������ͼ
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
//	// �߹���ͼ
//	void* pTexSpecularMap = pMatInfo->SpecularMap.pTex;
//
//	if (pMatInfo->SpecularMap.nFileNameLen > 0)
//	{
//		// �߹�������ͼ
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
//	// �Է�����ͼ
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

// ����ģ�ͽڵ����ͼ
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

// ж��ģ�ͽڵ����ͼ
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

// �����ͼ��Դָ��
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

// ���Cube��ͼ��Դָ��
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

// ���3D��ͼ��Դָ��
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

// �����ײ����õ���ͼ��Դָ��
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

// �����ͼռ�õ��ڴ��С
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

// ģ�͵���ͼ�Ƿ�������
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

// ����ģ�͵�������ͼ
bool load_model_texture(model_t* pModel, const char* tex_paths, bool async)
{
	Assert(pModel != NULL);
	Assert(tex_paths != NULL);

	if (pModel->RootNodes != NULL)
	{
		// ���ظ���������ͼ
		for (unsigned int i = 0; i < pModel->nRootNodeCount; ++i)
		{
			load_model_node_texture(&pModel->RootNodes[i], pModel->pszTexBase, 
				tex_paths, async);
		}
	}

	return true;
}

// ж��ģ�͵�������ͼ
bool unload_model_texture(model_t* pInstance)
{
	Assert(pInstance != NULL);

	if (pInstance->RootNodes != NULL)
	{
		// �ͷŸ���������ͼ
		for (unsigned int i = 0; i < pInstance->nRootNodeCount; ++i)
		{
			unload_model_node_texture(&pInstance->RootNodes[i]);
		}
	}

	return true;
}

// ģ�����õ���ͼ�Ƿ�������
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

// �����Զ������
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

// ����ģ�����õ�������ͼ
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
				// ʹ���Ѿ����ڴ��е���ͼ��Դ���п����Ƕ�̬��ͼ
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

//	// ��������ͼ
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
//	// ������ͼ
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
//	// �߹���ͼ
//	void* pTexSpecularMap = pMatInfo->SpecularMap.pTex;
//
//	if (pMatInfo->SpecularMap.nFileNameLen > 0)
//	{
//		// �߹�������ͼ
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
//	// �Է�����ͼ
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

// ����ģ�����õ�������ͼ
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

// ж��ģ�����õ�������ͼ
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
