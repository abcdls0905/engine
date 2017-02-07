//--------------------------------------------------------------------
// 文件名:		model_data.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年7月13日
// 创建人:		
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef	_MODEL_DATA_H
#define	_MODEL_DATA_H

#include "../visual/i_resource.h"
#include "../visual/i_texture.h"
#include "../visual/i_model_player.h"
#include "../visual/i_model_system.h"
#include "../public/core_log.h"
#include "location.h"

// 顶点数据结构
enum VERTEX_SHIFT_ENUM
{
	VERTEX_COLOR_SHIFT,
	VERTEX_TANGENT_SHIFT,
	VERTEX_BINORMAL_SHIFT,
	VERTEX_TEXTURE0_SHIFT,
	VERTEX_TEXTURE1_SHIFT,
	VERTEX_ANIMATION_SHIFT,
	VERTEX_SKELETON_SHIFT,
	VERTEX_INSTANCE_SHIFT,
	VERTEX_SHIFT_MAX,
};

#define VERTEX_COLOR_FLAG		(0x1 << VERTEX_COLOR_SHIFT)
#define VERTEX_TANGENT_FLAG		(0x1 << VERTEX_TANGENT_SHIFT)
#define VERTEX_BINORMAL_FLAG	(0x1 << VERTEX_BINORMAL_SHIFT)
#define VERTEX_TEXTURE0_FLAG	(0x1 << VERTEX_TEXTURE0_SHIFT)
#define VERTEX_TEXTURE1_FLAG	(0x1 << VERTEX_TEXTURE1_SHIFT)
#define VERTEX_ANIMATION_FLAG	(0x1 << VERTEX_ANIMATION_SHIFT)
#define VERTEX_SKELETON_FLAG	(0x1 << VERTEX_SKELETON_SHIFT)
#define VERTEX_INSTANCE_FLAG	(0x1 << VERTEX_INSTANCE_SHIFT)

// 模型标记信息
const unsigned int MODEL_POSITION_INFO		= 0x00000001; // 顶点位置信息标记
const unsigned int MODEL_POSITION_ANI		= 0x00000002; // 顶点位置动画信息标记
const unsigned int MODEL_NORMAL_INFO		= 0x00000004; // 顶点的法向信息标记
const unsigned int MODEL_NORMAL_ANI			= 0x00000008; // 顶点的法向动画信息标记
const unsigned int MODEL_COLOR_INFO			= 0x00000010; // 顶点色信息标记
const unsigned int MODEL_COLOR_ANI			= 0x00000020; // 顶点色动画信息标记
const unsigned int MODEL_ILLUM_INFO			= 0x00000040; // 顶点流明信息标记
const unsigned int MODEL_ILLUM_ANI			= 0x00000080; // 顶点流明动画信息标记
const unsigned int MODEL_TANGENT_INFO		= 0x00000100; // 顶点切线信息标记
const unsigned int MODEL_TANGENT_ANI		= 0x00000200; // 顶点切线动画信息标记
const unsigned int MODEL_BINORMAL_INFO		= 0x00000400; // 顶点次法线信息标记
const unsigned int MODEL_BINORMAL_ANI		= 0x00000800; // 顶点次法线动画信息标记
const unsigned int MODEL_MATRIX_INFO		= 0x00001000; // 矩阵动画信息标记
const unsigned int MODEL_MATRIX_ANI			= 0x00002000; // 矩阵动画信息标记
const unsigned int MODEL_SKIN_FLAG			= 0x00010000; // 蒙皮标记
const unsigned int MODEL_MESH_FLAG			= 0x00020000; // 网格标记
const unsigned int MODEL_DISAPPEAR_FLAG		= 0x00040000; // 淡入淡出标记
const unsigned int MODEL_SCENEFOG_FLAG		= 0x00080000; // 场景雾标记
const unsigned int MODEL_APPLIQUE_FLAG		= 0x00100000; // 贴花模型标记
const unsigned int MODEL_BB_FLAG			= 0x00200000;
const unsigned int MODEL_BBX_FLAG			= 0x00400000;
const unsigned int MODEL_BBY_FLAG			= 0x00800000;
const unsigned int MODEL_TREELEAF_FLAG		= 0x01000000;
const unsigned int MODEL_HIDE_FLAG			= 0x02000000;
const unsigned int MODEL_BLEND_FLAG			= 0x04000000;
const unsigned int MODEL_BBEX_FLAG			= 0x08000000;
const unsigned int MODEL_MAINMODEL_FLAG		= 0x10000000;
const unsigned int MODEL_WATERCLIP_FLAG		= 0x20000000; // 水面消除模型标记
const unsigned int MODEL_FLAG_EXT			= 0x80000000; // 模型标记扩展标记

const unsigned int HELPER_POSITION_INFO		= MODEL_POSITION_INFO; // 顶点位置信息标记
const unsigned int HELPER_POSITION_ANI		= MODEL_POSITION_ANI; // 顶点位置动画信息标记
const unsigned int HELPER_NORMAL_INFO		= MODEL_NORMAL_INFO; // 顶点的法向信息标记
const unsigned int HELPER_NORMAL_ANI		= MODEL_NORMAL_ANI; // 顶点的法向动画信息标记
const unsigned int HELPER_MATRIX_INFO		= MODEL_MATRIX_INFO; // 矩阵动画信息标记
const unsigned int HELPER_MATRIX_ANI		= MODEL_MATRIX_ANI; // 矩阵动画信息标记
const unsigned int HELPER_CAMERA_FLAG		= 0x00010000; // 辅助点为摄像机的标记
const unsigned int HELPER_FLAG_EXT			= 0x80000000; // 辅助点标记扩展标记

// 材质标记信息
const unsigned int MATERIAL_AMBIENT_INFO			= 0x00000001; // 材质环境光反射信息标记
const unsigned int MATERIAL_AMBIENT_ANI				= 0x00000002; // 材质环境光反射动画信息标记
const unsigned int MATERIAL_DIFFUSE_INFO			= 0x00000004; // 材质漫反射信息标记
const unsigned int MATERIAL_DIFFUSE_ANI				= 0x00000008; // 材质漫反射动画信息标记
const unsigned int MATERIAL_SPECULAR_INFO			= 0x00000010; // 材质镜面反射信息标记
const unsigned int MATERIAL_SPECULAR_ANI			= 0x00000020; // 材质镜面反射动画信息标记
const unsigned int MATERIAL_EMISSIVE_INFO			= 0x00000040; // 材质辐射信息标记
const unsigned int MATERIAL_EMISSIVE_ANI			= 0x00000080; // 材质辐射动画信息标记
const unsigned int MATERIAL_OPACITY_INFO			= 0x00000100; // 材质透明信息标记
const unsigned int MATERIAL_OPACITY_ANI				= 0x00000200; // 材质透明动画信息标记
const unsigned int MATERIAL_GLOSS_INFO				= 0x00000400; // 材质Glossiness信息标记
const unsigned int MATERIAL_GLOSS_ANI				= 0x00000800; // 材质Glossiness动画信息标记
const unsigned int MATERIAL_DIFFUSE_MAP_INFO		= 0x00001000; // Diffuse贴图信息标记
const unsigned int MATERIAL_BUMP_MAP_INFO			= 0x00002000; // Bump贴图信息标记
const unsigned int MATERIAL_SPECULAR_MAP_INFO		= 0x00004000; // Specular贴图信息标记
const unsigned int MATERIAL_SPECULAR_LEVEL_MAP_INFO	= 0x00008000; // Specularlevel贴图信息标记
const unsigned int MATERIAL_GLOSSINESS_MAP_INFO		= 0x00010000; // Glossiness贴图信息标记
const unsigned int MATERIAL_REFLECTION_ENV_MAP_INFO	= 0x00020000; // Reflection贴图信息标记
const unsigned int MATERIAL_LIGHTMAP_INFO			= 0x00040000; // Light贴图信息标记
const unsigned int MATERIAL_DIFFUSE_UV_ANI			= 0x00080000; // DIFFUSE_MAP的UV动画信息标记
const unsigned int MATERIAL_SELF_ILLUMINATION_INFO	= 0x00100000; // 自发光贴图信息标记
const unsigned int MATERIAL_ATEST_FLAG				= 0x00200000; // Alpha测试标记
const unsigned int MATERIAL_BLEND_FLAG				= 0x00400000; // Alpha混合标记
const unsigned int MATERIAL_NOLIGHT_FLAG			= 0x00800000; // 无光照标记
const unsigned int MATERIAL_BLENDENHANCE_FLAG		= 0x01000000; // 高亮叠加标记
const unsigned int MATERIAL_REFRACTION_FLAG			= 0x02000000; // 折射材质标记
const unsigned int MATERIAL_NOZWRITE_FLAG			= 0x04000000; // 无深度写入标记
const unsigned int MATERIAL_DOUBLESIDE_FLAG			= 0x08000000; // 双面材质标记
const unsigned int MATERIAL_BLENDQUALITY			= 0x10000000; // 模型二次绘制标记
const unsigned int MATERIAL_SAMPLER_CLAMP			= 0x20000000; // 采样过界CLAMP模式
const unsigned int MATERIAL_VOLUMEFOG_FLAG			= 0x40000000; // 体积雾材质标签
const unsigned int MATERIAL_FLAG_EXT				= 0x80000000; // 模型材质标记扩展标记

// 扩展材质标记信息
const unsigned int MATERIAL_FRAME_INFO				=	0x00000001;	// 序列帧动画信息标记
const unsigned int MATERIAL_SHADOW_INFO				=	0x00000004;	// 投射阴影标记
const unsigned int MATERIAL_SHADOWED_INFO			=	0x00000008;	// 被投影标记
const unsigned int MATERIAL_BLENDSHADOW_INFO		=	0x00000010;	// 半透明阴影标记
const unsigned int MATERIAL_SIEVE_INFO				=	0x00000020;	// 筛孔半透明标记
const unsigned int MATERIAL_FILTER_MAP_INFO			=	0x00000080;	// 遮罩贴图信息标记
const unsigned int MATERIAL_FILTER_PARAM_ANI		=	0x00000100;	// 遮罩参数动画标记
const unsigned int MATERIAL_FILTER_UV_ANI			=	0x00000200;	// 遮罩贴图的UV动画信息标记

// Shader宏定义标记位
//const unsigned int SHADOWMAP_VALUE		= 0; //	阴影图
//const unsigned int NORMAL_VALUE				= 1; //	法线
//const unsigned int VERTEXCOLOR_VALUE		= 2; //	顶点色
//const unsigned int VERTEXANI_VALUE			= 3; //	顶点动画
//const unsigned int SKELETONANI_VALUE		= 4; //	骨骼动画
//const unsigned int DISAPPEAR_VALUE			= 5; //	淡入淡出
//const unsigned int DIFFUSEMAP_VALUE			= 6; //	漫反射贴图
//const unsigned int NORMALMAP_VALUE			= 7; //	法线贴图
//const unsigned int DYNAMIC_LIGHTING_VALUE	= 8; //	动态光照
//const unsigned int LIGHTMAP_VALUE			= 9; //	光贴图
//const unsigned int REFLECTION_VALUE			= 10; // 反射
//const unsigned int SPECULARMAP_VALUE		= 11; // 高光贴图
//const unsigned int TEXTUREANI_VALUE			= 12; // 贴图动画
//const unsigned int SPECULAR_VALUE			= 13; // 高光
//const unsigned int OPACITY_VALUE			= 14; // 透明
//const unsigned int VOLUMETEXTURE_VALUE		= 15; // 体积贴图
//const unsigned int FALLOFF_VALUE			= 16; // 轮廓光
//const unsigned int POINTLIGHT_VALUE			= 17; // 点光源
//const unsigned int FOGLINEAR_VALUE			= 18; // 线性雾
//const unsigned int APPENDCOLOR_VALUE		= 19; // 附加色
//const unsigned int PRELIGHT_VALUE			= 20; // 延迟光照贴图
//const unsigned int EMISSIVE_VALUE			= 21; // 自发光
//const unsigned int EMISSIVEMAP_VALUE		= 22; // 自发光贴图
//const unsigned int SPHERE_AMBIENT_VALUE		= 23; // 半球环境光
//const unsigned int SKIN_EFFECT_VALUE		= 24; // 皮肤效果
//const unsigned int FIX_SHADOW_VALUE			= 25; // 阴影修正
//const unsigned int FOGEXP_VALUE				= 26; // 指数雾
//const unsigned int INSTANCE_VALUE			= 27; // 几何实例化
//const unsigned int PHASEMONITOR_VALUE		= 28; // 相位图
//const unsigned int CAMERALIGHT_VALUE		= 29; // 摄像机光源
//const unsigned int SATURATION_VALUE			= 30; // 调整贴图饱和度
//const unsigned int TYPELABEL_NUMBER			= 31; //	

// Shader宏定义标记位信息
//const unsigned int SHADOWMAP_FLAG			= 0x1 << SHADOWMAP_VALUE;
//const unsigned int NORMAL_FLAG				= 0x1 << NORMAL_VALUE;
//const unsigned int VERTEXCOLOR_FLAG			= 0x1 << VERTEXCOLOR_VALUE;
//const unsigned int VERTEXANI_FLAG			= 0x1 << VERTEXANI_VALUE;
//const unsigned int SKELETONANI_FLAG			= 0x1 << SKELETONANI_VALUE;
//const unsigned int DISAPPEAR_FLAG			= 0x1 << DISAPPEAR_VALUE;
//const unsigned int DIFFUSEMAP_FLAG			= 0x1 << DIFFUSEMAP_VALUE;
//const unsigned int NORMALMAP_FLAG			= 0x1 << NORMALMAP_VALUE;
//const unsigned int DYNAMIC_LIGHTING_FLAG	= 0x1 << DYNAMIC_LIGHTING_VALUE;
//const unsigned int LIGHTMAP_FLAG			= 0x1 << LIGHTMAP_VALUE;
//const unsigned int REFLECTION_FLAG			= 0x1 << REFLECTION_VALUE;
//const unsigned int SPECULARMAP_FLAG			= 0x1 << SPECULARMAP_VALUE;
//const unsigned int TEXTUREANI_FLAG			= 0x1 << TEXTUREANI_VALUE;
//const unsigned int SPECULAR_FLAG			= 0x1 << SPECULAR_VALUE;
//const unsigned int OPACITY_FLAG				= 0x1 << OPACITY_VALUE;
//const unsigned int VOLUMETEXTURE_FLAG		= 0x1 << VOLUMETEXTURE_VALUE;
//const unsigned int FALLOFF_FLAG				= 0x1 << FALLOFF_VALUE;
//const unsigned int POINTLIGHT_FLAG			= 0x1 << POINTLIGHT_VALUE;
//const unsigned int FOGLINEAR_FLAG			= 0x1 << FOGLINEAR_VALUE;
//const unsigned int APPENDCOLOR_FLAG			= 0x1 << APPENDCOLOR_VALUE;
//const unsigned int PRELIGHT_FLAG			= 0x1 << PRELIGHT_VALUE;
//const unsigned int EMISSIVE_FLAG			= 0x1 << EMISSIVE_VALUE;
//const unsigned int EMISSIVEMAP_FLAG			= 0x1 << EMISSIVEMAP_VALUE;
//const unsigned int SPHERE_AMBIENT_FLAG		= 0x1 << SPHERE_AMBIENT_VALUE;
//const unsigned int SKIN_EFFECT_FLAG			= 0x1 << SKIN_EFFECT_VALUE;
//const unsigned int FIX_SHADOW_FLAG			= 0x1 << FIX_SHADOW_VALUE;
//const unsigned int FOGEXP_FLAG				= 0x1 << FOGEXP_VALUE;
//const unsigned int INSTANCE_FLAG			= 0x1 << INSTANCE_VALUE;
//const unsigned int PHASEMONITOR_FLAG		= 0x1 << PHASEMONITOR_VALUE;
//const unsigned int CAMERALIGHT_FLAG			= 0x1 << CAMERALIGHT_VALUE;
//const unsigned int SATURATION_FLAG			= 0x1 << SATURATION_VALUE;

const unsigned int MODEL_POSITION_POS		= 0;
const unsigned int MODEL_NORMAL_POS			= 1;
const unsigned int MODEL_COLOR_POS			= 2;
const unsigned int MODEL_ILLUM_POS			= 3;
const unsigned int MODEL_TANGENT_POS		= 4;
const unsigned int MODEL_BINORMAL_POS		= 5;
const unsigned int MODEL_MATRIX_POS			= 6;

const unsigned int MATERIAL_AMBIENT_POS				= 0;
const unsigned int MATERIAL_DIFFUSE_POS				= 1;
const unsigned int MATERIAL_SPECULAR_POS			= 2;
const unsigned int MATERIAL_EMISSIVE_POS			= 3;
const unsigned int MATERIAL_OPACITY_POS				= 4;
const unsigned int MATERIAL_GLOSS_POS				= 5;
const unsigned int MATERIAL_DIFFUSE_MAP_POS			= 7;
const unsigned int MATERIAL_BUMP_MAP_POS			= 8;
const unsigned int MATERIAL_SPECULAR_MAP_POS		= 9;
const unsigned int MATERIAL_SPECULAR_LEVEL_MAP_POS	= 10;
const unsigned int MATERIAL_GLOSSINESS_MAP_POS		= 11;
const unsigned int MATERIAL_REFLECTION_ENV_MAP_POS	= 12;
const unsigned int MATERIAL_LIGHTMAP_POS			= 13;

const unsigned int MAX_VERTEX_BONE_BLEND = 4;
const unsigned int MAX_BONE = 78;

#include "model_vf.h"

class COctTree;
struct model_node_t;

// 缓冲的动画蒙皮数据
struct cache_skin_t
{
	int nSkinSequenceId;
	FmVec3* pSkinVectors;
	unsigned int nSkinVectorNum;
};

// 缓冲的骨骼矩阵数据
struct cache_bone_t
{
	int nBoneSequenceId;
	FmVec4* pBoneMatrices;
	unsigned int nBoneMatrixNum;
};

// 骨骼动画顶点数据
struct skinned_blend_vertex_t
{
	short nIndex[MAX_VERTEX_BONE_BLEND];
	float fWeight[MAX_VERTEX_BONE_BLEND];
};

// 骨骼权值表数据
struct node_weight_table_t
{
	int nBoneNameOffset;
	float fWeight;
	int nBoneID;
};

// 贴图数据
struct texture_info_t
{
	char* pFileName; // 贴图文件名 
	unsigned int nFileNameLen;
	ITexture* pTex; // 贴图资源指针
};

struct filter_param
{
	float fScale;
	float fBias;
	float fScaleAlpha;
	float fBiasAlpha;
};

// 材质数据
struct material_info_t
{
	FmVec3	vAmbient;	// 环境光
	FmVec3	vAmbientEx; //环境光扩展
	FmVec3 vDiffuse;	// 漫反射
	FmVec3 vSpecular;	// 高光反射
	FmVec3 vEmissive;	// 自发光

	float fGlossiness;		// 光泽度
	float fOpacity;			// 透明度
	float fAlphaRef;		// ALPHA测试参考值
	float fSubSurfaceScale;	// 次表面散射范围
	float fSubSurfaceFactor;// 次表面强度因子
	float fReflectFactor;	// 环境反射强度
	float fBumpScale;		// 法线强度
	float fSaturationScale;	// 材质饱和度
	float fHeightScale;		// 表面细分高度

	filter_param filterParam; // 遮罩参数

	// 材质效果开关
	bool bSpecularEnable;	// 高光开启
	bool bEmissiveEnable;	// 自发光开启
	bool bOpacityEnable;	// 透明开启
	bool bReflectEnable;	// 使用反射贴图
	bool bBumpMapEnable;	// 使用法线贴图
	bool bTessellationEnable; // 使用表面细分
	bool bFilterEnable;		// 使用遮罩贴图

	// 渲染状态
	bool bAlphaTest;		// ALPHA测试
	bool bAlphaTestGreaterQual;	//ALPHA测试函数是否为GREATERQUAL
	bool bBlend;			// ALPHA混合
	bool bBlendEnhance;		// 高亮ALPHA混合
	bool bBlendQuality;		// 二次渲染高质量混合
	bool bNoZWrite;			// 不写入深度缓冲
	bool bNoZTest;			// 无深度测试
	bool bDoubleSide;		// 双面显示
	bool bVolumeTexture;	// 体积纹理
	bool bBlendDownLevel;	// 半透明物体降级显示

	// 采样方式
	bool bSamplerClamp;		// 采样坐标过界后停留在边界上

	// 特殊效果显示
	bool bUIFloat;			// 属于界面的物体
	bool bNoLight;			// 无需动态光照
	bool bSphereAmbient;	// 简易半球光照	
	bool bDisappear;		// 淡入淡出（透明必须开启）
	bool bSkinEffect;		// 皮肤效果
	bool bHairEffect;		// 毛发效果

	// 泛光效果
	bool bGlowEnable;		// 泛光开关
	bool bGlowEntire;		// 是否整体泛光
	bool bGlowEmissive;		// 自发光颜色泛光
	bool bGlowEmissiveMap;	// 自发光贴图泛光
	bool bGlowDiffuse;		// 漫反射泛光
	bool bGlowVertexColor;	// 顶点色泛光
	bool bGlowAppendColor;	// 附加色泛光
	bool bGlowEnhance;		// 高亮泛光
	bool bGlowBlur;			// 泛光模糊
	float fGlowSize;		// 泛光大小
	float fGlowAlpha;		// 泛光透明度

	// 轮廓光（使用自发光颜色）
	bool bRimLight;			// 轮廓光开关
	FmVec3 vRimColor;		// 轮廓光颜色

	float fRimIntensity;	// 轮廓光亮度
	float fRimPower;		// 轮廓光指数

	// 专门用途的模型
	bool bRefraction;		// 屏幕扭曲（折射）
	bool bApplique;			// 模板贴花
	bool bSceneFog;			// 场景体积雾
	bool bForbidStandModel;	// 禁止站立碰撞辅助模型（不可见）
	bool bPhaseMonitor;		// 使用相位监视

	//unsigned int nShaderInfo;

	texture_info_t DiffuseMap;			// 漫反射贴图
	texture_info_t BumpMap;				// 法线贴图
	texture_info_t LightMap;			// 光贴图
	texture_info_t SpecularMap;			// 高光贴图
	texture_info_t SpecularLevelMap;	// 高光亮度贴图	
	texture_info_t GlossinessMap;		// 光泽度贴图
	texture_info_t EmissiveMap;			// 自发光贴图
	texture_info_t ReflectionMap;		// 反射贴图
	texture_info_t DisplacementMap;		// 位移贴图
	texture_info_t FilterMap;			// 遮罩贴图
};

// 为模型引用专门指定的材质参数
struct custom_material_t
{
	const char* pMaterialName;			// 材质名
	material_info_t MatInfo;			// 材质数据
};

// 贴图坐标参数
struct uv_param_t
{
	float fOffsetU;
	float fOffsetV;
	float fRotateU;
	float fRotateV;
	float fRotateW;
	float fTilingU;
	float fTilingV;
};

// 节点材质
struct node_material_t
{
	ModelVF vf;
	model_node_t* pNode;
	unsigned int nNameLen;
	char* pszName;
	int nIndexInNode; // 在节点中的索引值
	int nIndexInModel; // 在整个模型中的索引值
	bool bNoLerpM;
	bool bNoLerpT;
	unsigned int nVertexCount;
	unsigned int* pLODIndexCount;
	unsigned int nSingleVertexSize;
	unsigned int nMultiVertexSize;
	unsigned int nSkinnedVertexBoneBlendVertexSize;
	//D3D11_INPUT_ELEMENT_DESC* pVElement;
	//unsigned int nVElementSize;
	//IDxInputLayout* pInputLayout;
	char* pszVertexDefine;
	unsigned int nVertexDefine;
	unsigned int nCurLODLevel;
	unsigned short** pLODIB;
	//float* pSingleVB;
	vertex_data_t SingleVB;
	//float* pMultiVB;
	float**	pMultiVBs;
	IStaticIB* pGPUIB;
	IStaticVB* pSingleGPUVB;
	IDynamicVB* pMultiGPUVB;
	IStaticVB* pVBlendGPUVB;
	material_info_t MatInfo; // 材质数据
	FmVec3* pFrameAmbient; // 材质动画数据
	FmVec3* pFrameDiffuse;
	FmVec3* pFrameSpecular;
	FmVec3* pFrameEmissive;
	float* pFrameGlossiness;
	float* pFrameOpacity;
	float*							pFrameTexture; // 漫反射贴图序列帧动画数据
	filter_param*					pFrameFilterParam; // 遮罩参数动画数据
	uv_param_t* pFrameDiffuseTM; // 漫反射贴图UV动画数据
	uv_param_t*	pFrameFilterTM; // 遮罩贴图UV动画数据	
	node_weight_table_t* pWeightTable; // 
	unsigned int nBoneNameListLen; // Contain the character '\0'
	char* pszBoneNameList; // Bone name list format:Bone01\0Bone02\0
	unsigned int nMappedBoneCount;
	unsigned int* pBoneList;
	unsigned int nMaterialInfo;
	unsigned int nMaterialInfoEx;
};

// 节点摄像机数据
struct node_camera_t
{
	float fFov;
	float fFarClip;
	float fNearClip;
};

// 模型节点
struct model_node_t
{
	FXNODE_TYPE nType;
	unsigned int nNameLen;
	char* pszName;
	node_camera_t* pCamera;
	location_t* pLocation;
	FmVec3 vBoundMin;
	FmVec3 vBoundMax;
	unsigned int nMaterialCount;
	node_material_t* Materials;
	unsigned int nChildNodeCount;
	model_node_t* ChildNodes;
	COctTree* pOctTree;
	bool bNoLerp;
	bool bHide;
	bool bNeedDraw;
	bool bWaterClip;
	unsigned int nAttachedHelperCount;
	model_node_t* pAttachedHelpers;
	char* pszAttachedHelperParentName;
	unsigned int nAttachedHelperParentNameLen;
	unsigned int nModelInfo;
	int nIndex;
};

// 模型数据
struct model_t
{
	unsigned int nFlag;
	unsigned int nVersion;
	unsigned int nFileType;
	unsigned int bOrigin;
	unsigned int nNameLen;
	char* pszName;
	unsigned int nStartFrame;
	unsigned int nEndFrame;
	float fInternalFPS;
	FmVec3 vRootMin;
	FmVec3 vRootMax;
	FmVec3 vCenter;
	unsigned int nRootNodeCount;
	model_node_t* RootNodes;
	bool bLoaded;
	unsigned int nNodeCount;
	model_node_t** pNodeList;
	char* pszTexBase;
	unsigned int nMaterialCount; // Specified how many material in current model
	unsigned int nLODCount;
	unsigned int nCurLODLevel;
};

// 记录日志信息
static void LOG_ERROR(const char* format, ...)
{
	char buf[256] = { 0 };
	va_list	vlist;

	va_start(vlist, format);

	if (vsnprintf(buf, sizeof(buf) - 1, format, vlist) < 0)
	{
		buf[sizeof(buf) - 1] = 0;
	}

	va_end(vlist);
	CORE_TRACE(buf);
}

// 查找模型的节点
model_node_t* find_model_node(model_t* pModel, const char* node_name);
// 获得模型节点数据是否可编辑
bool get_model_node_allow_modify(const char* key);
// 设置模型节点数据
bool set_model_node_value(model_node_t* pNode, const char* key, 
	const char* val);
// 获得模型节点数据
bool get_model_node_value(model_node_t* pNode, const char* key, char* val, 
	unsigned int val_size);
// 加载模型实例
model_t* load_model(const char* pszModelName);
// 卸载模型实例
void unload_model(model_t* pModel);

#endif // _MODEL_DATA_H
