#pragma once

/*
场景相关代码
*/

#include <vector>
#include "../public/module.h"
#include "../public/core_type.h"


#include "../visual/i_scene.h"
#include "../visual/i_terrain.h"
#include "../visual/i_terrain.h"
#include "../public/i_ini_file.h"

class SceneManager
{
public:
	bool Load(const char* pstrfile,IScene* pScene, bool can_load_skybox = true);
	void Unload();
private:
    //加载材质参数
	void load_material(const char* pstrfile,ITerrain* pTerrain,IScene* pScene);
	//加载地表样式数据
	void load_format(const char* pstrfile,ITerrain* pTerrain,IScene* pScene);
	//加载地表
	bool load_terrain(const char* pstrfile,IScene* pScene, bool can_load_skybox = true);
	//加载水面
	bool load_water(const char* pstrfile,ITerrain* pTerrain,IScene* pScene);
    //加载草
	bool load_grass(const char* pstrfile,ITerrain* pTerrain,IScene* pScene);
	//加载天气
	void load_weather(const char* pstrfile,ITerrain* pTerrain,IScene* pScene, bool can_load_skybox = true);
	//加载天空盒
	void load_skybox(IIniFile* pIni,ITerrain* pTerrain,IScene* pScene);
	//加载太阳相关
	void load_sun(IIniFile* pIni,ITerrain* pTerrain,IScene* pScene);
   
	//读取天气信息
	result_string read_color_ini(IIniFile* pIni,const char* section_name,const char* key_name);
	

	//读取水面信息
	void read_water_data(IIniFile* pIni,PERSISTID water_id);
	void updata_seadata(PERSISTID water_id);

	//读取草信息
	void read_grass_data(IIniFile* pIni,PERSISTID grass_id);
private:
    //加载后处理效果
	void load_post_effect(const char* pstrfile,ITerrain* pTerrain,IScene* pScene);

	//加载后处理效果
  void load_ppfilter(const char* pstrfile,ITerrain* pTerrain,IScene* pScene);

  //加载动态天气系统
  void load_dynamic_weather_system(const char* szFileName, ITerrain* pTerrain, IScene* pScene);

private:
  std::vector<PERSISTID> m_vecSceneObjEnt;//加到场景里的实体列表
}; 
