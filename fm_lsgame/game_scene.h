#ifndef _GAME_SCENE_H
#define _GAME_SCENE_H
/*
������ش���
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

	// ��������ж��
	void UnLoad();

	virtual void MouseCallBack(IMouseEvent* Event);
private:
	//���صر�
	bool load_terrain(const char* pstrfile,IScene* pScene);
    //���ز��ʲ���
	void load_material(const char* pstrfile,ITerrain* pTerrain,IScene* pScene);
	//���ػ�׼������ʽ
	void load_base_format(const char* pstrfile,ITerrain* pTerrain,IScene* pScene);
	//���صر���ʽ����
	void load_format(const char* pstrfile,ITerrain* pTerrain,IScene* pScene);
	//��������ϵͳ
	void load_particle_system(const char* pstrfile,ITerrain* pTerrain,IScene* pScene);
	//����ˮ��
	bool load_water(const char* pstrfile,ITerrain* pTerrain,IScene* pScene);
    //���ز�
	bool load_grass(const char* pstrfile,ITerrain* pTerrain,IScene* pScene);
	//��������
	void load_weather(const char* pstrfile,ITerrain* pTerrain,IScene* pScene);
	//������պ�
	void load_skybox(IIniFile* pIni,ITerrain* pTerrain,IScene* pScene);
	//����̫�����
	void load_sun(IIniFile* pIni,ITerrain* pTerrain,IScene* pScene);
	//����Ӱ�ӹ�����
	void load_shadow_system(ITerrain* pTerrain,IScene* pScene);
	//����һ��NPC
	void load_npc();

	//��ȡ������Ϣ
	result_string read_color_ini(IIniFile* pIni,const char* section_name,const char* key_name);	

	//��ȡˮ����Ϣ
	void read_water_data(IIniFile* pIni,PERSISTID water_id);
	void updata_seadata(PERSISTID water_id);

	//��ȡ����Ϣ 
	void read_grass_data(IIniFile* pIni,PERSISTID grass_id);
private:
	//����ȫ��Ч������
	void load_screen_config(const char* pstrfile,ITerrain* pTerrain,IScene* pScene);

    //���غ���Ч��
	void load_post_effect(const char* pstrfile,ITerrain* pTerrain,IScene* pScene);

	//���غ���Ч��
	void load_ppfilter(const char* pstrfile,ITerrain* pTerrain,IScene* pScene);

	//���ض�̬����ϵͳ
	void load_dynamic_weather_system(const char* pstrfile,ITerrain* pTerrain,IScene* pScene);
    ITerrain* m_pTerrain;
	IScene*   m_pScene;
	IWorld*   m_pWorld;
	IShadowManager* m_pShadowManager;
	IInputManager* m_InputManager;

	IActor* m_pRaleNPC;

	IRender* m_pRender;

	std::vector<PERSISTID> m_vecSceneObjEnt;//�ӵ��������ʵ���б�
}; 



#endif