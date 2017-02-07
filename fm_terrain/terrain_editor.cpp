//--------------------------------------------------------------------
// �ļ���:		terrain_editor.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2007��8��17��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "terrain_editor.h"
#include "terrain.h"
#include "terrain_zone.h"
#include "terrain_height.h"
#include "terrain_light.h"
#include "terrain_chunk.h"
#include "terrain_visuals.h"
#include "terrain_water.h"
#include "terrain_grass.h"
#include "terrain_walkable.h"
#include "terrain_region.h"
#include "quad_visual.h"
#include "zone_manager.h"
#include "height_loader.h"
#include "light_loader.h"
#include "alpha_loader.h"
#include "visual_loader.h"
#include "walk_loader.h"
#include "walk_generator.h"
#include "region_loader.h"
#include "sea_water.h"
#include "blend_tex.h"
#include "blend_tex_set.h"
#include "tex_alpha_map.h"
#include "tex_light_map.h"
#include "block_file.h"
#include "block_height.h"
#include "block_height2.h"
#include "block_chunk.h"
#include "grass_type.h"
#include "../utils/array_pod.h"
#include "../visual/vis_utils.h"
#include "../visual/i_texture.h"
#include "../visual/i_image_access.h"
#include "../public/var.h"
#include "../public/i_ini_file.h"
#include "../public/auto_mem.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/core_file.h"
#include "../visual/i_context.h"

/// \file terrain_editor.cpp
/// \brief ���α༭

/// entity: TerrainEditor
/// \brief ���α༭ʵ��
DECLARE_ENTITY(TerrainEditor, 0, IEntity);

/*
/// method: int GetZoneIndex(float x, float z)
/// \brief ���ָ��λ����������������
/// \param x,z ����
DECLARE_METHOD_2(int, TerrainEditor, GetZoneIndex, float, float);
/// method: int GetZoneRow(int zone_index)
/// \brief ��������������ڵ�����
/// \param zone_index ��������
DECLARE_METHOD_1(int, TerrainEditor, GetZoneRow, int);
/// method: int GetZoneCol(int zone_index)
/// \brief ��������������ڵ�����
/// \param zone_index ��������
DECLARE_METHOD_1(int, TerrainEditor, GetZoneCol, int);
/// method: string GetZoneName(int zone_index)
/// \brief ���ָ���������Դ����
/// \param zone_index ��������
DECLARE_METHOD_1(const char*, TerrainEditor, GetZoneName, int);
/// method: float GetZoneMinY(int zone_index)
/// \brief ���ָ���������͵�߶�
/// \param zone_index ��������
DECLARE_METHOD_1(float, TerrainEditor, GetZoneMinY, int);
/// method: float GetZoneMaxY(int zone_index)
/// \brief ���ָ���������ߵ�߶�
/// \param zone_index ��������
DECLARE_METHOD_1(float, TerrainEditor, GetZoneMaxY, int);
/// method: table GetZoneList()
/// \brief ����ڴ������е�����������
DECLARE_METHOD_T(TerrainEditor, GetZoneList);
/// method: table GetRangeZoneList(float beg_x, float beg_z, float end_x, float end_z)
/// \brief ����ָ�����귶Χ����������������
/// \param beg_x,beg_z ��ʼ����
/// \param end_x,end_z ��ֹ����
DECLARE_METHOD_T(TerrainEditor, GetRangeZoneList);
/// method: bool GetZoneExists(int zone_index)
/// \brief ���ָ������ĸ߶��ļ��Ƿ����
DECLARE_METHOD_1(bool, TerrainEditor, GetZoneExists, int);
/// method: table GetExistZoneList()
/// \brief ��ô��ڸ߶��ļ�������������
DECLARE_METHOD_T(TerrainEditor, GetExistZoneList);
/// method: bool SetZoneChanged(int zone_index, string type, bool value)
/// \brief ����ָ������ı༭��Ŀ�ı�״̬
/// \param zone_index ��������
/// \param type �༭��Ŀ��"height", "alpha", "visual"��
/// \param value ״ֵ̬
DECLARE_METHOD_3(bool, TerrainEditor, SetZoneChanged, int, const char*, bool);
/// method: bool GetZoneChanged(int zone_index, string type)
/// \brief ���ָ������ı༭��Ŀ�ı�״̬
/// \param zone_index ��������
/// \param type �༭��Ŀ��"height", "alpha", "visual"��
DECLARE_METHOD_2(bool, TerrainEditor, GetZoneChanged, int, const char*);
/// method: bool SetZoneNoUnload(int zone_index, bool value)
/// \brief ����ָ������ж��
/// \param zone_index ��������
/// \param value ״ֵ̬
DECLARE_METHOD_2(bool, TerrainEditor, SetZoneNoUnload, int, bool);
/// method: bool GetZoneNoUnload(int zone_index)
/// \brief ���ָ�������Ƿ�ж��
/// \param zone_index ��������
DECLARE_METHOD_1(bool, TerrainEditor, GetZoneNoUnload, int);
/// method: bool GetInDesign(float x, float z)
/// \brief ָ���ĵ��Ƿ��ڿɱ༭��Χ֮��
/// \param x,z ����
DECLARE_METHOD_2(bool, TerrainEditor, GetInDesign, float, float);

/// method: [min_x, min_z, max_x, max_z] GetZoneRect(int zone_index)
/// \brief ����������������ƽ�淶Χ
/// \param zone_index ��������
DECLARE_METHOD_N(TerrainEditor, GetZoneRect);
/// method: [min_x, min_z, max_x, max_z] GetVisualRect(object visual)
/// \brief ��������ƽ�淶Χ
/// \param visual ��������
DECLARE_METHOD_N(TerrainEditor, GetVisualRect);
/// method: table GetRectLightList(float min_x, float min_z, float max_x, float max_z)
/// \brief ���ƽ�淶Χ�ڵ����е��Դ
/// \param min_x,min_z,max_x,max_z ��Χ
DECLARE_METHOD_T(TerrainEditor, GetRectLightList);

/// method: bool SetShowChunkBoundBox(float x, float z, bool value)
/// \brief ����ָ����λ���Ƿ���ʾ�ֿ�İ�Χ��
/// \param x,z ����
/// \param value �Ƿ���ʾ
DECLARE_METHOD_3(bool, TerrainEditor, SetShowChunkBoundBox, float, float, 
	bool);

/// method: bool SetPosiY(float x, float z, float y)
/// \brief ��������λ�ø߶�
/// \param x,z ����
/// \param y �߶�ֵ
DECLARE_METHOD_3(bool, TerrainEditor, SetPosiY, float, float, float);
/// method: bool IncPosiY(float x, float z, float inc_y)
/// \brief ���ӻ��������λ�ø߶�
/// \param x,z ����
/// \param inc_y �߶�ֵ
DECLARE_METHOD_3(bool, TerrainEditor, IncPosiY, float, float, float);
/// method: bool SetHeightChanged(x, z)
/// \brief ��ĳ��߶ȷ����仯�����»��Ƶ���
/// \param x,z ����
DECLARE_METHOD_2(bool, TerrainEditor, SetHeightChanged, float, float);
/// method: bool RangeHeightChanged(float beg_x, float beg_z, float end_x, float end_z)
/// \brief ��ָ����Χ�ڵĸ߶ȷ����仯�����»��Ƶ���
/// \param beg_x,beg_z ��ʼ����
/// \param end_x,end_z ��ֹ����
DECLARE_METHOD_4(bool, TerrainEditor, RangeHeightChanged, float, float, 
	float, float);

/// method: int GetTexBase(float x, float z)
/// \brief ���ָ����Ļ�����ͼ����
/// \param x,z ����
DECLARE_METHOD_2(int, TerrainEditor, GetTexBase, float, float);
/// method: bool SetTexBase(float x, float z, int tex_index)
/// \brief ����ָ����Ļ�����ͼ����
/// \param x,z ����
/// \param tex_index ������ͼ����
DECLARE_METHOD_3(bool, TerrainEditor, SetTexBase, float, float, int);
/// method: float GetTexBlend(float x, float z, int tex_index, int format)
/// \brief ��û����ͼ��ǰֵ
/// \param x,z ����
/// \param tex_index �����ͼ����
/// \param format ��ͼ��ʽ
DECLARE_METHOD_4(float, TerrainEditor, GetTexBlend, float, float, int, int);
/// method: [format, value] GetTexBlendPara(float x, float z, int tex_index)
/// \brief ��û����ͼ�ڵ�ǰλ�õ����ǰֵ
/// \param x,z ����
/// \param tex_index �����ͼ����
DECLARE_METHOD_N(TerrainEditor, GetTexBlendPara);
/// method: [tex_index, format, value] GetMaxBlendPara(float x, float z)
/// \brief ��õ�ǰλ�����ֵ�Ļ����ͼ����
/// \param x,z ����
DECLARE_METHOD_N(TerrainEditor, GetMaxBlendPara);
/// method: int GetTexBlendLayerCount(float x, float z)
/// \brief ���ָ��λ�õĻ����ͼ����
/// \param x,z ����
DECLARE_METHOD_2(int, TerrainEditor, GetTexBlendLayerCount, float, float);
/// method: int GetTexBlendLayerTexIndex(float x, float z, int layer)
/// \brief ���ָ��λ�õĻ����ͼ�����ͼ����
/// \param x,z ����
/// \param layer ������
DECLARE_METHOD_3(int, TerrainEditor, GetTexBlendLayerTexIndex, float, float, 
	int);
/// method: int GetTexBlendLayerFormat(float x, float z, int layer)
/// \brief ���ָ��λ�õĻ����ͼ�����ʽ
/// \param x,z ����
/// \param layer ������
DECLARE_METHOD_3(int, TerrainEditor, GetTexBlendLayerFormat, float, float, 
	int);
/// method: int CopyTexBlendLayer(float x, float z, int tex_index, int format, string tex_name)
/// \brief ��ָ��λ�õĻ����ͼALPHAMAP���Ƶ�ָ������ͼ��
/// \param x,z ����
/// \param tex_index �����ͼ����
/// \param format ��ʽ
/// \param tex_name ��ͼ�ļ���
DECLARE_METHOD_5(bool, TerrainEditor, CopyTexBlendLayer, float, float, int, 
	int, const char*);
/// method: int GetTexBlendLayerTexIndex(float x, float z, int tex_index, int format)
/// \brief ����ָ��λ�õĻ����ͼ��
/// \param x,z ����
/// \param tex_index �����ͼ����
/// \param format ��ʽ
DECLARE_METHOD_4(int, TerrainEditor, EraseTexBlendLayer, float, float, int, 
	int);
/// method: bool FindTexBlendLayer(float x, float z, int tex_index, int format)
/// \brief ��ָ��λ�ò���ָ�������ͼ��
/// \param x,z ����
/// \param tex_index �����ͼ����
/// \param format ��ͼ��ʽ
DECLARE_METHOD_4(bool, TerrainEditor, FindTexBlendLayer, float, float, int, 
	int);
/// method: bool SetTexBlend(float x, float z, int tex_index, int format, float streng)
/// \brief ����ָ����Ļ����ͼ����
/// \param x,z ����
/// \param tex_index �����ͼ����
/// \param format ��ͼ��ʽ
/// \param strength ���ǿ��
DECLARE_METHOD_5(bool, TerrainEditor, SetTexBlend, float, float, int, int, 
	float);
/// method: bool EraseTexBlend(float x, float z, tex_index)
/// \brief ����ָ����Ļ����ͼ
/// \param x,z ����
/// \param tex_index �����ͼ����
DECLARE_METHOD_3(bool, TerrainEditor, EraseTexBlend, float, float, int);
/// method: bool EraseAllBlend(float x, float z)
/// \brief ����ָ��������л����ͼ
/// \param x,z ����
DECLARE_METHOD_2(bool, TerrainEditor, EraseAllBlend, float, float);
/// method: bool SetBlendChanged(float x, float z)
/// \brief ĳ�����ͼ��Ϸ����仯
/// \param x,z ����
DECLARE_METHOD_2(bool, TerrainEditor, SetBlendChanged, float, float);
/// method: bool RangeBlendChanged(float beg_x, float beg_z, float end_x, float end_z)
/// \brief ��ָ����Χ�ڵĻ����ͼ�����仯�����»��Ƶ���
/// \param beg_x,beg_z ��ʼ����
/// \param end_x,end_z ��ֹ����
DECLARE_METHOD_4(bool, TerrainEditor, RangeBlendChanged, float, float, float, 
	float);

/// method: object GetZoneVisual(int zone_index, string name)
/// \brief ���ָ�������ڵ�����������
/// \param zone_index ��������
/// \param name ����������
DECLARE_METHOD_2(PERSISTID, TerrainEditor, GetZoneVisual, int, const char*);
/// method: string GetZoneVisualName(int zone_index, object obj)
/// \brief ���ָ�������ڵ�����������
/// \param zone_index ��������
/// \param obj �����������
DECLARE_METHOD_2(const char*, TerrainEditor, GetZoneVisualName, int, 
	const PERSISTID&);
/// method: table GetZoneVisualList(int zone_index)
/// \brief ���ָ�����������еĵ�������
/// \param zone_index ��������
DECLARE_METHOD_T(TerrainEditor, GetZoneVisualList);
/// method: table GetRectVisualList(float beg_x, float beg_z, float end_x, float end_z)
/// \brief ���ָ����Χ�����еĵ�������
/// \param beg_x,beg_z ��ʼ����
/// \param end_x,end_z ��ֹ����
DECLARE_METHOD_T(TerrainEditor, GetRectVisualList);
/// method: table GetScreenVisualList(float x1, float y1, float x2, float y2, float distance)
/// \brief �����Ļ�ռ�ָ����Χ�ڵ����е�������
/// \param x1,y1 ��Ļ��ʼλ��
/// \param x2,y2 ��Ļ��ֹλ��
/// \param distance ��Զ����
DECLARE_METHOD_T(TerrainEditor, GetScreenVisualList);

/// method: bool GetWater(float x, float z)
/// \brief ���ˮ������
/// \param x,z ����
DECLARE_METHOD_2(int, TerrainEditor, GetWater, float, float);
/// method: bool SetWater(float x, float z, int water_index)
/// \brief ����ˮ������
/// \param x,z ����
/// \param water_index ˮ����������
DECLARE_METHOD_3(bool, TerrainEditor, SetWater, float, float, int);
/// method: bool TestWater(float x, float z, int water_index)
/// \brief �����Ƿ���Ҫ����ˮ��
/// \param x,z ����
/// \param water_index ˮ����������
DECLARE_METHOD_3(bool, TerrainEditor, TestWater, float, float, int);

/// method: [scale, amount, seed] GetGrass(int grass_index, float x, float z)
/// \brief ���ָ��λ�õĻ�������
/// \param grass_index ������������
/// \param x,z ����
/// \return scale, amount, seef ���ݲ���
DECLARE_METHOD_N(TerrainEditor, GetGrass);
/// method: bool AddGrass(int grass_index, float x, float z,
///		int scale, int amount, int seed)
/// \brief ��ӻ���
/// \param grass_index ������������
/// \param x,z ����
/// \param scale ��Χ(1-10)
/// \param amount ����(1-15)
/// \param seed �������
DECLARE_METHOD_6(bool, TerrainEditor, AddGrass, int, float, float, int, int, 
	int);
/// method: bool RemoveGrass(int grass_index, float x, float z)
/// \brief ɾ��ָ��λ�õĻ���
/// \param grass_index ������������
/// \param x,z ����
DECLARE_METHOD_3(bool, TerrainEditor, RemoveGrass, int, float, float);
/// method: int RemoveGrassRect(int grass_index, float x1, float z1, float x2, float z2)
/// \brief ɾ�����η�Χ�ڵĻ���
/// \param grass_index ������������
/// \param x1,z1 ��ʼ����
/// \param x2,z2 ��ֹ����
DECLARE_METHOD_5(int, TerrainEditor, RemoveGrassRect, int, float, float, 
	float, float);
/// method: int RemoveGrassCircle(int grass_index, float x, float z, float radius)
/// \brief ɾ��Բ�η�Χ�ڵĻ���
/// \param grass_index ������������
/// \param x,z ��������
/// \param radius �뾶
DECLARE_METHOD_4(int, TerrainEditor, RemoveGrassCircle, int, float, float, 
	float);
/// method: int GetGrassCountRect(int grass_index, float x1, float z1, float x2, float z2)
/// \brief ��÷��η�Χ�ڵĻ�������
/// \param grass_index ������������
/// \param x1,z1 ��ʼ����
/// \param x2,z2 ��ֹ����
DECLARE_METHOD_5(int, TerrainEditor, GetGrassCountRect, int, float, float, 
	float, float);
/// method: int GetGrassCountCircle(int grass_index, float x, float z, float radius)
/// \brief ���Բ�η�Χ�ڵĻ�������
/// \param grass_index ������������
/// \param x,z ��������
/// \param radius �뾶
DECLARE_METHOD_4(int, TerrainEditor, GetGrassCountCircle, int, float, 
	float, float);
/// method: table GetGrassRect(float x1, float z1, float x2, float z2)
/// \brief ��÷��η�Χ�ڵĻ�������
/// \param x1,z1 ��ʼ����
/// \param x2,z2 ��ֹ����
/// \return index table
DECLARE_METHOD_T(TerrainEditor, GetGrassRect);

/// method: table GetGrassCircle(float x, float z, float radius)
/// \brief ���Բ�η�Χ�ڵĻ�������
/// \param x,z ��������
/// \param radius �뾶
/// \return index table
DECLARE_METHOD_T(TerrainEditor, GetGrassCircle);

/// method: bool SetBlendColor(float x, float z, int alpha, int red, int green, int blue)
/// \brief ���õر�����ɫ
/// \param x,z ����
/// \param alpha ͸����
/// \param red ��ɫ
/// \param green ��ɫ
/// \param blue ��ɫ
//DECLARE_METHOD_6(bool, TerrainEditor, SetBlendColor, float, float, int, int, 
//	int, int);
/// method: [alpha, red, green, blue] GetBlendColor(float x, float z)
/// \brief ��õر�����ɫ
/// \param x,z ����
//DECLARE_METHOD_N(TerrainEditor, GetBlendColor);

/// method: bool SetRegionColor(string area, string color)
/// \brief ���÷�Χ�����ɫ
/// \param area ��Χ����
/// \param color ��ɫֵ
DECLARE_METHOD_2(bool, TerrainEditor, SetRegionColor, const char*, const char*);
/// method: string GetRegionColor(string area)
/// \brief ��÷�Χ�����ɫ
/// \param area ��Χ����
DECLARE_METHOD_1(result_string, TerrainEditor, GetRegionColor, const char*);
/// method: bool SetRegionEnable(string area, float x, float z, bool value)
/// \brief ����ָ��λ�õķ�Χ���ֵ
/// \param area ��Χ����
/// \param x,z ����
/// \param value ���ֵ
DECLARE_METHOD_4(bool, TerrainEditor, SetRegionEnable, const char*, float, 
	float, bool);
/// method: bool ChangeRegionName(float x, float z, string ori_area, 
///		string new_area)
/// \brief �ı䷶Χ���ƣ��༭���ã�
/// \param x,z ����
/// \param ori_area ԭ��Χ����
/// \param new_area �·�Χ����
DECLARE_METHOD_4(bool, TerrainEditor, ChangeRegionName, float, float, 
	const char*, const char*);

/// method: bool SetAreaColor(string area, string color)
/// \brief ���õ��������ɫ
/// \param area ��������
/// \param color ��ɫֵ
DECLARE_METHOD_2(bool, TerrainEditor, SetAreaColor, const char*, const char*);
/// method: string GetAreaColor(string area)
/// \brief ��õ��������ɫ
/// \param area ��������
DECLARE_METHOD_1(result_string, TerrainEditor, GetAreaColor, const char*);
/// method: bool SetAreaEnable(string area, float x, float z, bool value)
/// \brief ����ָ��λ�õĵ������ֵ
/// \param area ��������
/// \param x,z ����
/// \param value ���ֵ
DECLARE_METHOD_4(bool, TerrainEditor, SetAreaEnable, const char*, float, 
	float, bool);
/// method: bool ClearAreaEnable(float x, float z)
/// \brief ���ָ��λ�õ����е������ֵ
/// \param x,z ����
DECLARE_METHOD_2(bool, TerrainEditor, ClearAreaEnable, float, float);
/// method: bool ChangeAreaName(float x, float z, string ori_area, 
///		string new_area)
/// \brief �ı����������ƣ��༭���ã�
/// \param x,z ����
/// \param ori_area ԭ�����������
/// \param new_area �µ����������
DECLARE_METHOD_4(bool, TerrainEditor, ChangeAreaName, float, float, 
	const char*, const char*);
/// method: bool ChangeAreaNameTable(string ori_area, string new_area)
/// \brief �ı����������Ʊ��༭���ã�ȫ����ChangeAreaName����ã�
/// \param ori_area ԭ�����������
/// \param new_area �µ����������
DECLARE_METHOD_2(bool, TerrainEditor, ChangeAreaNameTable, const char*, 
	const char*);
/// method: bool RemoveAreaColor(const char* name)
/// \brief ɾ��ָ����������ɫ��Ϣ���༭���ã�
/// \param name �����������
DECLARE_METHOD_1(bool, TerrainEditor, RemoveAreaColor, const char*);

/// method: bool SaveHeight(int zone_index, string file_name)
/// \brief ����ָ������ĸ߶�����
/// \param zone_index ��������
/// \param file_name �ļ���
DECLARE_METHOD_2(bool, TerrainEditor, SaveHeight, int, const char*);
/// method: bool SaveHeightCompress(int zone_index, string file_name)
/// \brief ����ѹ������ָ������ĸ߶�����
/// \param zone_index ��������
/// \param file_name �ļ���
DECLARE_METHOD_2(bool, TerrainEditor, SaveHeightCompress, int, const char*);

/// method: bool SaveWater(int zone_index, string file_name)
/// \brief ����ָ�������ˮ�������Ϣ
/// \param zone_index ��������
/// \param file_name �ļ���
DECLARE_METHOD_2(bool, TerrainEditor, SaveWater, int, const char*);
/// method: bool SaveDiffuseMap(int zone_index, string file_name)
/// \brief ����ָ������Ķ������ͼ
/// \param zone_index ��������
/// \param file_name �ļ���
//DECLARE_METHOD_2(bool, TerrainEditor, SaveDiffuseMap, int, const char*);
/// method: bool SaveAlphaBase(int zone_index, string file_name)
/// \brief ����ָ������Ļ�����ͼ��Ϣ
/// \param zone_index ��������
/// \param file_name �ļ���
DECLARE_METHOD_2(bool, TerrainEditor, SaveAlphaBase, int, const char*);

/// method: bool SaveAlphaMap(int zone_index, string file_name)
/// \brief ����ָ����������ջ����ͼ����
/// \param zone_index ��������
/// \param file_name �ļ���
DECLARE_METHOD_2(bool, TerrainEditor, SaveAlphaMap, int, const char*);
/// method: bool SaveVisuals(int zone_index, string file_name, 
///		string widget_file, string helper_file, string collide_file, 
///		string desc_file, string sound_file)
/// \brief ����ָ������ĵ���������Ϣ
/// \param zone_index ��������
/// \param file_name �ļ���
/// \param widget_file װ�����ļ���
/// \param helper_file ���������ļ���
/// \param collide_file ��ײ�����ļ���
/// \param desc_file �������������ļ���
/// \param sound_file ���������ļ���
DECLARE_METHOD_7(bool, TerrainEditor, SaveVisuals, int, const char*, 
	const char*, const char*, const char*, const char*, const char*);
/// method: bool SaveGrass(int zone_index, string file_name)
/// \brief ����ָ������Ļ�����Ϣ
/// \param zone_index ��������
/// \param file_name �ļ���
DECLARE_METHOD_2(bool, TerrainEditor, SaveGrass, int, const char*);
/// method: bool SaveWalk(int zone_index, string file_name)
/// \brief ����ָ�������������Ϣ
/// \param zone_index ��������
/// \param file_name �ļ���
DECLARE_METHOD_2(bool, TerrainEditor, SaveWalk, int, const char*);
/// method: bool SaveWalkMap(int zone_index, string file_name)
/// \brief ����ָ������Ŀ�����ͼ
/// \param zone_index ��������
/// \param file_name �ļ���
DECLARE_METHOD_2(bool, TerrainEditor, SaveWalkMap, int, const char*);
/// method: bool SaveBlendColors(int zone_index, string file_name)
/// \brief ����ָ������ĵر�����ɫͼ
/// \param zone_index ��������
/// \param file_name �ļ���
//DECLARE_METHOD_2(bool, TerrainEditor, SaveBlendColors, int, const char*);
/// method: bool SaveLightMap(int zone_index, string file_name)
/// \brief ����ָ������ĵر����ͼ
/// \param zone_index ��������
/// \param file_name �ļ���
//DECLARE_METHOD_2(bool, TerrainEditor, SaveLightMap, int, const char*);
/// method: bool SaveRegion(int zone_index, string file_name)
/// \brief ����ָ������ķ�Χ�����Ϣ
/// \param zone_index ��������
/// \param file_name �ļ���
DECLARE_METHOD_2(bool, TerrainEditor, SaveRegion, int, const char*);
/// method: bool SaveArea(int zone_index, string file_name)
/// \brief ����ָ������ĵ��������Ϣ
/// \param zone_index ��������
/// \param file_name �ļ���
DECLARE_METHOD_2(bool, TerrainEditor, SaveArea, int, const char*);

/// method: bool CompressZoneHeight(string src_file, string dst_file)
/// \brief ѹ��ָ������ĸ߶������ļ�
/// \param src_file Դ�ļ�
/// \param dst_file Ŀ���ļ�
DECLARE_METHOD_2(bool, TerrainEditor, CompressZoneHeight, const char*, 
	const char*);

/// method: bool GenWalkAddPermit(int zone_index, float x, float y, float z)
/// \brief �������������ײ���߲��
/// \param zone_index ��������
/// \param x,y,z λ��
DECLARE_METHOD_4(bool, TerrainEditor, GenWalkAddPermit, int, float, float, 
	float);
/// method: bool GenWalkAddForbid(int zone_index, float x, float y, float z, float radius)
/// \brief ��ӽ�ֹ������ײ���߲�ķ�Χ
/// \param zone_index ��������
/// \param x,y,z λ��
/// \param radius ��Χ
DECLARE_METHOD_5(bool, TerrainEditor, GenWalkAddForbid, int, float, float, 
	float, float);
/// method: bool GenWalkAddFence(int fence_index, float x, float z)
/// \brief ��ӳ�����Ч��Χդ����
/// \param fence_index դ������
/// \param x,z ��λ��
DECLARE_METHOD_3(bool, TerrainEditor, GenWalkAddFence, int, float, float);
/// method: bool GenWalkBegin(int zone_index, int precision, float role_height, 
///		float role_radius, float step_height, float trace_high, float trace_low, 
///		float ground_normal_y, float floor_normal_y, float wall_angle)
/// \brief ��ʼ������ײ��Ϣ
/// \param zone_index ��������
/// \param precision ����(1,2,4)
/// \param role_height ��ɫ�߶�
/// \param role_radius ��ɫ�뾶
/// \param step_height �����߸߶Ȳ�
/// \param trace_high ��ײ�����ߵ�
/// \param trace_low ��ײ�����͵�
/// \param ground_normal_y �ر�ƽ̹��
/// \param floor_normal_y ����ƽ̹��
/// \param wall_angle ǽ�������б�Ƕ�
DECLARE_METHOD_N(TerrainEditor, GenWalkBegin);
//DECLARE_METHOD_9(bool, TerrainEditor, GenWalkBegin, int, float, float, 
//	float, float, float, float, float, int);
/// method: bool GenWalkGround(int zone_index)
/// \brief ���ɵر�߶�
/// \param zone_index ��������
DECLARE_METHOD_1(bool, TerrainEditor, GenWalkGround, int);
/// method: bool GenWalkBalk(int zone_index)
/// \brief �����ϰ��߶�
/// \param zone_index ��������
/// \param row ������
DECLARE_METHOD_2(bool, TerrainEditor, GenWalkBalk, int, int);
/// method: bool GenWalkable(int zone_index, float start_x, float start_y, float start_z)
/// \brief ���ɿ�����������Ϣ
/// \param zone_index ��������
/// \param start_x,start_y,start_z ��ʼλ��
DECLARE_METHOD_4(bool, TerrainEditor, GenWalkable, int, float, float, float);
/// method: bool GenWalkFloor(int zone_index)
/// \brief ���ɿ����߲���Ϣ
/// \param zone_index ��������
DECLARE_METHOD_1(bool, TerrainEditor, GenWalkFloor, int);
/// method: bool GenWalkWall(int zone_index)
/// \brief ����ǽ�ڱ����Ϣ
/// \param zone_index ��������
DECLARE_METHOD_1(bool, TerrainEditor, GenWalkWall, int);
/// method: bool GenWalkWater(int zone_index)
/// \brief ���ɿ�����ˮ����Ϣ
/// \param zone_index ��������
DECLARE_METHOD_1(bool, TerrainEditor, GenWalkWater, int);
/// method: bool GenWalkEnd(int zone_index)
/// \brief ����������ײ��Ϣ
/// \param zone_index ��������
DECLARE_METHOD_1(bool, TerrainEditor, GenWalkEnd, int);

/// method: bool GenLightMapAtlas(int zone_index, int atlas_map_size, int small_file_size)
/// \brief �ϲ�ָ�������С���ģ�͹���ͼ
/// \param zone_index ��������
/// \param atlas_map_size �ϲ������ͼ�ߴ�
/// \param min_light_map_size ��С�Ĺ���ͼ�ߴ�
/// \param small_file_size С�ڴ˴�С�Ĺ���ͼ�ļ�����Ϊ��С�����ͼ
//DECLARE_METHOD_4(bool, TerrainEditor, GenLightMapAtlas, int, int, int, int);
*/
// TerrainEditor

// ����ȫ�ֿ���
bool set_global_on_off(ICore* pCore, const char* name, bool value)
{
	return pCore->SetGlobalValue(name, CVar(VTYPE_BOOL, value));
}

TerrainEditor::TerrainEditor()
{
	m_pTerrain = NULL;
	m_pZoneManager = NULL;
	m_pWalkGenerator = NULL;
	m_Height.pValues = NULL;
	m_Height.pNormals = NULL;
	m_Chunk.pChunks = NULL;
}

TerrainEditor::~TerrainEditor()
{
	if (m_pWalkGenerator)
	{
		CORE_DELETE(m_pWalkGenerator);
	}
	
	if (m_Height.pValues)
	{
		size_t dims = m_pTerrain->GetZoneScale() + 1;

		CORE_FREE(m_Height.pValues, sizeof(float) * dims * dims);
	}

	if (m_Height.pNormals)
	{
		size_t dims = m_pTerrain->GetZoneScale() + 1;

		CORE_FREE(m_Height.pNormals, sizeof(unsigned int) * dims * dims);
	}

	if (m_Chunk.pChunks)
	{
		size_t chunk_dims = 
			m_pTerrain->GetZoneScale() / m_pTerrain->GetChunkScale();
		size_t chunk_num = chunk_dims * chunk_dims;

		CORE_FREE(m_Chunk.pChunks, 
			sizeof(CHeightLoader::chunk_info_t) * chunk_num);
	}
}

bool TerrainEditor::Init(const IVarList& args)
{
	return true;
}

bool TerrainEditor::Shut()
{
	return true;
}

void TerrainEditor::SetTerrain(Terrain* value)
{ 
	m_pTerrain = value; 
	m_pZoneManager = m_pTerrain->GetZoneManager();
}

CTerrainZone* TerrainEditor::InnerGetZone(int zone_index)
{
	if (size_t(zone_index) >= m_pZoneManager->GetZoneNum())
	{
		return NULL;
	}
	
	return m_pZoneManager->GetZoneByIndex(zone_index);
}

int TerrainEditor::GetZoneIndex(float x, float z)
{
	CTerrainZone* zone = m_pZoneManager->GetInZone(x, z);
	
	if (NULL == zone)
	{
		return -1;
	}
	
	return (int)zone->GetIndex();
}

int TerrainEditor::GetZoneRow(int zone_index)
{
	return (int)(zone_index / m_pZoneManager->GetCols());
}

int TerrainEditor::GetZoneCol(int zone_index)
{
	return (int)(zone_index % m_pZoneManager->GetCols());
}

const char* TerrainEditor::GetZoneName(int zone_index)
{
	CTerrainZone* pZone = InnerGetZone(zone_index);

	if (NULL == pZone)
	{
		return "";
	}

	return pZone->GetName();
}

float TerrainEditor::GetZoneMinY(int zone_index)
{
	CTerrainZone* pZone = InnerGetZone(zone_index);

	if (NULL == pZone)
	{
		return 0.0F;
	}

	if (!pZone->GetHeightLoaded())
	{
		return 0.0F;
	}

	return pZone->GetHeight()->GetMinY();
}

float TerrainEditor::GetZoneMaxY(int zone_index)
{
	CTerrainZone* pZone = InnerGetZone(zone_index);

	if (NULL == pZone)
	{
		return 0.0F;
	}

	if (!pZone->GetHeightLoaded())
	{
		return 0.0F;
	}

	return pZone->GetHeight()->GetMaxY();
}

void TerrainEditor::GetZoneList(const IVarList& args, IVarList& result)
{
	result.Clear();

	size_t zone_num = m_pZoneManager->GetZoneNum();

	for (size_t i = 0; i < zone_num; ++i)
	{
		CTerrainZone* pZone = m_pZoneManager->GetZoneByIndex(i);

		if (pZone != NULL)
		{
			result.AddInt((int)pZone->GetIndex());
		}
	}
}

static bool find_vlist_int(const IVarList& vl, int value)
{
	size_t size = vl.GetCount();

	for (size_t i = 0; i < size; ++i)
	{
		if (vl.IntVal(i) == value)
		{
			return true;
		}
	}

	return false;
}

void TerrainEditor::GetRangeZoneList(const IVarList& args, IVarList& result)
{
	result.Clear();

	float beg_x = args.FloatVal(0);
	float beg_z = args.FloatVal(1);
	float end_x = args.FloatVal(2);
	float end_z = args.FloatVal(3);
	float step_x = m_pTerrain->GetZoneWidth();
	float step_z = m_pTerrain->GetZoneHeight();
	float z = beg_z;
	
	while (z < (end_z + step_z))
	{
		float x = beg_x;
		
		while (x < (end_x + step_x))
		{
			float px = x;
			float pz = z;

			if (px > end_x)
			{
				px = end_x;
			}

			if (pz > end_z)
			{
				pz = end_z;
			}
			
			CTerrainZone* pZone = m_pZoneManager->GetInZone(px, pz);
			
			if (pZone != NULL)
			{
				if (!find_vlist_int(result, (int)pZone->GetIndex()))
				{
					result.AddInt((int)pZone->GetIndex());
				}
			}
			
			x += step_x;
		}
		
		z += step_z;
	}
}

// �ļ��Ƿ����
static bool file_exists(const char* name)
{
	return g_pCore->GetFileSys()->FileExists(name);
}

bool TerrainEditor::GetZoneExists(int zone_index)
{
	size_t zone_cols = m_pZoneManager->GetCols();
	size_t r = size_t(zone_index) / zone_cols;
	size_t c = size_t(zone_index) - r * zone_cols;

	// �߶��ļ��Ƿ����
	char fname[256];

	SafeSprintf(fname, sizeof(fname), "%szone_%d_%d.height", 
		m_pTerrain->GetFilePath(), r, c);

	if (file_exists(fname))
	{
		return true;
	}

	SafeSprintf(fname, sizeof(fname), "%szone_%d_%d.visual", 
		m_pTerrain->GetFilePath(), r, c);

	if (file_exists(fname))
	{
		return true;
	}

	SafeSprintf(fname, sizeof(fname), "%szone_%d_%d.widget", 
		m_pTerrain->GetFilePath(), r, c);

	if (file_exists(fname))
	{
		return true;
	}

	return false;
}

void TerrainEditor::GetExistZoneList(const IVarList& args, IVarList& result)
{
	size_t zone_rows = m_pZoneManager->GetRows();
	size_t zone_cols = m_pZoneManager->GetCols();

	for (size_t r = 0; r < zone_rows; ++r)
	{
		for (size_t c = 0; c < zone_cols; ++c)
		{
			// �߶��ļ��Ƿ����
			char fname[256];

			SafeSprintf(fname, sizeof(fname), "%szone_%d_%d.height", 
				m_pTerrain->GetFilePath(), r, c);

			if (!file_exists(fname))
			{
				SafeSprintf(fname, sizeof(fname), "%szone_%d_%d.visual", 
					m_pTerrain->GetFilePath(), r, c);

				if (!file_exists(fname))
				{
					continue;
				}
			}

			result.AddInt((int)(r * zone_cols + c));
		}
	}
}

bool TerrainEditor::SetZoneChanged(int zone_index, const char* type, 
	bool value)
{
	CTerrainZone* pZone = InnerGetZone(zone_index);

	if (NULL == pZone)
	{
		return false;
	}

	if (stricmp(type, "height") == 0)
	{
		pZone->SetHeightChanged(value);
	}
	else if (stricmp(type, "alpha") == 0)
	{
		pZone->SetAlphaChanged(value);
	}
	//else if (stricmp(type, "light") == 0)
	//{
	//	pZone->SetLightChanged(value);
	//}
	else if (stricmp(type, "visual") == 0)
	{
		pZone->SetVisualChanged(value);
	}
	else if (stricmp(type, "widget") == 0)
	{
		pZone->SetWidgetChanged(value);
	}
	else if (stricmp(type, "helper_model") == 0)
	{
		pZone->SetHelperModelChanged(value);
	}
	else if (stricmp(type, "collide_info") == 0)
	{
		pZone->SetCollideInfoChanged(value);
	}
	else if (stricmp(type, "desc") == 0)
	{
		pZone->SetDescChanged(value);
	}
	else if (stricmp(type, "sound") == 0)
	{
		pZone->SetSoundChanged(value);
	}
	else if (stricmp(type, "area") == 0)
	{
		pZone->SetAreaChanged(value);
	}
	else if (stricmp(type, "region") == 0)
	{
		pZone->SetRegionChanged(value);
	}
	else
	{
		return false;
	}

	return true;
}

bool TerrainEditor::GetZoneChanged(int zone_index, const char* type)
{
	CTerrainZone* pZone = InnerGetZone(zone_index);
	
	if (NULL == pZone)
	{
		return false;
	}
	
	if (stricmp(type, "height") == 0)
	{
		return pZone->GetHeightChanged();
	}
	else if (stricmp(type, "alpha") == 0)
	{
		return pZone->GetAlphaChanged();
	}
	//else if (stricmp(type, "light") == 0)
	//{
	//	return pZone->GetLightChanged();
	//}
	else if (stricmp(type, "visual") == 0)
	{
		return pZone->GetVisualChanged();
	}
	else if (stricmp(type, "widget") == 0)
	{
		return pZone->GetWidgetChanged();
	}
	else if (stricmp(type, "helper_model") == 0)
	{
		return pZone->GetHelperModelChanged();
	}
	else if (stricmp(type, "collide_info") == 0)
	{
		return pZone->GetCollideInfoChanged();
	}
	else if (stricmp(type, "desc") == 0)
	{
		return pZone->GetDescChanged();
	}
	else if (stricmp(type, "sound") == 0)
	{
		return pZone->GetSoundChanged();
	}
	else if (stricmp(type, "area") == 0)
	{
		return pZone->GetAreaChanged();
	}
	else if (stricmp(type, "region") == 0)
	{
		return pZone->GetRegionChanged();
	}
	else
	{
		return false;
	}
}

bool TerrainEditor::SetZoneNoUnload(int zone_index, bool value)
{
	if (!m_pTerrain->GetDesignMode())
	{
		return false;
	}

	CTerrainZone* pZone = InnerGetZone(zone_index);

	if (NULL == pZone)
	{
		return false;
	}

	pZone->SetNoUnload(value);

	return true;
}

bool TerrainEditor::GetZoneNoUnload(int zone_index)
{
	if (!m_pTerrain->GetDesignMode())
	{
		return false;
	}

	CTerrainZone* pZone = InnerGetZone(zone_index);

	if (NULL == pZone)
	{
		return false;
	}

	return pZone->GetNoUnload();
}

bool TerrainEditor::GetInDesign(float x, float z)
{
	CTerrainZone* zone = m_pZoneManager->GetInZone(x, z);
	
	if (NULL == zone)
	{
		return false;
	}
	
	CTerrainChunk* chunk = zone->GetInChunk(x, z);

	if (NULL == chunk)
	{
		return false;
	}

	return chunk->GetInDesign();
}

void TerrainEditor::GetZoneRect(const IVarList& args, IVarList& result)
{
	int zone_index = args.IntVal(0);

	if (size_t(zone_index) >= m_pZoneManager->GetZoneNum())
	{
		return;
	}

	CTerrainZone* pZone = m_pZoneManager->GetZoneByIndex(zone_index);

	if (NULL == pZone)
	{
		return;
	}

	float min_x = pZone->GetLeft();
	float min_z = pZone->GetTop();
	float max_x = pZone->GetRight();
	float max_z = pZone->GetBottom();
	size_t visual_size = pZone->GetVisuals()->GetVisualSize();
	CQuadVisual** visuals = pZone->GetVisuals()->GetVisuals();

	for (size_t k = 0; k < visual_size; ++k)
	{
		if (NULL == visuals[k])
		{
			continue;
		}

		IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(
			visuals[k]->GetVisualID());

		if (NULL == pVisBase)
		{
			continue;
		}

		FmVec3 center = pVisBase->GetCenter();
		float radius = pVisBase->GetRadius();

		if ((center.x - radius) < min_x)
		{
			min_x = center.x - radius;
		}

		if ((center.z - radius) < min_z)
		{
			min_z = center.z - radius;
		}
		
		if ((center.x + radius) > max_x)
		{
			max_x = center.x + radius;
		}

		if ((center.z + radius) > max_z)
		{
			max_z = center.z + radius;
		}
	}

	result.AddFloat(min_x);
	result.AddFloat(min_z);
	result.AddFloat(max_x);
	result.AddFloat(max_z);
}

void TerrainEditor::GetVisualRect(const IVarList& args, IVarList& result)
{
	PERSISTID id = args.ObjectVal(0);
	IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(id);

	if (NULL == pVisBase)
	{
		return;
	}

	FmVec3 center = pVisBase->GetCenter();
	float radius = pVisBase->GetRadius();

	result.AddFloat(center.x - radius);
	result.AddFloat(center.z - radius);
	result.AddFloat(center.x + radius);
	result.AddFloat(center.z + radius);
}

// �����Ƿ�����ڷ��η�Χ��
static bool rect_contain_sphere(float min_x, float min_z,
	float max_x, float max_z, const FmVec3& center, float radius)
{
	return (center.x >= (min_x - radius)) 
		&& (center.x <= (max_x + radius))
		&& (center.z >= (min_z - radius)) 
		&& (center.z <= (max_z + radius));
}

void TerrainEditor::GetRectLightList(const IVarList& args, IVarList& result)
{
	float min_x = args.FloatVal(0);
	float min_z = args.FloatVal(1);
	float max_x = args.FloatVal(2);
	float max_z = args.FloatVal(3);
	size_t zone_num = m_pZoneManager->GetZoneNum();

	for (size_t i = 0; i < zone_num; ++i)
	{
		CTerrainZone* pZone = m_pZoneManager->GetZoneByIndex(i);

		if (NULL == pZone)
		{
			continue;
		}

		size_t visual_size = pZone->GetVisuals()->GetVisualSize();
		CQuadVisual** visuals = pZone->GetVisuals()->GetVisuals();

		for (size_t k = 0; k < visual_size; ++k)
		{
			if (NULL == visuals[k])
			{
				continue;
			}

			IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(
				visuals[k]->GetVisualID());

			if (NULL == pVisBase)
			{
				continue;
			}

			if (pVisBase->GetEntInfo()->IsKindOf("LightSource"))
			{
				FmVec3 center = pVisBase->GetPosition();
				CVar range;

				GetCore()->GetProperty(pVisBase, "Range", range);

				if (rect_contain_sphere(min_x, min_z, max_x, max_z,
					pVisBase->GetPosition(), range.FloatVal()))
				{
					result.AddObject(pVisBase->GetID());
				}
			}
		}
	}
}

bool TerrainEditor::SetShowChunkBoundBox(float x, float z, bool value)
{
	CTerrainZone* zone = m_pZoneManager->GetInZone(x, z);

	if (NULL == zone)
	{
		return false;
	}

	CTerrainChunk* chunk = zone->GetInChunk(x, z);

	if (NULL == chunk)
	{
		return false;
	}

	chunk->SetShowBoundBox(value);

	return true;
}

bool TerrainEditor::SetPosiY(float x, float z, float y)
{
	Assert((y < 10000.0F) && (y > -10000.0F));

	if ((y >= 10000.0F) || (y <= -10000.0F))
	{
		CORE_TRACE("(TerrainEditor::SetPosiY)invalid value");
		return false;
	}

	CTerrainZone* zone = m_pZoneManager->GetInZone(x, z);
	
	if (NULL == zone)
	{
		return false;
	}

	if (!zone->GetHeightLoaded())
	{
		CORE_TRACE("(TerrainEditor::SetPosiY)height not loaded");
		CORE_TRACE(zone->GetName());
		return false;
	}
	
	int col = int((x - zone->GetLeft()) / m_pTerrain->GetUnitSize());
	int row = int((z - zone->GetTop()) / m_pTerrain->GetUnitSize()); 

	return zone->GetHeight()->SetY(row, col, y);
}

bool TerrainEditor::IncPosiY(float x, float z, float y)
{
	CTerrainZone* zone = m_pZoneManager->GetInZone(x, z);
	
	if (NULL == zone)
	{
		return false;
	}
	
	if (!zone->GetHeightLoaded())
	{
		CORE_TRACE("(TerrainEditor::IncPosiY)height not loaded");
		CORE_TRACE(zone->GetName());
		return false;
	}
	
	int col = int((x - zone->GetLeft()) / m_pTerrain->GetUnitSize());
	int row = int((z - zone->GetTop()) / m_pTerrain->GetUnitSize()); 
	float old_y = zone->GetHeight()->GetValue(row, col);

	return zone->GetHeight()->SetY(row, col, old_y + y);
}

bool TerrainEditor::SetHeightChanged(float x, float z)
{
	float range_size = m_pTerrain->GetUnitSize() * 2;
	
	return RangeHeightChanged(x - range_size, z - range_size,
		x + range_size, z + range_size);
}

static bool find_chunk(TArrayPod<CTerrainChunk*, 1, TCoreAlloc>& chunks, 
	CTerrainChunk* pChunk)
{
	for (size_t i = 0; i < chunks.size(); ++i)
	{
		if (chunks[i] == pChunk)
		{
			return true;
		}
	}

	return false;
}

// ���ָ����Χ����صķֿ�
static void get_range_chunks(Terrain* pTerrain, float beg_x, float beg_z, 
	float end_x, float end_z, TArrayPod<CTerrainChunk*, 1, TCoreAlloc>& chunks)
{
	float step_size = pTerrain->GetChunkScale() * pTerrain->GetUnitSize();
	float z = beg_z;
	
	while (z < (end_z + step_size))
	{
		float x = beg_x;
		
		while (x < (end_x + step_size))
		{
			float px = x;
			float pz = z;
			
			if (px > end_x)
			{
				px = end_x;
			}
			
			if (pz > end_z)
			{
				pz = end_z;
			}
			
			CTerrainZone* pZone = 
				pTerrain->GetZoneManager()->GetInZone(px, pz);
			
			if (pZone != NULL)
			{
				CTerrainChunk* pChunk = pZone->GetInChunk(px, pz);
				
				if (pChunk != NULL)
				{
					if (!find_chunk(chunks, pChunk))
					{
						chunks.push_back(pChunk);
					}
				}
			}
			
			x += step_size;
		}

		z += step_size;
	}
}

static bool find_zone(TArrayPod<CTerrainZone*, 1, TCoreAlloc>& zones, 
	CTerrainZone* pZone)
{
	for (size_t i = 0; i < zones.size(); ++i)
	{
		if (zones[i] == pZone)
		{
			return true;
		}
	}
	
	return false;
}

// ���ָ����Χ����ص�����
void get_range_zones(Terrain* pTerrain, float beg_x, float beg_z, 
	float end_x, float end_z, TArrayPod<CTerrainZone*, 1, TCoreAlloc>& zones)
{
	float step_size = pTerrain->GetZoneScale() * pTerrain->GetUnitSize();
	float z = beg_z;
	
	while (z < (end_z + step_size))
	{
		float x = beg_x;
		
		while (x < (end_x + step_size))
		{
			float px = x;
			float pz = z;
			
			if (px > end_x)
			{
				px = end_x;
			}
			
			if (pz > end_z)
			{
				pz = end_z;
			}
			
			CTerrainZone* pZone = 
				pTerrain->GetZoneManager()->GetInZone(px, pz);
			
			if (pZone != NULL)
			{
				if (!find_zone(zones, pZone))
				{
					zones.push_back(pZone);
				}
			}
			
			x += step_size;
		}
		
		z += step_size;
	}
}

bool TerrainEditor::RangeHeightChanged(float beg_x, float beg_z, 
	float end_x, float end_z)
{
	TArrayPod<CTerrainChunk*, 1, TCoreAlloc> chunks;

	get_range_chunks(m_pTerrain, beg_x, beg_z, end_x, end_z, chunks);

	// �������ɶ���
	size_t chunk_num = chunks.size();

	for (size_t k = 0; k < chunks.size(); ++k)
	{
		CTerrainChunk* p = chunks[k];
		
		p->ReleaseVertexBuffer();
		p->Recalculate();

		// ��¼�仯
		if (m_pTerrain->GetDesignMode())
		{
			p->GetZone()->SetHeightChanged(true);
		}
	}
	
	return true;
}

int TerrainEditor::GetTexBase(float x, float z)
{
	CTerrainChunk* pChunk = m_pTerrain->GetInChunk(x, z);
	
	if (NULL == pChunk)
	{
		return false;
	}
	
	return pChunk->GetBaseTexIndex();
}

bool TerrainEditor::SetTexBase(float x, float z, int tex_index)
{
	if (size_t(tex_index) >= m_pTerrain->GetBaseTexSet()->GetCount())
	{
		return false;
	}
	
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);
	
	if (NULL == pZone)
	{
		return false;
	}
	
	CTerrainChunk* pChunk = m_pTerrain->GetInChunk(x, z);
	
	if (NULL == pChunk)
	{
		return false;
	}
	
	pChunk->SetBaseTexIndex(tex_index);
	pChunk->ReleaseConstBuffer();
	
	// ��¼�仯
	if (m_pTerrain->GetDesignMode())
	{
		pZone->SetAlphaChanged(true);
	}

	return true;
}

bool TerrainEditor::GetAlphaPosition(CTerrainZone* pZone, 
	CTerrainChunk* pChunk, float x, float z, int& row, int& col)
{
	size_t alpha_per_unit = m_pTerrain->GetAlphaPerUnit();
	size_t alpha_map_scale = m_pTerrain->GetAlphaMapScale();
	size_t chunk_scale = m_pTerrain->GetChunkScale();
	float unit_size = m_pTerrain->GetUnitSize();
	float offset_x = pZone->GetLeft() 
		+ pChunk->GetCol() * chunk_scale * unit_size;
	float offset_z = pZone->GetTop() 
		+ pChunk->GetRow() * chunk_scale * unit_size;

	row = int((z - offset_z) / (unit_size / alpha_per_unit));
	col = int((x - offset_x) / (unit_size / alpha_per_unit));

	if ((size_t(row) >= alpha_map_scale) || (size_t(col) >= alpha_map_scale))
	{
		return false;
	}
	
	return true;
}

float TerrainEditor::GetTexBlend(float x, float z, int tex_index, int format)
{
	if (size_t(tex_index) >= m_pTerrain->GetBlendTexSet()->GetCount())
	{
		return 0.0F;
	}
	
	CBlendTex* pBlendTex = 
		m_pTerrain->GetBlendTexSet()->GetByIndex(tex_index);
	
	if (size_t(format) >= pBlendTex->GetFormatCount())
	{
		return 0.0F;
	}
	
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);
	
	if (NULL == pZone)
	{
		return 0.0F;
	}

	CTerrainChunk* pChunk = pZone->GetInChunk(x, z);
	
	if (NULL == pChunk)
	{
		return 0.0F;
	}
	
	CTexAlphaMap* pAlphaMap = pChunk->GetAlphaMap(tex_index, format);

	if (NULL == pAlphaMap)
	{
		return 0.0F;
	}

	int row;
	int col;

	if (!GetAlphaPosition(pZone, pChunk, x, z, row, col))
	{
		return 0.0F;
	}

	size_t value = pAlphaMap->GetAlpha(row, col);
	
	return float(value) / 15.0F;
}

void TerrainEditor::GetTexBlendPara(const IVarList& args, IVarList& result)
{
	float x = args.FloatVal(0);
	float z = args.FloatVal(1);
	int tex_index = args.IntVal(2);
	
	if (size_t(tex_index) >= m_pTerrain->GetBlendTexSet()->GetCount())
	{
		return;
	}
	
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);
	
	if (NULL == pZone)
	{
		return;
	}
	
	CTerrainChunk* pChunk = pZone->GetInChunk(x, z);
	
	if (NULL == pChunk)
	{
		return;
	}
	
	int row;
	int col;

	if (!GetAlphaPosition(pZone, pChunk, x, z, row, col))
	{
		return;
	}

	int format = 0;
	float value = 0.0F;
	size_t map_count = pChunk->GetAlphaMapCount();

	for (size_t i = 0; i < map_count; ++i)
	{
		if (pChunk->GetAlphaMapTexIndex(i) != tex_index)
		{
			continue;
		}

		CTexAlphaMap* pAlphaMap = pChunk->GetAlphaMapByIndex(i);
		float v = pAlphaMap->GetAlpha(row, col) / 15.0F;

		if (v > value)
		{
			format = (int)pChunk->GetAlphaMapFormat(i);
			value = v;
		}
	}

	result << format << value;
}

void TerrainEditor::GetMaxBlendPara(const IVarList& args, IVarList& result)
{
	float x = args.FloatVal(0);
	float z = args.FloatVal(1);
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);
	
	if (NULL == pZone)
	{
		return;
	}
	
	CTerrainChunk* pChunk = pZone->GetInChunk(x, z);
	
	if (NULL == pChunk)
	{
		return;
	}
	
	int row;
	int col;

	if (!GetAlphaPosition(pZone, pChunk, x, z, row, col))
	{
		return;
	}

	int tex_index = 0;
	int format = 0;
	float value = 0.0F;
	size_t map_count = pChunk->GetAlphaMapCount();

	for (size_t i = 0; i < map_count; ++i)
	{
		CTexAlphaMap* pAlphaMap = pChunk->GetAlphaMapByIndex(i);
		float v = pAlphaMap->GetAlpha(row, col) / 15.0F;

		if (v > value)
		{
			tex_index = (int)pChunk->GetAlphaMapTexIndex(i);
			format = (int)pChunk->GetAlphaMapFormat(i);
			value = v;
		}
	}

	result << tex_index << format << value;
}

int TerrainEditor::GetTexBlendLayerCount(float x, float z)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);
	
	if (NULL == pZone)
	{
		return 0;
	}
	
	CTerrainChunk* pChunk = pZone->GetInChunk(x, z);

	if (NULL == pChunk)
	{
		return 0;
	}

	return (int)pChunk->GetAlphaMapCount();
}

int TerrainEditor::GetTexBlendLayerTexIndex(float x, float z, int index)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);
	
	if (NULL == pZone)
	{
		return 0;
	}
	
	CTerrainChunk* pChunk = pZone->GetInChunk(x, z);

	if (NULL == pChunk)
	{
		return 0;
	}

	return (int)pChunk->GetAlphaMapTexIndex(index);
}

int TerrainEditor::GetTexBlendLayerFormat(float x, float z, int index)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);
	
	if (NULL == pZone)
	{
		return 0;
	}
	
	CTerrainChunk* pChunk = pZone->GetInChunk(x, z);

	if (NULL == pChunk)
	{
		return 0;
	}

	return (int)pChunk->GetAlphaMapFormat(index);
}

bool TerrainEditor::CopyTexBlendLayer(float x, float z, int tex_index,
	int format, const char* tex_name)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);
	
	if (NULL == pZone)
	{
		return false;
	}
	
	CTerrainChunk* pChunk = pZone->GetInChunk(x, z);

	if (NULL == pChunk)
	{
		return false;
	}

	CTexAlphaMap* pAlphaMap = pChunk->GetAlphaMap(tex_index, format);

	if (NULL == pAlphaMap)
	{
		return false;
	}

	IRender* pRender = m_pTerrain->GetRender();
	ITexture* pTexture = pRender->FindTexture(tex_name);

	if (NULL == pTexture)
	{
		return false;
	}

	size_t alpha_map_scale = m_pTerrain->GetAlphaMapScale();

	if ((pTexture->GetWidth() != alpha_map_scale)
		|| (pTexture->GetHeight() != alpha_map_scale)
		|| (pTexture->GetFormat() != 0))
	{
		return false;
	}
	/*
	unsigned int pitch;

	unsigned int* dst = (unsigned int*)pRender->LockDynamicTexture(pTexture, 
		0, &pitch);

	if (NULL == dst)
	{
		return false;
	}

	unsigned char* src = pAlphaMap->GetMapData();
	
	for (size_t r = 0; r < alpha_map_scale; ++r)
	{
		for (size_t c = 0; c < alpha_map_scale; ++c)
		{
			unsigned int v;
			size_t k = c / 2;
			
			if ((c & 0x1) == 0)
			{
				v = src[k] & 0xF0;
			}
			else
			{
				v = (src[k] & 0xF) << 4;
			}

			dst[c] = D3DCOLOR_ARGB(0xFF, v, v, v);
		}
		
		dst += pitch / 4;
		src += alpha_map_scale / 2;
	}

	pRender->UnlockDynamicTexture(pTexture, 0);
	*/

	return true;
}

bool TerrainEditor::EraseTexBlendLayer(float x, float z, int tex_index, 
	int format)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);
	
	if (NULL == pZone)
	{
		return false;
	}
	
	CTerrainChunk* pChunk = pZone->GetInChunk(x, z);

	if (NULL == pChunk)
	{
		return false;
	}

	size_t map_count = pChunk->GetAlphaMapCount();

	for (size_t i = 0; i < map_count; ++i)
	{
		if ((pChunk->GetAlphaMapTexIndex(i) == tex_index)
			&& (pChunk->GetAlphaMapFormat(i) == format))
		{
			pChunk->RemoveAlphaMapByIndex(i);
			pChunk->ReleaseConstBuffer();
			pZone->SetAlphaChanged(true);
			return true;
		}
	}

	return false;
}

bool TerrainEditor::FindTexBlendLayer(float x, float z, int tex_index, 
	int format)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);
	
	if (NULL == pZone)
	{
		return false;
	}
	
	CTerrainChunk* pChunk = pZone->GetInChunk(x, z);

	if (NULL == pChunk)
	{
		return false;
	}

	return pChunk->GetAlphaMap(tex_index, format) != NULL;
}

bool TerrainEditor::SetTexBlend(float x, float z, int tex_index, int format, 
	float strength)
{
	if (size_t(tex_index) >= m_pTerrain->GetBlendTexSet()->GetCount())
	{
		return false;
	}

	CBlendTex* pBlendTex = 
		m_pTerrain->GetBlendTexSet()->GetByIndex(tex_index);

	if (size_t(format) >= pBlendTex->GetFormatCount())
	{
		return false;
	}
	
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);
	
	if (NULL == pZone)
	{
		return false;
	}
	
	CTerrainChunk* pChunk = pZone->GetInChunk(x, z);
	
	if (NULL == pChunk)
	{
		return false;
	}
	
	int row;
	int col;

	if (!GetAlphaPosition(pZone, pChunk, x, z, row, col))
	{
		return false;
	}

	// ����Alphaֵ
	CTexAlphaMap* pAlphaMap = pChunk->GetAlphaMap(tex_index, format);

	if (NULL == pAlphaMap)
	{
		pAlphaMap = pChunk->AddAlphaMap(tex_index, format);

		if (NULL == pAlphaMap)
		{
			return false;
		}
	}

	if (!pAlphaMap->SetAlpha(row, col, size_t(strength* 15.0F)))
	{
		return false;
	}
	
	// �淶ÿ�����ͼ���Alphaֵ
	size_t map_count = pChunk->GetAlphaMapCount();
	float sum = 0.0F;

	for (size_t i = 0; i < map_count; ++i)
	{
		CTexAlphaMap* pMap = pChunk->GetAlphaMapByIndex(i);
		float v = float(pMap->GetAlpha(row, col)) / 15.0F;

		sum += v;
	}

	if (sum > 1.0F)
	{
		float factor = 1.0F / sum;

		for (size_t k = 0; k < map_count; ++k)
		{
			CTexAlphaMap* pMap = pChunk->GetAlphaMapByIndex(k);
			float v = float(pMap->GetAlpha(row, col)) / 15.0F;

			v *= factor;

			unsigned int alpha = (unsigned int)(v * 15.0F + 0.5F);

			if (alpha > 15)
			{
				alpha = 15;
			}

			pMap->SetAlpha(row, col, alpha);
		}
	}
	
	pChunk->ReleaseConstBuffer();
	pZone->SetAlphaChanged(true);
	
	return true;
}

bool TerrainEditor::EraseTexBlend(float x, float z, int tex_index)
{
	if (size_t(tex_index) >= m_pTerrain->GetBlendTexSet()->GetCount())
	{
		return false;
	}
	
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);
	
	if (NULL == pZone)
	{
		return false;
	}
	
	CTerrainChunk* pChunk = pZone->GetInChunk(x, z);
	
	if (NULL == pChunk)
	{
		return false;
	}
	
	int row;
	int col;

	if (!GetAlphaPosition(pZone, pChunk, x, z, row, col))
	{
		return false;
	}

	size_t map_count = pChunk->GetAlphaMapCount();

	for (size_t i = 0; i < map_count; ++i)
	{
		if (pChunk->GetAlphaMapTexIndex(i) == tex_index)
		{
			CTexAlphaMap* pAlphaMap = pChunk->GetAlphaMapByIndex(i);

			if (pAlphaMap->SetAlpha(row, col, 0))
			{
				pChunk->ReleaseConstBuffer();
				pZone->SetAlphaChanged(true);
			}
		}
	}
	
	return true;
}

bool TerrainEditor::EraseAllBlend(float x, float z)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);
	
	if (NULL == pZone)
	{
		return false;
	}
	
	CTerrainChunk* pChunk = pZone->GetInChunk(x, z);
	
	if (NULL == pChunk)
	{
		return false;
	}
	
	int row;
	int col;

	if (!GetAlphaPosition(pZone, pChunk, x, z, row, col))
	{
		return false;
	}

	size_t map_count = pChunk->GetAlphaMapCount();

	for (size_t i = 0; i < map_count; ++i)
	{
		CTexAlphaMap* pAlphaMap = pChunk->GetAlphaMapByIndex(i);

		if (pAlphaMap->SetAlpha(row, col, 0))
		{
			pChunk->ReleaseConstBuffer();
			pZone->SetAlphaChanged(true);
		}
	}
	
	return true;
}

bool TerrainEditor::SetBlendChanged(float x, float z)
{
	float range_size = m_pTerrain->GetUnitSize() 
		* m_pTerrain->GetChunkScale() / 4;

	return RangeBlendChanged(x - range_size, z - range_size,
		x + range_size, z + range_size);
}

bool TerrainEditor::RangeBlendChanged(float beg_x, float beg_z, 
	float end_x, float end_z)
{
	TArrayPod<CTerrainChunk*, 1, TCoreAlloc> chunks;
	
	get_range_chunks(m_pTerrain, beg_x, beg_z, end_x, end_z, chunks);
	
	size_t chunk_num = chunks.size();

	// ���Ϊ�յĻ����ͼ
	for (size_t c = 0; c < chunk_num; ++c)
	{
		chunks[c]->ClearEmptyAlphaMap();
	}
	
	// �ؽ���ͼ
	for (size_t i = 0; i < chunk_num; ++i)
	{
		CTerrainChunk* pChunk = chunks[i];

		pChunk->ReleaseAlphaTex();
		pChunk->CreateAlphaTex(m_pTerrain->GetRender());
		pChunk->ReleaseConstBuffer();
		
		// ��¼�仯
		if (m_pTerrain->GetDesignMode())
		{
			pChunk->GetZone()->SetAlphaChanged(true);
		}
	}
	
	return true;
}

PERSISTID TerrainEditor::GetZoneVisual(int zone_index, const char* name)
{
	CTerrainZone* pZone = InnerGetZone(zone_index);
	
	if (NULL == pZone)
	{
		return PERSISTID();
	}

	size_t index;

	if (!pZone->GetVisuals()->FindNameIndex(name, index))
	{
		return PERSISTID();
	}
	
	CQuadVisual* pObject = pZone->GetVisuals()->GetVisualByIndex(index);
	
	return pObject->GetVisualID();
}

const char* TerrainEditor::GetZoneVisualName(int zone_index, 
	const PERSISTID& id)
{
	CTerrainZone* pZone = InnerGetZone(zone_index);
	
	if (NULL == pZone)
	{
		return "";
	}
	
	size_t index;
	
	if (!pZone->GetVisuals()->FindVisualIndex(id, index))
	{
		return "";
	}
	
	CQuadVisual* pObject = pZone->GetVisuals()->GetVisualByIndex(index);
	
	return pObject->GetName();
}

void TerrainEditor::GetZoneVisualList(const IVarList& args, 
	IVarList& result)
{
	result.Clear();
	
	int zone_index = args.IntVal(0);
	CTerrainZone* pZone = InnerGetZone(zone_index);
	
	if (NULL == pZone)
	{
		return;
	}
	
	CTerrainVisuals* pVisuals = pZone->GetVisuals();
	size_t SIZE1 = pVisuals->GetVisualSize();

	for (size_t i = 0; i < SIZE1; ++i)
	{
		CQuadVisual* pObject = pVisuals->GetVisualByIndex(i);
		
		if (NULL == pObject)
		{
			continue;
		}

		IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(
			pObject->GetVisualID());

		if (pVisBase == NULL)
		{
			continue;
		}

		result.AddObject(pObject->GetVisualID());
	}
}

void TerrainEditor::GetRectVisualList(const IVarList& args, 
	IVarList& result)
{
	float beg_x = args.FloatVal(0);
	float beg_z = args.FloatVal(1);
	float end_x = args.FloatVal(2);
	float end_z = args.FloatVal(3);
	TArrayPod<CTerrainZone*, 1, TCoreAlloc> zones;

	get_range_zones(m_pTerrain, beg_x, beg_z, end_x, end_z, zones);

	for (size_t i = 0; i < zones.size(); ++i)
	{
		zones[i]->GetVisuals()->GetRectVisuals(beg_x, beg_z, end_x, end_z, 
			result);
	}
}

void TerrainEditor::GetScreenVisualList(const IVarList& args, IVarList& result)
{
	float left = args.FloatVal(0);
	float top = args.FloatVal(1);
	float right = args.FloatVal(2);
	float bottom = args.FloatVal(3);
	float distance = args.FloatVal(4);
	IRenderContext* pContext = m_pTerrain->GetContext();

	if (NULL == pContext)
	{
		return;
	}

	//const dx_viewport_t& vp = pContext->GetCamera().Viewport;
	int view_width = pContext->GetViewWidth();
	int view_height = pContext->GetViewHeight();
	const FmMat4& mtxViewProj = pContext->GetCamera().mtxViewProj;
	//float x1 = (left - vp.fLeft) / vp.fWidth * 2.0F - 1.0F;
	//float x2 = (right - vp.fLeft) / vp.fWidth * 2.0F - 1.0F;
	//float y1 = 1.0F - (bottom - vp.fTop) / vp.fHeight * 2.0F;
	//float y2 = 1.0F - (top - vp.fTop) / vp.fHeight * 2.0F;
	float x1 = left / (float)view_width * 2.0F - 1.0F;
	float x2 = right / (float)view_width * 2.0F - 1.0F;
	float y1 = 1.0F - bottom / view_height * 2.0F;
	float y2 = 1.0F - top / view_height * 2.0F;
	const FmVec3& cam_pos = pContext->GetCamera().vPosition;
	float beg_x = cam_pos.x - distance;
	float beg_z = cam_pos.z - distance;
	float end_x = cam_pos.x + distance;
	float end_z = cam_pos.z + distance;
	TArrayPod<CTerrainZone*, 1, TCoreAlloc> zones;

	get_range_zones(m_pTerrain, beg_x, beg_z, end_x, end_z, zones);

	for (size_t i = 0; i < zones.size(); ++i)
	{
		CTerrainVisuals* pVisuals = zones[i]->GetVisuals();
		CQuadVisual** visuals = pVisuals->GetVisuals();
		size_t visual_size = pVisuals->GetVisualSize();

		for (size_t k = 0; k < visual_size; ++k)
		{
			if (NULL == visuals[k])
			{
				continue;
			}

			IVisBase* pVisBase = (IVisBase*)GetCore()->GetEntity(
				visuals[k]->GetVisualID());

			if (NULL == pVisBase)
			{
				continue;
			}

			FmVec3 center = pVisBase->GetCenter();
			FmVec3 vec3Temp = center - cam_pos;
			float dist_square = FmVec3LengthSq(&vec3Temp);

			if (dist_square > (distance * distance))
			{
				continue;
			}
			
			FmVec3 v;

			FmVec3TransformCoord(&v, &center, &mtxViewProj);

			if ((v.z < 0.0F) || (v.z > 1.0F)
				|| (v.x < x1) || (v.x > x2)
				|| (v.y < y1) || (v.y > y2))
			{
				continue;
			}

			result.AddObject(pVisBase->GetID());
		}
	}
}

int TerrainEditor::GetWater(float x, float z)
{
	CTerrainChunk* pChunk = m_pTerrain->GetInChunk(x, z);
	
	if (NULL == pChunk)
	{
		return -1;
	}
	
	return pChunk->GetWaterIndex();
}

bool TerrainEditor::SetWater(float x, float z, int water_index)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);
	
	if (NULL == pZone)
	{
		return false;
	}
	
	CTerrainChunk* pChunk = m_pTerrain->GetInChunk(x, z);
	
	if (NULL == pChunk)
	{
		return false;
	}
	
	if (water_index >= 0)
	{
		if (water_index >= m_pTerrain->GetWaterCount())
		{
			CORE_TRACE("(TerrainEditor::SetWater)water index error");
			return false;
		}
	}
	
	pChunk->SetWaterIndex(water_index);
	
	// ��¼�仯
	if (m_pTerrain->GetDesignMode())
	{
		pZone->SetHeightChanged(true);
	}

	return true;
}

bool TerrainEditor::TestWater(float x, float z, int water_index)
{
	if (size_t(water_index) >= m_pTerrain->GetWater()->GetCount())
	{
		return false;
	}
	
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);
	
	if (NULL == pZone)
	{
		return false;
	}
	
	CTerrainChunk* pChunk = m_pTerrain->GetInChunk(x, z);
	
	if (NULL == pChunk)
	{
		return false;
	}
	
	SeaWater* pWater = m_pTerrain->GetWater()->GetByIndex(water_index);
	float water_height = pWater->GetBaseHeight() + pWater->GetMaxSeaHeight();

	return pChunk->GetMin().y < water_height;
}

void TerrainEditor::GetGrass(const IVarList& args, IVarList& result)
{
	int grass_index = args.IntVal(0);
	float x = args.FloatVal(1);
	float z = args.FloatVal(2);
	
	if (size_t(grass_index) >= m_pTerrain->GetGrass()->GetCount())
	{
		return;
	}
	
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);
	
	if (NULL == pZone)
	{
		return;
	}
	
	CTerrainChunk* pChunk = m_pTerrain->GetInChunk(x, z);
	
	if (NULL == pChunk)
	{
		return;
	}

	grass_data_t* pData = pChunk->GetGrass(grass_index, x, z);

	if (NULL == pData)
	{
		return;
	}
	
	result.AddInt(pData->nScale);
	result.AddInt(pData->nAmount);
	result.AddInt(pData->nSeed);
}

#define GRASS_PIECE_NUM (0x10000 / 4)

bool TerrainEditor::AddGrass(int grass_index, float x, float z,
	int scale, int amount, int seed)
{
	CTerrainGrass* pGrass = m_pTerrain->GetGrass();
	
	if (size_t(grass_index) >= pGrass->GetCount())
	{
		return false;
	}
	
	if ((scale < 1) || (scale > 10))
	{
		return false;
	}

	if ((amount < 1) || (amount > 15))
	{
		return false;
	}
	
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);
	
	if (NULL == pZone)
	{
		return false;
	}
	
	CTerrainChunk* pChunk = m_pTerrain->GetInChunk(x, z);
	
	if (NULL == pChunk)
	{
		return false;
	}
	
	// �����ܶ�
	float grass_lod = m_pTerrain->GetGrassLod();
	const grass_data_t* grasses = pChunk->GetGrasses(grass_index);
	size_t grass_num = pChunk->GetGrassCount(grass_index);
	// �ȼ����ʵ��Ҫ���ɵĻ�������
	size_t count = 0;
	// ������������
	float remainder = 0.0F;

	for (size_t j = 0; j < grass_num; ++j)
	{
		const grass_data_t* pData = grasses + j;
		// ����
		float total_amount = float(pData->nAmount) * grass_lod + remainder;
		int amount = int(total_amount);

		// ��������
		remainder = total_amount - amount;

		if (amount <= 0)
		{
			continue;
		}

		count += amount * 3;
	}

	// ���ϵ�ǰҪ�����Ļ�������
	count += amount * 3;

	if (count > GRASS_PIECE_NUM)
	{
		return false;
	}

	grass_data_t data;
	
	data.fPosiX = x;
	data.fPosiZ = z;
	data.nGrassIndex = grass_index;
	data.nScale = scale;
	data.nAmount = amount;
	data.nSeed = seed;

	if (!pChunk->AddGrass(data))
	{
		return false;
	}
	
	// ���������
	pGrass->GetByIndex(grass_index)->Activate();

	// ��¼�仯
	if (m_pTerrain->GetDesignMode())
	{
		pZone->SetVisualChanged(true);
	}
	
	return true;
}

bool TerrainEditor::RemoveGrass(int grass_index, float x, float z)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);
	
	if (NULL == pZone)
	{
		return false;
	}
	
	CTerrainChunk* pChunk = m_pTerrain->GetInChunk(x, z);
	
	if (NULL == pChunk)
	{
		return false;
	}
	
	if (!pChunk->RemoveGrass(grass_index, x, z))
	{
		return false;
	}
	
	// ��¼�仯
	if (m_pTerrain->GetDesignMode())
	{
		pZone->SetVisualChanged(true);
	}
	
	return true;
}

int TerrainEditor::RemoveGrassRect(int grass_index, float x1, float z1, 
	float x2, float z2)
{
	int count = 0;
	TArrayPod<CTerrainChunk*, 1, TCoreAlloc> chunks;
	
	get_range_chunks(m_pTerrain, x1, z1, x2, z2, chunks);
	
	size_t chunk_num = chunks.size();
	
	for (size_t i = 0; i < chunk_num; ++i)
	{
		CTerrainChunk* pChunk = chunks[i];
		
		count += pChunk->RemoveGrassRect(grass_index, x1, z1, x2, z2);
	}

	if (count > 0)
	{
		// ��¼�仯
		if (m_pTerrain->GetDesignMode())
		{
			CTerrainZone* pZone = m_pZoneManager->GetInZone(x1, z1);
			
			if (pZone)
			{
				pZone->SetVisualChanged(true);
			}

			pZone = m_pZoneManager->GetInZone(x2, z2);
			
			if (pZone)
			{
				pZone->SetVisualChanged(true);
			}
		}
	}
	
	return count;
}

int TerrainEditor::RemoveGrassCircle(int grass_index, float x, 
	float z, float radius)
{
	int count = 0;
	float x1 = x - radius;
	float z1 = z - radius;
	float x2 = x + radius;
	float z2 = x + radius;
	TArrayPod<CTerrainChunk*, 1, TCoreAlloc> chunks;
	
	get_range_chunks(m_pTerrain, x1, z1, x2, z2, chunks);
	
	size_t chunk_num = chunks.size();
	
	for (size_t i = 0; i < chunk_num; ++i)
	{
		CTerrainChunk* pChunk = chunks[i];
		
		count += pChunk->RemoveGrassCircle(grass_index, x, z, radius);
	}
	
	if (count > 0)
	{
		// ��¼�仯
		if (m_pTerrain->GetDesignMode())
		{
			CTerrainZone* pZone = m_pZoneManager->GetInZone(x1, z1);
			
			if (pZone)
			{
				pZone->SetVisualChanged(true);
			}
			
			pZone = m_pZoneManager->GetInZone(x2, z2);
			
			if (pZone)
			{
				pZone->SetVisualChanged(true);
			}
		}
	}
	
	return count;
}

int TerrainEditor::GetGrassCountRect(int grass_index, float x1, float z1, 
	float x2, float z2)
{
	int count = 0;
	TArrayPod<CTerrainChunk*, 1, TCoreAlloc> chunks;
	
	get_range_chunks(m_pTerrain, x1, z1, x2, z2, chunks);
	
	size_t chunk_num = chunks.size();
	
	for (size_t i = 0; i < chunk_num; ++i)
	{
		CTerrainChunk* pChunk = chunks[i];
		
		count += pChunk->GetGrassCountRect(grass_index, x1, z1, x2, z2);
	}
	
	return count;
}

int TerrainEditor::GetGrassCountCircle(int grass_index, float x, 
	float z, float radius)
{
	int count = 0;
	float x1 = x - radius;
	float z1 = z - radius;
	float x2 = x + radius;
	float z2 = x + radius;
	TArrayPod<CTerrainChunk*, 1, TCoreAlloc> chunks;
	
	get_range_chunks(m_pTerrain, x1, z1, x2, z2, chunks);
	
	size_t chunk_num = chunks.size();
	
	for (size_t i = 0; i < chunk_num; ++i)
	{
		CTerrainChunk* pChunk = chunks[i];
		
		count += pChunk->GetGrassCountCircle(grass_index, x, z, radius);
	}
	
	return count;
}

void TerrainEditor::GetGrassRect(const IVarList& args, IVarList& result)
{
	float x1 = args.FloatVal(0);
	float z1 = args.FloatVal(1);
	float x2 = args.FloatVal(2);
	float z2 = args.FloatVal(3);
	TArrayPod<CTerrainChunk*, 1, TCoreAlloc> chunks;

	get_range_chunks(m_pTerrain, x1, z1, x2, z2, chunks);

	int grass_index[256];

	memset(grass_index, -1, sizeof(grass_index));

	size_t chunk_num = chunks.size();

	for (size_t i = 0; i < chunk_num; ++i)
	{
		CTerrainChunk* pChunk = chunks[i];

		pChunk->GetGrassRect(x1, z1, x2, z2, grass_index, 256);
	}

	for (int i = 0; i < 256; ++i)
	{
		if (grass_index[i] == -1)
		{
			return;
		}

		result.AddInt(grass_index[i]);
	}
}

void TerrainEditor::GetGrassCircle(const IVarList& args, IVarList& result)
{
	float x = args.FloatVal(0);
	float z = args.FloatVal(1);
	float radius = args.FloatVal(2);
	float x1 = x - radius;
	float z1 = z - radius;
	float x2 = x + radius;
	float z2 = x + radius;
	TArrayPod<CTerrainChunk*, 1, TCoreAlloc> chunks;

	get_range_chunks(m_pTerrain, x1, z1, x2, z2, chunks);

	int grass_index[256];

	memset(grass_index, -1, sizeof(grass_index));

	size_t chunk_num = chunks.size();

	for (size_t i = 0; i < chunk_num; ++i)
	{
		CTerrainChunk* pChunk = chunks[i];

		pChunk->GetGrassCircle(x, z, radius, grass_index, 256);
	}

	for (int i = 0; i < 256; ++i)
	{
		if (grass_index[i] == -1)
		{
			return;
		}

		result.AddInt(grass_index[i]);
	}
}

/*
bool TerrainEditor::SetBlendColor(float x, float z, int alpha, int red, 
	int green, int blue)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);

	if (NULL == pZone)
	{
		return false;
	}

	float unit = m_pTerrain->GetUnitSize() / m_pTerrain->GetLightPerUnit();
	int col = int((x - pZone->GetLeft()) / unit);
	int row = int((z - pZone->GetTop()) / unit);
	unsigned int value = D3DCOLOR_ARGB(alpha, red, green, blue);

	if (!pZone->GetLight()->SetBlendColor(row, col, value))
	{
		return false;
	}

	pZone->SetLightChanged(true);

	return true;
}

void TerrainEditor::GetBlendColor(const IVarList& args, IVarList& result)
{
	float x = args.FloatVal(0);
	float z = args.FloatVal(1);
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);

	if (NULL == pZone)
	{
		return;
	}

	float unit = m_pTerrain->GetUnitSize() / m_pTerrain->GetLightPerUnit();
	int col = int((x - pZone->GetLeft()) / unit);
	int row = int((z - pZone->GetTop()) / unit);
	unsigned int color = pZone->GetLight()->GetBlendColor(row, col);
	unsigned int alpha = (color >> 24) & 0xFF;
	unsigned int red = (color >> 16) & 0xFF;
	unsigned int green = (color >> 8) & 0xFF;
	unsigned int blue = (color >> 0) & 0xFF;

	result.AddInt(alpha);
	result.AddInt(red);
	result.AddInt(green);
	result.AddInt(blue);
}
*/

bool TerrainEditor::SetRegionColor(const char* name, const char* color)
{
	unsigned int value = VisUtil_ColorToInt(color);

	return m_pTerrain->SetRegionColor(name, value);
}

result_string TerrainEditor::GetRegionColor(const char* name)
{
	unsigned int value = m_pTerrain->GetRegionColor(name);

	return VisUtil_IntToColor(value);
}

bool TerrainEditor::SetRegionEnable(const char* name, float x, float z, 
	bool value)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);

	if (NULL == pZone)
	{
		return false;
	}

	float unit_size = m_pTerrain->GetUnitSize();
	int row = int((z - pZone->GetTop()) / unit_size);
	int col = int((x - pZone->GetLeft()) / unit_size);

	pZone->GetRegion()->SetRegionEnable(name, row, col, value);

	// ��¼�仯
	if (m_pTerrain->GetDesignMode())
	{
		pZone->SetRegionChanged(true);
	}

	return true;
}

bool TerrainEditor::ChangeRegionName(float x, float z, const char* ori_name, 
	const char* new_name)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);

	if (NULL == pZone)
	{
		return false;
	}

	bool succeed = pZone->GetRegion()->ChangeRegionName(ori_name, new_name);

	if (succeed)
	{
		m_pTerrain->ChangeRegionName(ori_name, new_name);

		// ��¼�仯
		if (m_pTerrain->GetDesignMode())
		{
			pZone->SetRegionChanged(true);
		}
	}

	return true;
}

bool TerrainEditor::SetAreaColor(const char* name, const char* color)
{
	unsigned int value = VisUtil_ColorToInt(color);

	return m_pTerrain->SetAreaColor(name, value);
}

result_string TerrainEditor::GetAreaColor(const char* name)
{
	unsigned int value = m_pTerrain->GetAreaColor(name);

	return VisUtil_IntToColor(value);
}

bool TerrainEditor::SetAreaEnable(const char* name, float x, float z, 
	bool value)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);

	if (NULL == pZone)
	{
		return false;
	}

	float unit_size = m_pTerrain->GetUnitSize();
	int row = int((z - pZone->GetTop()) / unit_size);
	int col = int((x - pZone->GetLeft()) / unit_size);

	pZone->GetRegion()->SetAreaEnable(name, row, col, value);

	// ��¼�仯
	if (m_pTerrain->GetDesignMode())
	{
		pZone->SetAreaChanged(true);
	}

	return true;
}

bool TerrainEditor::ClearAreaEnable(float x, float z)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);

	if (NULL == pZone)
	{
		return false;
	}

	float unit_size = m_pTerrain->GetUnitSize();
	int row = int((z - pZone->GetTop()) / unit_size);
	int col = int((x - pZone->GetLeft()) / unit_size);

	pZone->GetRegion()->ClearArea(row, col);

	// ��¼�仯
	if (m_pTerrain->GetDesignMode())
	{
		pZone->SetAreaChanged(true);
	}

	return true;
}

bool TerrainEditor::ChangeAreaName(float x, float z, const char* ori_name, 
	const char* new_name)
{
	CTerrainZone* pZone = m_pZoneManager->GetInZone(x, z);

	if (NULL == pZone)
	{
		return false;
	}

	bool succeed = pZone->GetRegion()->ChangeAreaName(ori_name, new_name);

	if (succeed)
	{
		//m_pTerrain->ChangeAreaName(ori_name, new_name);

		// ��¼�仯
		if (m_pTerrain->GetDesignMode())
		{
			pZone->SetAreaChanged(true);
		}
	}

	return true;
}

bool TerrainEditor::ChangeAreaNameTable(const char* ori_name, 
	const char* new_name)
{
	return m_pTerrain->ChangeAreaName(ori_name, new_name);
}

bool TerrainEditor::RemoveAreaColor(const char* name)
{
	return m_pTerrain->RemoveAreaColor(name);
}

bool TerrainEditor::SaveHeight(int zone_index, const char* file_name)
{
	CTerrainZone* pZone = InnerGetZone(zone_index);
	
	if (NULL == pZone)
	{
		return false;
	}

	return CHeightLoader::SaveBlockHeight(file_name, pZone);
}

bool TerrainEditor::SaveHeightCompress(int zone_index, const char* file_name)
{
	CTerrainZone* pZone = InnerGetZone(zone_index);

	if (NULL == pZone)
	{
		return false;
	}

	return CHeightLoader::SaveBlockHeightCompress(file_name, pZone);
}

bool TerrainEditor::SaveWater(int zone_index, const char* file_name)
{
	CTerrainZone* pZone = InnerGetZone(zone_index);
	
	if (NULL == pZone)
	{
		return false;
	}
	
	return CHeightLoader::SaveBlockWater(file_name, pZone);
}

/*
bool TerrainEditor::SaveDiffuseMap(int zone_index, const char* file_name)
{
	CTerrainZone* pZone = InnerGetZone(zone_index);
	
	if (NULL == pZone)
	{
		return false;
	}
	
	CTerrainLight* pLight = pZone->GetLight();
	size_t rows = pLight->GetRows();
	size_t cols = pLight->GetCols();
	size_t count = rows * cols;
	unsigned int* values = (unsigned int*)CORE_ALLOC(
		sizeof(unsigned int) * count);
	
	for (size_t r = 0; r < rows; ++r)
	{
		for (size_t c = 0; c < cols; ++c)
		{
			float u = float(c) / float(cols);
			float v = float(r) / float(rows);
			unsigned int val = unsigned int(pLight->GetDiffuse(u, v) * 255);

			values[r * cols + c] = 0xFF000000 + (val << 16) + (val << 8) + val;
		}
	}
	
	IImageAccess* pImage = 
		pZone->GetTerrain()->GetRender()->CreateImageAccess();

	pImage->SetFormat(cols, rows, 32);
	pImage->SetData(values, sizeof(unsigned int) * count);
	
	if (!pImage->SaveFile(file_name))
	{
		pImage->Release();
		CORE_FREE(values, sizeof(unsigned int) * count);
		return false;
	}

	pImage->Release();
	CORE_FREE(values, sizeof(unsigned int) * count);
	
	return true;
}
*/

bool TerrainEditor::SaveAlphaBase(int zone_index, const char* file_name)
{
	CTerrainZone* pZone = InnerGetZone(zone_index);
	
	if (NULL == pZone)
	{
		return false;
	}
	
	return CAlphaLoader::SaveBlockBase(file_name, pZone);
}

bool TerrainEditor::SaveAlphaMap(int zone_index, const char* file_name)
{
	CTerrainZone* pZone = InnerGetZone(zone_index);
	
	if (NULL == pZone)
	{
		return false;
	}
	
	return CAlphaLoader::SaveBlockAlphaMap(file_name, pZone);
}

bool TerrainEditor::SaveVisuals(int zone_index, const char* file_name,
	const char* widget_file, const char* helper_file, 
	const char* collide_file, const char* desc_file, const char* sound_file)
{
	CTerrainZone* pZone = InnerGetZone(zone_index);
	
	if (NULL == pZone)
	{
		return false;
	}
	
	return CVisualLoader::SaveVisuals(file_name, widget_file, helper_file, 
		collide_file, desc_file, sound_file, pZone);
}

bool TerrainEditor::SaveGrass(int zone_index, const char* file_name)
{
	CTerrainZone* pZone = InnerGetZone(zone_index);
	
	if (NULL == pZone)
	{
		return false;
	}
	
	return CVisualLoader::SaveBlockGrass(file_name, pZone);
}

bool TerrainEditor::SaveWalk(int zone_index, const char* file_name)
{
	CTerrainZone* pZone = InnerGetZone(zone_index);

	if (NULL == pZone)
	{
		return false;
	}

	return CWalkLoader::SaveBlockWalk(file_name, pZone);
}

bool TerrainEditor::SaveWalkMap(int zone_index, const char* file_name)
{
	CTerrainZone* pZone = InnerGetZone(zone_index);

	if (NULL == pZone)
	{
		return false;
	}

	return pZone->GetWalkable()->SaveWalkMap(file_name);
}

/*
bool TerrainEditor::SaveBlendColors(int zone_index, const char* file_name)
{
	CTerrainZone* pZone = InnerGetZone(zone_index);

	if (NULL == pZone)
	{
		return false;
	}

	return CLightLoader::SaveBlendColors(file_name, pZone);
}

bool TerrainEditor::SaveLightMap(int zone_index, const char* file_name)
{
	CTerrainZone* pZone = InnerGetZone(zone_index);

	if (NULL == pZone)
	{
		return false;
	}

	return pZone->GetLight()->GetLightTex()->SaveDDS(
		m_pTerrain->GetRender(), file_name, true);
}
*/

bool TerrainEditor::SaveRegion(int zone_index, const char* file_name)
{
	CTerrainZone* pZone = InnerGetZone(zone_index);

	if (NULL == pZone)
	{
		return false;
	}

	return CRegionLoader::SaveBlockRegion(file_name, pZone);
}

bool TerrainEditor::SaveArea(int zone_index, const char* file_name)
{
	CTerrainZone* pZone = InnerGetZone(zone_index);

	if (NULL == pZone)
	{
		return false;
	}

	return CRegionLoader::SaveBlockArea(file_name, pZone);
}

IBlockReader* TerrainEditor::CreateHeightReader(int block_id, void* context)
{
	TerrainEditor* pthis = (TerrainEditor*)context;
	Terrain* pTerrain = pthis->m_pTerrain;

	if (block_id == BLOCK_ID_HEIGHT_0)
	{
		return CBlockHeightReader::NewInstance(&pthis->m_Height,
			pTerrain->GetZoneScale() + 1, pTerrain->GetZoneScale() + 1);
	}
	else if (block_id == BLOCK_ID_CHUNK_0)
	{
		size_t scale = pTerrain->GetZoneScale() / pTerrain->GetChunkScale();
		
		return CBlockChunkReader::NewInstance(&pthis->m_Chunk, scale, scale, 
			0.0F, 0.0F);
	}

	return NULL;
}

bool TerrainEditor::CompressZoneHeight(const char* src_file, 
	const char* dst_file)
{
	Assert(src_file != NULL);
	Assert(dst_file != NULL);

	if (m_Height.pValues)
	{
		size_t dims = m_pTerrain->GetZoneScale() + 1;

		CORE_FREE(m_Height.pValues, sizeof(float) * dims * dims);
		m_Height.pValues = NULL;
	}

	if (m_Height.pNormals)
	{
		size_t dims = m_pTerrain->GetZoneScale() + 1;

		CORE_FREE(m_Height.pNormals, sizeof(unsigned int) * dims * dims);
		m_Height.pNormals = NULL;
	}

	if (m_Chunk.pChunks)
	{
		size_t chunk_dims = 
			m_pTerrain->GetZoneScale() / m_pTerrain->GetChunkScale();
		size_t chunk_num = chunk_dims * chunk_dims;

		CORE_FREE(m_Chunk.pChunks, 
			sizeof(CHeightLoader::chunk_info_t) * chunk_num);
		m_Chunk.pChunks = NULL;
	}

	CBlockFile read_file(TERRAIN_FILE_SIGNATURE, CreateHeightReader, this);

	if (!read_file.Load(src_file))
	{
		return false;
	}

	CBlockFile write_file(TERRAIN_FILE_SIGNATURE, NULL, NULL);
	void* handle = write_file.SaveBegin(dst_file);

	if (NULL == handle)
	{
		return false;
	}

	CBlockHeight2Writer2 writer(&m_Height, m_pTerrain->GetZoneScale() + 1,
		m_pTerrain->GetZoneScale() + 1);

	if (!write_file.SaveBlock(handle, BLOCK_ID_HEIGHT_2, &writer))
	{
		return false;
	}

	size_t chunk_rows = 
		m_pTerrain->GetZoneScale() / m_pTerrain->GetChunkScale();
	size_t chunk_cols = 
		m_pTerrain->GetZoneScale() / m_pTerrain->GetChunkScale();

	if (NULL == m_Chunk.pChunks)
	{
		// ����ֿ�����
		size_t chunk_num = chunk_rows * chunk_cols;
		CHeightLoader::chunk_info_t* chunks = 
			(CHeightLoader::chunk_info_t*)CORE_ALLOC(
			sizeof(CHeightLoader::chunk_info_t) * chunk_num);

		memset(chunks, 0, sizeof(CHeightLoader::chunk_info_t) * chunk_num);

		size_t chunk_scale = m_pTerrain->GetChunkScale();
		float unit_size = m_pTerrain->GetUnitSize();
		size_t height_cols = m_pTerrain->GetZoneScale() + 1;
		size_t lod_levels = m_pTerrain->GetLodLevels();

		for (size_t r = 0; r < chunk_rows; ++r)
		{
			for (size_t c = 0; c < chunk_cols; ++c)
			{
				CHeightLoader::chunk_info_t* chunk = 
					chunks + r * chunk_cols + c;
				size_t row = r * chunk_scale;
				size_t col = c * chunk_scale;

				CHeightLoader::CalcChunkErrors(m_Height.pValues, height_cols,
					row, col, chunk_scale, lod_levels, chunk->fErrors);
				CHeightLoader::CalcChunkBoundBox(m_Height.pValues, height_cols,
					row, col, chunk_scale, 0.0F, 0.0F, unit_size, chunk->vMin, 
					chunk->vMax);
			}
		}

		m_Chunk.pChunks = chunks;
	}

	CBlockChunkWriter2 writer1(&m_Chunk, chunk_rows, chunk_cols, 0.0F, 0.0F);

	if (!write_file.SaveBlock(handle, BLOCK_ID_CHUNK_0, &writer1))
	{
		return false;
	}

	return write_file.SaveEnd(handle);
}

bool TerrainEditor::GenWalkAddPermit(int zone_index, float x, float y, 
	float z)
{
	CTerrainZone* pZone = InnerGetZone(zone_index);

	if (NULL == pZone)
	{
		return false;
	}

	if (NULL == m_pWalkGenerator)
	{
		return false;
	}

	return m_pWalkGenerator->AddPermit(x, y, z);
}

bool TerrainEditor::GenWalkAddForbid(int zone_index, float x, float y, 
	float z, float radius)
{
	CTerrainZone* pZone = InnerGetZone(zone_index);

	if (NULL == pZone)
	{
		return false;
	}

	if (NULL == m_pWalkGenerator)
	{
		return false;
	}

	return m_pWalkGenerator->AddForbid(x, y, z, radius);
}

bool TerrainEditor::GenWalkAddFence(int fence_index, float x, float z)
{
	if (NULL == m_pWalkGenerator)
	{
		return false;
	}

	return m_pWalkGenerator->AddFencePoint(fence_index, x, z);
}

void TerrainEditor::GenWalkBegin(const IVarList& args, IVarList& result)
{
	int zone_index = args.IntVal(0);
	int precision = args.IntVal(1);
	float role_height = args.FloatVal(2);
	float role_radius = args.FloatVal(3);
	float step_height = args.FloatVal(4);
	float trace_high = args.FloatVal(5);
	float trace_low = args.FloatVal(6);
	float ground_normal_y = args.FloatVal(7);
	float floor_normal_y = args.FloatVal(8);
	float wall_angle = args.FloatVal(9);
	CTerrainZone* pZone = InnerGetZone(zone_index);

	if (NULL == pZone)
	{
		result.AddBool(false);
		return;
	}

	if (m_pWalkGenerator)
	{
		CORE_DELETE(m_pWalkGenerator);
	}

	set_global_on_off(GetCore(), "walk_generate_state", true);

	m_pWalkGenerator = (CWalkGenerator*)CORE_ALLOC(sizeof(CWalkGenerator));
	new (m_pWalkGenerator) CWalkGenerator(m_pTerrain, pZone, precision);
	m_pWalkGenerator->SetRoleHeight(role_height);
	m_pWalkGenerator->SetRoleRadius(role_radius);
	m_pWalkGenerator->SetStepHeight(step_height);
	m_pWalkGenerator->SetTraceHigh(trace_high);
	m_pWalkGenerator->SetTraceLow(trace_low);
	m_pWalkGenerator->SetGroundNormalY(ground_normal_y);
	m_pWalkGenerator->SetFloorNormalY(floor_normal_y);
	m_pWalkGenerator->SetWallAngle(wall_angle);

	bool succeed = m_pWalkGenerator->GenerateBegin();

	result.AddBool(succeed);
}

bool TerrainEditor::GenWalkGround(int zone_index)
{
	CTerrainZone* pZone = InnerGetZone(zone_index);

	if (NULL == pZone)
	{
		return false;
	}

	if (NULL == m_pWalkGenerator)
	{
		return false;
	}

	return m_pWalkGenerator->GenerateGround();
}

bool TerrainEditor::GenWalkBalk(int zone_index, int row)
{
	CTerrainZone* pZone = InnerGetZone(zone_index);

	if (NULL == pZone)
	{
		return false;
	}

	if (NULL == m_pWalkGenerator)
	{
		return false;
	}

	return m_pWalkGenerator->GenerateBalk(row);
}

bool TerrainEditor::GenWalkable(int zone_index, float start_x, 
	float start_y, float start_z)
{
	CTerrainZone* pZone = InnerGetZone(zone_index);

	if (NULL == pZone)
	{
		return false;
	}

	if (NULL == m_pWalkGenerator)
	{
		return false;
	}

	return m_pWalkGenerator->GenerateWalkable(start_x, start_y, start_z);
}

bool TerrainEditor::GenWalkFloor(int zone_index)
{
	CTerrainZone* pZone = InnerGetZone(zone_index);

	if (NULL == pZone)
	{
		return false;
	}

	if (NULL == m_pWalkGenerator)
	{
		return false;
	}

	return m_pWalkGenerator->GenerateFloor();
}

bool TerrainEditor::GenWalkWall(int zone_index)
{
	CTerrainZone* pZone = InnerGetZone(zone_index);

	if (NULL == pZone)
	{
		return false;
	}

	if (NULL == m_pWalkGenerator)
	{
		return false;
	}

	return m_pWalkGenerator->GenerateWall();
}

bool TerrainEditor::GenWalkWater(int zone_index)
{
	CTerrainZone* pZone = InnerGetZone(zone_index);

	if (NULL == pZone)
	{
		return false;
	}

	if (NULL == m_pWalkGenerator)
	{
		return false;
	}

	return m_pWalkGenerator->GenerateWater();
}

bool TerrainEditor::GenWalkEnd(int zone_index)
{
	CTerrainZone* pZone = InnerGetZone(zone_index);

	if (NULL == pZone)
	{
		return false;
	}

	if (NULL == m_pWalkGenerator)
	{
		return false;
	}

	if (!m_pWalkGenerator->GenerateEnd())
	{
		return false;
	}

	CORE_DELETE(m_pWalkGenerator);
	m_pWalkGenerator = NULL;

	set_global_on_off(GetCore(), "walk_generate_state", false);

	return true;
}

// С�������ͼ��Ϣ
struct light_map_info_t
{
	char strFileName[64];
	char strModelName[64];
	size_t nSize;
	int nMaterialIndex;
	int nAtlasMapIndex;
	float fScaleU;
	float fScaleV;
	float fOffsetU;
	float fOffsetV;
};

// ���ļ�����ȡģ�����ֺͲ�������
static bool get_model_name_material(const char* file_name, char* model_name, 
	int& material_index)
{
	const char* split = strchr(file_name, '_');

	if (NULL == split)
	{
		return false;
	}

	split = strchr(split + 1, '_');

	if (NULL == split)
	{
		return false;
	}
	
	split = strchr(split + 1, '_');

	if (NULL == split)
	{
		return false;
	}

	const char* beg = split + 1;
	const char* end = strrchr(file_name, '.');

	if (NULL == end)
	{
		return false;
	}

	if (end <= beg)
	{
		return false;
	}

	size_t size = end - beg;

	memcpy(model_name, beg, size);
	model_name[size] = 0;

	char* split1 = strchr(model_name, '_');

	if (NULL == split1)
	{
		return false;
	}

	*split1 = 0;
	material_index = atoi(split1 + 1);

	if ((material_index < 0) || (material_index >= 1024))
	{
		return false;
	}

	return true;
}

/*
// �ļ���С�Ƚ�
static int __cdecl file_size_compare(const void* elem1, const void* elem2)
{
	if (((light_map_info_t*)elem1)->nSize < 
		((light_map_info_t*)elem2)->nSize)
	{
		return 1;
	}

	if (((light_map_info_t*)elem1)->nSize > 
		((light_map_info_t*)elem2)->nSize)
	{
		return -1;
	}

	return 0;
}

bool TerrainEditor::GenLightMapAtlas(int zone_index, int atlas_map_size, 
	int min_light_map_size, int small_file_size)
{
	size_t atlas_grid_scale = atlas_map_size / min_light_map_size;

	if (atlas_grid_scale < 2)
	{
		return false;
	}
	
	int zone_row = GetZoneRow(zone_index);
	int zone_col = GetZoneCol(zone_index);
	char zone_name[32];
	char find_name[256];

	SafeSprintf(zone_name, sizeof(zone_name), "zone_%d_%d", zone_row, zone_col);
	SafeSprintf(find_name, sizeof(find_name), "%s%s\\%s_*.dds", 
		m_pTerrain->GetFilePath(), m_pTerrain->GetModelLightPath(),
		zone_name);

	WIN32_FIND_DATA find_data;
	HANDLE find_handle = FindFirstFile(find_name, &find_data);
	TArrayPod<light_map_info_t, 1, TCoreAlloc> lightmaps;
	
	if (find_handle != INVALID_HANDLE_VALUE)
	{
		do 
		{
			if ((find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				char fname[256];

				SafeSprintf(fname, sizeof(fname), "%s%s\\%s", 
					m_pTerrain->GetFilePath(), m_pTerrain->GetModelLightPath(),
					find_data.cFileName);
				
				HANDLE fp = CreateFile(fname, GENERIC_READ, 
					FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
				DWORD file_size = 0xFFFFFFFF;

				if (fp != INVALID_HANDLE_VALUE)
				{
					file_size = GetFileSize(fp, NULL);
					CloseHandle(fp);
				}
				
				if (file_size <= (DWORD)small_file_size)
				{
					light_map_info_t data;

					memset(&data, 0, sizeof(data));

					if (get_model_name_material(find_data.cFileName, 
						data.strModelName, data.nMaterialIndex))
					{
						CopyString(data.strFileName, sizeof(data.strFileName), 
							find_data.cFileName);
						data.nSize = file_size;
						data.nAtlasMapIndex = -1;
						data.fScaleU = 1.0F;
						data.fScaleV = 1.0F;
						data.fOffsetU = 0.0F;
						data.fOffsetV = 0.0F;
						
						lightmaps.push_back(data);
					}
				}
			}
		} while (FindNextFile(find_handle, &find_data));

		FindClose(find_handle);
	}

	// ɾ���ɵĺϲ�����ͼ
	SafeSprintf(find_name, sizeof(find_name), "%s%s\\atlas_%s_*.dds", 
		m_pTerrain->GetFilePath(), m_pTerrain->GetModelLightPath(),
		zone_name);
	find_handle = FindFirstFile(find_name, &find_data);

	if (find_handle != INVALID_HANDLE_VALUE)
	{
		do 
		{
			if ((find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				char fname[256];

				SafeSprintf(fname, sizeof(fname), "%s%s\\%s", 
					m_pTerrain->GetFilePath(), m_pTerrain->GetModelLightPath(),
					find_data.cFileName);

				if (DeleteFile(fname) == FALSE)
				{
					FindClose(find_handle);
					return false;
				}
			}
		} while (FindNextFile(find_handle, &find_data));

		FindClose(find_handle);
	}

	// ɾ����ͼӳ���ļ�
	char ini_name[256];

	SafeSprintf(ini_name, sizeof(ini_name), "%s%s.atlas", 
		m_pTerrain->GetFilePath(), zone_name);
	DeleteFile(ini_name);

	if (0 == lightmaps.size())
	{
		// û���κ���Ҫ�ϲ����ļ�
		return true;
	}

	// ���ļ��ߴ�Ӵ�С����
	qsort(lightmaps.data(), lightmaps.size(), sizeof(light_map_info_t),
		file_size_compare);

	// �ϲ���ͼ
	TArrayPod<DxTexture*, 1, TCoreAlloc> atlasmaps;
	size_t atlas_grid_num = atlas_grid_scale * atlas_grid_scale;
	TAutoMem<int, 256, TCoreAlloc> auto_buf(atlas_grid_num);
	int* atlas_in_use = auto_buf.GetBuffer();
	size_t in_use_count = 0;
	DxTexture* pAtlasTex = NULL;
	DxDevice* pDevice = m_pTerrain->GetRender()->GetDevice();

	for (size_t i = 0; i < lightmaps.size(); ++i)
	{
		light_map_info_t* pLight = &lightmaps[i];
		char fname[256];

		SafeSprintf(fname, sizeof(fname), "%s%s\\%s", 
			m_pTerrain->GetFilePath(), m_pTerrain->GetModelLightPath(),
			pLight->strFileName);

		D3DXIMAGE_INFO image_info;

		memset(&image_info, 0, sizeof(image_info));
		D3DXGetImageInfoFromFile(fname, &image_info);

		if (image_info.ImageFileFormat != D3DXIFF_DDS)
		{
			continue;
		}

		if (image_info.Format != D3DFMT_DXT1)
		{
			continue;
		}

		size_t image_size = image_info.Width;

		if ((image_size > (size_t)atlas_map_size) 
			|| (image_size < (size_t)min_light_map_size))
		{
			continue;
		}

		DxTexture* pLightTex = NULL;

		D3DXCreateTextureFromFileEx(pDevice, fname, image_info.Width, 
			image_info.Height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, 
			D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &pLightTex);

		if (NULL == pLightTex)
		{
			continue;
		}

		// ���Һϲ���ͼ�Ͽ��е�λ��
		size_t light_scale = image_size / min_light_map_size;
		size_t light_grid_num = light_scale * light_scale;

		if ((in_use_count + light_grid_num) > atlas_grid_num)
		{
			pAtlasTex = NULL;
		}
	
		if (NULL == pAtlasTex)
		{
			D3DXCreateTexture(pDevice, atlas_map_size, atlas_map_size,
				1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pAtlasTex);

			// ��ʼ����ͼ����
			D3DLOCKED_RECT lr;

			pAtlasTex->LockRect(0, &lr, NULL, 0);

			DWORD* pDstData = (DWORD*)lr.pBits;

			for (int r = 0; r < atlas_map_size; ++r)
			{
				for (int c = 0; c < atlas_map_size; ++c)
				{
					pDstData[c] = 0xFF808080;
				}

				pDstData = (DWORD*)((char*)pDstData + lr.Pitch);
			}

			pAtlasTex->UnlockRect(0);

			atlasmaps.push_back(pAtlasTex);
			memset(atlas_in_use, 0, sizeof(int) * atlas_grid_num);
			in_use_count = 0;
		}

		// �ںϲ���ͼ�ϵ���ʼλ��
		size_t start_row;
		size_t start_col;

		for (size_t t1 = 0; t1 < atlas_grid_num; ++t1)
		{
			if (atlas_in_use[t1] != 0)
			{
				continue;
			}

			size_t beg_row = t1 / atlas_grid_scale;
			size_t beg_col = t1 % atlas_grid_scale;
			size_t end_row = beg_row + light_scale;
			size_t end_col = beg_col + light_scale;

			if ((end_row > atlas_grid_scale) || (end_col > atlas_grid_scale))
			{
				continue;
			}

			for (size_t row = beg_row; row < end_row; ++row)
			{
				for (size_t col = beg_col; col < end_col; ++col)
				{
					size_t grid_index = row * atlas_grid_scale + col;
					
					if (atlas_in_use[grid_index] != 0)
					{
						for (size_t t2 = 0; t2 < atlasmaps.size(); ++t2)
						{
							atlasmaps[t2]->Release();
						}

						pLightTex->Release();
						return false;
					}

					atlas_in_use[grid_index] = 1;
					in_use_count++;
				}
			}

			start_row = beg_row * min_light_map_size;
			start_col = beg_col * min_light_map_size;
			break;
		}

		// ������ͼ����
		D3DLOCKED_RECT src_lr;
		D3DLOCKED_RECT dst_lr;

		pLightTex->LockRect(0, &src_lr, NULL, 0);
		pAtlasTex->LockRect(0, &dst_lr, NULL, 0);

		DWORD* pSrcData = (DWORD*)src_lr.pBits;
		DWORD* pDstData = (DWORD*)
			((char*)dst_lr.pBits + start_row * dst_lr.Pitch);

		for (size_t r = 0; r < image_size; ++r)
		{
			memcpy(pDstData + start_col, pSrcData, sizeof(DWORD) * image_size);
			pSrcData = (DWORD*)((char*)pSrcData + src_lr.Pitch);
			pDstData = (DWORD*)((char*)pDstData + dst_lr.Pitch);
		}

		pAtlasTex->UnlockRect(0);
		pLightTex->UnlockRect(0);
		pLightTex->Release();

		pLight->nAtlasMapIndex = (int)(atlasmaps.size() - 1);
		pLight->fScaleU = (float)image_size / (float)atlas_map_size;
		pLight->fScaleV = (float)image_size / (float)atlas_map_size;
		pLight->fOffsetU = (float)start_col / (float)atlas_map_size;
		pLight->fOffsetV = (float)start_row / (float)atlas_map_size;
	}

	// ����ϲ���Ĺ���ͼ
	for (size_t j = 0; j < atlasmaps.size(); ++j)
	{
		DxTexture* pAtlasTex = atlasmaps[j];
		char fname[256];

		SafeSprintf(fname, sizeof(fname), "%s%s\\atlas_%s_%d.dds",
			m_pTerrain->GetFilePath(), m_pTerrain->GetModelLightPath(),
			zone_name, j);
		
		// ת���ļ���ʽΪDXT1
		ID3DXBuffer* pTempBuffer = NULL;
		DxTexture* pTexSave = NULL;
		HRESULT hr;
		
		hr = D3DXSaveTextureToFileInMemory(&pTempBuffer, D3DXIFF_DDS, 
			pAtlasTex, NULL);
		hr = D3DXCreateTextureFromFileInMemoryEx(pDevice, 
			pTempBuffer->GetBufferPointer(), pTempBuffer->GetBufferSize(),
			atlas_map_size, atlas_map_size, 0, 0, D3DFMT_DXT1, D3DPOOL_MANAGED, 
			D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &pTexSave);
		hr = D3DXSaveTextureToFile(fname, D3DXIFF_DDS, pTexSave, NULL);

		pTempBuffer->Release();
		pTexSave->Release();
		pAtlasTex->Release();
	}

	// �������ͼӳ���ļ�
	IIniFile* pIni = g_pCore->CreateIniFile(ini_name);

	for (size_t k = 0; k < lightmaps.size(); ++k)
	{
		light_map_info_t* pLight = &lightmaps[k];

		if (pLight->nAtlasMapIndex < 0)
		{
			continue;
		}

		char ini_key[32];
		char ini_value[128];

		SafeSprintf(ini_key, sizeof(ini_key), "%d", pLight->nMaterialIndex);
		SafeSprintf(ini_value, sizeof(ini_value), "%d,%f,%f,%f,%f",
			pLight->nAtlasMapIndex, pLight->fScaleU, pLight->fScaleV,
			pLight->fOffsetU, pLight->fOffsetV);
		pIni->WriteString(pLight->strModelName, ini_key, ini_value);
	}

	bool succeed = pIni->SaveToFile();

	pIni->Release();

	return succeed;
}
*/