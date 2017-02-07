#pragma once

/*
������ش���
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
    //���ز��ʲ���
	void load_material(const char* pstrfile,ITerrain* pTerrain,IScene* pScene);
	//���صر���ʽ����
	void load_format(const char* pstrfile,ITerrain* pTerrain,IScene* pScene);
	//���صر�
	bool load_terrain(const char* pstrfile,IScene* pScene, bool can_load_skybox = true);
	//����ˮ��
	bool load_water(const char* pstrfile,ITerrain* pTerrain,IScene* pScene);
    //���ز�
	bool load_grass(const char* pstrfile,ITerrain* pTerrain,IScene* pScene);
	//��������
	void load_weather(const char* pstrfile,ITerrain* pTerrain,IScene* pScene, bool can_load_skybox = true);
	//������պ�
	void load_skybox(IIniFile* pIni,ITerrain* pTerrain,IScene* pScene);
	//����̫�����
	void load_sun(IIniFile* pIni,ITerrain* pTerrain,IScene* pScene);
   
	//��ȡ������Ϣ
	result_string read_color_ini(IIniFile* pIni,const char* section_name,const char* key_name);
	

	//��ȡˮ����Ϣ
	void read_water_data(IIniFile* pIni,PERSISTID water_id);
	void updata_seadata(PERSISTID water_id);

	//��ȡ����Ϣ
	void read_grass_data(IIniFile* pIni,PERSISTID grass_id);
private:
    //���غ���Ч��
	void load_post_effect(const char* pstrfile,ITerrain* pTerrain,IScene* pScene);

	//���غ���Ч��
  void load_ppfilter(const char* pstrfile,ITerrain* pTerrain,IScene* pScene);

  //���ض�̬����ϵͳ
  void load_dynamic_weather_system(const char* szFileName, ITerrain* pTerrain, IScene* pScene);

private:
  std::vector<PERSISTID> m_vecSceneObjEnt;//�ӵ��������ʵ���б�
}; 
