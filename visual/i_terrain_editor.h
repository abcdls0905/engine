//--------------------------------------------------------------------
// �ļ���:		i_terrain_editor.h
// ��  ��:		
// ˵  ��:		
// ��������:
// ������:
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _VISUAL_I_TERRAIN_EDITOR_H
#define _VISUAL_I_TERRAIN_EDITOR_H

#include "../public/module.h"

class Terrain;
class CTerrainChunk;
class CTerrainZone;
class CZoneManager;
class CWalkGenerator;

class ITerrainEditor: public IEntity
{
public:
	virtual void SetTerrain(Terrain* value) = 0;

	// ��õ����ڵ���������ֵ
	virtual int GetZoneIndex(float x, float z) = 0;
	// ���������к�
	virtual int GetZoneRow(int zone_index) = 0;
	// ���������к�
	virtual int GetZoneCol(int zone_index) = 0;
	// ������������
	virtual const char* GetZoneName(int zone_index) = 0;
	// �������ĵ�����͵�߶�
	virtual float GetZoneMinY(int zone_index) = 0;
	// �������ĵ�����ߵ�߶�
	virtual float GetZoneMaxY(int zone_index) = 0;
	// ������ڴ����������������ֵ
	virtual void GetZoneList(const IVarList& args, IVarList& result) = 0;
	// ���ָ����Χ����ص���������ֵ��
	virtual void GetRangeZoneList(const IVarList& args, IVarList& result) = 0;
	// ָ������߶��ļ��Ƿ����
	virtual bool GetZoneExists(int zone_index) = 0;
	// ��ô����ļ�����������
	virtual void GetExistZoneList(const IVarList& args, IVarList& result) = 0;
	// ����ָ����������ݸı��־
	virtual bool SetZoneChanged(int zone_index, const char* type, bool value) = 0;
	// ���ָ����������ݸı��־
	virtual bool GetZoneChanged(int zone_index, const char* type) = 0;
	// ����ָ������ж��
	virtual bool SetZoneNoUnload(int zone_index, bool value) = 0;
	// ���ָ�������Ƿ�ж��
	virtual bool GetZoneNoUnload(int zone_index) = 0;
	// ָ���ĵ��Ƿ��ڿɱ༭��Χ֮��
	virtual bool GetInDesign(float x, float z) = 0;

	// ��������������������ƽ�淶Χ
	virtual void GetZoneRect(const IVarList& args, IVarList& result) = 0;
	// ��������ƽ�淶Χ
	virtual void GetVisualRect(const IVarList& args, IVarList& result) = 0;
	// ���ƽ�淶Χ�е����е��Դ
	virtual void GetRectLightList(const IVarList& args, IVarList& result) = 0;

	// ����ָ����λ���Ƿ���ʾ�ֿ�İ�Χ��
	virtual bool SetShowChunkBoundBox(float x, float z, bool value) = 0;

	// ��������λ�ø߶�
	virtual bool SetPosiY(float x, float z, float y) = 0;
	// ����ָ��λ�ø߶�
	virtual bool IncPosiY(float x, float z, float y) = 0;
	// ĳ��ĸ߶ȷ����仯
	virtual bool SetHeightChanged(float x, float z) = 0;
	// ĳ����Χ�ڵĸ߶ȷ����仯
	virtual bool RangeHeightChanged(float beg_x, float beg_z, float end_x, 
		float end_z) = 0;

	// ��û�����ͼ
	virtual int GetTexBase(float x, float z) = 0;
	// ���û�����ͼ
	virtual bool SetTexBase(float x, float z, int tex_index) = 0;

	// ��û����ͼ�ڵ�ǰλ�õ�ֵ
	virtual float GetTexBlend(float x, float z, int tex_index, int format) = 0;
	// ��û����ͼ�ڵ�ǰλ�õ����ǰֵ
	virtual void GetTexBlendPara(const IVarList& args, IVarList& result) = 0;
	// ��õ�ǰλ�����ֵ�Ļ����ͼ����
	virtual void GetMaxBlendPara(const IVarList& args, IVarList& result) = 0;
	// ���ָ��λ�õĻ����ͼ����
	virtual int GetTexBlendLayerCount(float x, float z) = 0;
	// ���ָ��λ�õ�ָ����Ļ����ͼ����
	virtual int GetTexBlendLayerTexIndex(float x, float z, int index) = 0;
	// ���ָ��λ�õ�ָ����Ļ����ͼ��ʽ
	virtual int GetTexBlendLayerFormat(float x, float z, int index) = 0;
	// ����ָ��λ�õ�ָ����Ļ��ͼ����ͼ
	virtual bool CopyTexBlendLayer(float x, float z, int tex_index, int format,
		const char* tex_name) = 0;
	// ����ָ��λ�õ�ָ����ϲ�
	virtual bool EraseTexBlendLayer(float x, float z, int tex_index, int format) = 0;
	// ��ָ��λ�ò���ָ�������ͼ��
	virtual bool FindTexBlendLayer(float x, float z, int tex_index, int format) = 0;
	// ���û����ͼ��ֵ
	virtual bool SetTexBlend(float x, float z, int tex_index, int format, 
		float strength) = 0;
	// ����ָ�������ͼ
	virtual bool EraseTexBlend(float x, float z, int tex_index) = 0;
	// �������л����ͼ
	virtual bool EraseAllBlend(float x, float z) = 0;
	// ĳ�����ͼ��Ϸ����仯
	virtual bool SetBlendChanged(float x, float z) = 0;
	// ĳ����Χ�ڵĻ����ͼ�����仯
	virtual bool RangeBlendChanged(float beg_x, float beg_z, float end_x, 
		float end_z) = 0;

	// ���ָ�������ڵ�����������
	virtual PERSISTID GetZoneVisual(int zone_index, const char* name) = 0;
	// ���ָ�������ڵ�����������
	virtual const char* GetZoneVisualName(int zone_index, const PERSISTID& id) = 0;
	// ���ָ�����������еĵ�������
	virtual void GetZoneVisualList(const IVarList& args, IVarList& result) = 0;
	// ���ָ����Χ�ڵ����е�������
	virtual void GetRectVisualList(const IVarList& args, IVarList& result) = 0;
	// �����Ļ�ռ�ָ����Χ�ڵ����е�������
	virtual void GetScreenVisualList(const IVarList& args, IVarList& result) = 0;

	// ���ˮ������
	virtual int GetWater(float x, float z) = 0;
	// ����ˮ������
	virtual bool SetWater(float x, float z, int water_index) = 0;
	// �����Ƿ���Ҫ����ˮ��
	virtual bool TestWater(float x, float z, int water_index) = 0;

	// ��û�������
	virtual void GetGrass(const IVarList& args, IVarList& result) = 0;
	// ��ӻ���
	virtual bool AddGrass(int grass_index, float x, float z, int scale, int amount, 
		int seed) = 0;
	// ɾ������
	virtual bool RemoveGrass(int grass_index, float x, float z) = 0;
	// ɾ�����η�Χ�ڵĻ���
	virtual int RemoveGrassRect(int grass_index, float x1, float z1, float x2, 
		float z2) = 0;
	// ɾ��Բ�η�Χ�ڵĻ���
	virtual int RemoveGrassCircle(int grass_index, float x, float z, float radius) = 0;
	// ��÷��η�Χ�ڵĻ�������
	virtual int GetGrassCountRect(int grass_index, float x1, float z1, float x2, 
		float z2) = 0;
	// ���Բ�η�Χ�ڵĻ�������
	virtual int GetGrassCountCircle(int grass_index, float x, float z, 
		float radius) = 0;
	// ��÷��η�Χ�ڵĻ�������
	virtual void GetGrassRect(const IVarList& args, IVarList& result) = 0;
	// ���Բ�η�Χ�ڵĻ�������
	virtual void GetGrassCircle(const IVarList& args, IVarList& result) = 0;

	// ���÷�Χ�����ɫ
	virtual bool SetRegionColor(const char* name, const char* color) = 0;
	// ��÷�Χ�����ɫ
	virtual result_string GetRegionColor(const char* name) = 0;
	// ����ָ��λ�õķ�Χ���
	virtual bool SetRegionEnable(const char* name, float x, float z, bool value) = 0;
	// �ı䷶Χ���ƣ��༭���ã�
	virtual bool ChangeRegionName(float x, float z, const char* ori_name, 
		const char* new_name) = 0;

	// ���õ��������ɫ
	virtual bool SetAreaColor(const char* name, const char* color) = 0;
	// ��õ��������ɫ
	virtual result_string GetAreaColor(const char* name) = 0;
	// ����ָ��λ�õĵ������
	virtual bool SetAreaEnable(const char* name, float x, float z, bool value) = 0;
	// ���ָ��λ�õ����е������
	virtual bool ClearAreaEnable(float x, float z) = 0;
	// �ı����������ƣ��༭���ã�
	virtual bool ChangeAreaName(float x, float z, const char* ori_name, 
		const char* new_name) = 0;
	// �ı����������Ʊ��༭���ã�ȫ����ChangeAreaName����ã�
	virtual bool ChangeAreaNameTable(const char* ori_name, const char* new_name) = 0;
	// ɾ��ָ����������ɫ��Ϣ���༭���ã�
	virtual bool RemoveAreaColor(const char* name) = 0;

	// ����ָ������Ķ���߶�����
	virtual bool SaveHeight(int zone_index, const char* file_name) = 0;
	// ����ѹ������ָ������Ķ���߶�����
	virtual bool SaveHeightCompress(int zone_index, const char* file_name) = 0;
	// ����ָ�������ˮ�������Ϣ
	virtual bool SaveWater(int zone_index, const char* file_name) = 0;
	// ����ָ������Ĺ���ͼ��������ͼƬ
	//bool SaveDiffuseMap(int zone_index, const char* file_name) = 0;
	// ����ָ������Ļ�����ͼ��Ϣ
	virtual bool SaveAlphaBase(int zone_index, const char* file_name) = 0;
	// ����ָ����������ջ����ͼ
	virtual bool SaveAlphaMap(int zone_index, const char* file_name) = 0;
	// ����ָ������ĵ���������Ϣ
	virtual bool SaveVisuals(int zone_index, const char* file_name, 
		const char* widget_file, const char* helper_file, 
		const char* collide_file, const char* desc_file, 
		const char* sound_file) = 0;
	// ����ָ������Ļ�����Ϣ
	virtual bool SaveGrass(int zone_index, const char* file_name) = 0;
	// ����ָ������Ŀͻ���������Ϣ
	virtual bool SaveWalk(int zone_index, const char* file_name) = 0;
	// ����ָ������Ŀ�����ͼ
	virtual bool SaveWalkMap(int zone_index, const char* file_name) = 0;
	// ����ָ������ĵر�����ɫ
	//bool SaveBlendColors(int zone_index, const char* file_name) = 0;
	// ����ָ������Ĺ���ͼ
	//bool SaveLightMap(int zone_index, const char* file_name) = 0;
	// ����ָ������ķ�Χ�����Ϣ
	virtual bool SaveRegion(int zone_index, const char* file_name) = 0;
	// ����ָ������ĵ��������Ϣ
	virtual bool SaveArea(int zone_index, const char* file_name) = 0;

	// ѹ��ָ������ĸ߶������ļ�
	virtual bool CompressZoneHeight(const char* src_file, const char* dst_file) = 0;

	// �������������ײ���߲��
	virtual bool GenWalkAddPermit(int zone_index, float x, float y, float z) = 0;
	// ��ӽ�ֹ������ײ���߲�ķ�Χ
	virtual bool GenWalkAddForbid(int zone_index, float x, float y, float z, 
		float radius) = 0;
	// ��ӳ�����Ч��Χդ����
	virtual bool GenWalkAddFence(int fence_index, float x, float z) = 0;
	// ��ʼ������ײ��Ϣ
	virtual void GenWalkBegin(const IVarList& args, IVarList& result) = 0;
	// ���ɵر�߶�
	virtual bool GenWalkGround(int zone_index) = 0;
	// �����ϰ���߶�
	virtual bool GenWalkBalk(int zone_index, int row) = 0;
	// ���ɿ���������
	virtual bool GenWalkable(int zone_index, float start_x, float start_y, 
		float start_z) = 0;
	// ���ɿ����߲�߶�
	virtual bool GenWalkFloor(int zone_index) = 0;
	// ����ǽ����
	virtual bool GenWalkWall(int zone_index) = 0;
	// ����ˮ����Ϣ
	virtual bool GenWalkWater(int zone_index) = 0;
	// ����������ײ��Ϣ
	virtual bool GenWalkEnd(int zone_index) = 0;
};

#endif // _VISUAL_I_TERRAIN_EDITOR_H
