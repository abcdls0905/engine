//--------------------------------------------------------------------
// �ļ���:		terrain_editor.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��8��17��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _TERRAIN_EDITOR_H
#define _TERRAIN_EDITOR_H

#include "../visual/i_render.h"
#include "../visual/i_terrain_editor.h"
#include "height_loader.h"

// ���α༭
class TerrainEditor: public ITerrainEditor
{
public:
	TerrainEditor();
	virtual ~TerrainEditor();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	void SetTerrain(Terrain* value);

	// ��õ����ڵ���������ֵ
	int GetZoneIndex(float x, float z);
	// ���������к�
	int GetZoneRow(int zone_index);
	// ���������к�
	int GetZoneCol(int zone_index);
	// ������������
	const char* GetZoneName(int zone_index);
	// �������ĵ�����͵�߶�
	float GetZoneMinY(int zone_index);
	// �������ĵ�����ߵ�߶�
	float GetZoneMaxY(int zone_index);
	// ������ڴ����������������ֵ
	void GetZoneList(const IVarList& args, IVarList& result);
	// ���ָ����Χ����ص���������ֵ��
	void GetRangeZoneList(const IVarList& args, IVarList& result);
	// ָ������߶��ļ��Ƿ����
	bool GetZoneExists(int zone_index);
	// ��ô����ļ�����������
	void GetExistZoneList(const IVarList& args, IVarList& result);
	// ����ָ����������ݸı��־
	bool SetZoneChanged(int zone_index, const char* type, bool value);
	// ���ָ����������ݸı��־
	bool GetZoneChanged(int zone_index, const char* type);
	// ����ָ������ж��
	bool SetZoneNoUnload(int zone_index, bool value);
	// ���ָ�������Ƿ�ж��
	bool GetZoneNoUnload(int zone_index);
	// ָ���ĵ��Ƿ��ڿɱ༭��Χ֮��
	bool GetInDesign(float x, float z);

	// ��������������������ƽ�淶Χ
	void GetZoneRect(const IVarList& args, IVarList& result);
	// ��������ƽ�淶Χ
	void GetVisualRect(const IVarList& args, IVarList& result);
	// ���ƽ�淶Χ�е����е��Դ
	void GetRectLightList(const IVarList& args, IVarList& result);

	// ����ָ����λ���Ƿ���ʾ�ֿ�İ�Χ��
	bool SetShowChunkBoundBox(float x, float z, bool value);
	
	// ��������λ�ø߶�
	bool SetPosiY(float x, float z, float y);
	// ����ָ��λ�ø߶�
	bool IncPosiY(float x, float z, float y);
	// ĳ��ĸ߶ȷ����仯
	bool SetHeightChanged(float x, float z);
	// ĳ����Χ�ڵĸ߶ȷ����仯
	bool RangeHeightChanged(float beg_x, float beg_z, float end_x, 
		float end_z);
	
	// ��û�����ͼ
	int GetTexBase(float x, float z);
	// ���û�����ͼ
	bool SetTexBase(float x, float z, int tex_index);

	// ��û����ͼ�ڵ�ǰλ�õ�ֵ
	float GetTexBlend(float x, float z, int tex_index, int format);
	// ��û����ͼ�ڵ�ǰλ�õ����ǰֵ
	void GetTexBlendPara(const IVarList& args, IVarList& result);
	// ��õ�ǰλ�����ֵ�Ļ����ͼ����
	void GetMaxBlendPara(const IVarList& args, IVarList& result);
	// ���ָ��λ�õĻ����ͼ����
	int GetTexBlendLayerCount(float x, float z);
	// ���ָ��λ�õ�ָ����Ļ����ͼ����
	int GetTexBlendLayerTexIndex(float x, float z, int index);
	// ���ָ��λ�õ�ָ����Ļ����ͼ��ʽ
	int GetTexBlendLayerFormat(float x, float z, int index);
	// ����ָ��λ�õ�ָ����Ļ��ͼ����ͼ
	bool CopyTexBlendLayer(float x, float z, int tex_index, int format,
		const char* tex_name);
	// ����ָ��λ�õ�ָ����ϲ�
	bool EraseTexBlendLayer(float x, float z, int tex_index, int format);
	// ��ָ��λ�ò���ָ�������ͼ��
	bool FindTexBlendLayer(float x, float z, int tex_index, int format);
	// ���û����ͼ��ֵ
	bool SetTexBlend(float x, float z, int tex_index, int format, 
		float strength);
	// ����ָ�������ͼ
	bool EraseTexBlend(float x, float z, int tex_index);
	// �������л����ͼ
	bool EraseAllBlend(float x, float z);
	// ĳ�����ͼ��Ϸ����仯
	bool SetBlendChanged(float x, float z);
	// ĳ����Χ�ڵĻ����ͼ�����仯
	bool RangeBlendChanged(float beg_x, float beg_z, float end_x, 
		float end_z);

	// ���ָ�������ڵ�����������
	PERSISTID GetZoneVisual(int zone_index, const char* name);
	// ���ָ�������ڵ�����������
	const char* GetZoneVisualName(int zone_index, const PERSISTID& id);
	// ���ָ�����������еĵ�������
	void GetZoneVisualList(const IVarList& args, IVarList& result);
	// ���ָ����Χ�ڵ����е�������
	void GetRectVisualList(const IVarList& args, IVarList& result);
	// �����Ļ�ռ�ָ����Χ�ڵ����е�������
	void GetScreenVisualList(const IVarList& args, IVarList& result);

	// ���ˮ������
	int GetWater(float x, float z);
	// ����ˮ������
	bool SetWater(float x, float z, int water_index);
	// �����Ƿ���Ҫ����ˮ��
	bool TestWater(float x, float z, int water_index);
	
	// ��û�������
	void GetGrass(const IVarList& args, IVarList& result);
	// ��ӻ���
	bool AddGrass(int grass_index, float x, float z, int scale, int amount, 
		int seed);
	// ɾ������
	bool RemoveGrass(int grass_index, float x, float z);
	// ɾ�����η�Χ�ڵĻ���
	int RemoveGrassRect(int grass_index, float x1, float z1, float x2, 
		float z2);
	// ɾ��Բ�η�Χ�ڵĻ���
	int RemoveGrassCircle(int grass_index, float x, float z, float radius);
	// ��÷��η�Χ�ڵĻ�������
	int GetGrassCountRect(int grass_index, float x1, float z1, float x2, 
		float z2);
	// ���Բ�η�Χ�ڵĻ�������
	int GetGrassCountCircle(int grass_index, float x, float z, 
		float radius);
	// ��÷��η�Χ�ڵĻ�������
	void GetGrassRect(const IVarList& args, IVarList& result);
	// ���Բ�η�Χ�ڵĻ�������
	void GetGrassCircle(const IVarList& args, IVarList& result);

	// ���÷�Χ�����ɫ
	bool SetRegionColor(const char* name, const char* color);
	// ��÷�Χ�����ɫ
	result_string GetRegionColor(const char* name);
	// ����ָ��λ�õķ�Χ���
	bool SetRegionEnable(const char* name, float x, float z, bool value);
	// �ı䷶Χ���ƣ��༭���ã�
	bool ChangeRegionName(float x, float z, const char* ori_name, 
		const char* new_name);

	// ���õ��������ɫ
	bool SetAreaColor(const char* name, const char* color);
	// ��õ��������ɫ
	result_string GetAreaColor(const char* name);
	// ����ָ��λ�õĵ������
	bool SetAreaEnable(const char* name, float x, float z, bool value);
	// ���ָ��λ�õ����е������
	bool ClearAreaEnable(float x, float z);
	// �ı����������ƣ��༭���ã�
	bool ChangeAreaName(float x, float z, const char* ori_name, 
		const char* new_name);
	// �ı����������Ʊ��༭���ã�ȫ����ChangeAreaName����ã�
	bool ChangeAreaNameTable(const char* ori_name, const char* new_name);
	// ɾ��ָ����������ɫ��Ϣ���༭���ã�
	bool RemoveAreaColor(const char* name);

	// ����ָ������Ķ���߶�����
	bool SaveHeight(int zone_index, const char* file_name);
	// ����ѹ������ָ������Ķ���߶�����
	bool SaveHeightCompress(int zone_index, const char* file_name);
	// ����ָ�������ˮ�������Ϣ
	bool SaveWater(int zone_index, const char* file_name);
	// ����ָ������Ĺ���ͼ��������ͼƬ
	//bool SaveDiffuseMap(int zone_index, const char* file_name);
	// ����ָ������Ļ�����ͼ��Ϣ
	bool SaveAlphaBase(int zone_index, const char* file_name);
	// ����ָ����������ջ����ͼ
	bool SaveAlphaMap(int zone_index, const char* file_name);
	// ����ָ������ĵ���������Ϣ
	bool SaveVisuals(int zone_index, const char* file_name, 
		const char* widget_file, const char* helper_file, 
		const char* collide_file, const char* desc_file, 
		const char* sound_file);
	// ����ָ������Ļ�����Ϣ
	bool SaveGrass(int zone_index, const char* file_name);
	// ����ָ������Ŀͻ���������Ϣ
	bool SaveWalk(int zone_index, const char* file_name);
	// ����ָ������Ŀ�����ͼ
	bool SaveWalkMap(int zone_index, const char* file_name);
	// ����ָ������ĵر�����ɫ
	//bool SaveBlendColors(int zone_index, const char* file_name);
	// ����ָ������Ĺ���ͼ
	//bool SaveLightMap(int zone_index, const char* file_name);
	// ����ָ������ķ�Χ�����Ϣ
	bool SaveRegion(int zone_index, const char* file_name);
	// ����ָ������ĵ��������Ϣ
	bool SaveArea(int zone_index, const char* file_name);

	// �����߶����ݶ�ȡ
	static IBlockReader* CreateHeightReader(int block_id, void* context);
	// ѹ��ָ������ĸ߶������ļ�
	bool CompressZoneHeight(const char* src_file, const char* dst_file);

	// �������������ײ���߲��
	bool GenWalkAddPermit(int zone_index, float x, float y, float z);
	// ��ӽ�ֹ������ײ���߲�ķ�Χ
	bool GenWalkAddForbid(int zone_index, float x, float y, float z, 
		float radius);
	// ��ӳ�����Ч��Χդ����
	bool GenWalkAddFence(int fence_index, float x, float z);
	// ��ʼ������ײ��Ϣ
	void GenWalkBegin(const IVarList& args, IVarList& result);
	// ���ɵر�߶�
	bool GenWalkGround(int zone_index);
	// �����ϰ���߶�
	bool GenWalkBalk(int zone_index, int row);
	// ���ɿ���������
	bool GenWalkable(int zone_index, float start_x, float start_y, 
		float start_z);
	// ���ɿ����߲�߶�
	bool GenWalkFloor(int zone_index);
	// ����ǽ����
	bool GenWalkWall(int zone_index);
	// ����ˮ����Ϣ
	bool GenWalkWater(int zone_index);
	// ����������ײ��Ϣ
	bool GenWalkEnd(int zone_index);

	// �ϲ�ָ�������С���ģ�͹���ͼ
	//bool GenLightMapAtlas(int zone_index, int atlas_map_size, 
	//	int min_light_map_size, int small_file_size);
	
private:
	TerrainEditor(const TerrainEditor&);
	TerrainEditor& operator=(const TerrainEditor&);

	CTerrainZone* InnerGetZone(int zone_index);
	// ���ָ������AlphaMap�ϵ�λ��
	bool GetAlphaPosition(CTerrainZone* pZone, CTerrainChunk* pChunk,
		float x, float z, int& row, int& col);

private:
	Terrain* m_pTerrain;
	CZoneManager* m_pZoneManager;
	CWalkGenerator* m_pWalkGenerator;
	CHeightLoader::height_all_t m_Height;
	CHeightLoader::chunk_all_t m_Chunk;
};

#endif // _TERRAIN_EDITOR_H
