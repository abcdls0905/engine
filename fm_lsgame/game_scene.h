#ifndef _GAME_SCENE_H
#define _GAME_SCENE_H
/*
场景相关代码
*/

#include "../public/module.h"
#include "../public/core_type.h"


#include "../visual/i_world.h"
#include "../visual/i_scene.h"
#include "../visual/i_terrain.h"
#include "../visual/i_shadow_manager.h"

#include "../public/i_ini_file.h"
#include "../visual/i_input.h"
#include "../visual/i_actor.h"

#include "../visual/i_render.h"
#include <vector>

class Game_Scene : public IEntity,IInputMouseCallBack
{
public:
	Game_Scene();
	virtual ~Game_Scene();
 
	virtual bool Init(const IVarList& args);
 
	virtual bool Shut() ;
 
	virtual void Execute(float seconds);

	bool Load(const char* pstrfile,IScene* pScene,IWorld* pWorld, bool bLoadSkybox = true);

	// 新增场景卸载
	void UnLoad();

	virtual void MouseCallBack(IMouseEvent* Event);
private:
	//加载地表
	bool load_terrain(const char* pstrfile,IScene* pScene);
    //加载材质参数
	void load_material(const char* pstrfile,ITerrain* pTerrain,IScene* pScene);
	//加载基准纹理样式
	void load_base_format(const char* pstrfile,ITerrain* pTerrain,IScene* pScene);
	//加载地表样式数据
	void load_format(const char* pstrfile,ITerrain* pTerrain,IScene* pScene);
	//加载粒子系统
	void load_particle_system(const char* pstrfile,ITerrain* pTerrain,IScene* pScene);
	//加载水面
	bool load_water(const char* pstrfile,ITerrain* pTerrain,IScene* pScene);
    //加载草
	bool load_grass(const char* pstrfile,ITerrain* pTerrain,IScene* pScene);
	//加载天气
	void load_weather(const char* pstrfile,ITerrain* pTerrain,IScene* pScene);
	//加载天空盒
	void load_skybox(IIniFile* pIni,ITerrain* pTerrain,IScene* pScene);
	//加载太阳相关
	void load_sun(IIniFile* pIni,ITerrain* pTerrain,IScene* pScene);
	//加载影子管理器
	void load_shadow_system(ITerrain* pTerrain,IScene* pScene);
	//加载一个NPC
	void load_npc();

	//读取天气信息
	result_string read_color_ini(IIniFile* pIni,const char* section_name,const char* key_name);	

	//读取水面信息
	void read_water_data(IIniFile* pIni,PERSISTID water_id);
	void updata_seadata(PERSISTID water_id);

	//读取草信息 
	void read_grass_data(IIniFile* pIni,PERSISTID grass_id);
private:
	//加载全屏效果参数
	void load_screen_config(const char* pstrfile,ITerrain* pTerrain,IScene* pScene);

    //加载后处理效果
	void load_post_effect(const char* pstrfile,ITerrain* pTerrain,IScene* pScene);

	//加载后处理效果
	void load_ppfilter(const char* pstrfile,ITerrain* pTerrain,IScene* pScene);

	//加载动态天气系统
	void load_dynamic_weather_system(const char* pstrfile,ITerrain* pTerrain,IScene* pScene);
    ITerrain* m_pTerrain;
	IScene*   m_pScene;
	IWorld*   m_pWorld;
	IShadowManager* m_pShadowManager;
	IInputManager* m_InputManager;

	IActor* m_pRaleNPC;

	IRender* m_pRender;

	std::vector<PERSISTID> m_vecSceneObjEnt;//加到场景里的实体列表
}; 



#endif