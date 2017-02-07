//--------------------------------------------------------------------
// 文件名:		model_player_shader.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2009年5月4日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "model_player.h"
#include "../visual/i_render.h"
#include "../utils/array_pod.h"
#include "../visual/shader_list.h"
#include "../utils/pod_hash_map.h" 
//#include <map>

extern IRender* g_pRender;

ShaderManager * ShaderManager::s_inst = NULL;

#define SET_VERTEX_DEFINE(vs_list) \
	vs_list.SetDefine(VERTEX_COLOR_SHIFT, "#define VERTEX_COLOR 1\n"); \
	vs_list.SetDefine(VERTEX_TANGENT_SHIFT, "#define VERTEX_TANGENT 1\n"); \
	vs_list.SetDefine(VERTEX_BINORMAL_SHIFT, "#define VERTEX_BINORMAL 1\n"); \
	vs_list.SetDefine(VERTEX_TEXTURE0_SHIFT, "#define VERTEX_TEXTURE0 1\n"); \
	vs_list.SetDefine(VERTEX_TEXTURE1_SHIFT, "#define VERTEX_TEXTURE1 1\n"); \
	vs_list.SetDefine(VERTEX_ANIMATION_SHIFT, "#define VERTEX_ANIMATION 1\n"); \
	vs_list.SetDefine(VERTEX_SKELETON_SHIFT, "#define VERTEX_SKELETON 1\n"); \
	vs_list.SetDefine(VERTEX_INSTANCE_SHIFT, "#define VERTEX_INSTANCE 1\n");

enum MODEL_VS_SHIFT_ENUM
{
	MODEL_VS_TEXTUREANI = VERTEX_SHIFT_MAX,
	MODEL_VS_DIFFUSEMAP,
	MODEL_VS_NORMALMAP,
	MODEL_VS_SPECULARMAP,
	MODEL_VS_DISAPPEAR,
	MODEL_VS_VIEWVECTOR,
	MODEL_VS_GAMMA,
	MODEL_VS_CLIPPLANE,
	MODEL_VS_FILTER,
	MODEL_VS_KEEPOUT,
	MODEL_VS_SHADOWMAP,
	MODEL_VS_HEIGHT_FOG,
	MODEL_VS_FOGEXP,
	MODEL_VS_FOGLINEAR,
	MODEL_VS_FILTERTEXTUREANI,
	MODEL_VS_MAX,
};

enum MODEL_PS_SHIFT_ENUM
{
	MODEL_PS_DIFFUSE,
	MODEL_PS_GREATERQUAL,
	MODEL_PS_DIFFUSEMAP,
	MODEL_PS_NORMALMAP,
	MODEL_PS_SPECULAR,
	MODEL_PS_SPECULARMAP,
	MODEL_PS_EMISSIVE,
	MODEL_PS_EMISSIVEMAP,
	MODEL_PS_LIGHTMAP,
	MODEL_PS_REFLECTION,
	//MODEL_PS_VOLUMETEXTURE,
	MODEL_PS_DYNAMICLIGHTING,
	MODEL_PS_SHADOWMAP,
	//MODEL_PS_PRELIGHT,
	MODEL_PS_FIXSHADOW,
	MODEL_PS_SKELETON,
	MODEL_PS_DISAPPEAR,
	MODEL_PS_OPACITY,
	MODEL_PS_APPENDCOLOR,
	MODEL_PS_FALLOFF,
	MODEL_PS_SPHEREAMBIENT,
	MODEL_PS_SKINEFFECT,
	MODEL_PS_FOGEXP,
	MODEL_PS_FOGLINEAR,
	MODEL_PS_ALPHATEST,
	//MODEL_PS_CLIPBLEND,
	MODEL_PS_CLIPSOLID,
	MODEL_PS_CLIPPLANE,
	MODEL_PS_ONLYZPASS,
	MODEL_PS_ONLYCOLOR,
	MODEL_PS_POINTLIGHT,
	MODEL_PS_FILTER,
	MODEL_PS_KEEPOUT,
	MODEL_PS_HEIGHT_FOG,
	MODEL_PS_BLEND_ENHANCE,
	MODEL_PS_MAX,
};

enum GBUFFER_VS_SHIFT_ENUM
{
	GBUFFER_VS_TEXTUREANI = VERTEX_SHIFT_MAX,
	GBUFFER_VS_NORMALMAP,
	GBUFFER_VS_FALLOFF,
	GBUFFER_VS_GAMMA,
	GBUFFER_VS_MAX,
};

enum GBUFFER_TESS_VS_SHIFT_ENUM
{
	GBUFFER_TESS_VS_TEXTUREANI = VERTEX_SHIFT_MAX,
	GBUFFER_TESS_VS_NORMALMAP,
	GBUFFER_TESS_VS_GAMMA,
	GBUFFER_TESS_VS_MAX,
};

enum GBUFFER_HS_SHIFT_ENUM
{
	GBUFFER_HS_NORMALMAP,
	GBUFFER_HS_VERTEX_COLOR,
	GBUFFER_HS_VERTEX_TEXTURE0,
	GBUFFER_HS_MAX,
};

enum GBUFFER_DS_SHIFT_ENUM
{
	GBUFFER_DS_NORMALMAP,
	GBUFFER_DS_VERTEX_COLOR,
	GBUFFER_DS_VERTEX_TEXTURE0,
	GBUFFER_DS_FALLOFF,
	GBUFFER_DS_MAX,
};

enum GBUFFER_PS_SHIFT_ENUM
{
	GBUFFER_PS_DIFFUSE,
	GBUFFER_PS_DIFFUSEMAP,
	GBUFFER_PS_NORMALMAP,
	GBUFFER_PS_SPECULARMAP,
	GBUFFER_PS_VOLUMETEXTURE,
	GBUFFER_PS_FALLOFF,
	GBUFFER_PS_APPENDCOLOR,
	GBUFFER_PS_SATURATION,
	GBUFFER_PS_ALPHATEST,
	GBUFFER_PS_CLIPBLEND,
	GBUFFER_PS_MAX,
};

enum DEPTH_VS_SHIFT_ENUM
{
	DEPTH_VS_ALPHATEST = VERTEX_SHIFT_MAX,
	DEPTH_VS_MAX,
};

enum DEPTH_PS_SHIFT_ENUM
{
	DEPTH_PS_ALPHATEST,
	DEPTH_PS_MAX,
};

enum SHADOWMAP_VS_SHIFT_ENUM
{
	SHADOWMAP_VS_ALPHATEST = VERTEX_SHIFT_MAX,
	SHADOWMAP_VS_MAX,
};

enum SHADOWMAP_TESS_VS_SHIFT_ENUM
{
	SHADOWMAP_TESS_VS_ALPHATEST = VERTEX_SHIFT_MAX,
	SHADOWMAP_TESS_VS_MAX,
};

enum SHADOWMAP_HS_SHIFT_ENUM
{
	SHADOWMAP_HS_ALPHATEST,
	SHADOWMAP_HS_MAX,
};

enum SHADOWMAP_DS_SHIFT_ENUM
{
	SHADOWMAP_DS_ALPHATEST,
	SHADOWMAP_DS_MAX,
};

enum SHADOWMAP_PS_SHIFT_ENUM
{
	SHADOWMAP_PS_ALPHATEST,
	SHADOWMAP_PS_MAX,
};

enum APPLIQUE_VS_SHIFT_ENUM
{
	APPLIQUE_VS_FOGDEPTH = VERTEX_SHIFT_MAX,
	APPLIQUE_VS_MAX,
};

enum APPLIQUE_PS_SHIFT_ENUM
{
	APPLIQUE_PS_VOLUMETEXTURE,
	APPLIQUE_PS_SHADOWMAP,
	APPLIQUE_PS_PRELIGHT,
	APPLIQUE_PS_DISAPPEAR,
	APPLIQUE_PS_OPACITY,
	APPLIQUE_PS_APPENDCOLOR,
	APPLIQUE_PS_FOGEXP,
	APPLIQUE_PS_FOGLINEAR,
	APPLIQUE_PS_USEINTZ,
	APPLIQUE_PS_MULTIPLY,
	APPLIQUE_PS_STEREO,
	APPLIQUE_PS_MAX,
};

enum REFRACTION_VS_SHIFT_ENUM
{
	REFRACTION_VS_TEXTUREANI = VERTEX_SHIFT_MAX,
	REFRACTION_VS_NORMALMAP,
	REFRACTION_VS_FALLOFF,
	REFRACTION_VS_MAX,
};

enum REFRACTION_PS_SHIFT_ENUM
{
	REFRACTION_PS_DIFFUSE,
	REFRACTION_PS_DIFFUSEMAP,
	REFRACTION_PS_NORMALMAP,
	REFRACTION_PS_VOLUMETEXTURE,
	REFRACTION_PS_OPACITY,
	REFRACTION_PS_MAX,
};

enum WATER_VS_SHIFT_ENUM
{
	WATER_VS_TEXTUREANI = VERTEX_SHIFT_MAX,
	WATER_VS_NORMALMAP,
	WATER_VS_VIEWVECTOR,
	WATER_VS_GAMMA,
	WATER_VS_MAX,
};

enum WATER_PS_SHIFT_ENUM
{
	WATER_PS_DIFFUSE,
	WATER_PS_DIFFUSEMAP,
	WATER_PS_NORMALMAP,
	WATER_PS_SPECULAR,
	WATER_PS_SPECULARMAP,
	WATER_PS_EMISSIVE,
	WATER_PS_EMISSIVEMAP,
	WATER_PS_REFLECTION,
	WATER_PS_OPACITY,
	WATER_PS_DEPTHMAP,
	WATER_PS_FOGEXP,
	WATER_PS_FOGLINEAR,
	WATER_PS_MAX,
};

enum VOLUMEFOG_VS_SHIFT_ENUM
{
	VOLUMEFOG_VS_FOGDEPTH = VERTEX_SHIFT_MAX,
	VOLUMEFOG_VS_MAX,
};

enum VOLUMEDEPTH_PS_SHIFT_ENUM
{
	VOLUMEDEPTH_PS_CW,
	VOLUMEDEPTH_PS_MAX,
};

enum VOLUMECOLOR_PS_SHIFT_ENUM
{
	VOLUMECOLOR_PS_APPENDCOLOR,
	VOLUMECOLOR_PS_OPACITY,
	VOLUMECOLOR_PS_FOGEXP,
	VOLUMECOLOR_PS_FOGLINEAR,
	VOLUMECOLOR_PS_MAX,
};

enum GLOW_VS_SHIFT_ENUM
{
	GLOW_VS_TEXTUREANI = VERTEX_SHIFT_MAX,
	GLOW_VS_FOGDEPTH,
	GLOW_VS_GAMMA,
	GLOW_VS_MAX,
};

enum GLOW_PS_SHIFT_ENUM
{
	GLOW_PS_DIFFUSE,
	GLOW_PS_DIFFUSEMAP,
	GLOW_PS_DYNAMICLIGHTING,
	GLOW_PS_APPENDCOLOR,
	GLOW_PS_EMISSIVE,
	GLOW_PS_EMISSIVEMAP,
	GLOW_PS_FOGEXP,
	GLOW_PS_FOGLINEAR,
	GLOW_PS_MAX,
};

static TShaderManyDefines<IVertexShader, MODEL_VS_MAX> s_VSList;
static TShaderManyDefines<IPixelShader, MODEL_PS_MAX> s_PSList;
static TShaderList<IVertexShader, GBUFFER_VS_MAX> s_GBufferVSList;
static TShaderList<IVertexShader, GBUFFER_TESS_VS_MAX> s_GBufferTessVSList;
//static TShaderList<IDxHullShader, GBUFFER_HS_MAX> s_GBufferHSList;
//static TShaderList<IDxDomainShader, GBUFFER_DS_MAX> s_GBufferDSList;
static TShaderList<IPixelShader, GBUFFER_PS_MAX> s_GBufferPSList;
static TShaderList<IVertexShader, DEPTH_VS_MAX> s_DepthVSList;
static TShaderList<IPixelShader, DEPTH_PS_MAX> s_DepthPSList;
static TShaderManyDefines<IVertexShader, SHADOWMAP_VS_MAX> s_ShadowMapVSList;
static TShaderList<IVertexShader, SHADOWMAP_TESS_VS_MAX> s_ShadowMapTessVSList;
//static TShaderList<IDxHullShader, SHADOWMAP_HS_MAX> s_ShadowMapHSList;
//static TShaderList<IDxDomainShader, SHADOWMAP_DS_MAX> s_ShadowMapDSList;
static TShaderManyDefines<IPixelShader, SHADOWMAP_PS_MAX> s_ShadowMapPSList;
static TShaderList<IVertexShader, APPLIQUE_VS_MAX> s_AppliqueVSList;
static TShaderList<IPixelShader, APPLIQUE_PS_MAX> s_AppliquePSList;
static TShaderList<IVertexShader, REFRACTION_VS_MAX> s_RefractionVSList;
static TShaderList<IPixelShader, REFRACTION_PS_MAX> s_RefractionPSList;
static TShaderList<IVertexShader, WATER_VS_MAX> s_WaterVSList;
static TShaderList<IPixelShader, WATER_PS_MAX> s_WaterPSList;
static TShaderList<IVertexShader, VOLUMEFOG_VS_MAX> s_VolumeFogVSList;
static TShaderList<IPixelShader, VOLUMEDEPTH_PS_MAX> s_VolumeFogDepthPSList;
static TShaderList<IPixelShader, VOLUMECOLOR_PS_MAX> s_VolumeFogColorPSList;
static TShaderList<IVertexShader, GLOW_VS_MAX> s_GlowVSList;
static TShaderList<IPixelShader, GLOW_PS_MAX> s_GlowPSList;

/*
IDxDepthState* g_pAppliqueMaskDepthState = NULL;
IDxDepthState* g_pAppliqueDrawDepthState = NULL;
IDxDepthState* g_pAppliqueClearDepthState = NULL;
IDxDepthState* g_pZWriteEnableDepthState = NULL;
IDxDepthState* g_pGlowMaskDepthState = NULL;
IDxDepthState* g_pGlowDrawDepthState = NULL;
IDxDepthState* g_pVolumeFogDepthState = NULL;
IDxRasterState* g_pCullNoneRasterState = NULL;
IDxRasterState* g_pCullFrontRasterState = NULL;
IDxBlendState* g_pNormalBlendState = NULL;
IDxBlendState* g_pBrightBlendState = NULL;
IDxBlendState* g_pEnhanceBlendState = NULL;
IDxBlendState* g_pDefaultBlendState = NULL;
IDxBlendState* g_pInverseBlendState = NULL;
IDxBlendState* g_pFactorBlendState = NULL;
IDxBlendState* g_pColorDisableBlendState = NULL;
*/


//IVertexShader* g_pVS = NULL;
//IPixelShader*  g_pPS = NULL;
//IShaderProgram* g_pShader = NULL;

void shader_initialize()
{
	// 设置着色器

	//g_pVS = g_pRender->LoadVertexShader("model.vsh", "main", "");
	//g_pPS = g_pRender->LoadPixelShader("model.fsh", "main", "");
	//g_pShader = g_pRender->CreateShaderProgram(g_pVS, g_pPS, vertex_t_name ,5);

	s_VSList.SetRender(g_pRender);
	s_VSList.SetShaderName("model.vsh", "main");
	SET_VERTEX_DEFINE(s_VSList);

	s_VSList.SetDefine(MODEL_VS_TEXTUREANI, "#define TEXTUREANI 1\n");
	s_VSList.SetDefine(MODEL_VS_DIFFUSEMAP, "#define DIFFUSEMAP 1\n");
	s_VSList.SetDefine(MODEL_VS_NORMALMAP, "#define NORMALMAP 1\n");
	s_VSList.SetDefine(MODEL_VS_SPECULARMAP, "#define SPECULARMAP 1\n");
	s_VSList.SetDefine(MODEL_VS_DISAPPEAR, "#define DISAPPEAR 1\n");
	s_VSList.SetDefine(MODEL_VS_VIEWVECTOR, "#define VIEWVECTOR 1\n");
	s_VSList.SetDefine(MODEL_VS_GAMMA, "#define GAMMA 1\n");
	s_VSList.SetDefine(MODEL_VS_CLIPPLANE, "#define CLIPPLANE 1\n");
	s_VSList.SetDefine(MODEL_VS_FILTER, "#define FILTERMAP 1\n");
	s_VSList.SetDefine(MODEL_VS_KEEPOUT, "#define KEEPOUT 1\n");
	s_VSList.SetDefine(MODEL_VS_SHADOWMAP, "#define SHADOWMAP 1\n");
	s_VSList.SetDefine(MODEL_VS_FOGEXP, "#define FOGEXP 1\n");
	s_VSList.SetDefine(MODEL_VS_FOGLINEAR, "#define FOGLINEAR 1\n");
	s_VSList.SetDefine(MODEL_VS_HEIGHT_FOG, "#define HEIGHT_FOG 1\n");
	s_VSList.SetDefine(MODEL_VS_FILTERTEXTUREANI, "#define FILTERTEXTUREANI 1\n");
	
	
	//s_VSList.LoadShader(0);
	
	s_PSList.SetRender(g_pRender);
	s_PSList.SetShaderName("model.fsh", "main");
	s_PSList.SetDefine(MODEL_PS_DIFFUSE, "#define DIFFUSE 1\n");
	s_PSList.SetDefine(MODEL_PS_GREATERQUAL, "#define ALPHATEST_GREATERQUAL 1\n");
	s_PSList.SetDefine(MODEL_PS_DIFFUSEMAP, "#define DIFFUSEMAP 1\n");
	s_PSList.SetDefine(MODEL_PS_NORMALMAP, "#define NORMALMAP 1\n");
	s_PSList.SetDefine(MODEL_PS_SPECULAR, "#define SPECULAR 1\n");
	s_PSList.SetDefine(MODEL_PS_SPECULARMAP, "#define SPECULARMAP 1\n");
	s_PSList.SetDefine(MODEL_PS_EMISSIVE, "#define EMISSIVE 1\n");
	s_PSList.SetDefine(MODEL_PS_EMISSIVEMAP, "#define EMISSIVEMAP 1\n");
	s_PSList.SetDefine(MODEL_PS_LIGHTMAP, "#define LIGHTMAP 1\n");
	s_PSList.SetDefine(MODEL_PS_REFLECTION, "#define REFLECTION 1\n");
//	s_PSList.SetDefine(MODEL_PS_VOLUMETEXTURE, "#define VOLUMETEXTURE 1\n");
	s_PSList.SetDefine(MODEL_PS_DYNAMICLIGHTING, "#define DYNAMICLIGHTING 1\n");
	s_PSList.SetDefine(MODEL_PS_SHADOWMAP, "#define SHADOWMAP 1\n");
//	s_PSList.SetDefine(MODEL_PS_PRELIGHT, "#define PRELIGHT 1\n");
	s_PSList.SetDefine(MODEL_PS_FIXSHADOW, "#define FIXSHADOW 1\n");
	s_PSList.SetDefine(MODEL_PS_SKELETON, "#define SKELETON 1\n");
	s_PSList.SetDefine(MODEL_PS_DISAPPEAR, "#define DISAPPEAR 1\n");
	s_PSList.SetDefine(MODEL_PS_OPACITY, "#define OPACITY 1\n");
	s_PSList.SetDefine(MODEL_PS_APPENDCOLOR, "#define APPENDCOLOR 1\n");
	s_PSList.SetDefine(MODEL_PS_FALLOFF, "#define FALLOFF 1\n");
	s_PSList.SetDefine(MODEL_PS_SPHEREAMBIENT, "#define SPHEREAMBIENT 1\n");
	s_PSList.SetDefine(MODEL_PS_SKINEFFECT, "#define SKINEFFECT 1\n");
	s_PSList.SetDefine(MODEL_PS_FOGEXP, "#define FOGEXP 1\n");
	s_PSList.SetDefine(MODEL_PS_FOGLINEAR, "#define FOGLINEAR 1\n");
	s_PSList.SetDefine(MODEL_PS_ALPHATEST, "#define ALPHATEST 1\n");
	//s_PSList.SetDefine(MODEL_PS_CLIPBLEND, "#define CLIPBLEND 1\n");
	s_PSList.SetDefine(MODEL_PS_CLIPSOLID, "#define CLIPSOLID 1\n");
	s_PSList.SetDefine(MODEL_PS_CLIPPLANE, "#define CLIPPLANE 1\n");
	s_PSList.SetDefine(MODEL_PS_ONLYZPASS, "#define ONLYZPASS 1\n");
	s_PSList.SetDefine(MODEL_PS_ONLYCOLOR, "#define ONLYCOLOR 1\n");
	s_PSList.SetDefine(MODEL_PS_POINTLIGHT, "#define POINTLIGHT 1\n");
	s_PSList.SetDefine(MODEL_PS_FILTER, "#define FILTERMAP 1\n");
	s_PSList.SetDefine(MODEL_PS_KEEPOUT, "#define KEEPOUT 1\n");
	s_PSList.SetDefine(MODEL_PS_HEIGHT_FOG, "#define HEIGHT_FOG 1\n");
	s_PSList.SetDefine(MODEL_PS_BLEND_ENHANCE, "#define BLEND_ENHANCE 1\n");

	s_ShadowMapVSList.SetRender(g_pRender);
	s_ShadowMapVSList.SetShaderName("model_shadow_map.vsh", "vs_main_model_shadow_map");
	SET_VERTEX_DEFINE(s_ShadowMapVSList);
	s_ShadowMapVSList.SetDefine(SHADOWMAP_VS_ALPHATEST, "#define ALPHATEST 1\n");

	s_ShadowMapPSList.SetRender(g_pRender);
	s_ShadowMapPSList.SetShaderName("model_shadow_map.fsh", "ps_main_model_shadow_map");
	s_ShadowMapPSList.SetDefine(SHADOWMAP_PS_ALPHATEST, "#define ALPHATEST 1\n");
	
	/*
	s_PSList.LoadShader(0);

	s_GBufferVSList.SetRender(g_pRender);
	s_GBufferVSList.SetShaderName("model_gbuffer.hlsl", 
		"vs_main_model_gbuffer");
	SET_VERTEX_DEFINE(s_GBufferVSList);
	s_GBufferVSList.SetDefine(GBUFFER_VS_TEXTUREANI, "TEXTUREANI=1;");
	s_GBufferVSList.SetDefine(GBUFFER_VS_NORMALMAP, "NORMALMAP=1;");
	s_GBufferVSList.SetDefine(GBUFFER_VS_FALLOFF, "FALLOFF=1;");
	s_GBufferVSList.SetDefine(GBUFFER_VS_GAMMA, "GAMMA=1;");
	s_GBufferVSList.LoadShader(0);

	s_GBufferTessVSList.SetRender(g_pRender);
	s_GBufferTessVSList.SetShaderName("model_gbuffer.hlsl", 
		"vs_tess_model_gbuffer");
	s_GBufferTessVSList.SetDefine(GBUFFER_TESS_VS_TEXTUREANI, "TEXTUREANI=1;");
	s_GBufferTessVSList.SetDefine(GBUFFER_TESS_VS_NORMALMAP, "NORMALMAP=1;");
	s_GBufferTessVSList.SetDefine(GBUFFER_TESS_VS_GAMMA, "GAMMA=1;");
	*/

	/*
	s_GBufferHSList.SetRender(g_pRender);
	s_GBufferHSList.SetShaderName("model_gbuffer.hlsl", 
		"hs_main_model_gbuffer");
	s_GBufferHSList.SetDefine(GBUFFER_HS_NORMALMAP, "NORMALMAP=1;");
	s_GBufferHSList.SetDefine(GBUFFER_HS_VERTEX_COLOR, "VERTEX_COLOR=1;");
	s_GBufferHSList.SetDefine(GBUFFER_HS_VERTEX_TEXTURE0, "VERTEX_TEXTURE0=1;");

	s_GBufferDSList.SetRender(g_pRender);
	s_GBufferDSList.SetShaderName("model_gbuffer.hlsl", 
		"ds_main_model_gbuffer");
	s_GBufferDSList.SetDefine(GBUFFER_DS_NORMALMAP, "NORMALMAP=1;");
	s_GBufferDSList.SetDefine(GBUFFER_DS_VERTEX_COLOR, "VERTEX_COLOR=1;");
	s_GBufferDSList.SetDefine(GBUFFER_DS_VERTEX_TEXTURE0, "VERTEX_TEXTURE0=1;");
	s_GBufferDSList.SetDefine(GBUFFER_DS_FALLOFF, "FALLOFF=1;");

	s_GBufferPSList.SetRender(g_pRender);
	s_GBufferPSList.SetShaderName("model_gbuffer.hlsl", 
		"ps_main_model_gbuffer");
	s_GBufferPSList.SetDefine(GBUFFER_PS_DIFFUSE, "DIFFUSE=1;");
	s_GBufferPSList.SetDefine(GBUFFER_PS_DIFFUSEMAP, "DIFFUSEMAP=1;");
	s_GBufferPSList.SetDefine(GBUFFER_PS_NORMALMAP, "NORMALMAP=1;");
	s_GBufferPSList.SetDefine(GBUFFER_PS_SPECULARMAP, "SPECULARMAP=1;");
	s_GBufferPSList.SetDefine(GBUFFER_PS_VOLUMETEXTURE, "VOLUMETEXTURE=1;");
	s_GBufferPSList.SetDefine(GBUFFER_PS_FALLOFF, "FALLOFF=1;");
	s_GBufferPSList.SetDefine(GBUFFER_PS_APPENDCOLOR, "APPENDCOLOR=1;");
	s_GBufferPSList.SetDefine(GBUFFER_PS_SATURATION, "SATURATION=1;");
	s_GBufferPSList.SetDefine(GBUFFER_PS_ALPHATEST, "ALPHATEST=1;");
	s_GBufferPSList.SetDefine(GBUFFER_PS_CLIPBLEND, "CLIPBLEND=1;");
	s_GBufferPSList.LoadShader(0);
	*/

	//s_DepthMapVSList.SetRender(g_pRender);
	//s_DepthMapVSList.SetShaderName("model_depth_map.hlsl", 
	//	"vs_main_model_depth_map");
	//s_DepthMapVSList.SetDefine(0, "SKELETONANI=1;");
	//s_DepthMapVSList.SetDefine(1, "NORMAL=1;");
	//s_DepthMapVSList.SetDefine(2, "ALPHATEST=1;");
	//s_DepthMapVSList.SetDefine(3, "VERTEXANI=1;");
	//s_DepthMapVSList.SetDefine(4, "NORMALMAP=1;");
	//s_DepthMapVSList.SetDefine(5, "INSTANCE=1;");
	//s_DepthMapVSList.SetDefine(6, "ONLY_NORMAL=1;");
	//s_DepthMapVSList.LoadShader(0);

	//s_DepthMapPSList.SetRender(g_pRender);
	//s_DepthMapPSList.SetShaderName("model_depth_map.hlsl", 
	//	"ps_main_model_depth_map");
	//s_DepthMapPSList.SetDefine(0, "SKELETONANI=1;");
	//s_DepthMapPSList.SetDefine(1, "NORMAL=1;");
	//s_DepthMapPSList.SetDefine(2, "ALPHATEST=1;");
	//s_DepthMapPSList.SetDefine(3, "VERTEXANI=1;");
	//s_DepthMapPSList.SetDefine(4, "NORMALMAP=1;");
	//s_DepthMapPSList.SetDefine(5, "INSTANCE=1;");
	//s_DepthMapPSList.SetDefine(6, "ONLY_NORMAL=1;");
	//s_DepthMapPSList.LoadShader(0);

	//s_VelocityVSList.SetRender(g_pRender);
	//s_VelocityVSList.SetShaderName("model_velocity_map.hlsl", 
	//	"vs_main_model_velocity_map");
	//s_VelocityVSList.SetDefine(0, "SKELETON=1;");
	//s_VelocityVSList.SetDefine(1, "ALPHATEST=1;");
	//s_VelocityVSList.SetDefine(2, "SETNULL=1;");
	//s_VelocityVSList.SetDefine(3, "VERTEXANI=1;");
	//s_VelocityVSList.LoadShader(0);

	//s_VelocityPSList.SetRender(g_pRender);
	//s_VelocityPSList.SetShaderName("model_velocity_map.hlsl", 
	//	"ps_main_model_velocity_map");
	//s_VelocityPSList.SetDefine(0, "SKELETON=1;");
	//s_VelocityPSList.SetDefine(1, "ALPHATEST=1;");
	//s_VelocityPSList.SetDefine(2, "SETNULL=1;");
	//s_VelocityPSList.SetDefine(3, "VERTEXANI=1;");
	//s_VelocityPSList.LoadShader(0);

	/*
	s_DepthVSList.SetRender(g_pRender);
	s_DepthVSList.SetShaderName("model_depth.hlsl", "vs_main_model_depth");
	SET_VERTEX_DEFINE(s_DepthVSList);
	s_DepthVSList.SetDefine(DEPTH_VS_ALPHATEST, "ALPHATEST=1;");
	s_DepthVSList.LoadShader(0);

	s_DepthPSList.SetRender(g_pRender);
	s_DepthPSList.SetShaderName("model_depth.hlsl", "ps_main_model_depth");
	s_DepthPSList.SetDefine(DEPTH_PS_ALPHATEST, "ALPHATEST=1;");
	s_DepthPSList.LoadShader(0);

	s_ShadowMapVSList.SetRender(g_pRender);
	s_ShadowMapVSList.SetShaderName("model_shadow_map.hlsl", 
		"vs_main_model_shadow_map");
	SET_VERTEX_DEFINE(s_ShadowMapVSList);
	s_ShadowMapVSList.SetDefine(SHADOWMAP_VS_ALPHATEST, "ALPHATEST=1;");
	s_ShadowMapVSList.LoadShader(0);

	s_ShadowMapTessVSList.SetRender(g_pRender);
	s_ShadowMapTessVSList.SetShaderName("model_shadow_map.hlsl", 
		"vs_tess_model_shadow_map");
	s_ShadowMapTessVSList.SetDefine(SHADOWMAP_TESS_VS_ALPHATEST, "ALPHATEST=1;");
	*/

	/*
	s_ShadowMapHSList.SetRender(g_pRender);
	s_ShadowMapHSList.SetShaderName("model_shadow_map.hlsl", 
		"hs_main_model_shadow_map");
	s_ShadowMapHSList.SetDefine(SHADOWMAP_HS_ALPHATEST, "ALPHATEST=1;");

	s_ShadowMapDSList.SetRender(g_pRender);
	s_ShadowMapDSList.SetShaderName("model_shadow_map.hlsl", 
		"ds_main_model_shadow_map");
	s_ShadowMapDSList.SetDefine(SHADOWMAP_HS_ALPHATEST, "ALPHATEST=1;");

	s_ShadowMapPSList.SetRender(g_pRender);
	s_ShadowMapPSList.SetShaderName("model_shadow_map.hlsl", 
		"ps_main_model_shadow_map");
	s_ShadowMapPSList.SetDefine(SHADOWMAP_PS_ALPHATEST, "ALPHATEST=1;");
	s_ShadowMapPSList.LoadShader(0);
	*/

//	s_AppliqueVSList.SetRender(g_pRender);
//	s_AppliqueVSList.SetShaderName("model_applique.hlsl", 
//		"vs_main_model_applique");
//	s_AppliqueVSList.SetDefine(0, "SHADOWMAP=1;");
//	s_AppliqueVSList.SetDefine(1, "VERTEXANI=1;");
//	s_AppliqueVSList.SetDefine(2, "DISAPPEAR=1;");
//	s_AppliqueVSList.SetDefine(3, "OPACITY=1;");
//	s_AppliqueVSList.SetDefine(4, "VOLUMETEXTURE=1;");
//	s_AppliqueVSList.SetDefine(5, "APPENDCOLOR=1;");
//	s_AppliqueVSList.SetDefine(6, "PRELIGHT=1;");
//	s_AppliqueVSList.SetDefine(7, "FOGLINEAR=1;");
//	s_AppliqueVSList.SetDefine(8, "FOGEXP=1;");
//	s_AppliqueVSList.SetDefine(9, "USE_INTZ=1;");
//	s_AppliqueVSList.SetDefine(10, "USE_RAWZ=1;");
//	s_AppliqueVSList.SetDefine(11, "ONLY_NORMAL=1;");
//	s_AppliqueVSList.SetDefine(12, "MULTIPLY=1;");
//	s_AppliqueVSList.SetDefine(13, "STEREO=1;");
//	s_AppliqueVSList.LoadShader(0);
//
//	s_AppliquePSList.SetRender(g_pRender);
//	s_AppliquePSList.SetShaderName("model_applique.hlsl", 
//		"ps_main_model_applique");
//	s_AppliquePSList.SetDefine(0, "SHADOWMAP=1;");
//	s_AppliquePSList.SetDefine(1, "VERTEXANI=1;");
//	s_AppliquePSList.SetDefine(2, "DISAPPEAR=1;");
//	s_AppliquePSList.SetDefine(3, "OPACITY=1;");
//	s_AppliquePSList.SetDefine(4, "VOLUMETEXTURE=1;");
//	s_AppliquePSList.SetDefine(5, "APPENDCOLOR=1;");
//	s_AppliquePSList.SetDefine(6, "PRELIGHT=1;");
//	s_AppliquePSList.SetDefine(7, "FOGLINEAR=1;");
//	s_AppliquePSList.SetDefine(8, "FOGEXP=1;");
//	s_AppliquePSList.SetDefine(9, "USE_INTZ=1;");
//	s_AppliquePSList.SetDefine(10, "USE_RAWZ=1;");
//	s_AppliquePSList.SetDefine(11, "ONLY_NORMAL=1;");
//	s_AppliquePSList.SetDefine(12, "MULTIPLY=1;");
//	s_AppliquePSList.SetDefine(13, "STEREO=1;");
//	s_AppliquePSList.LoadShader(0);

    /*
	s_AppliqueVSList.SetRender(g_pRender);
	s_AppliqueVSList.SetShaderName("model_applique.hlsl", 
		"vs_main_model_applique");
	SET_VERTEX_DEFINE(s_AppliqueVSList);
	s_AppliqueVSList.SetDefine(APPLIQUE_VS_FOGDEPTH, "FOGDEPTH=1;");
	s_AppliqueVSList.LoadShader(0);

	s_AppliquePSList.SetRender(g_pRender);
	s_AppliquePSList.SetShaderName("model_applique.hlsl", 
		"ps_main_model_applique");
	s_AppliquePSList.SetDefine(APPLIQUE_PS_VOLUMETEXTURE, "VOLUMETEXTURE=1;");
	s_AppliquePSList.SetDefine(APPLIQUE_PS_SHADOWMAP, "SHADOWMAP=1;");
	s_AppliquePSList.SetDefine(APPLIQUE_PS_PRELIGHT, "PRELIGHT=1;");
	s_AppliquePSList.SetDefine(APPLIQUE_PS_DISAPPEAR, "DISAPPEAR=1;");
	s_AppliquePSList.SetDefine(APPLIQUE_PS_OPACITY, "OPACITY=1;");
	s_AppliquePSList.SetDefine(APPLIQUE_PS_APPENDCOLOR, "APPENDCOLOR=1;");
	s_AppliquePSList.SetDefine(APPLIQUE_PS_FOGEXP, "FOGEXP=1;");
	s_AppliquePSList.SetDefine(APPLIQUE_PS_FOGLINEAR, "FOGLINEAR=1;");
	s_AppliquePSList.SetDefine(APPLIQUE_PS_USEINTZ, "USEINTZ=1;");
	s_AppliquePSList.SetDefine(APPLIQUE_PS_MULTIPLY, "MULTIPLY=1;");
	s_AppliquePSList.SetDefine(APPLIQUE_PS_STEREO, "STEREO=1;");
	s_AppliquePSList.LoadShader(0);

	s_RefractionVSList.SetRender(g_pRender);
	s_RefractionVSList.SetShaderName("model_refraction.hlsl", 
		"vs_main_model_refraction");
	SET_VERTEX_DEFINE(s_RefractionVSList);
	s_RefractionVSList.SetDefine(REFRACTION_VS_TEXTUREANI, "TEXTUREANI=1;");
	s_RefractionVSList.SetDefine(REFRACTION_VS_NORMALMAP, "NORMALMAP=1;");
	s_RefractionVSList.LoadShader(0);

	s_RefractionPSList.SetRender(g_pRender);
	s_RefractionPSList.SetShaderName("model_refraction.hlsl", 
		"ps_main_model_refraction");
	s_RefractionPSList.SetDefine(REFRACTION_PS_DIFFUSE, "DIFFUSE=1;");
	s_RefractionPSList.SetDefine(REFRACTION_PS_DIFFUSEMAP, "DIFFUSEMAP=1;");
	s_RefractionPSList.SetDefine(REFRACTION_PS_NORMALMAP, "NORMALMAP=1;");
	s_RefractionPSList.SetDefine(REFRACTION_PS_VOLUMETEXTURE, "VOLUMETEXTURE=1;");
	s_RefractionPSList.SetDefine(REFRACTION_PS_OPACITY, "OPACITY=1;");
	s_RefractionPSList.LoadShader(0);
	*/

//	s_RefractionVSList.SetRender(g_pRender);
//	s_RefractionVSList.SetShaderName("model_refraction.hlsl", 
//		"vs_main_model_refraction");
//	s_RefractionVSList.SetDefine(0, "VERTEXCOLOR=1;");
//	s_RefractionVSList.SetDefine(1, "VERTEXANI=1;");
//	s_RefractionVSList.SetDefine(2, "SKELETONANI=1;");
//	s_RefractionVSList.SetDefine(3, "DIFFUSEMAP=1;");
//	s_RefractionVSList.SetDefine(4, "TEXTUREANI=1;");
//	s_RefractionVSList.SetDefine(5, "NORMALMAP=1;");
//	s_RefractionVSList.SetDefine(6, "OPACITY=1;");
//	s_RefractionVSList.SetDefine(7, "VOLUMETEXTURE=1;");
//	s_RefractionVSList.LoadShader(0);
//
//	s_RefractionPSList.SetRender(g_pRender);
//	s_RefractionPSList.SetShaderName("model_refraction.hlsl", 
//		"ps_main_model_refraction");
//	s_RefractionPSList.SetDefine(0, "VERTEXCOLOR=1;");
//	s_RefractionPSList.SetDefine(1, "VERTEXANI=1;");
//	s_RefractionPSList.SetDefine(2, "SKELETONANI=1;");
//	s_RefractionPSList.SetDefine(3, "DIFFUSEMAP=1;");
//	s_RefractionPSList.SetDefine(4, "TEXTUREANI=1;");
//	s_RefractionPSList.SetDefine(5, "NORMALMAP=1;");
//	s_RefractionPSList.SetDefine(6, "OPACITY=1;");
//	s_RefractionPSList.SetDefine(7, "VOLUMETEXTURE=1;");
//	s_RefractionPSList.LoadShader(0);

//	s_WaterSurfaceVSList.SetRender(g_pRender);
//	s_WaterSurfaceVSList.SetShaderName("model_water.hlsl", 
//		"vs_main_model_water");
//	s_WaterSurfaceVSList.SetDefine(0, "VERTEXCOLOR=1;");
//	s_WaterSurfaceVSList.SetDefine(1, "VERTEXANI=1;");
//	s_WaterSurfaceVSList.SetDefine(2, "SKELETONANI=1;");
//	s_WaterSurfaceVSList.SetDefine(3, "DIFFUSEMAP=1;");
//	s_WaterSurfaceVSList.SetDefine(4, "NORMALMAP=1;");
//	s_WaterSurfaceVSList.SetDefine(5, "REFLECTION=1;");
//	s_WaterSurfaceVSList.SetDefine(6, "SPECULARMAP=1;");
//	s_WaterSurfaceVSList.SetDefine(7, "TEXTUREANI=1;");
//	s_WaterSurfaceVSList.SetDefine(8, "SPECULAR=1;");
//	s_WaterSurfaceVSList.SetDefine(9, "DEPTHMAP=1;");
//	s_WaterSurfaceVSList.SetDefine(10, "APPENDCOLOR=1;");
//	s_WaterSurfaceVSList.SetDefine(11, "EMISSIVE=1;");
//	s_WaterSurfaceVSList.SetDefine(12, "EMISSIVEMAP=1;");
//	s_WaterSurfaceVSList.SetDefine(13, "FOGLINEAR=1;");
//	s_WaterSurfaceVSList.SetDefine(14, "FOGEXP=1;");
//	s_WaterSurfaceVSList.SetDefine(15, "OPACITY=1;");
//	s_WaterSurfaceVSList.SetDefine(16, "VOLUMETEXTURE=1;");
//	s_WaterSurfaceVSList.LoadShader(0);
//
//	s_WaterSurfacePSList.SetRender(g_pRender);
//	s_WaterSurfacePSList.SetShaderName("model_water.hlsl", 
//		"ps_main_model_water");
//	s_WaterSurfacePSList.SetDefine(0, "VERTEXCOLOR=1;");
//	s_WaterSurfacePSList.SetDefine(1, "VERTEXANI=1;");
//	s_WaterSurfacePSList.SetDefine(2, "SKELETONANI=1;");
//	s_WaterSurfacePSList.SetDefine(3, "DIFFUSEMAP=1;");
//	s_WaterSurfacePSList.SetDefine(4, "NORMALMAP=1;");
//	s_WaterSurfacePSList.SetDefine(5, "REFLECTION=1;");
//	s_WaterSurfacePSList.SetDefine(6, "SPECULARMAP=1;");
//	s_WaterSurfacePSList.SetDefine(7, "TEXTUREANI=1;");
//	s_WaterSurfacePSList.SetDefine(8, "SPECULAR=1;");
//	s_WaterSurfacePSList.SetDefine(9, "DEPTHMAP=1;");
//	s_WaterSurfacePSList.SetDefine(10, "APPENDCOLOR=1;");
//	s_WaterSurfacePSList.SetDefine(11, "EMISSIVE=1;");
//	s_WaterSurfacePSList.SetDefine(12, "EMISSIVEMAP=1;");
//	s_WaterSurfacePSList.SetDefine(13, "FOGLINEAR=1;");
//	s_WaterSurfacePSList.SetDefine(14, "FOGEXP=1;");
//	s_WaterSurfacePSList.SetDefine(15, "OPACITY=1;");
//	s_WaterSurfacePSList.SetDefine(16, "VOLUMETEXTURE=1;");
//	s_WaterSurfacePSList.LoadShader(0);

    /*
	s_WaterVSList.SetRender(g_pRender);
	s_WaterVSList.SetShaderName("model_water.hlsl", "vs_main_model_water");
	SET_VERTEX_DEFINE(s_WaterVSList);
	s_WaterVSList.SetDefine(WATER_VS_TEXTUREANI, "TEXTUREANI=1;");
	s_WaterVSList.SetDefine(WATER_VS_NORMALMAP, "NORMALMAP=1;");
	s_WaterVSList.SetDefine(WATER_VS_VIEWVECTOR, "VIEWVECTOR=1;");
	s_WaterVSList.SetDefine(WATER_VS_GAMMA, "GAMMA=1;");
	s_WaterVSList.LoadShader(0);

	s_WaterPSList.SetRender(g_pRender);
	s_WaterPSList.SetShaderName("model_water.hlsl", "ps_main_model_water");
	s_WaterPSList.SetDefine(WATER_PS_DIFFUSE, "DIFFUSE=1;");
	s_WaterPSList.SetDefine(WATER_PS_DIFFUSEMAP, "DIFFUSEMAP=1;");
	s_WaterPSList.SetDefine(WATER_PS_NORMALMAP, "NORMALMAP=1;");
	s_WaterPSList.SetDefine(WATER_PS_SPECULAR, "SPECULAR=1;");
	s_WaterPSList.SetDefine(WATER_PS_SPECULARMAP, "SPECULARMAP=1;");
	s_WaterPSList.SetDefine(WATER_PS_EMISSIVE, "EMISSIVE=1;");
	s_WaterPSList.SetDefine(WATER_PS_EMISSIVEMAP, "EMISSIVEMAP=1;");
	s_WaterPSList.SetDefine(WATER_PS_REFLECTION, "REFLECTION=1;");
	s_WaterPSList.SetDefine(WATER_PS_OPACITY, "OPACITY=1;");
	s_WaterPSList.SetDefine(WATER_PS_DEPTHMAP, "DEPTHMAP=1;");
	s_WaterPSList.SetDefine(WATER_PS_FOGEXP, "FOGEXP=1;");
	s_WaterPSList.SetDefine(WATER_PS_FOGLINEAR, "FOGLINEAR=1;");
	s_WaterPSList.LoadShader(0);
	*/

//	s_VolumeFogDepthVSList.SetRender(g_pRender);
//	s_VolumeFogDepthVSList.SetShaderName("model_volume_fog.hlsl", 
//		"vs_main_model_volume_fog");
//	s_VolumeFogDepthVSList.SetDefine(0, "VERTEXANI=1;");
//	s_VolumeFogDepthVSList.SetDefine(1, "SKELETONANI=1;");
//	s_VolumeFogDepthVSList.SetDefine(2, "CW=1;");
//	s_VolumeFogDepthVSList.LoadShader(0);
//
//	s_VolumeFogDepthPSList.SetRender(g_pRender);
//	s_VolumeFogDepthPSList.SetShaderName("model_volume_fog.hlsl", 
//		"ps_main_model_volume_depth");
//	s_VolumeFogDepthPSList.SetDefine(0, "VERTEXANI=1;");
//	s_VolumeFogDepthPSList.SetDefine(1, "SKELETONANI=1;");
//	s_VolumeFogDepthPSList.SetDefine(2, "CW=1;");
//	s_VolumeFogDepthPSList.LoadShader(0);
//
//	s_VolumeFogColorVSList.SetRender(g_pRender);
//	s_VolumeFogColorVSList.SetShaderName("model_volume_fog.hlsl", 
//		"vs_main_model_volume_fog");
//	s_VolumeFogColorVSList.SetDefine(0, "VERTEXANI=1;");
//	s_VolumeFogColorVSList.SetDefine(1, "SKELETONANI=1;");
//	s_VolumeFogColorVSList.SetDefine(2, "OPACITY=1;");
//	s_VolumeFogColorVSList.SetDefine(3, "APPENDCOLOR=1;");
//	s_VolumeFogColorVSList.SetDefine(4, "FOGLINEAR=1;");
//	s_VolumeFogColorVSList.SetDefine(5, "FOGEXP=1;");
//	s_VolumeFogColorVSList.LoadShader(0);
//
//	s_VolumeFogColorPSList.SetRender(g_pRender);
//	s_VolumeFogColorPSList.SetShaderName("model_volume_fog.hlsl", 
//		"ps_main_model_volume_color");
//	s_VolumeFogColorPSList.SetDefine(0, "VERTEXANI=1;");
//	s_VolumeFogColorPSList.SetDefine(1, "SKELETONANI=1;");
//	s_VolumeFogColorPSList.SetDefine(2, "OPACITY=1;");
//	s_VolumeFogColorPSList.SetDefine(3, "APPENDCOLOR=1;");
//	s_VolumeFogColorPSList.SetDefine(4, "FOGLINEAR=1;");
//	s_VolumeFogColorPSList.SetDefine(5, "FOGEXP=1;");
//	s_VolumeFogColorPSList.LoadShader(0);

	/*
	s_VolumeFogVSList.SetRender(g_pRender);
	s_VolumeFogVSList.SetShaderName("model_volume_fog.hlsl", 
		"vs_main_model_volume_fog");
	SET_VERTEX_DEFINE(s_VolumeFogVSList);
	s_VolumeFogVSList.SetDefine(VOLUMEFOG_VS_FOGDEPTH, "FOGDEPTH=1;");
	s_VolumeFogVSList.LoadShader(0);

	s_VolumeFogDepthPSList.SetRender(g_pRender);
	s_VolumeFogDepthPSList.SetShaderName("model_volume_fog.hlsl", 
		"ps_main_model_volume_depth");
	s_VolumeFogDepthPSList.SetDefine(VOLUMEDEPTH_PS_CW, "CW=1;");
	s_VolumeFogDepthPSList.LoadShader(0);

	s_VolumeFogColorPSList.SetRender(g_pRender);
	s_VolumeFogColorPSList.SetShaderName("model_volume_fog.hlsl", 
		"ps_main_model_volume_color");
	s_VolumeFogColorPSList.SetDefine(VOLUMECOLOR_PS_APPENDCOLOR, "APPENDCOLOR=1;");
	s_VolumeFogColorPSList.SetDefine(VOLUMECOLOR_PS_OPACITY, "OPACITY=1;");
	s_VolumeFogColorPSList.SetDefine(VOLUMECOLOR_PS_FOGEXP, "FOGEXP=1;");
	s_VolumeFogColorPSList.SetDefine(VOLUMECOLOR_PS_FOGLINEAR, "FOGLINEAR=1;");
	s_VolumeFogColorPSList.LoadShader(0);
	*/

//	s_GlowVSList.SetRender(g_pRender);
//	s_GlowVSList.SetShaderName("model_glow.hlsl", "vs_main_model_glow");
//	s_GlowVSList.SetDefine(0, "VERTEXCOLOR=1;");
//	s_GlowVSList.SetDefine(1, "VERTEXANI=1;");
//	s_GlowVSList.SetDefine(2, "SKELETONANI=1;");
//	s_GlowVSList.SetDefine(3, "DIFFUSEMAP=1;");
//	s_GlowVSList.SetDefine(4, "DYNAMIC_LIGHTING=1;");
//	s_GlowVSList.SetDefine(5, "TEXTUREANI=1;");
//	s_GlowVSList.SetDefine(6, "APPENDCOLOR=1;");
//	s_GlowVSList.SetDefine(7, "EMISSIVE=1;");
//	s_GlowVSList.SetDefine(8, "EMISSIVEMAP=1;");
//	s_GlowVSList.SetDefine(9, "FOGLINEAR=1;");
//	s_GlowVSList.SetDefine(10, "FOGEXP=1;");
//	s_GlowVSList.LoadShader(0);
//
//	s_GlowPSList.SetRender(g_pRender);
//	s_GlowPSList.SetShaderName("model_glow.hlsl", "ps_main_model_glow");
//	s_GlowPSList.SetDefine(0, "VERTEXCOLOR=1;");
//	s_GlowPSList.SetDefine(1, "VERTEXANI=1;");
//	s_GlowPSList.SetDefine(2, "SKELETONANI=1;");
//	s_GlowPSList.SetDefine(3, "DIFFUSEMAP=1;");
//	s_GlowPSList.SetDefine(4, "DYNAMIC_LIGHTING=1;");
//	s_GlowPSList.SetDefine(5, "TEXTUREANI=1;");
//	s_GlowPSList.SetDefine(6, "APPENDCOLOR=1;");
//	s_GlowPSList.SetDefine(7, "EMISSIVE=1;");
//	s_GlowPSList.SetDefine(8, "EMISSIVEMAP=1;");
//	s_GlowPSList.SetDefine(9, "FOGLINEAR=1;");
//	s_GlowPSList.SetDefine(10, "FOGEXP=1;");
//	s_GlowPSList.LoadShader(0);

	/*
	s_GlowVSList.SetRender(g_pRender);
	s_GlowVSList.SetShaderName("model_glow.hlsl", "vs_main_model_glow");
	SET_VERTEX_DEFINE(s_GlowVSList);
	s_GlowVSList.SetDefine(GLOW_VS_TEXTUREANI, "TEXTUREANI=1;");
	s_GlowVSList.SetDefine(GLOW_VS_FOGDEPTH, "FOGDEPTH=1;");
	s_GlowVSList.SetDefine(GLOW_VS_GAMMA, "GAMMA=1;");
	s_GlowVSList.LoadShader(0);

	s_GlowPSList.SetRender(g_pRender);
	s_GlowPSList.SetShaderName("model_glow.hlsl", "ps_main_model_glow");
	s_GlowPSList.SetDefine(GLOW_PS_DIFFUSE, "DIFFUSE=1;");
	s_GlowPSList.SetDefine(GLOW_PS_DIFFUSEMAP, "DIFFUSEMAP=1;");
	s_GlowPSList.SetDefine(GLOW_PS_DYNAMICLIGHTING, "DYNAMICLIGHTING=1;");
	s_GlowPSList.SetDefine(GLOW_PS_APPENDCOLOR, "APPENDCOLOR=1;");
	s_GlowPSList.SetDefine(GLOW_PS_EMISSIVE, "EMISSIVE=1;");
	s_GlowPSList.SetDefine(GLOW_PS_EMISSIVEMAP, "EMISSIVEMAP=1;");
	s_GlowPSList.SetDefine(GLOW_PS_FOGEXP, "FOGEXP=1;");
	s_GlowPSList.SetDefine(GLOW_PS_FOGLINEAR, "FOGLINEAR=1;");
	s_GlowPSList.LoadShader(0);
	*/

	/*
	D3D11_DEPTH_STENCIL_DESC dsd;

	memset(&dsd, 0, sizeof(dsd));
	dsd.DepthEnable = TRUE;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dsd.StencilEnable = TRUE;
	dsd.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsd.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsd.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	g_pAppliqueMaskDepthState = g_pRender->CreateDepthState(&dsd);

	dsd.DepthEnable = TRUE;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsd.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
	dsd.StencilEnable = TRUE;
	dsd.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsd.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_DECR;
	dsd.FrontFace.StencilFunc = D3D11_COMPARISON_LESS;
	dsd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilPassOp = D3D11_STENCIL_OP_DECR;
	dsd.BackFace.StencilFunc = D3D11_COMPARISON_LESS;

	g_pAppliqueDrawDepthState = g_pRender->CreateDepthState(&dsd);

	dsd.DepthEnable = TRUE;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsd.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
	dsd.StencilEnable = TRUE;
	dsd.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsd.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_DECR;
	dsd.FrontFace.StencilFunc = D3D11_COMPARISON_LESS;
	dsd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilPassOp = D3D11_STENCIL_OP_DECR;
	dsd.BackFace.StencilFunc = D3D11_COMPARISON_LESS;

	g_pAppliqueClearDepthState = g_pRender->CreateDepthState(&dsd);

	dsd.DepthEnable = TRUE;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dsd.StencilEnable = FALSE;
	dsd.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsd.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	g_pZWriteEnableDepthState = g_pRender->CreateDepthState(&dsd);

	dsd.DepthEnable = TRUE;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dsd.StencilEnable = TRUE;
	dsd.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsd.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	dsd.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	g_pGlowMaskDepthState = g_pRender->CreateDepthState(&dsd, 0x1);

	dsd.DepthEnable = TRUE;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dsd.StencilEnable = TRUE;
	dsd.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsd.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	dsd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	g_pGlowDrawDepthState = g_pRender->CreateDepthState(&dsd);

	dsd.DepthEnable = FALSE;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dsd.StencilEnable = TRUE;
	dsd.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsd.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	dsd.BackFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;

	g_pVolumeFogDepthState = g_pRender->CreateDepthState(&dsd, 0x1);

	D3D11_RASTERIZER_DESC rd;

	memset(&rd, 0, sizeof(rd));
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_NONE;
	rd.DepthClipEnable = TRUE;
	rd.MultisampleEnable = TRUE;
	rd.AntialiasedLineEnable = TRUE;

	g_pCullNoneRasterState = g_pRender->CreateRasterState(&rd);
	
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_FRONT;
	rd.DepthClipEnable = TRUE;
	rd.MultisampleEnable = TRUE;
	rd.AntialiasedLineEnable = TRUE;

	g_pCullFrontRasterState = g_pRender->CreateRasterState(&rd);

	D3D11_BLEND_DESC bd;
	
	memset(&bd, 0, sizeof(bd));
	bd.RenderTarget[0].BlendEnable = TRUE;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	g_pNormalBlendState = g_pRender->CreateBlendState(&bd);

	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;

	g_pBrightBlendState = g_pRender->CreateBlendState(&bd);
	
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;

	g_pEnhanceBlendState = g_pRender->CreateBlendState(&bd);

	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

	g_pDefaultBlendState = g_pRender->CreateBlendState(&bd);
	
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR;

	g_pInverseBlendState = g_pRender->CreateBlendState(&bd);
	
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_BLEND_FACTOR;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;

	g_pFactorBlendState = g_pRender->CreateBlendState(&bd);
	
	bd.RenderTarget[0].BlendEnable = FALSE;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].RenderTargetWriteMask = 0;

	g_pColorDisableBlendState = g_pRender->CreateBlendState(&bd);
	*/



}

void shader_release()
{
	ShaderManager::Inst().Destroy();
	s_VSList.Clear();
	s_PSList.Clear();
	s_ShadowMapVSList.Clear();
	s_ShadowMapPSList.Clear();

	/*
	SAFE_RELEASE(g_pAppliqueMaskDepthState);
	SAFE_RELEASE(g_pAppliqueDrawDepthState);
	SAFE_RELEASE(g_pAppliqueClearDepthState);
	SAFE_RELEASE(g_pZWriteEnableDepthState);
	SAFE_RELEASE(g_pGlowMaskDepthState);
	SAFE_RELEASE(g_pGlowDrawDepthState);
	SAFE_RELEASE(g_pCullNoneRasterState);
	SAFE_RELEASE(g_pCullFrontRasterState);
	SAFE_RELEASE(g_pNormalBlendState);
	SAFE_RELEASE(g_pBrightBlendState);
	SAFE_RELEASE(g_pEnhanceBlendState);
	SAFE_RELEASE(g_pDefaultBlendState);
	SAFE_RELEASE(g_pInverseBlendState);
	SAFE_RELEASE(g_pFactorBlendState);
	SAFE_RELEASE(g_pColorDisableBlendState);
	*/
}

/*
bool CModelPlayer::SelectRefractionShader(DxSceneBatch* pBatch, 
	model_node_t* pNode, node_material_t* pMat)
{
	material_info_t* pMatInfo = m_pCurMatInfo;
	//unsigned int si = m_pCurMatInfo->nShaderInfo;
	size_t vs_flags = pMat->nVertexDefine;
	bool texture_ani = (pMat->pFrameDiffuseTM != NULL);
	bool normal_map = (pMatInfo->BumpMap.pFileName != NULL)
		&& ((pMat->nVertexDefine & VERTEX_TANGENT_FLAG) != 0);

	vs_flags |= size_t(texture_ani) << REFRACTION_VS_TEXTUREANI;
	vs_flags |= size_t(normal_map) << REFRACTION_VS_NORMALMAP;

	size_t ps_flags = 0;
	bool use_diffuse = (pMat->nVertexDefine & VERTEX_COLOR_FLAG) != 0;
	bool diffuse_map = (pMatInfo->DiffuseMap.pFileName != NULL);

	ps_flags |= size_t(use_diffuse) << REFRACTION_PS_DIFFUSE;
	ps_flags |= size_t(diffuse_map) << REFRACTION_PS_DIFFUSEMAP;
	ps_flags |= size_t(normal_map) << REFRACTION_PS_NORMALMAP;
	ps_flags |= size_t(pMatInfo->bVolumeTexture) << REFRACTION_PS_VOLUMETEXTURE;
	ps_flags |= size_t(pMatInfo->bOpacityEnable) << REFRACTION_PS_OPACITY;

	IVertexShader* pVS = s_RefractionVSList.GetShader(vs_flags);
	IPixelShader* pPS = s_RefractionPSList.GetShader(ps_flags);

	pBatch->SetVertexShader(pVS);
	pBatch->SetPixelShader(pPS);

	return true;
}
*/

/*
bool CModelPlayer::SelectAppliqueShader(DxSceneBatch* pBatch, 
	model_node_t* pNode, node_material_t* pMat, bool shadow, bool prelight, 
	bool fog_linear, bool fog_exp, bool use_intz, bool multiply, bool stereo)
{
	material_info_t* pMatInfo = m_pCurMatInfo;
	//unsigned int si = m_pCurMatInfo->nShaderInfo;
	size_t vs_flags = pMat->nVertexDefine;

	vs_flags |= 0x1 << APPLIQUE_VS_FOGDEPTH;

	size_t ps_flags = 0;
	bool append_color = (!multiply) 
		&& (pMatInfo->bOpacityEnable || (m_nColor != 0xFFFFFFFF)); 

	ps_flags |= size_t(pMatInfo->bDisappear) << APPLIQUE_PS_DISAPPEAR;
	ps_flags |= size_t(pMatInfo->bOpacityEnable) << APPLIQUE_PS_OPACITY;
	ps_flags |= size_t(pMatInfo->bVolumeTexture) << APPLIQUE_PS_VOLUMETEXTURE;
	ps_flags |= size_t(append_color) << APPLIQUE_PS_APPENDCOLOR;
	ps_flags |= size_t(shadow) << APPLIQUE_PS_SHADOWMAP;
	ps_flags |= size_t(prelight) << APPLIQUE_PS_PRELIGHT;
	ps_flags |= size_t(fog_exp) << APPLIQUE_PS_FOGEXP;
	ps_flags |= size_t(fog_linear) << APPLIQUE_PS_FOGLINEAR;
	ps_flags |= size_t(use_intz) << APPLIQUE_PS_USEINTZ;
	ps_flags |= size_t(multiply) << APPLIQUE_PS_MULTIPLY;
	ps_flags |= size_t(stereo) << APPLIQUE_PS_STEREO;

	IVertexShader* pVS = s_AppliqueVSList.GetShader(vs_flags);
	IPixelShader* pPS = s_AppliquePSList.GetShader(ps_flags);

	pBatch->SetVertexShader(pVS);
	pBatch->SetPixelShader(pPS);

	return true;
}
*/

/*
bool CModelPlayer::SelectGlowShader(DxSceneBatch* pBatch, 
	model_node_t* pNode, node_material_t* pMat, bool fog_linear, 
	bool fog_exp, bool gamma)
{
	material_info_t* pMatInfo = m_pCurMatInfo;
	//unsigned int si = pMatInfo->nShaderInfo;
	size_t vs_flags = pMat->nVertexDefine;
	bool texture_ani = (pMat->pFrameDiffuseTM != NULL);

	vs_flags |= size_t(texture_ani) << GLOW_VS_TEXTUREANI;
	vs_flags |= size_t(fog_linear || fog_exp) << GLOW_VS_FOGDEPTH;
	vs_flags |= size_t(gamma) << GLOW_VS_GAMMA;

	size_t ps_flags = 0;

	if (pMatInfo->bGlowVertexColor)
	{
		bool use_diffuse = (pMat->nVertexDefine & VERTEX_COLOR_FLAG) != 0;

		ps_flags |= size_t(use_diffuse) << GLOW_PS_DIFFUSE;
	}

	if (pMatInfo->bAlphaTest || pMatInfo->bGlowDiffuse)
	{
		bool diffuse_map = (pMatInfo->DiffuseMap.pFileName != NULL);

		ps_flags |= size_t(diffuse_map) << GLOW_PS_DIFFUSEMAP;
		ps_flags |= 0x1 << GLOW_PS_DYNAMICLIGHTING;
	}

	if (pMatInfo->bGlowAppendColor)
	{
		ps_flags |= size_t(m_nColor != 0xFFFFFFFF) << GLOW_PS_APPENDCOLOR;
	}
	
	ps_flags |= size_t(pMatInfo->bGlowEmissive) << GLOW_PS_EMISSIVE;
	ps_flags |= size_t(pMatInfo->bGlowEmissiveMap) << GLOW_PS_EMISSIVEMAP;
	ps_flags |= size_t(fog_exp) << GLOW_PS_FOGEXP;
	ps_flags |= size_t(fog_linear) << GLOW_PS_FOGLINEAR;

	IVertexShader* pVS = s_GlowVSList.GetShader(vs_flags);
	IPixelShader* pPS = s_GlowPSList.GetShader(ps_flags);

	pBatch->SetVertexShader(pVS);
	pBatch->SetPixelShader(pPS);

	return true;
}
*/

/*
bool CModelPlayer::SelectVolumeFogDepthShader(DxSceneBatch* pBatch, 
	model_node_t* pNode, node_material_t* pMat, bool bCW)
{
	size_t vs_flags = pMat->nVertexDefine;
	size_t ps_flags = 0;

	ps_flags |= size_t(bCW) << VOLUMEDEPTH_PS_CW;
	
	IVertexShader* pVS = s_VolumeFogVSList.GetShader(vs_flags);
	IPixelShader* pPS = s_VolumeFogDepthPSList.GetShader(ps_flags);

	pBatch->SetVertexShader(pVS);
	pBatch->SetPixelShader(pPS);

	return true;
}
*/

/*
bool CModelPlayer::SelectVolumeFogColorShader(DxSceneBatch* pBatch, 
	model_node_t* pNode, node_material_t* pMat, bool fog_linear, bool fog_exp)
{
	material_info_t* pMatInfo = m_pCurMatInfo;
	//unsigned int si = m_pCurMatInfo->nShaderInfo;
	size_t vs_flags = pMat->nVertexDefine;

	vs_flags |= size_t(fog_exp || fog_linear) << VOLUMEFOG_VS_FOGDEPTH;

	size_t ps_flags = 0;

	ps_flags |= size_t(m_nColor != 0xFFFFFFFF) << VOLUMECOLOR_PS_APPENDCOLOR;
	ps_flags |= size_t(pMatInfo->bOpacityEnable) << VOLUMECOLOR_PS_OPACITY;
	ps_flags |= size_t(fog_exp) << VOLUMECOLOR_PS_FOGEXP;
	ps_flags |= size_t(fog_linear) << VOLUMECOLOR_PS_FOGLINEAR;

	IVertexShader* pVS = s_VolumeFogVSList.GetShader(vs_flags);
	IPixelShader* pPS = s_VolumeFogColorPSList.GetShader(ps_flags);

	pBatch->SetVertexShader(pVS);
	pBatch->SetPixelShader(pPS);

	return true;
}
*/

/*
bool CModelPlayer::SelectWaterSurfaceShader(DxSceneBatch* pBatch, 
	model_node_t* pNode, node_material_t* pMat, bool fog_linear, bool fog_exp, 
	bool depth_map, bool refraction, bool gamma)
{
	material_info_t* pMatInfo = m_pCurMatInfo;
	//unsigned int si = m_pCurMatInfo->nShaderInfo;
	size_t vs_flags = pMat->nVertexDefine;
	bool texture_ani = (pMat->pFrameDiffuseTM != NULL);
	bool normal_map = (pMatInfo->BumpMap.pFileName != NULL)
		&& ((pMat->nVertexDefine & VERTEX_TANGENT_FLAG) != 0);

	vs_flags |= size_t(texture_ani) << WATER_VS_TEXTUREANI;
	vs_flags |= size_t(normal_map) << WATER_VS_NORMALMAP;
	vs_flags |= 0x1 << WATER_VS_VIEWVECTOR;
	vs_flags |= size_t(gamma) << WATER_VS_GAMMA;

	size_t ps_flags = 0;
	bool use_diffuse = (pMat->nVertexDefine & VERTEX_COLOR_FLAG) != 0;
	bool diffuse_map = (pMatInfo->DiffuseMap.pFileName != NULL);
	bool emissive_map = (pMatInfo->EmissiveMap.pFileName != NULL);
	bool reflection = pMatInfo->bSpecularEnable && pMatInfo->bReflectEnable;

	ps_flags |= size_t(use_diffuse) << WATER_PS_DIFFUSE;
	ps_flags |= size_t(diffuse_map) << WATER_PS_DIFFUSEMAP;
	ps_flags |= size_t(normal_map) << WATER_PS_NORMALMAP;
	ps_flags |= size_t(pMatInfo->bSpecularEnable) << WATER_PS_SPECULAR;
	ps_flags |= size_t(pMatInfo->bSpecularEnable) << WATER_PS_SPECULARMAP;
	ps_flags |= size_t(pMatInfo->bEmissiveEnable) << WATER_PS_EMISSIVE;
	ps_flags |= size_t(emissive_map) << WATER_PS_EMISSIVEMAP;
	ps_flags |= size_t(reflection) << WATER_PS_REFLECTION;
	ps_flags |= size_t(!refraction) << WATER_PS_OPACITY;
	ps_flags |= size_t(depth_map) << WATER_PS_DEPTHMAP;
	ps_flags |= size_t(fog_exp) << WATER_PS_FOGEXP;
	ps_flags |= size_t(fog_linear) << WATER_PS_FOGLINEAR;

	IVertexShader* pVS = s_WaterVSList.GetShader(vs_flags);
	IPixelShader* pPS = s_WaterPSList.GetShader(ps_flags);

	pBatch->SetVertexShader(pVS);
	pBatch->SetPixelShader(pPS);

	return true;
}
*/

IShaderProgram* CModelPlayer::SelectShadowMapShader(const MatInfo* info, node_material_t* pMat, model_node_t* pNode)
{
	material_info_t* pMatInfo = info->pMatInfo;
	bool alpha_test = pMatInfo->bAlphaTest;

	size_t vs_flags = size_t(alpha_test) << SHADOWMAP_VS_ALPHATEST;
	size_t ps_flags = size_t(alpha_test) << SHADOWMAP_PS_ALPHATEST;

	IVertexShader* pVS = NULL;
	IPixelShader* pPS = s_ShadowMapPSList.GetShader(ps_flags);

	if (pNode->nType != FXNODE_TYPE_MESH)
	{
		vs_flags |= (0x1 << VERTEX_SKELETON_SHIFT);

		if (vs_flags != 0)
		{
			vs_flags |= (0x1 << VERTEX_TEXTURE0_SHIFT);

			pVS = s_ShadowMapVSList.GetShader(vs_flags);
		}
		else
		{
			pVS = s_ShadowMapVSList.GetShader(vs_flags);
		}
	}
	else
	{
		if (vs_flags != 0)
		{
			vs_flags |= (0x1 << VERTEX_TEXTURE0_SHIFT);

			pVS = s_ShadowMapVSList.GetShader(vs_flags);
		}
		else
		{
			pVS = s_ShadowMapVSList.GetShader(vs_flags);
		}
	}

	const char* ptr[ModelVF::MAX];
	int len;
	pMat->vf.GetNames(ptr, len);
	ShaderKey key;
	key.ps = 0;
	key.vs = 0;
	return ShaderManager::Inst().GetShader(vs_flags, ps_flags, pVS, pPS, ptr, len, key, const_value_name, c_max);
}

IShaderProgram* CModelPlayer::SelectShader(const MatInfo* info, 
	model_node_t* pNode, node_material_t* pMat, bool dynamic_lighting, 
	bool dynamic_shadow, bool fog_linear, bool fog_exp, bool height_fog, 
	bool prelight, bool reflect_enable, bool fix_shadow, bool sphere_ambient, 
	bool point_light, unsigned int model_alpha, bool use_instance,
	bool camera_light, bool is_blend, bool gamma, bool filter, bool use_clip_plane, 
	bool onlyzpass, bool onlycolor, bool keepout)
{
	
	const material_info_t* pMatInfo = info->pMatInfo;
	size_t vs_flags = pMat->nVertexDefine;
	bool texture_ani = (pMat->pFrameDiffuseTM != NULL);
	bool normal_map = (pMatInfo->BumpMap.pFileName != NULL)
		&& ((pMat->nVertexDefine & VERTEX_TANGENT_FLAG) != 0);

	bool blend_quality = pMatInfo->bBlendQuality && (model_alpha == 255);

	vs_flags |= size_t(texture_ani) << MODEL_VS_TEXTUREANI;
	vs_flags |= size_t(normal_map) << MODEL_VS_NORMALMAP;
	vs_flags |= size_t(pMatInfo->bDisappear) << MODEL_VS_DISAPPEAR;
	vs_flags |= 0x1 << MODEL_VS_VIEWVECTOR;
	vs_flags |= size_t(gamma) << MODEL_VS_GAMMA;
	vs_flags |= size_t(use_clip_plane) << MODEL_VS_CLIPPLANE;
	vs_flags |= size_t(filter) << MODEL_VS_FILTER;
	vs_flags |= size_t(keepout) << MODEL_VS_KEEPOUT;
	vs_flags |= size_t(dynamic_shadow) << MODEL_VS_SHADOWMAP;

	vs_flags |= size_t(fog_exp) << MODEL_VS_FOGEXP;
	vs_flags |= size_t(fog_linear) << MODEL_VS_FOGLINEAR;
	vs_flags |= size_t(height_fog) << MODEL_VS_HEIGHT_FOG;
	bool filter_texture_ani = info->pMat->nMaterialInfoEx & MATERIAL_FILTER_UV_ANI;
	vs_flags |= size_t(filter_texture_ani) << MODEL_VS_FILTERTEXTUREANI;

	size_t ps_flags = 0;
	bool diffuse_map = (pMatInfo->DiffuseMap.pFileName != NULL);
	bool specular_map = (pMatInfo->SpecularMap.pFileName != NULL)
		|| (pMatInfo->SpecularLevelMap.pFileName != NULL);
	bool emissive_map = (pMatInfo->EmissiveMap.pFileName != NULL);
	bool skeleton_ani = (pMat->nVertexDefine & VERTEX_SKELETON_FLAG) != 0;

	vs_flags |= size_t(diffuse_map) << MODEL_VS_DIFFUSEMAP;
	vs_flags |= size_t(specular_map) << MODEL_VS_SPECULARMAP;

	ps_flags |= size_t(use_clip_plane) << MODEL_PS_CLIPPLANE;
	ps_flags |= size_t(diffuse_map) << MODEL_PS_DIFFUSEMAP;
	ps_flags |= size_t(normal_map) << MODEL_PS_NORMALMAP;
	ps_flags |= size_t(pMatInfo->bSpecularEnable) << MODEL_PS_SPECULAR;
	ps_flags |= size_t(specular_map) << MODEL_PS_SPECULARMAP;
	ps_flags |= size_t(pMatInfo->bEmissiveEnable) << MODEL_PS_EMISSIVE;
	ps_flags |= size_t(emissive_map) << MODEL_PS_EMISSIVEMAP;
	ps_flags |= size_t(skeleton_ani) << MODEL_PS_SKELETON;
	ps_flags |= size_t(dynamic_lighting) << MODEL_PS_DYNAMICLIGHTING;
	ps_flags |= size_t(dynamic_shadow) << MODEL_PS_SHADOWMAP;
//	ps_flags |= size_t(prelight) << MODEL_PS_PRELIGHT;
	ps_flags |= size_t(fix_shadow) << MODEL_PS_FIXSHADOW;
	ps_flags |= size_t(reflect_enable) << MODEL_PS_REFLECTION;
	ps_flags |= size_t(sphere_ambient) << MODEL_PS_SPHEREAMBIENT;
	ps_flags |= size_t(pMatInfo->bRimLight) << MODEL_PS_FALLOFF;
	ps_flags |= size_t(fog_exp) << MODEL_PS_FOGEXP;
	ps_flags |= size_t(fog_linear) << MODEL_PS_FOGLINEAR;

	bool alpha_test = blend_quality || pMatInfo->bAlphaTest;
	ps_flags |= size_t(alpha_test) << MODEL_PS_ALPHATEST;
	ps_flags |= size_t(is_blend && blend_quality) << MODEL_PS_GREATERQUAL;
	
//	ps_flags |= size_t(pMatInfo->bAlphaTestGreaterQual) << MODEL_PS_GREATERQUAL;
	ps_flags |= size_t(pMatInfo->bSkinEffect) << MODEL_PS_SKINEFFECT;
	ps_flags |= size_t(pMatInfo->bDisappear) << MODEL_PS_DISAPPEAR;
	ps_flags |= size_t(pMatInfo->bOpacityEnable) << MODEL_PS_OPACITY;
//	ps_flags |= size_t(pMatInfo->bVolumeTexture) << MODEL_PS_VOLUMETEXTURE;
	//只要是半透明，都打开这个宏
	ps_flags |= size_t(is_blend) << MODEL_PS_OPACITY;
	
	ps_flags |= size_t(onlyzpass) << MODEL_PS_ONLYZPASS;

	ps_flags |= size_t(onlycolor) << MODEL_PS_ONLYCOLOR;

	ps_flags |= size_t(point_light) << MODEL_PS_POINTLIGHT;

	ps_flags |= size_t(filter) << MODEL_PS_FILTER;

	ps_flags |= size_t(keepout) << MODEL_PS_KEEPOUT;

	ps_flags |= size_t(height_fog) << MODEL_PS_HEIGHT_FOG;

	if (pMatInfo->bOpacityEnable || (m_nColor != 0xFFFFFFFF))
	{
		ps_flags |= 0x1 << MODEL_PS_APPENDCOLOR;
	}

	// 在淡入淡出时不使用模型的顶点色
	
	if (model_alpha == 255)
	{
		bool use_diffuse = (pMat->nVertexDefine & VERTEX_COLOR_FLAG) != 0;

		ps_flags |= size_t(use_diffuse) << MODEL_PS_DIFFUSE;
	}

	if (!dynamic_lighting)
	{
		bool light_map = true;//(pMatInfo->LightMap.pFileName != NULL);
		
		ps_flags |= size_t(light_map) << MODEL_PS_LIGHTMAP;
	}

	ps_flags |= size_t(pMatInfo->bBlendEnhance) << MODEL_PS_BLEND_ENHANCE;

//	ps_flags |= size_t(pMatInfo->bNoLight) << MODEL_PS_NOLIGHT;
	
	/*
	dx9引擎并没有这个代码。暂时注掉
	if (is_blend)
	{
		// 剪裁掉实体部分
		ps_flags |= size_t(pMatInfo->bBlendQuality) << MODEL_PS_CLIPSOLID;
	}
	else
	{
		// 剪裁掉半透明部分
		ps_flags |= size_t(pMatInfo->bBlendQuality) << MODEL_PS_CLIPBLEND;
	}
	*/
	
	IVertexShader* pVS = s_VSList.GetShader(vs_flags);
	IPixelShader* pPS = s_PSList.GetShader(ps_flags);

	return ShaderManager::Inst().GetShader(vs_flags,ps_flags,pVS, pPS, pMat, pNode, const_value_name, c_max);
}


/*
bool CModelPlayer::SelectGBufferShader(DxSceneBatch* pBatch, 
	model_node_t* pNode, node_material_t* pMat, bool gamma, bool use_tesse)
{
	material_info_t* pMatInfo = m_pCurMatInfo;
	//unsigned int si = pMatInfo->nShaderInfo;
	//size_t vs_flags = pMat->nVertexDefine;

	//vs_flags |= size_t((si & TEXTUREANI_FLAG) != 0) << GBUFFER_VS_TEXTUREANI;
	//vs_flags |= size_t((si & NORMALMAP_FLAG) != 0) << GBUFFER_VS_NORMALMAP;
	//vs_flags |= size_t(pMatInfo->bRimLight) << GBUFFER_VS_FALLOFF;
	//vs_flags |= size_t(gamma) << GBUFFER_VS_GAMMA;

	size_t ps_flags = 0;
	bool use_diffuse = (pMat->nVertexDefine & VERTEX_COLOR_FLAG) != 0;
	bool diffuse_map = (pMatInfo->DiffuseMap.pFileName != NULL);
	bool normal_map = (pMatInfo->BumpMap.pFileName != NULL)
		&& ((pMat->nVertexDefine & VERTEX_TANGENT_FLAG) != 0);
	bool specular_map = (pMatInfo->SpecularMap.pFileName != NULL)
		|| (pMatInfo->SpecularLevelMap.pFileName != NULL);

	ps_flags |= size_t(use_diffuse) << GBUFFER_PS_DIFFUSE;
	ps_flags |= size_t(diffuse_map) << GBUFFER_PS_DIFFUSEMAP;
	ps_flags |= size_t(normal_map) << GBUFFER_PS_NORMALMAP;
	ps_flags |= size_t(specular_map) << GBUFFER_PS_SPECULARMAP;
	ps_flags |= size_t(m_nColor != 0xFFFFFFFF) << GBUFFER_PS_APPENDCOLOR;
	ps_flags |= size_t(pMatInfo->bVolumeTexture) << GBUFFER_PS_VOLUMETEXTURE;
	ps_flags |= size_t(pMatInfo->bRimLight) << GBUFFER_PS_FALLOFF;
	ps_flags |= size_t(pMatInfo->bAlphaTest) << GBUFFER_PS_ALPHATEST;
	ps_flags |= size_t(pMatInfo->bBlendQuality) << GBUFFER_PS_CLIPBLEND;

	//IVertexShader* pVS = s_GBufferVSList.GetShader(vs_flags);
	IPixelShader* pPS = s_GBufferPSList.GetShader(ps_flags);

	//pBatch->SetVertexShader(pVS);
	pBatch->SetPixelShader(pPS);

	if (use_tesse)
	{
		size_t vs_flags = pMat->nVertexDefine;
		bool texture_ani = (pMat->pFrameDiffuseTM != NULL);
		
		vs_flags |= size_t(texture_ani) << GBUFFER_TESS_VS_TEXTUREANI;
		vs_flags |= size_t(normal_map) << GBUFFER_TESS_VS_NORMALMAP;
		vs_flags |= size_t(gamma) << GBUFFER_TESS_VS_GAMMA;

		bool use_texture0 = (pMat->nVertexDefine & VERTEX_TEXTURE0_FLAG) != 0;
		size_t hs_flags = 0;

		hs_flags |= size_t(normal_map) << GBUFFER_HS_NORMALMAP;
		hs_flags |= size_t(use_diffuse) << GBUFFER_HS_VERTEX_COLOR;
		hs_flags |= size_t(use_texture0) << GBUFFER_HS_VERTEX_TEXTURE0;

		size_t ds_flags = 0;
		
		ds_flags |= size_t(normal_map) << GBUFFER_DS_NORMALMAP;
		ds_flags |= size_t(use_diffuse) << GBUFFER_DS_VERTEX_COLOR;
		ds_flags |= size_t(use_texture0) << GBUFFER_DS_VERTEX_TEXTURE0;
		ds_flags |= size_t(pMatInfo->bRimLight) << GBUFFER_DS_FALLOFF;
		
		IVertexShader* pVS = s_GBufferTessVSList.GetShader(vs_flags);
		IDxHullShader* pHS = s_GBufferHSList.GetShader(hs_flags);
		IDxDomainShader* pDS = s_GBufferDSList.GetShader(ds_flags);

		pBatch->SetVertexShader(pVS);
		pBatch->SetHullShader(pHS);
		pBatch->SetDomainShader(pDS);
	}
	else
	{
		size_t vs_flags = pMat->nVertexDefine;
		bool texture_ani = (pMat->pFrameDiffuseTM != NULL);

		vs_flags |= size_t(texture_ani) << GBUFFER_VS_TEXTUREANI;
		vs_flags |= size_t(normal_map) << GBUFFER_VS_NORMALMAP;
		vs_flags |= size_t(pMatInfo->bRimLight) << GBUFFER_VS_FALLOFF;
		vs_flags |= size_t(gamma) << GBUFFER_VS_GAMMA;

		IVertexShader* pVS = s_GBufferVSList.GetShader(vs_flags);

		pBatch->SetVertexShader(pVS);
	}

	return true;
}
*/

//bool CModelPlayer::SelectDepthMapShader(DxSceneBatch* pBatch, 
//	model_node_t* pNode, node_material_t* pMat, bool use_normal, 
//	bool use_normalmap, bool use_instance, bool only_normal)
//{
//	size_t flags = 0;
//	material_info_t* pMatInfo = m_pCurMatInfo;
//	unsigned int si = pMatInfo->nShaderInfo;
//
//	flags |= size_t((si & SKELETONANI_FLAG) != 0) << 0;
//	flags |= size_t(use_normal) << 1;
//	flags |= size_t(pMatInfo->bAlphaTest || pMatInfo->bBlendQuality) << 2;
//	flags |= size_t((si & VERTEXANI_FLAG) != 0) << 3;
//	flags |= size_t((si & NORMALMAP_FLAG) != 0) << 4;
//	flags |= size_t(only_normal) << 6;
//
//	IVertexShader* pVS = s_DepthMapVSList.GetShaderWithPrefix(flags,
//		pMat->pszVertexDefine);
//	IPixelShader* pPS = s_DepthMapPSList.GetShaderWithPrefix(flags,
//		pMat->pszVertexDefine);
//
//	pBatch->SetVertexShader(pVS);
//	pBatch->SetPixelShader(pPS);
//
//	return true;
//}

//bool CModelPlayer::SelectVelocityShader(DxSceneBatch* pBatch, 
//	model_node_t* pNode, node_material_t* pMat)
//{
//	size_t shader_flag = 0;
//
//	shader_flag |= size_t(pNode->nType == FXNODE_TYPE_SKIN) << 0;
//	shader_flag |= size_t(m_pCurMatInfo->bAlphaTest) << 1;
//	shader_flag |= size_t(m_nWriteVelocity == 2) << 2;
//	shader_flag |= size_t(pMat->pMultiGPUVB != NULL) << 3;
//
//	IVertexShader* pVS = s_VelocityVSList.GetShader(shader_flag);
//	IPixelShader* pPS = s_VelocityPSList.GetShader(shader_flag);
//
//	pBatch->SetVertexShader(pVS);
//	pBatch->SetPixelShader(pPS);
//
//	return true;
//}


/*
bool CModelPlayer::SelectDepthShader(DxSceneBatch* pBatch, 
	model_node_t* pNode, node_material_t* pMat)
{
	size_t vs_flags = pMat->nVertexDefine;
	bool alpha_test = m_pCurMatInfo->bAlphaTest;

	vs_flags |= size_t(alpha_test) << DEPTH_VS_ALPHATEST;

	size_t ps_flags = 0;

	ps_flags |= size_t(alpha_test) << DEPTH_PS_ALPHATEST;

	IVertexShader* pVS = s_DepthVSList.GetShader(vs_flags);
	IPixelShader* pPS = s_DepthPSList.GetShader(ps_flags);

	pBatch->SetVertexShader(pVS);
	pBatch->SetPixelShader(pPS);

	return true;
}
*/

/*
bool CModelPlayer::SelectShadowMapShader(DxSceneBatch* pBatch, 
	model_node_t* pNode, node_material_t* pMat, bool use_instance, 
	bool use_tesse)
{
	//size_t vs_flags = pMat->nVertexDefine;
	material_info_t* pMatInfo = m_pCurMatInfo;
	bool alpha_test = pMatInfo->bAlphaTest
		&& ((pMat->nVertexDefine & VERTEX_TEXTURE0_FLAG) != 0);
	//vs_flags |= size_t(alpha_test) << SHADOWMAP_VS_ALPHATEST;
	size_t ps_flags = 0;

	ps_flags |= size_t(alpha_test) << SHADOWMAP_PS_ALPHATEST;
	
	//IVertexShader* pVS = s_ShadowMapVSList.GetShader(vs_flags);
	IPixelShader* pPS = s_ShadowMapPSList.GetShader(ps_flags);

	//pBatch->SetVertexShader(pVS);
	pBatch->SetPixelShader(pPS);

	if (use_tesse)
	{
		size_t vs_flags = pMat->nVertexDefine;
		
		vs_flags |= size_t(alpha_test) << SHADOWMAP_TESS_VS_ALPHATEST;

		size_t hs_flags = 0;

		hs_flags |= size_t(alpha_test) << SHADOWMAP_HS_ALPHATEST;

		size_t ds_flags = 0;
		
		ds_flags |= size_t(alpha_test) << SHADOWMAP_DS_ALPHATEST;

		IVertexShader* pVS = s_ShadowMapTessVSList.GetShader(vs_flags);
		IDxHullShader* pHS = s_ShadowMapHSList.GetShader(hs_flags);
		IDxDomainShader* pDS = s_ShadowMapDSList.GetShader(ds_flags);

		pBatch->SetVertexShader(pVS);
		pBatch->SetHullShader(pHS);
		pBatch->SetDomainShader(pDS);
	}
	else
	{
		size_t vs_flags = pMat->nVertexDefine;

		vs_flags |= size_t(alpha_test) << SHADOWMAP_VS_ALPHATEST;

		IVertexShader* pVS = s_ShadowMapVSList.GetShader(vs_flags);

		pBatch->SetVertexShader(pVS);
	}

	return true;
}
*/

IShaderProgram* CModelPlayer::SelectShader(const MatInfo* info, node_material_t* pMat, model_node_t* pNode)
{
	// 实时阴影
	bool dynamic_shadow = false;
	
	// 环境反射是否开启
	bool reflect_enable = false;

	material_info_t* pMatInfo = m_pCurMatInfo;


	// 半球环境光
	bool sphere_ambient = pMatInfo->bSphereAmbient;

	IRenderContext* pContext = g_pRender->GetContext();

	

	// 是否开启静态光照下的人物点光源照明
	bool point_light = (!dynamic_shadow)
		&& (pContext->GetInt(IRenderContext::I_ENABLE_ROLE_LIGHT) != 0)
		&& (pNode->nType == FXNODE_TYPE_SKIN);

	// 指数雾效
	bool fog_exp = pContext->GetInt(IRenderContext::I_FOG_EXP) != 0;

	// 线性雾效
	bool fog_linear = pContext->GetInt(IRenderContext::I_FOG_LINEAR) != 0;

	// 高度雾效
	bool height_fog = pContext->GetInt(IRenderContext::I_HEIGHT_FOG) != 0;

	bool camera_light = (pNode->nType == FXNODE_TYPE_SKIN) 
		&& (pContext->GetInt(IRenderContext::I_ENABLE_CAMERA_LIGHT) != 0);
	//bool gamma = pContext->GetEnableGammaCorrect();
	bool gamma = false;
	bool use_clip_plane = pContext->GetInt(IRenderContext::I_USE_CLIP_PLANE) != 0;

	bool prelight = false;
	bool fix_shadow = false;
	bool filter = info->pMat->nMaterialInfoEx && MATERIAL_FILTER_MAP_INFO && info->pMatInfo->bFilterEnable;
	IShaderProgram* pShader = SelectShader(info, pNode, pMat, true, dynamic_shadow, fog_linear, 
		fog_exp, height_fog, prelight, reflect_enable, fix_shadow, sphere_ambient, 
		point_light, 255, false, camera_light, false, gamma, filter, use_clip_plane);
	
	return pShader;
}

const char* const_value_name[c_max] = 
{
	"c_mtxWorld",
	"c_mtxWVP",
	"c_fBumpScale",
	"c_fReflectScale",
	"c_fSaturationScale",
	"c_fGlowAlpha",
	"c_fBlendWeight",
	"c_fAlphaRefValue",
	"c_fTime",
	"c_fGlowSize",
	"c_BoneList",
	"c_mtxDiffuseTM",
	"c_MaterialEmissive",
	"c_MaterialSpecular",
	"c_fSpecularPower",
	"c_MaterialDiffuse",
	"c_AppendColor",
	"c_MaterialAmbient",
	"c_MaterialAmbientEX",
	"c_vLightDir",
	"c_mtxViewInverse",
	"c_LightAmbient",
	"c_vViewPos",
	"c_vReferPos",
	"c_ClipPlane",
	"c_LightDiffuse",
	"c_FogColor",
	"c_FogParam",
	"c_FogExpParam",
	"c_FallOffParam",
	"c_SubSurfaceParam",
	"c_LightMapParam",
	"c_vLightMapColorScale",
	"c_PointLightDiffuse",
	"c_PointLightPosition",
	"c_FilterParam",
	"c_mtxFilterTM",
	"c_mtxShadowViewProj",
	"c_HeightFogColor",
	"c_HeightFogParam",
	"tex_Diffuse",
	"tex_Bump",
	"tex_SpecularMap",
	"tex_EmissiveMap",
	"tex_ReflectionMap",
	"tex_LightMap",
	"tex_FilterMap",
	"tex_KeepoutMap",
	"tex_Shadow",
};
