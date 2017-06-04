#include "game_scene.h"
#include "helper.h"
#include "../visual/i_world.h"
#include "../visual/i_sea_water.h"
#include "../visual/i_weather.h"
#include "../visual/i_sky_box.h"
#include "../visual/i_sun_glow.h"
#include "../visual/i_particle_manager.h"
#include "global.h"
#include "../visual/i_context.h"
#include "../define/priority_define.h"

#define PERFORMANCE_DEBUG

inline const char* gettexpath(const char* file, char* pre = "map/", const char* ext = "dds", char* buf = NULL)
{
	static char buffer[4096];
	if(!buf)
		buf = buffer;
// 	if(ext && ext[0])
// 	{
// 		sprintf(buf, "%s/%s.%s", pre, file, ext);
// 	}
// 	else
// 	{
// 		sprintf(buf, "%s/%s", pre, file);
//   }
  sprintf(buf, "%s/%s", "", file);
	return buf;
}

inline void ReadWaterTexture(IIniFile* pIni, IEntity* pEntity, const char* water_name, const char* name) 
{
	CHelper::SetPropertyString(pEntity, name,gettexpath(pIni->ReadString(water_name,name, CHelper::QueryPropertyString(pEntity,name).c_str()), "map/", ""));
}

bool SceneManager::Load(const char* pstrfile,IScene* pScene, bool can_load_skybox)
{
	//pScene->SetEnableEarlyZ(true);
	return load_terrain(pstrfile,pScene, can_load_skybox);
}

void SceneManager::Unload()
{
	if(g_terrain)
	{
		g_scene->RemoveObject(g_terrain->GetID());
		g_core->DeleteEntity(g_terrain->GetID());
		g_terrain = NULL;
	}
}

bool SceneManager::load_terrain(const char* pstrfile,IScene* pScene, bool can_load_skybox)
{
	 
	Unload();
	char mbzTemp[512];
	sprintf(mbzTemp,"%s%s",pstrfile,"terrain.ini");
	//�ȴ���һ������
	IIniFile* pIni = g_core->CreateIniFile(pstrfile);
	pIni->SetFileName(mbzTemp);
	bool b = pIni->LoadFromFile();
	if(!b)
	{
		pIni->Release();
		return false;
	}

	char res_path[256];
	strcpy(res_path, g_core->GetResourcePath());
	
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

	//��������
	PERSISTID terrain_id = pScene->Create("Terrain");
  ITerrain* pTerrain = (ITerrain*) g_core->GetEntity(terrain_id);
	g_terrain = pTerrain;

	//CHelper::SetPropertyString(pTerrain,"AppendPath","map\\");
	pTerrain->SetParameter(zone_scale, chunk_scale, texture_scale,texture_units, alpha_per_unit, collide_per_unit);
	CHelper::SetPropertyInt(pTerrain,"LightPerUnit",light_per_unit);
	CHelper::SetPropertyString(pTerrain,"FilePath",pstrfile);

	CHelper::SetPropertyInt(pTerrain,"DesignRadius",120);
	CHelper::SetPropertyInt(pTerrain,"CollideRadius",50);

	//��λ����
	CHelper::SetPropertyString(pTerrain,"HeightMap",height_map);
	CHelper::SetPropertyFloat(pTerrain,"InitHeight",init_height);
	CHelper::SetPropertyInt(pTerrain,"LightTexScale",light_tex_scale);
	//��������ļ�·��
	CHelper::SetPropertyString(pTerrain,"ZoneLightPath",zone_light_path);
	//ģ�͹����ļ�·��
	CHelper::SetPropertyString(pTerrain,"ModelLightPath",model_light_path);
	//�������ļ�·��
	CHelper::SetPropertyString(pTerrain,"WalkablePath",walkable_path);
	//�Ƿ񲻼��ع���
	CHelper::SetPropertyBool(pTerrain,"NoLight",bno_light);
	//�Ƿ���������ͼ
	CHelper::SetPropertyBool(pTerrain,"EnableNormalMap",bnormalmap);
	//��ײ����ߴ�
	CHelper::SetPropertyInt(pTerrain,"CollideScale",8);
	pTerrain->ClearBaseTex();

	CVarList baselist;
	pIni->GetItemList("base",baselist);


	for(size_t i=0;i<baselist.GetCount();i++)
	{
		const char* base_name = baselist.StringVal(i);
		pTerrain->AddBaseTex(base_name, gettexpath(pIni->ReadString("base",base_name,"")));

		//��ȡ�������ͼ
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
		pTerrain->AddBlendTex(blend_name, gettexpath(pIni->ReadString("blend", blend_name, ""), "map/"));
		float power = pIni->ReadFloat("specular_power", blend_name, 0.0);
		int blend_index = pTerrain->GetBlendTexIndex(blend_name);
		pTerrain->SetBlendTexSpecularPower(blend_index,power);
	}

	//����������ͼ·��
	pTerrain->ClearTexturePath();
	CVarList tex_pathlist;
	pIni->GetItemList("tex_path",tex_pathlist);
	for(size_t i=0;i<tex_pathlist.GetCount();i++)
	{
		const char* key = tex_pathlist.StringVal(i);
		pTerrain->AddTexturePath(gettexpath(pIni->ReadString("tex_path",key,""),"map/", "") );
	}

	PERSISTID particle_man_id = g_scene->Create("ParticleManager");
	IParticleManager* pParticleMan = (IParticleManager*)g_core->GetEntity(particle_man_id);
	pParticleMan->SetTexturePath("map/tex/particles/");
	g_scene->AddObject(particle_man_id, 100);
	g_scene->GetContext()->SetUnique(g_core->GetEntity(particle_man_id));
	//����ˮ�����
	sprintf(mbzTemp,"%s%s",pstrfile,"water.ini");
  load_water(mbzTemp,pTerrain,pScene);

	//���صر�������
	sprintf(mbzTemp,"%s%s",pstrfile,"material.ini");
    load_material(mbzTemp,pTerrain,pScene);

	//������ͼ��ʽ
	sprintf(mbzTemp,"%s%s",pstrfile,"base_format.ini");
	load_format(mbzTemp,pTerrain,pScene);

	//���صر���ʽ
    sprintf(mbzTemp,"%s%s",pstrfile,"format.ini");
    load_format(mbzTemp,pTerrain,pScene);
    //����ֲ�������
	sprintf(mbzTemp,"%s%s",pstrfile,"grass.ini");
    load_grass(mbzTemp,pTerrain,pScene);
	//��������
	sprintf(mbzTemp,"%s%s",pstrfile,"weather.ini");
  load_weather(mbzTemp,pTerrain,pScene, can_load_skybox);
  pTerrain->Load();

  //���ض�̬����ϵͳ
  sprintf(mbzTemp,"%s%s",pstrfile,"dynamic_weather.ini");
  //load_dynamic_weather_system(mbzTemp,pTerrain,pScene);
	 
 	pScene->AddObject( pTerrain->GetID() , 30 );

	//ȫ�ֻ�����ͼ
    CHelper::SetPropertyString(pTerrain,"GlobalEnvironmentMap","tex\\environment_reflect.dds");
 
	//��ʼ������
	pTerrain->InitZones(zone_rows, zone_cols, origin_row, origin_col, zone_range);

  pTerrain->SetVisualEnvMap(2);
  pTerrain->SetWaterVisible(true);//��ʱ��ֹˮ��������
 	pTerrain->SetWaterReflect(true);
  pTerrain->SetWaterWave(true);
  pTerrain->SetWaterEnvMapReusable(true);
  pTerrain->SetWaterSunReflect(true);
  pTerrain->SetWaterBorder(true);
  pTerrain->SetWaterDepth(true);

  pTerrain->SetGroundVisible(true);

  pTerrain->SetCastShadow(false);
  pScene->SetEnableDynamicShadow(false);

  //������Ӱ����
  //load_shadow_system(pTerrain,pScene);

	pIni->Release();

	load_post_effect(pstrfile,pTerrain,pScene);

	return true;
}


//���صر���ʽ����
void SceneManager::load_format(const char* pstrfile,ITerrain* pTerrain,IScene* pScene)
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



//���ز��ʲ���
void SceneManager::load_material(const char* pstrfile,ITerrain* pTerrain,IScene* pScene)
{
	IIniFile* pIni = g_core->CreateIniFile(pstrfile);
	pIni->SetFileName(pstrfile);
	bool b = pIni->LoadFromFile();
	if(!b)
	{
        pIni->Release();
		return;
	}

	//��ȡ���εĲ�������
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

bool SceneManager::load_water(const char* pstrfile,ITerrain* pTerrain,IScene* pScene)
{	 
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

//���ز�
bool SceneManager::load_grass(const char* pstrfile,ITerrain* pTerrain,IScene* pScene)
{
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
		//PERSISTID grass_id = pTerrain->CreateGrass(gettexpath(pgrass_name, "map/"));
		PERSISTID grass_id = pTerrain->CreateGrass(pgrass_name);
		if(!grass_id.IsNull())
		{
           read_grass_data(pIni,grass_id);
		}
	}
	pIni->Release();
	return true;
}


//���غ���Ч��
void SceneManager::load_post_effect(const char* pstrfile,ITerrain* pTerrain,IScene* pScene)
{
	char mbzTemp[512];
	//����ˮ�����
	sprintf(mbzTemp,"%s%s",pstrfile,"ppfilter.ini");
    load_ppfilter(mbzTemp,pTerrain,pScene);
}

//���غ���Ч��
void SceneManager::load_ppfilter(const char* pstrfile,ITerrain* pTerrain,IScene* pScene)
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

void SceneManager::load_weather(const char* pstrfile,ITerrain* pTerrain,IScene* pScene, bool can_load_skybox)
{

	IIniFile* pIni = g_core->CreateIniFile(pstrfile);
	pIni->SetFileName(pstrfile);
	bool b = pIni->LoadFromFile();
	if(!b)
	{
		pIni->Release();
		return ;
	}

	if(can_load_skybox)
		load_skybox(pIni,pTerrain,pScene);
	load_sun(pIni,pTerrain,pScene);

	//������Ч��
	PERSISTID weather_id = pScene->GetWeatherID();
 
	IWeather* pWeather = (IWeather*) g_core->GetEntity(weather_id);
 

	result_string weather_name = "weather";

#define SetWeatherBool(name,ininame) CHelper::SetPropertyBool(pWeather,name, strcmp( (pIni->ReadString(weather_name.c_str(),ininame,"true")),"true")== 0 ?true:false)
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
	SetWeatherColor("DiffuseColor","Sunglow");
 
#undef SetWeatherBool
#undef SetWeatherFloat
#undef SetWeatherInt
	pIni->Release();
 
}

void SceneManager::load_skybox(IIniFile* pIni,ITerrain* pTerrain,IScene* pScene)
{
    //����һ�����ʵ��
	PERSISTID skybox_id = pScene->Create("SkyBox");
	ISkyBox* pSkyBox = (ISkyBox*) g_core->GetEntity(skybox_id);

#define SetSkyBoxFloat(name,ininame)  CHelper::SetPropertyFloat(pSkyBox,name,pIni->ReadFloat("weather",ininame,CHelper::QueryPropertyFloat(pSkyBox,name)))
#define SetSkyBoxString(name,ininame) CHelper::SetPropertyString(pSkyBox,name,pIni->ReadString("weather",ininame,CHelper::QueryPropertyString(pSkyBox,name).c_str()))
#define SetSkyBoxTexture(name,ininame) CHelper::SetPropertyString(pSkyBox,name,gettexpath(pIni->ReadString("weather",ininame,CHelper::QueryPropertyString(pSkyBox,name).c_str()), "map/", "dds"))

	SetSkyBoxFloat("YawSpeed","SkyYawSpeed");

	//CHelper::SetPropertyString(pTerrain, "UpTex",gettexpath(pIni->ReadString("weather", "UpTex", CHelper::QueryPropertyString(pTerrain,"SkyUpTex").c_str()), "map/", "dds"));
	//CHelper::SetPropertyString(pTerrain, "SideTex",gettexpath(pIni->ReadString("weather", "SideTex", CHelper::QueryPropertyString(pTerrain,"SkySideTex").c_str()), "map/", "dds"));

	SetSkyBoxTexture("UpTex","SkyUpTex");
    SetSkyBoxTexture("SideTex","SkySideTex");

	pSkyBox->Load();
 
	//�������
	pTerrain->SetSkyID( skybox_id );

#undef SetSkyBoxTexture
#undef SetSkyBoxString
#undef SetSkyBoxFloat
}

//����̫�����
void SceneManager::load_sun(IIniFile* pIni,ITerrain* pTerrain,IScene* pScene)
{
	//������Ч��
	PERSISTID weather_id = pScene->GetWeatherID();
	IWeather* pWeather = (IWeather*) g_core->GetEntity(weather_id);
	result_string weather_name = "weather";
    float fSunHeight = (float)(pIni->ReadInteger(weather_name.c_str(),"SunHeight",42) / 360.0) * FM_PI * 2.0f;
    float fSunAzimuth = (float)(pIni->ReadInteger(weather_name.c_str(),"SunAzimuth",158) / 360.0) * FM_PI * 2.0f;
	pWeather->MakeSunDirection(fSunHeight,fSunAzimuth);

	//����̫������
    //����һ�����ʵ��
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

#undef SetSunGlowString
#undef SetSunGlowFloat
}


//��ȡ������Ϣ
result_string SceneManager::read_color_ini(IIniFile* pIni,const char* section_name,const char* key_name)
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

void SceneManager::read_water_data(IIniFile* pIni,PERSISTID water_id)
{
  IEntity* pEntity = (IEntity*) g_core->GetEntity(water_id);
	result_string water_name = CHelper::QueryPropertyString( pEntity,"Name");

	//ˮ�������Ϣ
#define ReadWaterString(name) CHelper::SetPropertyString(pEntity,name,pIni->ReadString(water_name.c_str(),name,CHelper::QueryPropertyString(pEntity,name).c_str()))
#define ReadWaterBool(name)   CHelper::SetPropertyBool(pEntity,name, strcmp( (pIni->ReadString(water_name.c_str(),name,"true")),"true")== 0 ?true:false)
#define ReadWaterFloat(name)  CHelper::SetPropertyFloat(pEntity,name,pIni->ReadFloat(water_name.c_str(),name,CHelper::QueryPropertyFloat(pEntity,name)))
//#define ReadWaterTexture(name) CHelper::SetPropertyString(pEntity,name,pIni->ReadString(water_name.c_str(),name,gettexpath(CHelper::QueryPropertyString(pEntity,name).c_str())) )

	ReadWaterString("WaterColor");
	ReadWaterString("SkyColor");
	ReadWaterBool("Reflect");
	ReadWaterFloat("BaseHeight");

	//��������
	ReadWaterString("SunColorStart");
    ReadWaterString("SunColorEnd");
	ReadWaterFloat("SunLevel");
	ReadWaterFloat("ReflectionSunInten");

	//������
	ReadWaterFloat("WaveGradient0");
	ReadWaterFloat("WaveGradient1");
	ReadWaterFloat("WaveGradient2");
	ReadWaterFloat("WaveGradient3");
	ReadWaterFloat("WaveGradient4");

	//���
	ReadWaterFloat("WaveSwing0");
	ReadWaterFloat("WaveSwing1");
	ReadWaterFloat("WaveSwing2");
	ReadWaterFloat("WaveSwing3");
	ReadWaterFloat("WaveSwing4");

	//��Ƶ��
	ReadWaterFloat("WaveAngleFreq0");
	ReadWaterFloat("WaveAngleFreq1");
	ReadWaterFloat("WaveAngleFreq2");
	ReadWaterFloat("WaveAngleFreq3");
	ReadWaterFloat("WaveAngleFreq4");

	//��λ x
	ReadWaterFloat("WavePhaX0");
	ReadWaterFloat("WavePhaX1");
	ReadWaterFloat("WavePhaX2");
	ReadWaterFloat("WavePhaX3");
	ReadWaterFloat("WavePhaX4");

	//��λ z
	ReadWaterFloat("WavePhaZ0");
	ReadWaterFloat("WavePhaZ1");
	ReadWaterFloat("WavePhaZ2");
	ReadWaterFloat("WavePhaZ3");
	ReadWaterFloat("WavePhaZ4");

	//��ֱ�ڲ������ˮƽx����
	ReadWaterFloat("WaveDirectionX0");
	ReadWaterFloat("WaveDirectionX1");
	ReadWaterFloat("WaveDirectionX2");
	ReadWaterFloat("WaveDirectionX3");
	ReadWaterFloat("WaveDirectionX4");

	//��ֱ�ڲ������ˮƽz����
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

	//��ȡ��ͼ·��
	//NormalMap File Name
	ReadWaterTexture(pIni, pEntity, water_name.c_str(), "NormalMapFileName1");
  ReadWaterTexture(pIni, pEntity, water_name.c_str(), "NormalMapFileName2");
  ReadWaterTexture(pIni, pEntity, water_name.c_str(), "NormalMapFileName3");
  ReadWaterTexture(pIni, pEntity, water_name.c_str(), "NormalMapFileName4");

	//��ɫˮ����ͼ
	ReadWaterTexture(pIni, pEntity, water_name.c_str(), "WhiteWaterMap");
   
	//ˮ�¿�ʴ
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

//��ȡ����Ϣ
void SceneManager::read_grass_data(IIniFile* pIni,PERSISTID grass_id)
{
    IEntity* pEntity = (IEntity*) g_core->GetEntity(grass_id);
	result_string grass_name = CHelper::QueryPropertyString( pEntity,"Name");

#define ReadGrassTexture(name) CHelper::SetPropertyString(pEntity,name,gettexpath(pIni->ReadString(grass_name.c_str(),name,CHelper::QueryPropertyString(pEntity,name).c_str())))
#define ReadGrassString(name) CHelper::SetPropertyString(pEntity,name,pIni->ReadString(grass_name.c_str(),name,CHelper::QueryPropertyString(pEntity,name).c_str()))
#define ReadGrassFloat(name)  CHelper::SetPropertyFloat(pEntity,name,pIni->ReadFloat(grass_name.c_str(),name,CHelper::QueryPropertyFloat(pEntity,name)))
#define ReadGrassInt(name)    CHelper::SetPropertyInt(pEntity,name,pIni->ReadInteger(grass_name.c_str(),name,CHelper::QueryPropertyInt(pEntity,name)))

    ReadGrassTexture("Texture");
	ReadGrassInt("TexScale");
	ReadGrassInt("FrameWidth");
	ReadGrassInt("FrameHeight");

	ReadGrassFloat("MinWidth");
	ReadGrassFloat("MaxWidth"); 
	ReadGrassFloat("MinHeight");
	ReadGrassFloat("MaxHeight");
	ReadGrassFloat("MinPitch");
	ReadGrassFloat("MaxPitch");

#undef ReadGrassTexture
#undef ReadGrassInt
#undef ReadGrassFloat
#undef ReadGrassString
}

void SceneManager::updata_seadata(PERSISTID water_id)
{
  ISeaWater* pSeaWater = (ISeaWater*) g_core->GetEntity(water_id);
	pSeaWater->UpdateSeaData();
}

void SceneManager::load_dynamic_weather_system(const char* szFileName, ITerrain* pTerrain, IScene* pScene)
{
  IIniFile* pIni = g_core->CreateIniFile(szFileName);
  pIni->SetFileName(szFileName);
  bool b = pIni->LoadFromFile();
  if(!b)
  {
    pIni->Release();
    return ;
  }

  PERSISTID dynamic_weather_id = pScene->Create("DynamicWeather");
  IVisBase* pVisBase = (IVisBase*) g_core->GetEntity(dynamic_weather_id);
  pScene->AddObject(dynamic_weather_id,Priority_Scene_DynamicWeather); 
  m_vecSceneObjEnt.push_back(dynamic_weather_id);

  pVisBase->Load();
#define SetDynamicWeatherBool(name,ininame)  CHelper::SetPropertyBool(pVisBase,name, strcmp( (pIni->ReadString("DynamicWeather", ininame, "true")),"true")== 0 ?true:false)
#define SetDynamicWeatherString(name,ininame) CHelper::SetPropertyString(pVisBase,name,gettexpath(pIni->ReadString("DynamicWeather", ininame, CHelper::QueryPropertyString(pVisBase,name).c_str())))
#define SetDynamicWeatherFloat(name,ininame)  CHelper::SetPropertyFloat(pVisBase,name,pIni->ReadFloat("DynamicWeather", ininame, CHelper::QueryPropertyFloat(pVisBase,name)))
  SetDynamicWeatherBool("Visible", "Visible");
  SetDynamicWeatherBool("UseTexture", "UseTexture");
  SetDynamicWeatherBool("AsyncLoad", "AsyncLoad");
  SetDynamicWeatherBool("Occlusion", "Occlusion");
  SetDynamicWeatherFloat("Density", "Density");
  SetDynamicWeatherFloat("Speed", "Speed");
  SetDynamicWeatherFloat("Length", "Length");
  SetDynamicWeatherFloat("Width", "Width");
  SetDynamicWeatherFloat("Alpha", "Alpha");
  SetDynamicWeatherFloat("Wind", "Wind");
  SetDynamicWeatherFloat("AlphaFadeSpeedBegin", "AlphaFadeSpeedBegin");
  SetDynamicWeatherFloat("AlphaFadeSpeedEnd", "AlphaFadeSpeedEnd");
  SetDynamicWeatherString("ParticleNodeTex", "ParticleNodeTex");

#undef SetDynamicWeatherBool
#undef SetDynamicWeatherString
#undef SetDynamicWeatherFloat

  pIni->Release();
}

SceneManager g_game_scene;
bool terrain_load(const char* pstrfile, bool can_load_skybox)
{
	IScene* pScene = (IScene*)g_core->LookupEntity("Scene");
	return g_game_scene.Load(pstrfile, pScene, can_load_skybox);
}

