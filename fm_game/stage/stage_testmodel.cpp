#include "stage_testmodel.h"
#include "global.h"
#include "../visual/i_model_system.h"
#include "../visual/i_model.h"
#include "../visual/i_scene.h"
#include "../public/i_ini_file.h"
#include "helper.h"
#include "../visual/i_weather.h"
#include "../visual/i_effect_model.h"
#include <assert.h>
#include "../visual/i_context.h"
#include "../visual/i_particle_manager.h"
#include "../visual/i_particle.h"
#include "../visual/i_camera.h"
#include "global.h"
#include "utility.h"
#include "../visual/i_camera.h"
#include "../visual/i_scene.h"
#include "../visual/i_terrain.h"
#include "../visual/i_render.h"

DECLARE_ENTITY(StageTestModel, 0, IEntity);


inline result_string read_color_ini(IIniFile* pIni,const char* section_name,const char* key_name)
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

inline void load_weather(const char* pstrfile)
{

	IIniFile* pIni = g_core->CreateIniFile(pstrfile);
	pIni->SetFileName(pstrfile);
	bool b = pIni->LoadFromFile();
	if(!b)
	{
		pIni->Release();
		return ;
	}

	//¿ªÆôÎíÐ§¹û
	PERSISTID weather_id = g_scene->GetWeatherID();
 
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

StageTestModel::StageTestModel(void)
{
}

StageTestModel::~StageTestModel(void)
{
}


inline IVisBase* add_model(const char* name, const char* texpath = "effect/ter/", bool showboundbox = true)
{
	IEntity* pEntity = g_core->CreateEntity("Model");
	((IModel*)pEntity)->SetContext(g_scene->GetContext());
	((IModel*)pEntity)->SetModelFile(name);
	((IModel*)pEntity)->SetTexPaths(texpath);
	((IModel*)pEntity)->SetShowBoundBox(showboundbox);
	((IModel*)pEntity)->Load();
	((IModel*)pEntity)->SetPosition(0, 0, 0);
	((IVisBase*)pEntity)->SetContext(g_scene->GetContext());
	g_scene->AddObject(pEntity->GetID(), 45);
	return (IVisBase*)pEntity;
}

inline bool add_effectmodel(const char* ini_name, const char* name)
{
	IEntity* pEntity = g_core->CreateEntity("EffectModel");
	
	((IVisBase*)pEntity)->SetContext(g_scene->GetContext());
	g_scene->AddObject(pEntity->GetID(), 45);
	
	return ((IEffectModel*)pEntity)->CreateFromIniEx(ini_name, name, false, "map/");
}

void StageTestModel::OnEnterStage()
{
	char mbzTemp[256];
	//arena
	sprintf(mbzTemp,"%s%s%s/", g_core->GetResourcePath(), "map/ter/", "login2d");
	terrain_load(mbzTemp);
	g_terrain->SetGroundVisible(false);
	g_terrain->SetVisualVisible(false);
	ICamera* pCamera = (ICamera*)g_core->GetEntity(g_scene->GetCameraID());
	//pCamera->SetPosition(19.497921, 10.894909, -8.118525);
	//pCamera->SetAngle(-0.116355, -1.230457, 0.000000);
	pCamera->SetPosition(0, 0, 0);
	pCamera->SetAngle(0,0,0);

	IVisBase* pVisBase;	
	
//	IVisBase* pVisBase = add_model("effect\\skill\\player\\skill_vortex_01B.xmod");
//	pVisBase->SetPosition(0, 0, 0);
	
	
	//pVisBase = add_model("effect\\common\\common_gun_017.xmod");
	
//	pVisBase = add_model("effect\\skill\\player\\skill_cartridge_001.xmod");
//	pVisBase->SetPosition(0, 0, 10);

#ifdef TEST_3DUI
	IEntity* pEntity = g_core->CreateEntity("UI3D");
	((IVisBase*)pEntity)->SetContext(g_render->GetContext());
	g_scene->AddObject(pEntity->GetID(), 0xffffffff);
#endif
}

void StageTestModel::OnLeaveStage()
{
}

