#include "game_scene.h"
#include "helper.h"
#include "../visual/i_sea_water.h"
#include "../visual/i_weather.h"
#include "../visual/i_sky_box.h"
#include "../visual/i_sun_glow.h"
#include "../visual/i_context.h"

#include "global.h"


DECLARE_ENTITY(Game_Scene, 0, IEntity);



Game_Scene::Game_Scene()
{
	m_pRaleNPC = NULL;
}

Game_Scene::~Game_Scene()
{
}

bool Game_Scene::Init(const IVarList& args)
{
	m_InputManager = (IInputManager*)g_core->GetInterface("CInputManager");
 	m_InputManager->RegisterInputEventCallBack(this);

	m_pRender = (IRender*)g_core->GetInterface("Render");
	return true;
}

bool Game_Scene::Shut()
{
	return true;
}

void Game_Scene::Execute(float seconds)
{
}

bool Game_Scene::Load(const char* pstrfile,IScene* pScene,IWorld* pWorld, bool bLoadSkybox)
{
	m_pWorld = pWorld;
	return load_terrain(pstrfile,pScene);
}

// 新增场景卸载
void Game_Scene::UnLoad()
{
	for(size_t i=0; i<m_vecSceneObjEnt.size(); i++)
	{
		g_scene->Delete(m_vecSceneObjEnt[i]);
	}
	m_vecSceneObjEnt.clear();

	if(g_terrain)
	{
		g_terrain = NULL;
	}
}

void Game_Scene::MouseCallBack(IMouseEvent* Event)
{
	if(Event->GetMouseEventType() == Mouse_M_Down )
	{
	    // 	load_npc();
		/*
		for(int i = 0 ; i < m_pRender->GetShaderInfoNum(); i++ )
		{
			IResource* pRes = m_pRender->GetShaderInfo(i);
			if(pRes->GetResourceType() == IResource::TYPE_VERTEX_SHADER)
			{
				IVertexShader* pVS = (IVertexShader*)pRes;
				CORE_TRACE(pVS->GetFileName());
				CORE_TRACE(pVS->GetFuncName());
				CORE_TRACE(pVS->GetDefine());
			}
			else if(pRes->GetResourceType() == IResource::TYPE_PIXEL_SHADER)
			{
				IPixelShader* pPS = (IPixelShader*)pRes;
				CORE_TRACE(pPS->GetFileName());
				CORE_TRACE(pPS->GetFuncName());
				CORE_TRACE(pPS->GetDefine());
			}

		}
		*/
	}
}

bool Game_Scene::load_terrain(const char* pstrfile,IScene* pScene)
{	 
	char mbzTemp[512];
	SafeSprintf(mbzTemp,sizeof(mbzTemp),"%s%s",pstrfile,"terrain.ini");
	//先创建一个场景
	IIniFile* pIni = g_core->CreateIniFile(pstrfile);
	pIni->SetFileName(mbzTemp);
	bool b = pIni->LoadFromFile();
	if(!b)
	{
		pIni->Release();
		return false;
	}

	//
	int zone_scale = pIni->ReadInteger("system", "ZoneScale", 256);
	int chunk_scale = pIni->ReadInteger("system", "ChunkScale", 32);
	int texture_scale = pIni->ReadInteger("system", "TextureScale", 256);
	int texture_units = pIni->ReadInteger("system", "TextureUnits", 4);
	int alpha_per_unit = pIni->ReadInteger("system", "AlphaPerUnit", 2);
	int light_per_unit = pIni->ReadInteger("system", "LightPerUnit", 2);
	int collide_per_unit = pIni->ReadInteger("system", "CollidePerUnit", 2);
	int zone_rows = pIni->ReadInteger("system", "ZoneRows", 100);
	int zone_cols = pIni->ReadInteger("system", "ZoneCols", 100);
	int origin_row = pIni->ReadInteger("system", "OriginRow", 50);
	int origin_col = pIni->ReadInteger("system", "OriginCol", 50);
	int zone_range = pIni->ReadInteger("system", "ZoneRange", 2);
	float unit_size = pIni->ReadFloat("system", "UnitSize", 1.0);

	const char* height_map = pIni->ReadString("system", "HeightMap", "");
	float init_height = pIni->ReadFloat("system", "InitHeight", 0.0);
	int light_tex_scale = pIni->ReadInteger("system", "LightTexScale", 256);
	const char* zone_light_path = pIni->ReadString("system", "ZoneLightPath", "lzone");
	const char* model_light_path = pIni->ReadString("system", "ModelLightPath", "lmodel");
	const char* walkable_path = pIni->ReadString("system", "WalkablePath", "walk");
	const char* no_light = pIni->ReadString("system", "NoLight", "false");
	const char* normalmap = pIni->ReadString("system", "NormalMap", "false");

	bool bno_light = strcmp(no_light,"true") ==0 ? true:false;
	bool bnormalmap =  strcmp(normalmap,"true") ==0 ? true:false;

	//创建地形
	PERSISTID terrain_id = pScene->Create("Terrain");
	ITerrain* pTerrain = (ITerrain*) g_core->GetEntity(terrain_id);
	CHelper::SetPropertyString(pTerrain,"AppendPath","");
	pTerrain->SetParameter(zone_scale, chunk_scale, texture_scale,texture_units, alpha_per_unit, collide_per_unit);
	CHelper::SetPropertyInt(pTerrain,"LightPerUnit",light_per_unit);
	CHelper::SetPropertyString(pTerrain,"FilePath",pstrfile);

	CHelper::SetPropertyInt(pTerrain,"DesignRadius",120);
	CHelper::SetPropertyInt(pTerrain,"CollideRadius",50);

	//单位长度
	CHelper::SetPropertyString(pTerrain,"HeightMap",height_map);
	CHelper::SetPropertyFloat(pTerrain,"InitHeight",init_height);
	CHelper::SetPropertyInt(pTerrain,"LightTexScale",light_tex_scale);
	//区域光照文件路径
	CHelper::SetPropertyString(pTerrain,"ZoneLightPath",zone_light_path);
	//模型光照文件路径
	CHelper::SetPropertyString(pTerrain,"ModelLightPath",model_light_path);
	//可行走文件路径
	CHelper::SetPropertyString(pTerrain,"WalkablePath",walkable_path);
	//是否不加载光照
	CHelper::SetPropertyBool(pTerrain,"NoLight",bno_light);
	//是否开启法线贴图
	CHelper::SetPropertyBool(pTerrain,"EnableNormalMap",bnormalmap);
	//碰撞对象尺寸
	CHelper::SetPropertyInt(pTerrain,"CollideScale",8);
	pTerrain->ClearBaseTex();

	CVarList baselist;
	pIni->GetItemList("base",baselist);


	for(size_t i=0;i<baselist.GetCount();i++)
	{
		const char* base_name = baselist.StringVal(i);
		pTerrain->AddBaseTex(base_name,pIni->ReadString("base",base_name,""));

		//读取光泽度帖图
		float power = pIni->ReadFloat("specular_power", base_name, 0.0);
		int base_index = pTerrain->GetBaseTexIndex(base_name);
		pTerrain->SetBaseTexSpecularPower(base_index,power);
	}

	pTerrain->ClearBlendTex();
	CVarList blendlist;

	pIni->GetItemList("blend",blendlist);

	for(size_t i=0;i<blendlist.GetCount();i++ )
	{
		const char* blend_name = blendlist.StringVal(i);
		pTerrain->AddBlendTex(blend_name,pIni->ReadString("blend", blend_name, ""));
		float power = pIni->ReadFloat("specular_power", blend_name, 0.0);
		int blend_index = pTerrain->GetBlendTexIndex(blend_name);
		pTerrain->SetBlendTexSpecularPower(blend_index,power);
	}

	//地面物体贴图路径
	pTerrain->ClearTexturePath();
	CVarList tex_pathlist;
	pIni->GetItemList("tex_path",tex_pathlist);
	for(size_t i=0;i<tex_pathlist.GetCount();i++)
	{
		const char* key = tex_pathlist.StringVal(i);
		pTerrain->AddTexturePath( pIni->ReadString("tex_path",key,"") );
	}

	//加载地表材质相关
	SafeSprintf(mbzTemp,sizeof(mbzTemp),"%s%s",pstrfile,"material.ini");
	load_material(mbzTemp,pTerrain,pScene);


	//加载地表样式
	SafeSprintf(mbzTemp,sizeof(mbzTemp),"%s%s",pstrfile,"base_format.ini");
	load_base_format(mbzTemp,pTerrain,pScene);

	//加载地表样式
	SafeSprintf(mbzTemp,sizeof(mbzTemp),"%s%s",pstrfile,"format.ini");
	load_format(mbzTemp,pTerrain,pScene);

	//加载水面相关
	SafeSprintf(mbzTemp,sizeof(mbzTemp),"%s%s",pstrfile,"water.ini");
	load_water(mbzTemp,pTerrain,pScene);
	//加载植被草相关
	SafeSprintf(mbzTemp,sizeof(mbzTemp),"%s%s",pstrfile,"grass.ini");
	load_grass(mbzTemp,pTerrain,pScene);
	//加载天气
	SafeSprintf(mbzTemp,sizeof(mbzTemp),"%s%s",pstrfile,"weather.ini");
	load_weather(mbzTemp,pTerrain,pScene);
	pTerrain->Load();
    //加载动态天气系统
	SafeSprintf(mbzTemp,sizeof(mbzTemp),"%s%s",pstrfile,"dynamic_weather.ini");
	load_dynamic_weather_system(mbzTemp,pTerrain,pScene);

	pScene->AddObject( pTerrain->GetID() , 30 );

	//全局环境帖图
	CHelper::SetPropertyString(pTerrain,"GlobalEnvironmentMap","tex\\environment_reflect.dds");

	//初始化地形
	pTerrain->InitZones(zone_rows, zone_cols, origin_row, origin_col, zone_range);

	pIni->Release();

#ifndef FX_SYSTEM_WINDOWS
	pTerrain->SetWaterReflect(false);
#endif
	pTerrain->SetWaterReflect(true);
	pTerrain->SetWaterSunReflect(true);
	load_particle_system(pstrfile,pTerrain,pScene);

	load_post_effect(pstrfile,pTerrain,pScene);

	//  编辑配置全屏设置逻辑
	load_screen_config(pstrfile,pTerrain,pScene);

	m_pTerrain = pTerrain;
	m_pScene = pScene;


	//pTerrain->SetGroundEnvMap(2);
	pTerrain->SetVisualEnvMap(1);
	//	m_pScene->SetEnableEarlyZ(true);

	//	pTerrain->SetWaterBorder(true);
	//	pTerrain->SetWaterDepth(true);

   // pTerrain->SetGroundVisible(false);

	// m_pTerrain->SetCastShadow(true);
	// m_pScene->SetEnableDynamicShadow(true);

	//加载阴影管理
	load_shadow_system(pTerrain,pScene);
	return true;
}

void Game_Scene::load_base_format(const char* pstrfile,ITerrain* pTerrain,IScene* pScene)
{
	IIniFile* pIni = g_core->CreateIniFile(pstrfile);
	pIni->SetFileName(pstrfile);
	bool b = pIni->LoadFromFile();
	if(!b)
	{
		pIni->Release();
		return;
	}
	size_t SectionNum = pIni->GetSectionCount();
	for( size_t i = 0; i < SectionNum; i++ )
	{
		const char* ptex_name = pIni->ReadString(i,"BaseTex","");
		if(pTerrain->FindBaseTex(ptex_name))
		{
			int nformat = pIni->ReadInteger(i,"Format",0);
			int nscale_u = pIni->ReadInteger(i,"ScaleU",100); 
			int nscale_v = pIni->ReadInteger(i,"ScaleV",100); 
			int nangle_x = pIni->ReadInteger(i,"AngleX",0); 
			int nangle_y = pIni->ReadInteger(i,"AngleY",0); 
			int nangle_z = pIni->ReadInteger(i,"AngleZ",0); 
			pTerrain->SetBaseFormatScaleU(ptex_name,0,nscale_u);
			pTerrain->SetBaseFormatScaleV(ptex_name,0,nscale_v);
			pTerrain->SetBaseFormatAngleX(ptex_name,0,nangle_x);
			pTerrain->SetBaseFormatAngleY(ptex_name,0,nangle_y);
			pTerrain->SetBaseFormatAngleZ(ptex_name,0,nangle_z);
		}
	}

	pIni->Release();
}

//加载地表样式数据
void Game_Scene::load_format(const char* pstrfile,ITerrain* pTerrain,IScene* pScene)
{
	IIniFile* pIni = g_core->CreateIniFile(pstrfile);
	pIni->SetFileName(pstrfile);
	bool b = pIni->LoadFromFile();
	if(!b)
	{
		pIni->Release();
		return;
	}
	size_t SectionNum = pIni->GetSectionCount();
	for( size_t i = 0; i < SectionNum; i++ )
	{
		const char* ptex_name = pIni->ReadString(i,"BlendTex","");
		if(pTerrain->FindBlendTex(ptex_name))
		{
			int nformat = pIni->ReadInteger(i,"Format",0);
			int nscale_u = pIni->ReadInteger(i,"ScaleU",100); 
			int nscale_v = pIni->ReadInteger(i,"ScaleV",100); 
			int nangle_x = pIni->ReadInteger(i,"AngleX",0); 
			int nangle_y = pIni->ReadInteger(i,"AngleY",0); 
			int nangle_z = pIni->ReadInteger(i,"AngleZ",0); 

			if( nformat >= pTerrain->GetBlendFormatCount(ptex_name))
			{
				pTerrain->AddBlendFormat(ptex_name,nscale_u,nscale_v,nangle_x,nangle_y,nangle_z);
			}
			else
			{
				pTerrain->SetBlendFormatScaleU(ptex_name,nformat,nscale_u);
				pTerrain->SetBlendFormatScaleV(ptex_name,nformat,nscale_v);
				pTerrain->SetBlendFormatAngleX(ptex_name,nformat,nangle_x);
				pTerrain->SetBlendFormatAngleY(ptex_name,nformat,nangle_y);
				pTerrain->SetBlendFormatAngleZ(ptex_name,nformat,nangle_z);
			}
		}
	}

	pIni->Release();

	return ;
}

//加载材质参数
void Game_Scene::load_material(const char* pstrfile,ITerrain* pTerrain,IScene* pScene)
{
	CORE_TRACE("Game_Scene::load_material...");
	IIniFile* pIni = g_core->CreateIniFile(pstrfile);
	pIni->SetFileName(pstrfile);
	bool b = pIni->LoadFromFile();
	if(!b)
	{
		pIni->Release();
		return;
	}

	//获取地形的材质数据
	PERSISTID material_id = pTerrain->GetMaterialID();
	IEntity* pEntity = (IEntity*) g_core->GetEntity(material_id);

#define SetMaterialFloat(name,ininame)  CHelper::SetPropertyFloat(pEntity,name,pIni->ReadFloat("material",ininame,CHelper::QueryPropertyFloat(pEntity,name)))
#define SetMaterialBool(name,ininame) CHelper::SetPropertyBool(pEntity,name, strcmp( (pIni->ReadString("material",ininame,"true")),"true")== 0 ?true:false)
	SetMaterialFloat("DiffuseAlpha","DiffuseAlpha");
	SetMaterialFloat("DiffuseRed","DiffuseRed");
	SetMaterialFloat("DiffuseGreen","DiffuseGreen");
	SetMaterialFloat("DiffuseBlue","DiffuseBlue");

	SetMaterialFloat("AmbientAlpha","AmbientAlpha");
	SetMaterialFloat("AmbientRed","AmbientRed");
	SetMaterialFloat("AmbientGreen","AmbientGreen");
	SetMaterialFloat("AmbientBlue","AmbientBlue");

	SetMaterialFloat("SpecularAlpha","SpecularAlpha");
	SetMaterialFloat("SpecularRed","SpecularRed");
	SetMaterialFloat("SpecularGreen","SpecularGreen");
	SetMaterialFloat("SpecularBlue","SpecularBlue");

	SetMaterialFloat("EmissiveAlpha","EmissiveAlpha");
	SetMaterialFloat("EmissiveRed","EmissiveRed");
	SetMaterialFloat("EmissiveGreen","EmissiveGreen");
	SetMaterialFloat("EmissiveBlue","EmissiveBlue");

	SetMaterialFloat("SpecularPower","SpecularPower");
	SetMaterialBool("SpecularEnable","SpecularEnable");
#undef SetMaterialFloat
	pIni->Release();
}

//加载粒子系统
void Game_Scene::load_particle_system(const char* pstrfile,ITerrain* pTerrain,IScene* pScene)
{
	CORE_TRACE("Game_Scene::load_particle_system...");
	PERSISTID particle_man_id = pScene->Create("ParticleManager");
	IEntity* pParticleMan = (IEntity*)g_core->GetEntity(particle_man_id);
	CHelper::SetPropertyString(pParticleMan,"TexPaths","tex\\particles\\");
	bool b = pScene->AddObject( particle_man_id ,100 );
	pScene->GetContext()->SetUnique(pParticleMan);
}


bool Game_Scene::load_water(const char* pstrfile,ITerrain* pTerrain,IScene* pScene)
{	 
	CORE_TRACE("Game_Scene::load_water...");
	IIniFile* pIni = g_core->CreateIniFile(pstrfile);
	pIni->SetFileName(pstrfile);
	bool b = pIni->LoadFromFile();
	if(!b)
	{
		pIni->Release();
		return false;
	}

	pTerrain->ClearWater();
	CVarList baselist;
	size_t SectionNum = pIni->GetSectionCount();
	for(size_t i=0;i<SectionNum;i++)
	{
		const char* pwater_name = pIni->GetSectionByIndex(i);
		PERSISTID water_id = pTerrain->CreateWater(pwater_name);
		if(!water_id.IsNull())
		{
			read_water_data(pIni,water_id);
			updata_seadata(water_id);
		}
	}

	pIni->Release();
	return true;
}

//加载草
bool Game_Scene::load_grass(const char* pstrfile,ITerrain* pTerrain,IScene* pScene)
{
	CORE_TRACE("Game_Scene::load_grass...");
	IIniFile* pIni = g_core->CreateIniFile(pstrfile);
	pIni->SetFileName(pstrfile);
	bool b = pIni->LoadFromFile();
	if(!b)
	{
		pIni->Release();
		return false;
	}

	pTerrain->ClearGrass();
	CVarList baselist;
	size_t SectionNum = pIni->GetSectionCount();
	for(size_t i=0;i<SectionNum;i++)
	{
		const char* pgrass_name = pIni->GetSectionByIndex(i);
		PERSISTID grass_id = pTerrain->CreateGrass(pgrass_name);
		if(!grass_id.IsNull())
		{
			read_grass_data(pIni,grass_id);
		}
	}
	pIni->Release();
	return true;
}

void Game_Scene::load_weather(const char* pstrfile,ITerrain* pTerrain,IScene* pScene)
{
	CORE_TRACE("Game_Scene::load_weather...");
	IIniFile* pIni = g_core->CreateIniFile(pstrfile);
	pIni->SetFileName(pstrfile);
	bool b = pIni->LoadFromFile();
	if(!b)
	{
		pIni->Release();
		return ;
	}

	load_skybox(pIni,pTerrain,pScene);
	load_sun(pIni,pTerrain,pScene);

	//开启雾效果
	PERSISTID weather_id = pScene->GetWeatherID();

	IWeather* pWeather = (IWeather*) g_core->GetEntity(weather_id);


	result_string weather_name = "weather";

#define SetWeatherBool(name,ininame) CHelper::SetPropertyBool(pWeather,name, strcmp( (pIni->ReadString(weather_name.c_str(),ininame,"true")),"false")== 0 ?true:false)
#define SetWeatherFloat(name,ininame)  CHelper::SetPropertyFloat(pWeather,name,pIni->ReadFloat(weather_name.c_str(),ininame,CHelper::QueryPropertyFloat(pWeather,name)))
#define SetWeatherInt(name,ininame)    CHelper::SetPropertyInt(pWeather,name,pIni->ReadInteger(weather_name.c_str(),ininame,CHelper::QueryPropertyInt(pWeather,name)))
#define SetWeatherColor(name,ininame)  CHelper::SetPropertyString(pWeather,name,read_color_ini(pIni,weather_name.c_str(),ininame).c_str())


	SetWeatherFloat("FogDensity","FogDensity");
	SetWeatherFloat("FogHeight","FogHeight");
	SetWeatherBool("FogLinear","FogLinear");
	SetWeatherBool("FogExp","FogExp");
	SetWeatherFloat("FogStart","FogStart");
	SetWeatherFloat("FogEnd","FogEnd");
	SetWeatherFloat("WindSpeed","WindSpeed");
	SetWeatherFloat("WindAngle","WindAngle");

	SetWeatherColor("FogColor","Fog");
	SetWeatherColor("FogExpColor","FogExp");
	SetWeatherColor("AmbientColor","Ambient");
	SetWeatherFloat("AmbientIntensity","AmbientIntensity");
	SetWeatherFloat("DiffuseIntensity","DiffuseIntensity");
	SetWeatherFloat("SpecularIntensity","SpecularIntensity");
	SetWeatherFloat("SunGlowIntensity","SunGlowIntensity");
	SetWeatherColor("DiffuseColor","Sunglow");
	SetWeatherColor("SpecularColor","Specular");

	SetWeatherBool("HeightFog","HeightFog");
	SetWeatherColor("HeightFogColor","HeightFog");
	SetWeatherFloat("HeightFogStart","HeightFogStart");
	SetWeatherFloat("HeightFogEnd","HeightFogEnd");

#undef SetWeatherBool
#undef SetWeatherFloat
#undef SetWeatherInt
	pIni->Release();
}

void Game_Scene::load_skybox(IIniFile* pIni,ITerrain* pTerrain,IScene* pScene)
{
	CORE_TRACE("Game_Scene::load_skybox...");
	//创建一个天空实体
	PERSISTID skybox_id = pScene->Create("SkyBox");
	ISkyBox* pSkyBox = (ISkyBox*) g_core->GetEntity(skybox_id);

#define SetSkyBoxFloat(name,ininame)  CHelper::SetPropertyFloat(pSkyBox,name,pIni->ReadFloat("weather",ininame,CHelper::QueryPropertyFloat(pSkyBox,name)))
#define SetSkyBoxString(name,ininame) CHelper::SetPropertyString(pSkyBox,name,pIni->ReadString("weather",ininame,CHelper::QueryPropertyString(pSkyBox,name).c_str()))

	SetSkyBoxFloat("YawSpeed","SkyYawSpeed");

	SetSkyBoxString("UpTex","SkyUpTex");
	SetSkyBoxString("SideTex","SkySideTex");

	pSkyBox->Load();

	//设置天空
	pTerrain->SetSkyID( skybox_id );


#undef SetSkyBoxString
#undef SetSkyBoxFloat
}

//加载太阳相关
void Game_Scene::load_sun(IIniFile* pIni,ITerrain* pTerrain,IScene* pScene)
{
	CORE_TRACE("Game_Scene::load_sun...");
	//开启雾效果
	PERSISTID weather_id = pScene->GetWeatherID();
	IWeather* pWeather = (IWeather*) g_core->GetEntity(weather_id);
	result_string weather_name = "weather";
	float fSunHeight = (float)(pIni->ReadInteger(weather_name.c_str(),"SunHeight",42) / 360.0) * FM_PI * 2.0f;
	float fSunAzimuth = (float)(pIni->ReadInteger(weather_name.c_str(),"SunAzimuth",158) / 360.0) * FM_PI * 2.0f;
	pWeather->MakeSunDirection(fSunHeight,fSunAzimuth);

	//创建太阳光晕
	//创建一个天空实体
	PERSISTID sunglow_id = pScene->Create("SunGlow");
	ISunGlow* pSunGlow = (ISunGlow*) g_core->GetEntity(sunglow_id);
#define SetSunGlowFloat(name,ininame)  CHelper::SetPropertyFloat(pSunGlow,name,pIni->ReadFloat("weather",ininame,CHelper::QueryPropertyFloat(pSunGlow,name)))
#define SetSunGlowString(name,ininame) CHelper::SetPropertyString(pSunGlow,name,pIni->ReadString("weather",ininame,CHelper::QueryPropertyString(pSunGlow,name).c_str()))
#define SetSunGlowBool(name,ininame)   CHelper::SetPropertyBool(pSunGlow,name, strcmp( (pIni->ReadString("weather",ininame,"true")),"true")== 0 ?true:false)
	SetSunGlowString("GlowTex","SunTex");
	SetSunGlowString("FlareTex","SunFlareTex");

	CHelper::SetPropertyFloat(pSunGlow,"GlowSize",200.0);
	SetSunGlowBool("ShowFlare","ShowFlare");

	pSunGlow->AddFlare(0.80f, 55.0f, 1, "0,31,31,31");
	pSunGlow->AddFlare(0.75f, 25.0f, 2, "0,31,31,31");
	pSunGlow->AddFlare(0.70f, 15.0f, 3, "0,31,31,31");
	pSunGlow->AddFlare(0.60f, 30.0f, 4, "0,31,31,31");
	pSunGlow->AddFlare(0.54f, 25.0f, 3, "0,31,31,31");
	pSunGlow->AddFlare(0.49f, 50.0f, 2, "0,31,31,31");
	pSunGlow->AddFlare(0.47f, 80.0f, 2, "0,47,47,47");
	pSunGlow->AddFlare(0.40f, 40.0f, 4, "0,31,31,31");
	pSunGlow->AddFlare(0.35f, 20.0f, 3, "0,31,31,31");
	pSunGlow->AddFlare(0.27f, 90.0f, 2, "0,31,31,31");
	pSunGlow->AddFlare(0.25f, 120.0f, 1, "0,47,47,47");
	pSunGlow->AddFlare(0.15f, 30.0f, 4, "0,31,31,31");
	pSunGlow->Load();

	pScene->AddObject(sunglow_id,80);

	pScene->LayerAdd("sun_trace",pTerrain->GetID());

#undef SetSunGlowString
#undef SetSunGlowFloat
}

//加载影子管理器
void Game_Scene::load_shadow_system(ITerrain* pTerrain,IScene* pScene)
{
	CORE_TRACE("Game_Scene::load_shadow_system...");
    PERSISTID shadow_man = pScene->Create("ShadowManager");
	m_pShadowManager =  (IShadowManager*) g_core->GetEntity(shadow_man);
	m_pShadowManager->SetMaxShadowNum( 10 );
	m_pShadowManager->Load();
	pScene->AddObject( shadow_man, 70 );
	pScene->LayerAdd("shadow",pTerrain->GetID());
}

//加载一个NPC
void Game_Scene::load_npc()
{ 
	/*
	if(m_pRaleNPC)
	{
		FmVec3 _pos = m_pRaleNPC->GetPosition();
		_pos.x += 1.0;
		m_pRaleNPC->SetPosition( _pos.x,_pos.y,_pos.z );
		return;
	}


	RoleComposite* pRale = (RoleComposite*)g_core->CreateEntity("RoleComposite");
	FmVec3 pos = FmVec3(9.638f, 15.774f, -3.731f);
	//pos.y = m_pTerrain->GetFloorHeight( pos.x,pos.z,0) + 10.0f;
	IActor* pRaleNPC = pRale->CreateSceneObj(m_pScene,m_pWorld,m_pTerrain,"char/he_m_0001c",pos);

	//启动阴影
	m_pShadowManager->Create( pRaleNPC->GetID() );

	m_pShadowManager->SetUseSimpleShadow(true);

	m_pRaleNPC = pRaleNPC;
	*/
}

//加载后处理效果
void Game_Scene::load_post_effect(const char* pstrfile,ITerrain* pTerrain,IScene* pScene)
{
	CORE_TRACE("Game_Scene::load_post_effect...");
	char mbzTemp[512];
	//加载水面相关
	SafeSprintf(mbzTemp,sizeof(mbzTemp),"%s%s",pstrfile,"ppfilter.ini");
	load_ppfilter(mbzTemp,pTerrain,pScene);
}

//加载全屏效果参数
void Game_Scene::load_screen_config(const char* pstrfile,ITerrain* pTerrain,IScene* pScene)
{
	char mbzTemp[512];
	//加载水面相关
	SafeSprintf(mbzTemp,sizeof(mbzTemp),"%s%s",pstrfile,"screen.ini");
	IIniFile* pIni = g_core->CreateIniFile(mbzTemp);
	pIni->SetFileName(mbzTemp);
	bool b = pIni->LoadFromFile();
	if(!b)
	{
		pIni->Release();
		return ;
	}

    //读取相关参数
	float fTextureLod = pIni->ReadFloat("screen","TextureLod", -1.0f);
	int nAlphaRef = pIni->ReadInteger("screen","AlphaRef",150);
	float fFarClip = pIni->ReadFloat("screen","FarClip",600.0f);
	float fGrassRadius = pIni->ReadFloat("screen","GrassRadius",100.0f);
	int nVisualEnvMap = pIni->ReadInteger("screen","VisualEnvMap",0);
	float fAlphaFadeRadius = pIni->ReadFloat("screen","AlphaFadeRadius",350.0f);
	float fAlphaHideRadius = pIni->ReadFloat("screen","AlphaHideRadius",500.0f);
	float fVisualRadiusSmall = pIni->ReadFloat("screen","VisualRadiusSmall",1.0f);
	float fVisualRadiusBig = pIni->ReadFloat("screen","VisualRadiusBig",30.0f);
	float fClipRadiusNear = pIni->ReadFloat("screen","ClipRadiusNear",300);
	float fClipRadiusFar = pIni->ReadFloat("screen","ClipRadiusFar",500);
	float fLoadRadiusAdd = pIni->ReadFloat("screen","LoadRadiusAdd",50.0f);
	float fUnloadRadiusAdd = pIni->ReadFloat("screen","UnloadRadiusAdd",50.0f);

	m_pWorld->SetTextureLod(FM_CLAMP( (int)(fTextureLod+0.5f) ,-5,5 ));
	m_pWorld->SetRenderAlphaRef(nAlphaRef);
	 
	pScene->SetFarClipDistance(fFarClip);
    pTerrain->SetGrassRadius(fGrassRadius);
    pTerrain->SetVisualEnvMap(nVisualEnvMap);
	pTerrain->SetAlphaFadeRadius(fAlphaFadeRadius);
    pTerrain->SetAlphaHideRadius(fAlphaHideRadius);
    pTerrain->SetVisualRadiusSmall(fVisualRadiusSmall);
	pTerrain->SetVisualRadiusBig(fVisualRadiusBig);
	pTerrain->SetClipRadiusNear(fClipRadiusNear);
    pTerrain->SetClipRadiusFar(fClipRadiusFar);
    pTerrain->SetLoadRadiusAdd(fLoadRadiusAdd);
    pTerrain->SetUnloadRadiusAdd(fUnloadRadiusAdd);

    pTerrain->RefreshGrass();
    pTerrain->RefreshGround();
	pTerrain->RefreshVisual();

    pIni->Release(); 
}

//加载后处理效果
void Game_Scene::load_ppfilter(const char* pstrfile,ITerrain* pTerrain,IScene* pScene)
{
	IIniFile* pIni = g_core->CreateIniFile(pstrfile);
	pIni->SetFileName(pstrfile);
	bool b = pIni->LoadFromFile();
	if(!b)
	{
		pIni->Release();
		return ;
	}

	PERSISTID post_effect_filter_id = pScene->Create("CPostEffectFilter");
	IEntity* pEntity = (IEntity*) g_core->GetEntity(post_effect_filter_id);
	if(pEntity == NULL)
		return;
	pScene->AddObject(post_effect_filter_id,80);

#define SetPPFilterBool(name,ininame)  CHelper::SetPropertyBool(pEntity,name, strcmp( (pIni->ReadString("ppfilter",ininame,"true")),"true")== 0 ?true:false)
#define SetPPFilterString(name,ininame) CHelper::SetPropertyString(pEntity,name,pIni->ReadString("ppfilter",ininame,CHelper::QueryPropertyString(pEntity,name).c_str()))
#define SetPPFilterFloat(name,ininame)  CHelper::SetPropertyFloat(pEntity,name,pIni->ReadFloat("ppfilter",ininame,CHelper::QueryPropertyFloat(pEntity,name)))
#define SetPPFilterInt(name,ininame)    CHelper::SetPropertyInt(pEntity,name,pIni->ReadInteger("ppfilter",ininame,CHelper::QueryPropertyInt(pEntity,name)))

	SetPPFilterBool("GradualEnable","GradualEnable");
	SetPPFilterFloat("GradualStart","GradualStart");
	SetPPFilterFloat("GradualEnd","GradualEnd");
	SetPPFilterFloat("GradualExp","GradualExp");
	SetPPFilterString("GradualColor","GradualColor");

	SetPPFilterBool("AdjustEnable","HSIEnable");
	SetPPFilterString("AdjustBaseColor","AdjustBaseColor");
	SetPPFilterFloat("AdjustBrightness","AdjustBrightness");
	SetPPFilterFloat("AdjustContrast","AdjustContrast");
	SetPPFilterFloat("AdjustSaturation","AdjustSaturation");

	SetPPFilterBool("AngleEnable","AngleEnable");
	SetPPFilterString("AngleColor","AngleColor");
	SetPPFilterFloat("AngleStart","AngleStart");
	SetPPFilterFloat("AngleEnd","AngleEnd");
	SetPPFilterFloat("AngleExp","AngleExp");
#undef SetPPFilterString
#undef SetPPFilterInt
#undef SetPPFilterFloat
#undef SetPPFilterBool

	pIni->Release();
}

//加载动态天气系统
void Game_Scene::load_dynamic_weather_system(const char* pstrfile,ITerrain* pTerrain,IScene* pScene)
{
	IIniFile* pIni = g_core->CreateIniFile(pstrfile);
	pIni->SetFileName(pstrfile);
	bool b = pIni->LoadFromFile();
	if(!b)
	{
		pIni->Release();
		return ;
	}

	PERSISTID dynamic_weather_id = pScene->Create("DynamicWeather");
	IVisBase* pVisBase = (IVisBase*) g_core->GetEntity(dynamic_weather_id);
	pScene->AddObject(dynamic_weather_id,100); 
	pVisBase->Load();
#define SetDynamicWeatherBool(name,ininame)  CHelper::SetPropertyBool(pVisBase,name, strcmp( (pIni->ReadString("DynamicWeather",ininame,"true")),"true")== 0 ?true:false)
#define SetDynamicWeatherString(name,ininame) CHelper::SetPropertyString(pVisBase,name,pIni->ReadString("DynamicWeather",ininame,CHelper::QueryPropertyString(pVisBase,name).c_str()))
#define SetDynamicWeatherFloat(name,ininame)  CHelper::SetPropertyFloat(pVisBase,name,pIni->ReadFloat("DynamicWeather",ininame,CHelper::QueryPropertyFloat(pVisBase,name)))
	SetDynamicWeatherBool("Visible","Visible");
	SetDynamicWeatherBool("UseTexture","UseTexture");
	SetDynamicWeatherBool("AsyncLoad","AsyncLoad");
    SetDynamicWeatherBool("Occlusion","Occlusion");
    SetDynamicWeatherFloat("Density","Density");
    SetDynamicWeatherFloat("Speed","Speed");
    SetDynamicWeatherFloat("Length","Length");
    SetDynamicWeatherFloat("Width","Width");
    SetDynamicWeatherFloat("Alpha","Alpha");
    SetDynamicWeatherFloat("Wind","Wind");
    SetDynamicWeatherFloat("AlphaFadeSpeedBegin","AlphaFadeSpeedBegin");
    SetDynamicWeatherFloat("AlphaFadeSpeedEnd","AlphaFadeSpeedEnd");
    SetDynamicWeatherString("ParticleNodeTex","ParticleNodeTex");

#undef SetDynamicWeatherBool
#undef SetDynamicWeatherString
#undef SetDynamicWeatherFloat

	pIni->Release();
}


//读取天气信息
result_string Game_Scene::read_color_ini(IIniFile* pIni,const char* section_name,const char* key_name)
{
	char mbzTemp[64];
	sprintf(mbzTemp,"%sAlpha",key_name);
	int a = pIni->ReadInteger(section_name,mbzTemp,0);
	sprintf(mbzTemp,"%sRed",key_name);
	int r = pIni->ReadInteger(section_name,mbzTemp,0);
	sprintf(mbzTemp,"%sGreen",key_name);
	int g = pIni->ReadInteger(section_name,mbzTemp,0);
	sprintf(mbzTemp,"%sBlue",key_name);
	int b = pIni->ReadInteger(section_name,mbzTemp,0);
	sprintf(mbzTemp,"%d,%d,%d,%d",a,r,g,b);
	return mbzTemp;
}


void Game_Scene::read_water_data(IIniFile* pIni,PERSISTID water_id)
{
	IEntity* pEntity = (IEntity*) g_core->GetEntity(water_id);
	result_string water_name = CHelper::QueryPropertyString( pEntity,"Name");

	//水面基本信息
#define ReadWaterString(name) CHelper::SetPropertyString(pEntity,name,pIni->ReadString(water_name.c_str(),name,CHelper::QueryPropertyString(pEntity,name).c_str()))
#define ReadWaterBool(name)   CHelper::SetPropertyBool(pEntity,name, strcmp( (pIni->ReadString(water_name.c_str(),name,"true")),"true")== 0 ?true:false)
#define ReadWaterFloat(name)  CHelper::SetPropertyFloat(pEntity,name,pIni->ReadFloat(water_name.c_str(),name,CHelper::QueryPropertyFloat(pEntity,name)))

	ReadWaterString("WaterColor");
	ReadWaterString("SkyColor");
	ReadWaterBool("Reflect");
	ReadWaterFloat("BaseHeight");

	//反射阳光
	ReadWaterString("SunColorStart");
	ReadWaterString("SunColorEnd");
	ReadWaterFloat("SunLevel");
	ReadWaterFloat("ReflectionSunInten");

	//波陡度
	ReadWaterFloat("WaveGradient0");
	ReadWaterFloat("WaveGradient1");
	ReadWaterFloat("WaveGradient2");
	ReadWaterFloat("WaveGradient3");
	ReadWaterFloat("WaveGradient4");

	//振幅
	ReadWaterFloat("WaveSwing0");
	ReadWaterFloat("WaveSwing1");
	ReadWaterFloat("WaveSwing2");
	ReadWaterFloat("WaveSwing3");
	ReadWaterFloat("WaveSwing4");

	//角频率
	ReadWaterFloat("WaveAngleFreq0");
	ReadWaterFloat("WaveAngleFreq1");
	ReadWaterFloat("WaveAngleFreq2");
	ReadWaterFloat("WaveAngleFreq3");
	ReadWaterFloat("WaveAngleFreq4");

	//相位 x
	ReadWaterFloat("WavePhaX0");
	ReadWaterFloat("WavePhaX1");
	ReadWaterFloat("WavePhaX2");
	ReadWaterFloat("WavePhaX3");
	ReadWaterFloat("WavePhaX4");

	//相位 z
	ReadWaterFloat("WavePhaZ0");
	ReadWaterFloat("WavePhaZ1");
	ReadWaterFloat("WavePhaZ2");
	ReadWaterFloat("WavePhaZ3");
	ReadWaterFloat("WavePhaZ4");

	//垂直于波阵面的水平x方向
	ReadWaterFloat("WaveDirectionX0");
	ReadWaterFloat("WaveDirectionX1");
	ReadWaterFloat("WaveDirectionX2");
	ReadWaterFloat("WaveDirectionX3");
	ReadWaterFloat("WaveDirectionX4");

	//垂直于波阵面的水平z方向
	ReadWaterFloat("WaveDirectionZ0");
	ReadWaterFloat("WaveDirectionZ1");
	ReadWaterFloat("WaveDirectionZ2");
	ReadWaterFloat("WaveDirectionZ3");
	ReadWaterFloat("WaveDirectionZ4");

	//normalmap scale
	ReadWaterFloat("NormalMapScale0");
	ReadWaterFloat("NormalMapScale1");
	ReadWaterFloat("NormalMapScale2");
	ReadWaterFloat("NormalMapScale3");

	//normalmap speed x
	ReadWaterFloat("NormalMapSpeedX0");
	ReadWaterFloat("NormalMapSpeedX1");
	ReadWaterFloat("NormalMapSpeedX2");
	ReadWaterFloat("NormalMapSpeedX3");

	//normalmap speed z
	ReadWaterFloat("NormalMapSpeedZ0");
	ReadWaterFloat("NormalMapSpeedZ1");
	ReadWaterFloat("NormalMapSpeedZ2");
	ReadWaterFloat("NormalMapSpeedZ3");

	//distance scale
	ReadWaterFloat("DistanceScale");
	//reflection inten
	ReadWaterFloat("ReflectionInten");

	ReadWaterFloat("RefractionInten");
	//reflection weight
	ReadWaterFloat("ReflectionWeight");

	ReadWaterFloat("RefractionWeight");

	//读取贴图路径
	//NormalMap File Name
	ReadWaterString("NormalMapFileName1");
	ReadWaterString("NormalMapFileName2");
	ReadWaterString("NormalMapFileName3");
	ReadWaterString("NormalMapFileName4");

	//白色水域贴图
	ReadWaterString("WhiteWaterMap");

	//水下刻蚀
	ReadWaterFloat("CausticSpeed");
	ReadWaterFloat("CausticScale");
	ReadWaterFloat("CausticBright");
	ReadWaterFloat("CausticFocus");
	ReadWaterString("CausticMap");
	ReadWaterString("NoiseMap");

#undef ReadWaterString
#undef ReadWaterBool
#undef ReadWaterFloat
}

//读取草信息
void Game_Scene::read_grass_data(IIniFile* pIni,PERSISTID grass_id)
{
	IEntity* pEntity = (IEntity*) g_core->GetEntity(grass_id);
	result_string grass_name = CHelper::QueryPropertyString( pEntity,"Name");

#define ReadGrassString(name) CHelper::SetPropertyString(pEntity,name,pIni->ReadString(grass_name.c_str(),name,CHelper::QueryPropertyString(pEntity,name).c_str()))
#define ReadGrassFloat(name)  CHelper::SetPropertyFloat(pEntity,name,pIni->ReadFloat(grass_name.c_str(),name,CHelper::QueryPropertyFloat(pEntity,name)))
#define ReadGrassInt(name)    CHelper::SetPropertyInt(pEntity,name,pIni->ReadInteger(grass_name.c_str(),name,CHelper::QueryPropertyInt(pEntity,name)))

	ReadGrassString("Texture");
	ReadGrassInt("TexScale");
	ReadGrassInt("FrameWidth");
	ReadGrassInt("FrameHeight");

	ReadGrassFloat("MinWidth");
	ReadGrassFloat("MaxWidth"); 
	ReadGrassFloat("MinHeight");
	ReadGrassFloat("MaxHeight");
	ReadGrassFloat("MinPitch");
	ReadGrassFloat("MaxPitch");

#undef ReadGrassInt
#undef ReadGrassFloat
#undef ReadGrassString
}

void Game_Scene::updata_seadata(PERSISTID water_id)
{
	ISeaWater* pSeaWater = (ISeaWater*) g_core->GetEntity(water_id);
	pSeaWater->UpdateSeaData();
}


Game_Scene g_game_scene;

void terrain_unload()
{
	g_game_scene.UnLoad();

	//清理缓存
	g_render->ClearTextureCache();
}

//
bool terrain_load(const char* pstrfile, bool bLoadSkyBox)
{
	IScene* pScene = (IScene*)g_core->LookupEntity("Scene");

	//
	terrain_unload();

	return g_game_scene.Load(pstrfile, pScene, g_world, bLoadSkyBox);
}