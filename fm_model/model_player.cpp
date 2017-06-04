//--------------------------------------------------------------------
// 文件名:		model_player.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2009年5月4日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "model_player.h"
#include "action_player.h"
#include "res_model.h"
#include "res_manager.h"
#include "material_loader.h"
#include "math_3d.h"
#include "../visual/i_render.h"
#include "../visual/i_resource.h"
#include "../visual/i_texture.h"
#include "../visual/vis_utils.h"
#include "../public/i_ini_file.h"
#include "../public/inlines.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/i_file_sys.h"

#define MATERIAL_NUMBER_MAX 1024

// 获得引用编号
static unsigned int get_ref_id()
{
	static unsigned int s_ref_id = 0;

	return ++s_ref_id;
}

// CModelPlayer

CModelPlayer* CModelPlayer::NewInstance(IRender* pRender, 
	CResModel* pResModel)
{
	CModelPlayer* p = (CModelPlayer*)CORE_ALLOC(sizeof(CModelPlayer));

	new (p) CModelPlayer(pRender, pResModel);

	return p;
}

CModelPlayer::CModelPlayer(IRender* pRender, CResModel* pResModel)
{
	Assert(pRender != NULL);
	Assert(pResModel != NULL);
	
	m_pRender = pRender;
	
	m_KeepOutTexture = NULL;
	m_bKeepOut = false;
	m_pContext = NULL;
	m_pResModel = pResModel;
	m_pActionPlayer = NULL;
	m_dBeginTime = 0.0;
	m_dCurrentTime = 0.0;
	m_dLastTime = 0.0;
	m_pMaterialLoader = NULL;
	m_nCustomMaterialNum = 0;
	m_pCustomMaterials = NULL;
	m_pCurMatInfo = NULL;
	m_vScale = FmVec3(1.0F, 1.0F, 1.0F);
	m_vBoxSize = FmVec3(0.0F, 0.0F, 0.0F);
	m_vCenter = FmVec3(0.0F, 0.0F, 0.0F);
	m_Vec3DefaultValve = FmVec3(1.0f, 1.0f, 1.0f);
	m_fRadius = 0.0F;
	m_nCurrentFrame = 0;
	m_fCurrentFrameOffset = 0.0F;
	m_fSpeed = 1.0F;
	m_nRefId = 0;
	m_nColor = 0xFFFFFFFF;
	m_nState = 0;
	m_nSelectHelper = -1;
	m_nWriteVelocity = 0;
	m_bAsyncLoad = false;
	m_bShowBoundBox = false;
	m_bSkeletonMapped = false;
	m_bUseVertexColor = false;
	m_bUseLightMap = false;
	m_bLoop = true;
	m_bSelected = false;
	m_bTraceMainModel = false;
	m_bHide = false;
	m_bLive = false;
	m_bShowHelper = false;
	m_bUnderWater = false;
	m_bFallOff = true;
	m_bPointLighting = true;
	m_bNeedDraw = false;
	m_bCastShadow = false;
	m_bReceiveShadow = false;
	FmMatrixIdentity(&m_mtxWorldTM);
	FmMatrixIdentity(&m_mtxPrevWorldTM);
	FmMatrixIdentity(&m_mtxCurrentTM);
	m_mtxNodeCurrentTM = NULL;
	m_pCacheSkins = NULL;
	m_pCacheBones = NULL;
	m_MatList = NULL;
//	m_MatListReflect = NULL;
//	m_MatListDynamicShadow = NULL;
}

CModelPlayer::~CModelPlayer()
{
	UnloadCustomMaterials();
#ifdef CPU_COMPUTE_BONE
	if (m_MatList)
	{
		size_t count = GetMaterialCount();
		for(size_t i = 0; i < count; i++)
		{
			if(m_MatList[i].pMat)
			{
				if(m_MatList[i].pMat->SingleVB.pDynamicVertices)
				{
					delete(m_MatList[i].pMat->SingleVB.pDynamicVertices);
					m_MatList[i].pMat->SingleVB.pDynamicVertices = NULL;
				}
			}
		}
	}
#endif
	delete[] m_MatList;
//	if (m_MatListReflect)
//		delete[] m_MatListReflect;

//	if (m_MatListDynamicShadow)
//		delete[] m_MatListDynamicShadow;
	
	ReleaseLightMaps();

	for (size_t j = 0; j < m_AtlasLightMaps.size(); ++j)
	{
		atlas_light_map_t* pAtlas = m_AtlasLightMaps[j];
		
		if (pAtlas)
		{
			CORE_FREE(pAtlas, pAtlas->nSize);
		}
	}

//	for (size_t k = 0; k < m_MaterialCBs.size(); ++k)
//	{
//		SAFE_RELEASE(m_MaterialCBs[k]);
//	}

	model_t* pModel = m_pResModel->GetModelData();

	if (m_mtxNodeCurrentTM && pModel)
	{
		model_t* pModel = m_pResModel->GetModelData();

		CORE_FREE(m_mtxNodeCurrentTM, sizeof(FmMat4) * pModel->nNodeCount);
	}

	if (m_pCacheSkins && pModel)
	{
		for (size_t t2 = 0; t2 < pModel->nMaterialCount; ++t2)
		{
			CORE_FREE(m_pCacheSkins[t2].pSkinVectors, 
				m_pCacheSkins[t2].nSkinVectorNum * sizeof(FmVec3));
		}

		CORE_FREE(m_pCacheSkins, sizeof(cache_skin_t) * pModel->nMaterialCount);
	}

	if (m_pCacheBones && pModel)
	{
		for (size_t t1 = 0; t1 < pModel->nMaterialCount; ++t1)
		{
			CORE_FREE(m_pCacheBones[t1].pBoneMatrices, 
				m_pCacheBones[t1].nBoneMatrixNum * sizeof(FmVec4) * 3);
		}

		CORE_FREE(m_pCacheBones, sizeof(cache_bone_t) * pModel->nMaterialCount);
	}

	//m_AtlasLightMaps.clear();
	m_pResModel->Release();
	SAFE_RELEASE(m_pMaterialLoader);
}

void CModelPlayer::Release()
{
	if (this->DecRefs() == 0)
	{
		CORE_DELETE(this);
	}
}

bool CModelPlayer::IsReady()
{
	if (!m_pResModel->IsReady())
	{
		return false;
	}

	return true;
}

bool CModelPlayer::IsLoadComplete()
{
	if (!m_pResModel->IsLoadComplete())
	{
		return false;
	}

	if (!m_pResModel->GetTextureLoadComplete())
	{
		return false;
	}

	//if (m_pLightLoader)
	//{
	//	return false;
	//}

	if (m_pMaterialLoader)
	{
		return false;
	}

	if (!CustomTextureLoadComplete())
	{
		return false;
	}

	// add by simon: 修改模型资源异步加载，导致第一次读模型包围盒信息出错的问题
	if (0 == m_nState)
	{
		Create();
	}

	return true;
}

//是否创建成功
bool CModelPlayer::IsCreate()
{
	return m_nState != 0;
}

void CModelPlayer::SetContext(IRenderContext* value)
{
	Assert(value != NULL);

	m_pContext = value;
}

void CModelPlayer::GetNodeLocationBoundingBox(FmVec3& bbDstMin, 
	FmVec3& bbDstMax, FmVec3& bbDstCenter)
{
	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::Draw] pModel == NULL model name is %s", m_pResModel->GetName());
		return ;
	}

	GetNodeLocationBoundingBox(bbDstMin, bbDstMax, bbDstCenter, pModel->vRootMin, pModel->vRootMax, pModel->vCenter);
}

void CModelPlayer::GetNodeLocationBoundingBox( 
	FmVec3& bbDstMin, FmVec3& bbDstMax, FmVec3& bbDstCenter, 
	const FmVec3& bbSrcMin, const FmVec3& bbSrcMax, 
	const FmVec3& bbSrcCenter)
{
	bbDstMin = bbSrcMin;
	bbDstMax = bbSrcMax;
	bbDstCenter = bbSrcCenter;

	if (m_pActionPlayer)
	{
		if (NULL == m_pActionPlayer->GetParentAction())
		{
			return;
		}

		skt_pose_t* pose = m_pActionPlayer->GetSkeletonPose();

		Assert(pose != NULL);

		FmVec3 pos;

		m_pActionPlayer->GetRootBoneOriginPosition(&pos);

		FmMat4 mtxRoot;

		get_current_root_node_tm(pose, &mtxRoot);

		bbDstMin.x += mtxRoot._41 - pos.x;
		bbDstMin.y += mtxRoot._42 - pos.y;
		bbDstMin.z += mtxRoot._43 - pos.z;
		bbDstMax.x += mtxRoot._41 - pos.x;
		bbDstMax.y += mtxRoot._42 - pos.y;
		bbDstMax.z += mtxRoot._43 - pos.z;
		bbDstCenter.x += mtxRoot._41 - pos.x;
		bbDstCenter.y += mtxRoot._42 - pos.y;
		bbDstCenter.z += mtxRoot._43 - pos.z;
	}
}

bool CModelPlayer::Draw(const FmPlane* planes, 
	size_t plane_num)
{
	if (!IsReady())
	{
		return false;
	}

	if (m_pActionPlayer)
	{
		if (!m_pActionPlayer->IsReady())
		{
			return false;
		}

		if (!m_bSkeletonMapped)
		{
			if (!CreateSkeletonMapping())
			{
				return false;
			}
		}
	}

	UpdateModel(true);
	//MatInfo* pMat = m_MatInfoMan.Create(GetMaterialCount());
	//MatInfo* pMat = new MatInfo[GetMaterialCount()];
	DrawModel(true, 0);

	if (m_bShowBoundBox && false)
	{
		model_t* pModel = m_pResModel->GetModelData();

		if (pModel == NULL)
		{
			CORE_TRACE_EX("WARNING: [CModelPlayer::Draw] pModel == NULL model name is %s", m_pResModel->GetName());
			return false;
		}

		FmVec3 vBoundMin;
		FmVec3 vBoundMax;
		FmVec3 vCenter;

		GetNodeLocationBoundingBox(vBoundMin, vBoundMax, vCenter, 
			pModel->vRootMin, pModel->vRootMax, pModel->vCenter);

		m_pRender->DrawBoundBox(m_mtxWorldTM, vCenter, vBoundMax - vBoundMin, 0xFF00FF00);
	}

	return true;
}

bool CModelPlayer::DrawReflect(const FmPlane* planes, size_t plane_num)
{
	if (!IsReady())
	{
		return false;
	}

	if (m_pActionPlayer)
	{
		if (!m_pActionPlayer->IsReady())
		{
			return false;
		}

		if (!m_bSkeletonMapped)
		{
			if (!CreateSkeletonMapping())
			{
				return false;
			}
		}
	}

	UpdateModel(true);
	DrawModel(true, 0);

	if (m_bShowBoundBox)
	{
		model_t* pModel = m_pResModel->GetModelData();

		if (pModel == NULL)
		{
			CORE_TRACE_EX("WARNING: [CModelPlayer::DrawReflect] pModel == NULL model name is %s", m_pResModel->GetName());
			return false;
		}

		FmVec3 vBoundMin;
		FmVec3 vBoundMax;
		FmVec3 vCenter;

		GetNodeLocationBoundingBox(vBoundMin, vBoundMax, vCenter, 
			pModel->vRootMin, pModel->vRootMax, pModel->vCenter);

		m_pRender->DrawBoundBox(m_mtxWorldTM, vCenter, vBoundMax - vBoundMin, 0xFF00FF00);
	}

	return true;
}

bool CModelPlayer::DrawShadowMap(const FmPlane* planes, size_t plane_num)
{
	if (!IsReady())
	{
		return false;
	}

	if (m_pActionPlayer)
	{
		if (!m_pActionPlayer->IsReady())
		{
			return false;
		}

		if (!m_bSkeletonMapped)
		{
			if (!CreateSkeletonMapping())
			{
				return false;
			}
		}
	}

	UpdateModel(true);
	DrawModelShadowMap(true);

	return true;
}

bool CModelPlayer::TraceDetail(const FmVec3& src, 
	const FmVec3& dst, trace_info_t& result)
{
	if (NULL == m_pResModel)
	{
		return false;
	}
	
	FmVec3 direction = dst - src;
	bool can_trace_box = (NULL == m_pActionPlayer) && (m_fRadius < 2.0F);
	int trace_alpha_ref = m_pRender->GetTraceAlphaRef();

	if (trace_alpha_ref > 0)
	{
		if (!ModelRayTraceAlpha(can_trace_box, &src, &direction, &result))
		{
			return false;
		}
	}
	else
	{
		if (!ModelRayTrace(can_trace_box, &src, &direction, &result))
		{
			return false;
		}
	}

	return true;
}

bool CModelPlayer::TraceHitted(const FmVec3& src, const FmVec3& dst)
{
	if (NULL == m_pResModel)
	{
		return false;
	}

	FmVec3 direction = dst - src;
	int trace_alpha_ref = m_pRender->GetTraceAlphaRef();

	if (trace_alpha_ref > 0)
	{
		if (!ModelRayTraceFastAlpha(&src, &direction))
		{
			return false;
		}
	}
	else
	{
		if (!ModelRayTraceFast(&src, &direction))
		{
			return false;
		}
	}

	return true;
}

bool CModelPlayer::TraceSphere(const FmVec3& center, float radius)
{ 
	if (NULL == m_pResModel)
	{
		return false;
	}

	float r2 = m_fRadius + radius;

	
	FmVec3 len = m_vCenter - center;
	if (FmVec3LengthSq(&len) > (r2 * r2))
	{
		return false;
	}

	return ModelSphereTrace(&center, radius) == true;
}

// 三角形碰撞
bool CModelPlayer::TraceTriangle(const FmVec3& v0, const FmVec3& v1, const FmVec3& v2)
{
    if (NULL == m_pResModel)
    {
        return false;
    }

    return ModelTriangleTrace(&v0, &v1, &v2);
}

bool CModelPlayer::Cull(const FmPlane* planes, size_t plane_num, 
	const FmVec3& center, float radius, CULL_FUNC cull_func, 
	void* context, const FmMat4& mat)
{
	if (NULL == m_pResModel)
	{
		return false;
	}

	ModelCull(planes, (unsigned int)plane_num, &center, radius, (void*)cull_func, 
		context, &mat);

	return true;
}

bool CModelPlayer::GetLightingEnable()
{
	return (NULL == m_pActionPlayer);
}

bool CModelPlayer::GetLightMapEnable()
{
	if (NULL == m_pResModel)
	{
		return false;
	}

	int object_num = (int)GetMaterialCount();

	for (int i = 0; i < object_num; ++i)
	{
		node_material_t* pMat = (node_material_t*)GetMaterialFromID(i);
		model_node_t* pNode = (model_node_t*)pMat->pNode;

		Assert(pMat != NULL);
		Assert(pNode != NULL);

		if ((pMat->nMaterialInfo & MATERIAL_LIGHTMAP_INFO) 
			&& (FXNODE_TYPE_MESH == pNode->nType))
		{
			return true;
		}
	}
	
	return false;
}

bool CModelPlayer::GetNormalMapEnable()
{
	if (NULL == m_pResModel)
	{
		return false;
	}

	int object_num = (int)GetMaterialCount();

	for (int i = 0; i < object_num; ++i)
	{
		node_material_t* pMat = (node_material_t*)GetMaterialFromID(i);
		const char* bump_file_name = pMat->MatInfo.BumpMap.pFileName;
		
		if ((bump_file_name != NULL) && (bump_file_name[0] != 0))
		{
			return true;
		}
	}

	return false;
}

bool CModelPlayer::GetVertexColorEnable()
{
	return false;
}

FmVec3 CModelPlayer::GetBoxSize()
{
	return m_vBoxSize;
}

size_t CModelPlayer::GetLabelCount()
{
	if (NULL == m_pResModel)
	{
		return 0;
	}

	return GetNodeCount();
}

const char* CModelPlayer::GetLabelGroup(size_t index)
{
	return "";
}

const char* CModelPlayer::GetLabelName(size_t index)
{
	if (NULL == m_pResModel)
	{
		return "";
	}

	model_t* pInst = m_pResModel->GetModelData();

	if (pInst == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::GetLabelName] pInst == NULL model name is %s", m_pResModel->GetName());
		return "";
	}

	if (index >= pInst->nNodeCount)
	{
		return "";
	}

	model_node_t* pNode = pInst->pNodeList[index];

	return pNode->pszName;
}

int CModelPlayer::FindLabelIndex(const char* group, const char* name)
{
	return FindLabelNameIndex(name);
}

int CModelPlayer::FindLabelNameIndex(const char* name)
{
	if (NULL == m_pResModel)
	{
		return -1;
	}

	model_t* pInst = m_pResModel->GetModelData();

	if (pInst == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::FindLabelNameIndex] pInst == NULL model name is %s", m_pResModel->GetName());
		return -1;
	}

	unsigned int node_num = pInst->nNodeCount;
	model_node_t** node_list = pInst->pNodeList;

	for (unsigned int i = 0; i < node_num; ++i)
	{
		model_node_t* pNode = node_list[i];

		if (strcmp(pNode->pszName, name) == 0)
		{
			return (int)i;
		}
	}

	return -1;
}

bool CModelPlayer::GetLabelMatrix(size_t index, FmMat4& mat)
{
	if (NULL == m_pResModel)
	{
		return false;
	}

	void* pNode = GetNodeFromID((int)index);

	if (pNode == NULL)
	{
		return false;
	}

	if(m_nState == 0)
		return false;
	GetCurrentNodeTM(pNode, &mat);

	return true;
}

int CModelPlayer::GetLabelBoneId(size_t index)
{
	return -1;
}

size_t CModelPlayer::GetObjectCount()
{
	if (NULL == m_pResModel)
	{
		return 0;
	}

	return GetMaterialCount();
}

size_t CModelPlayer::GetObjectVertexCount(size_t index)
{
	if (NULL == m_pResModel)
	{
		return 0;
	}

	void* pMat = GetMaterialFromID((int)index);

	return GetMaterialVertexCount(pMat);
}

size_t CModelPlayer::GetObjectStartVertex(size_t index)
{
	return 0;
}

size_t CModelPlayer::GetObjectTriangleCount(size_t index)
{
	if (NULL == m_pResModel)
	{
		return 0;
	}

	void* pMat = GetMaterialFromID((int)index);

	return GetMaterialIndexCount(pMat) / 3;
}

size_t CModelPlayer::GetObjectVerterBufferIndex(size_t index)
{
	return index;
}

size_t CModelPlayer::GetObjectMaterial(size_t index)
{
	return index;
}

unsigned short* CModelPlayer::GetObjectIndexData(size_t index)
{
	if (NULL == m_pResModel)
	{
		return NULL;
	}

	void* pMat = GetMaterialFromID((int)index);

	return (unsigned short*)GetMaterialIndexBuffer(pMat);
}

bool CModelPlayer::GetObjectMatrix(size_t index, FmMat4& mat)
{
	if (NULL == m_pResModel)
	{
		return false;
	}

	void* pNode = GetNodeFromID((int)index);

	GetCurrentNodeTM(pNode, &mat);
	FmMatrixMultiply(&mat, &mat, &m_mtxWorldTM);

	return true;
}

size_t CModelPlayer::GetMaterialCount()
{
	if (NULL == m_pResModel)
	{
		return 0;
	}

	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::GetMaterialCount] pModel == NULL model name is %s", m_pResModel->GetName());
		return 0;
	}

	return pModel->nMaterialCount;
	//return GetMaterialCount();
}

size_t CModelPlayer::GetMaterialTextureCount(size_t index)
{
	if (NULL == m_pResModel)
	{
		return 0;
	}

	void* pMat = GetMaterialFromID((int)index);

	return GetMaterialTextureCount(pMat);
}

bool CModelPlayer::GetMaterialLightmapEnable(size_t mat_index)
{
	if (NULL == m_pResModel)
	{
		return false;
	}

	node_material_t* pMat = (node_material_t*)GetMaterialFromID((int)mat_index);
	model_node_t* pNode = (model_node_t*)pMat->pNode;

	Assert(pMat != NULL);
	Assert(pNode != NULL);

	if ((pMat->nMaterialInfo & MATERIAL_LIGHTMAP_INFO) 
		&& (FXNODE_TYPE_MESH == pNode->nType))
	{
		return true;
	}

	return false;
}

const char* CModelPlayer::GetMaterialTextureName(size_t mat_index, 
	size_t tex_index)
{
	if (NULL == m_pResModel)
	{
		return "";
	}

	void* pMat = GetMaterialFromID((int)mat_index);

	return GetMaterialTextureName(pMat, (int)tex_index);
}

const char* CModelPlayer::GetMaterialLightmapName(size_t mat_index)
{
	if (NULL == m_pResModel)
	{
		return "";
	}

	node_material_t* pMat = (node_material_t*)GetMaterialFromID((int)mat_index);
	const char* lightmap_file_name = pMat->MatInfo.LightMap.pFileName;

	if (NULL == lightmap_file_name)
	{
		return "";
	}

	return lightmap_file_name;
}

unsigned int CModelPlayer::GetMaterialAmbient(size_t mat_index)
{
	if (NULL == m_pResModel)
	{
		return 0xFFFFFFFF;
	}

	node_material_t* pMat = (node_material_t*)GetMaterialFromID((int)mat_index);

	return GetMaterialAmbient(pMat);
}

unsigned int CModelPlayer::GetMaterialDiffuse(size_t mat_index)
{
	if (NULL == m_pResModel)
	{
		return 0xFFFFFFFF;
	}

	node_material_t* pMat = (node_material_t*)GetMaterialFromID((int)mat_index);

	return GetMaterialDiffuse(pMat);
}

bool CModelPlayer::SetMaterialAtlasLightMap(size_t mat_index, 
	const char* atlas_light_map, float scale_u, float scale_v,
	float offset_u, float offset_v)
{
	if (mat_index >= MATERIAL_NUMBER_MAX)
	{
		return false;
	}

	if (mat_index >= m_AtlasLightMaps.size())
	{
		m_AtlasLightMaps.resize(mat_index + 1, NULL);
	}

	size_t name_len = strlen(atlas_light_map);
	size_t new_size = name_len + sizeof(atlas_light_map_t);
	atlas_light_map_t* data = (atlas_light_map_t*)CORE_ALLOC(new_size);

	data->nSize = new_size;
	data->fScaleU = scale_u;
	data->fScaleV = scale_v;
	data->fOffsetU = offset_u;
	data->fOffsetV = offset_v;
	memcpy(data->strName, atlas_light_map, name_len + 1);

	atlas_light_map_t* pOld = m_AtlasLightMaps[mat_index];

	if (pOld)
	{
		CORE_FREE(pOld, pOld->nSize);
	}

	m_AtlasLightMaps[mat_index] = data;

	return true;
}

bool CModelPlayer::SetMaterialLigthMapColorRangeScale(size_t mat_index, float x, float y, float z)
{
	if (mat_index >= MATERIAL_NUMBER_MAX)
	{
		return false;
	}

	if (mat_index >= m_LightMapConfig.size())
	{
		m_LightMapConfig.resize(mat_index + 1, FmVec3(0.0f, 0.0f, 0.0f));
	}

	m_LightMapConfig[mat_index] = FmVec3(x, y, z);

	return true;
}

const FmVec3& CModelPlayer::GetMaterialLigthMapColorRangeScale(size_t mat_index)
{
	if (mat_index < m_LightMapConfig.size())
	{
		return m_LightMapConfig[mat_index];
	}

	return m_Vec3DefaultValve;
}

size_t CModelPlayer::GetVertexBufferCount()
{
	if (NULL == m_pResModel)
	{
		return 0;
	}

	return GetMaterialCount();
}

size_t CModelPlayer::GetVertexBufferSize(size_t index)
{
	if (NULL == m_pResModel)
	{
		return 0;
	}

	void* pMat = GetMaterialFromID((int)index);

	return GetMaterialVertexSize(pMat) * GetMaterialVertexCount(pMat);
}

size_t CModelPlayer::GetVertexDataStride(size_t index)
{
	if (NULL == m_pResModel)
	{
		return 0;
	}

	void* pMat = GetMaterialFromID((int)index);

	return GetMaterialVertexSize(pMat);
}

size_t CModelPlayer::GetVertexStride(size_t index)
{
	if (NULL == m_pResModel)
	{
		return 0;
	}

	void* pMat = GetMaterialFromID((int)index);

	return GetMaterialVertexSize(pMat);
}

size_t CModelPlayer::GetVertexCount(size_t index)
{
	if (NULL == m_pResModel)
	{
		return 0;
	}

	void* pMat = GetMaterialFromID((int)index);

	return GetMaterialVertexCount(pMat);
}

int CModelPlayer::GetVertexPositionOffset(size_t index)
{
	if (NULL == m_pResModel)
	{
		return 0;
	}

	void* pMat = GetMaterialFromID((int)index);

	return GetPositionOffset(pMat);
}

bool CModelPlayer::GetVertexPosition(size_t index, size_t vertex_index, FmVec3& pos, bool checkbone)
{
	vertex_data_t * vertex = GetVertexData(index);
	assert(vertex_index >= 0 && vertex_index < vertex->nCount);
	int offset = GetVertexPositionOffset(index);
	
	pos = *(FmVec3*)&vertex->pVertices[vertex->nStride * vertex_index+offset];
	node_material_t* pMat = (node_material_t*)GetMaterialFromID((int)index);
	
	if(pMat->pWeightTable && checkbone)
	{
		FmVec4 dpos(0.0, 0.0, 0.0, 0.0);
		FmVec4 _pos(pos.x, pos.y, pos.z, 1.0);
	
		
		const node_weight_table_t* node_weight = &pMat->pWeightTable[vertex_index *4];
		
		const FmVec4* bone_matrices = NULL;
		if(m_MatList[index].bone_matrices == 0)
			bone_matrices = CreateBoneMatrices(pMat);
		else
			bone_matrices = m_MatList[index].bone_matrices;

		for (int x = 0; x < 4; x++)
		{
			long boneid = node_weight[x].nBoneID;
			if(boneid < 0)
				boneid = 0;
			float weight = node_weight[x].fWeight;

			FmVec4 v1(bone_matrices[boneid * 3 + 0]);
			FmVec4 v2(bone_matrices[boneid * 3 + 1]);
			FmVec4 v3(bone_matrices[boneid * 3 + 2]);

			FmMat4 TM;
			TM._11 = v1.x;	TM._12 = v1.y;	TM._13 = v1.z;	TM._14 = v1.w;
			TM._21 = v2.x;	TM._22 = v2.y;	TM._23 = v2.z;	TM._24 = v2.w;
			TM._31 = v3.x;	TM._32 = v3.y;	TM._33 = v3.z;	TM._34 = v3.w;
			TM._41 = 0.0;	TM._42 = 0.0;	TM._43 = 0.0;	TM._44 = 1.0;

			dpos += _pos * TM  * weight;
		//	pos = dpos;
		}
		pos = dpos / dpos.w;
	}
	
	return true;
}

int CModelPlayer::GetVertexNormalOffset(size_t index)
{
	if (NULL == m_pResModel)
	{
		return 0;
	}

	void* pMat = GetMaterialFromID((int)index);

	return GetNormalOffset(pMat);
}

int CModelPlayer::GetVertexDiffuseOffset(size_t index)
{
	if (NULL == m_pResModel)
	{
		return 0;
	}

	void* pMat = GetMaterialFromID((int)index);

	return GetDiffuseOffset(pMat);
}

unsigned int CModelPlayer::GetVertexDiffuse(size_t vb_index, 
	size_t vertex_index)
{
	size_t geo_vb_count = GetVertexBufferCount();

	if (0 == geo_vb_count)
	{
		CORE_TRACE("(CModelPlayer::GetVertexDiffuse)no vb");
		return 0;
	}

	Assert(vb_index < geo_vb_count);

	int diffuse_offset = GetVertexDiffuseOffset(vb_index);

	if (diffuse_offset < 0)
	{
		return 0;
	}

	//int stride = GetVertexDataStride(vb_index);
	int vertex_num = (int)GetVertexCount(vb_index);

	if (vertex_num <= 0)
	{
		CORE_TRACE("(CModelPlayer::GetVertexDiffuse)"
			"incorrect number of vertices in vb");
		return 0;
	}

	Assert((int)vertex_index < vertex_num);

	//unsigned char* pnt = GetVertexData(vb_index);
	//unsigned int diffuse = 
	//	*(unsigned int*)(pnt + vertex_index * stride + diffuse_offset);
	vertex_data_t* pVertexData = GetVertexData(vb_index);
	unsigned int diffuse = *(unsigned int*)
		(get_vertex_by_index(pVertexData, vertex_index) + diffuse_offset);

	return diffuse;
}

// 获得指定顶点缓冲的贴图的UV数据偏移
int CModelPlayer::GetVertexUVOffset(size_t index)
{
	if (NULL == m_pResModel)
	{
		return 0;
	}

	void* pMat = GetMaterialFromID((int)index);

	return GetUVOffset(pMat);
}

int CModelPlayer::GetVertexLightmapUVOffset(size_t index)
{
	if (NULL == m_pResModel)
	{
		return 0;
	}

	void* pMat = GetMaterialFromID((int)index);

	return GetLightmapUVOffset(pMat);
}

vertex_data_t* CModelPlayer::GetVertexData(size_t index)
{
	if (NULL == m_pResModel)
	{
		return NULL;
	}

	node_material_t* pMat = (node_material_t*)GetMaterialFromID((int)index);

	return &(pMat->SingleVB);
//	void* pMat = GetMaterialFromID(model, index);
//	return (unsigned char*)GetMaterialVertexBuffer(pMat);
}

bool CModelPlayer::UpdateVertexData(size_t index, vertex_data_t* data)
{
	if (NULL == m_pResModel)
	{
		return false;
	}

	node_material_t* pMat = (node_material_t*)GetMaterialFromID((int)index);
	if(pMat == NULL)
		return false;

	if(pMat->pSingleGPUVB == NULL)
		return false;

	if(pMat->pSingleGPUVB)
		pMat->pSingleGPUVB->Release();
	pMat->pSingleGPUVB = m_pRender->CreateStaticVB(data->pVertices, (unsigned int)data->nCount * data->nStride);
	return pMat->pSingleGPUVB != NULL;
}

void CModelPlayer::SetActionPlayer(IActionPlayer* value)
{
	m_pActionPlayer = (CActionPlayer*)value;
	m_bSkeletonMapped = false;

	if (IsReady() && m_pActionPlayer && m_pActionPlayer->IsReady())
	{
		CreateSkeletonMapping();
	}
}

IActionPlayer* CModelPlayer::GetActionPlayer()
{
	return m_pActionPlayer;
}

void CModelPlayer::UpdateCenterRadius()
{
	if(m_nState == 0)
		return;

	if (NULL == m_pResModel)
	{
		return;
	}

	VisUtil_GetMatrixScale(&m_vScale, &m_mtxWorldTM);

	model_t* pModel = m_pResModel->GetModelData();

	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::UpdateCenterRadius] pModel == NULL model name is %s", m_pResModel->GetName());
		return;
	}

	FmVec3 bbMin;
	FmVec3 bbMax;
	FmVec3 bbCenter;

	GetNodeLocationBoundingBox(bbMin, bbMax, bbCenter, 
		pModel->vRootMin, pModel->vRootMax, pModel->vCenter);
	FmVec3TransformCoord(&m_vCenter, &bbCenter, &m_mtxWorldTM);
	m_vBoxSize = bbMax - bbMin;
	m_vBoxSize.x *= m_vScale.x;
	m_vBoxSize.y *= m_vScale.y;
	m_vBoxSize.z *= m_vScale.z;
	m_fRadius = VisUtil_Vec3Length(&m_vBoxSize) * 0.5F;
}

void CModelPlayer::SetWorldMatrix(const FmMat4& mat)
{
	m_mtxWorldTM = mat;
	
	if (m_pResModel)
	{
		//LPFXVAMODEL model = (LPFXVAMODEL)m_pModelRef;
		//FXVAMODEL_SetWorldMatrix(&mat);

		// 性能优化，有骨骼动画的物体不需要更新
		if (NULL == m_pActionPlayer)
		{
			UpdateModel(true);
		}

		UpdateCenterRadius();
	}
	else
	{
		// 中心点先设置为当前位置
		m_vCenter.x = mat._41;
		m_vCenter.y = mat._42;
		m_vCenter.z = mat._43;
	}
}

FmMat4 CModelPlayer::GetWorldMatrix()
{
	return m_mtxWorldTM;
}

void CModelPlayer::SetShowBoundBox(bool value)
{
	if (m_pResModel)
	{
		SetSelection(value);
	}

	m_bShowBoundBox = value;
}

bool CModelPlayer::GetShowBoundBox()
{
	return m_bShowBoundBox;
}

void CModelPlayer::SetUseVertexColor(bool value)
{
	m_bUseVertexColor = value;
}

bool CModelPlayer::GetUseVertexColor()
{
	return m_bUseVertexColor;
}

void CModelPlayer::SetUseLightMap(bool value)
{
	m_bUseLightMap = value;
}

bool CModelPlayer::GetUseLightMap()
{
	return m_bUseLightMap;
}

void CModelPlayer::SetWriteVelocity(int value)
{
	m_nWriteVelocity = value;
}

int CModelPlayer::GetWriteVelocity()
{
	return m_nWriteVelocity;
}

bool CModelPlayer::GetLightLoaded()
{
	if (m_LightTexs.empty())
	{
		return false;
	}

	for (size_t i = 0; i < m_LightTexs.size(); ++i)
	{
		if (m_LightTexs[i])
		{
			if (!m_LightTexs[i]->IsLoadComplete())
			{
				return false;
			}
		}
	}

	return true;
	//return m_bLightLoaded;
}

bool CModelPlayer::UpdateVertexColor(unsigned int* color_data, 
	size_t color_size)
{
	return false;
}

/*
bool CModelPlayer::UpdateLightMap(size_t material, unsigned int* tex_data, 
	size_t tex_width, size_t tex_height)
{
	IDxStaticTex* pTex = m_pRender->CreateStaticTex(tex_width, tex_height, 1,
		D3DFMT_A8R8G8B8);

	if (NULL == pTex)
	{
		CORE_TRACE("(CModelPlayer::UpdateLightMap)CreateTexture failed");
		return false;
	}

	D3DLOCKED_RECT lr;

	pTex->LockRect(0, &lr, NULL);

	// 图片的数据是从上到下的
	BYTE* pDstRow = (BYTE*)lr.pBits;

	for (size_t r = 0; r < tex_height; ++r)
	{
		DWORD* pDst32 = (DWORD*)pDstRow;

		for (size_t c = 0; c < tex_width; ++c)
		{
			*pDst32++ = tex_data[r * tex_width + c];
		}

		pDstRow += lr.Pitch;
	}

	pTex->UnlockRect(0);

	if (material >= m_LightTexs.size())
	{
		m_LightTexs.resize(material + 1, NULL);
	}

	SAFE_RELEASE(m_LightTexs[material]);
	m_LightTexs[material] = pTex;
	m_bLightLoaded = true;

	return true;
}
*/
bool CModelPlayer::UpdateLightMap(size_t material, const char* light_file,
	unsigned int* tex_data, size_t tex_width, size_t tex_height)
{
//	if (material < m_LightTexs.size())
//	{
//		SAFE_RELEASE(m_LightTexs[material]);
//	}
//
//	if (material < m_AtlasLightMaps.size())
//	{
//		atlas_light_map_t* pAtlas = m_AtlasLightMaps[material];
//
//		if (pAtlas)
//		{
//			CORE_FREE(pAtlas, pAtlas->nSize);
//			m_AtlasLightMaps[material] = NULL;
//		}
//	}
//
//	char fname[256];
//
//	SafeSprintf(fname, sizeof(fname), "%s%s.dds", 
//		m_pRender->GetPathPrefix(light_file), light_file);
//
//	ITexture* pTex = m_pRender->CreateNewTexture(fname, 
//		(unsigned int)tex_width, (unsigned int)tex_height, 1, 
//		DXGI_FORMAT_B8G8R8A8_UNORM, tex_data, 
//		(unsigned int)(tex_width * tex_height * 4));
//
//	if (NULL == pTex)
//	{
//		CORE_TRACE("(CModelPlayer::UpdateLightMap)CreateTexture failed");
//		return false;
//	}
//
//	if (material >= m_LightTexs.size())
//	{
//		m_LightTexs.resize(material + 1, NULL);
//	}
//
//	SAFE_RELEASE(m_LightTexs[material]);
//	m_LightTexs[material] = pTex;

	return true;
}

bool CModelPlayer::LoadLight(const char* light_file, bool async_load)
{
	Assert(light_file != NULL);

	m_strLightFile = light_file;
	m_bAsyncLoad = async_load;
	//m_bLightLoaded = false;

	
	if (m_bUseLightMap && IsReady())
	{
		InnerLoadLight();
	}

	return true;
}

bool CModelPlayer::LoadMaterial(const char* material_file, bool async_load)
{
	Assert(material_file != NULL);

    size_t len = strlen(material_file);
    if (len < 4)
    {
        m_strMaterialFile.clear();
        return false;
    }

    if (stricmp(material_file + len - 4, ".mtl") == 0)
    {
        if (strnicmp(material_file, m_strName.c_str(), len - 3) == 0)
        {
            // 自定义的材质文件与默认的相同,无需设置
            m_strMaterialFile.clear();
            return false;
        }
    }

    m_strMaterialFile = material_file;

	m_bAsyncLoad = async_load;

	if (this->IsReady())
	{
		return InnerLoadMaterial();
	}

	return true;
}

void CModelPlayer::ReleaseLightMaps()
{
	for (size_t i = 0; i < m_LightTexs.size(); ++i)
	{
		SAFE_RELEASE(m_LightTexs[i]);
	}

	m_LightTexs.clear();
}

bool CModelPlayer::InnerLoadLight()
{
	if (!GetLightMapEnable())
	{
		// 光贴图不可用，不再加载预渲染的顶点色
		return false;
	}

	

	ReleaseLightMaps();

	size_t material_num = this->GetMaterialCount();
	const char* light = m_strLightFile.c_str();

	for (size_t i = 0; i < material_num; ++i)
	{
		ITexture* pTex = NULL;

		if ((i < m_AtlasLightMaps.size()) && (m_AtlasLightMaps[i] != NULL))
		{
			atlas_light_map_t* pAtlas = m_AtlasLightMaps[i];

			pTex = m_pRender->CreateTexture(pAtlas->strName, m_bAsyncLoad);
		}
		else
		{
			char fname[256];

			SafeSprintf(fname, sizeof(fname), "%s%s_%d%s", 
				m_pRender->GetPathPrefix(light), light, i, m_pRender->GetTexExtfix());
			//pTex = m_pRender->CreateTexture(fname, m_bAsyncLoad);

			// 如果文件打包的话只在文件包中搜索
			if (g_pCore->GetFileSys()->FileInPack(fname))
			{
				pTex = m_pRender->CreateTexture(fname, m_bAsyncLoad);
			}
		}

		//m_LightTexs.push_back(pTex);

		if (pTex)
		{
			if (i >= m_LightTexs.size())
			{
				m_LightTexs.resize(i + 1, NULL);
			}

			SAFE_RELEASE(m_LightTexs[i]);
			m_LightTexs[i] = pTex;
		}
	}
	/*
	// 记录加载资源的名称
	this->SetName(m_strLightFile.c_str());

	if (m_bAsyncLoad)
	{
		// 创建异步加载的相关内容
		if (!BeginCreateLight(m_strLightFile.c_str()))
		{
			return false;
		}

		// 请求线程加载
		if (!m_pRender->RequestLoadResource( 
			CModelPlayer::RES_CATEGORY_LIGHT, m_pLightLoader))
		{
			return false;
		}
	}
	else
	{
		// 同步加载
		if (!CreateLight(m_strLightFile.c_str()))
		{
			return false;
		}
	}
	*/

	return true;
}

bool CModelPlayer::InnerLoadMaterial()
{
	if (m_bAsyncLoad)
	{
		// 创建异步加载的相关内容
		if (!BeginCreateMaterial(m_strMaterialFile.c_str()))
		{
			return false;
		}

		// 请求线程加载
		if (!m_pRender->RequestLoadResource( 
			CModelPlayer::RES_CATEGORY_MATERIAL, m_pMaterialLoader))
		{
			return false;
		}
	}
	else
	{
		// 同步加载
		if (!CreateMaterial(m_strMaterialFile.c_str()))
		{
			return false;
		}
	}

	return true;
}

bool CModelPlayer::EndCreate(int category, bool succeed)
{
//	if (category == RES_CATEGORY_LIGHT)
//	{
//		SAFE_RELEASE(m_pLightLoader);
//	}
//	else 
	if (category == RES_CATEGORY_MATERIAL)
	{
		SAFE_RELEASE(m_pMaterialLoader);
	}

	return true;
}

/*
bool CModelPlayer::CreateLight(const char* light)
{
	if (!BeginCreateLight(light))
	{
		return false;
	}

	if (!m_pLightLoader->Load(false))
	{
		EndCreate(RES_CATEGORY_LIGHT, false);
		return false;
	}

	if (!m_pLightLoader->Create(false))
	{
		EndCreate(RES_CATEGORY_LIGHT, false);
		return false;
	}

	if (!EndCreate(RES_CATEGORY_LIGHT, true))
	{
		return false;
	}

	return true;
}

bool CModelPlayer::BeginCreateLight(const char* light)
{
	Assert(light != NULL);

	SAFE_RELEASE(m_pLightLoader);

	char fname[256];

	SafeSprintf(fname, sizeof(fname), "%s%s", 
		m_pRender->GetPathPrefix(light), light);

	m_pLightLoader = CResLightLoader::NewInstance();
	m_pLightLoader->SetOwner(this);
	m_pLightLoader->SetFileName(fname);
	m_pLightLoader->SetMaterialCount(this->GetMaterialCount());

	m_bLightLoaded = false;

	return true;
}

bool CModelPlayer::BuildLightMap(const char* light, size_t material, 
	DxTexture* pTex)
{
	if (!IsReady())
	{
		CORE_TRACE("(CModelPlayer::BuildLightMap)not ready");
		CORE_TRACE(light);
		return false;
	}

	if (material >= m_LightTexs.size())
	{
		m_LightTexs.resize(material + 1, NULL);
	}

	SAFE_RELEASE(m_LightTexs[material]);
	m_LightTexs[material] = m_pRender->CreateStaticTexFrom(pTex);
	m_bLightLoaded = true;

	return true;
}
*/

bool CModelPlayer::CreateMaterial(const char* material)
{
	if (!BeginCreateMaterial(material))
	{
		return false;
	}

	if (!m_pMaterialLoader->Load(false))
	{
		EndCreate(RES_CATEGORY_MATERIAL, false);
		return false;
	}

	if (!m_pMaterialLoader->Create(false))
	{
		EndCreate(RES_CATEGORY_MATERIAL, false);
		return false;
	}

	if (!EndCreate(RES_CATEGORY_MATERIAL, true))
	{
		return false;
	}

	return true;
}

bool CModelPlayer::BeginCreateMaterial(const char* material)
{
	Assert(material != NULL);

	SAFE_RELEASE(m_pMaterialLoader);

	char fname[256];

	SafeSprintf(fname, sizeof(fname), "%s%s", 
		m_pRender->GetPathPrefix(material), material);

	m_pMaterialLoader = CMaterialLoader::NewInstance();
	m_pMaterialLoader->SetOwner(this);
	m_pMaterialLoader->SetFileName(fname);

	return true;
}

bool CModelPlayer::BuildMaterial(IIniFile* pIni)
{
	Assert(pIni != NULL);

	if (NULL == m_pResModel)
	{
		return false;
	}

	if (!LoadCustomMaterialFile(pIni))
	{
		return false;
	}

	LoadCustomTextures(m_pResModel->GetTexPaths(), m_bAsyncLoad);

	return true;
}

void CModelPlayer::Update(float seconds)
{
	if (0 == m_nState)
	{
		if (m_pResModel->IsLoadComplete())
		{
			Create();
		}
	}
	else
	{
		double cur_time = m_pRender->GetCore()->GetTotalSeconds();

		UpdateAniTime(cur_time);
		
		for (size_t i = 0; i < m_UpdateFlags.size(); ++i)
		{
			m_UpdateFlags[i] = false;
		}
	}
}

bool CModelPlayer::Create()
{
	model_t* pModel = m_pResModel->GetModelData();
	
	if (pModel == NULL)
	{
		CORE_TRACE_EX("WARNING: [CModelPlayer::Create] pModel == NULL model name is %s", m_pResModel->GetName());
		return false;
	}
	
	double beg_time = m_pRender->GetCore()->GetTotalSeconds();
	
	//m_pModelRef = FXCreateVertexAniModelReference(m_pResModel->GetName(),
	//	m_pResModel->GetModelData(), beg_time);
	//m_pszResourceFileName = alloc_string(m_pResModel->GetName());
	//FmMatrixIdentity(&m_mtxWorldTM);
	//FmMatrixIdentity(&m_mtxPrevWorldTM);
	m_mtxPrevWorldTM = m_mtxWorldTM;
	m_dLastTime = beg_time;
	m_nState = MODEL_STATE_PLAYING;
	m_nRefId = get_ref_id();
	m_mtxNodeCurrentTM = (FmMat4*)CORE_ALLOC(
		sizeof(FmMat4) * pModel->nNodeCount);
	memset(m_mtxNodeCurrentTM, 0, sizeof(FmMat4) * pModel->nNodeCount);
	UpdateModel(true);
	UpdateCenterRadius();
	
//	//if ((!m_bLightLoaded) && m_bUseLightMap && (!m_strLightFile.empty()))
	if ((!GetLightLoaded()) && m_bUseLightMap 
		&& (!m_strLightFile.empty()))
	{
		InnerLoadLight();
	}

	if (!m_strMaterialFile.empty())
	{
		InnerLoadMaterial();
	}
	
	m_MatList = new MatInfo[GetMaterialCount()];
//	m_MatListReflect = new MatInfo[GetMaterialCount()];
//	m_MatListDynamicShadow = new MatInfo[GetMaterialCount()];

	return true;
}

bool CModelPlayer::CreateSkeletonMapping()
{
	if (NULL == m_pActionPlayer)
	{
		return false;
	}

	if (!InitSkeletonMappingTable())
	{
		return false;
	}

	m_bSkeletonMapped = true;

	return true;
}

//设置用户传入Sahder的常量
void CModelPlayer::SetUserVsConstValue(const char* name, const FmVec4& value)
{
	m_ShaderUserConstValue.SetValue(name, value);
}

void CModelPlayer::SetKeepOutTexture(const char* filename)
{
	if(filename != m_KeepOutName)
	{
		m_KeepOutName = filename;
		if(m_KeepOutTexture)
			m_KeepOutTexture->Release();
	}

	m_KeepOutTexture = m_pRender->CreateTexture(filename, true);
}