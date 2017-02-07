//--------------------------------------------------------------------
// �ļ���:		i_terrain.h
// ��  ��:		
// ˵  ��:		
// ��������:	2010��8��9��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _VISUAL_I_TERRAIN_H
#define _VISUAL_I_TERRAIN_H

#include "i_vis_base.h"
//#include "i_dx_render.h"

// ���ζ������

class ITerrain: public IVisBase
{
public:
	// ��ײ�������
	enum
	{
		TRACEMASK_GROUND = 0x1,
		TRACEMASK_MODEL = 0x2,
		TRACEMASK_PARTICLE = 0x4,
		TRACEMASK_LIGHT = 0x8,
		TRACEMASK_SOUND = 0x10,
		TRACEMASK_ACTOR = 0x20,
		TRACEMASK_TRIGGER = 0x80,
		TRACEMASK_ROLE = 0x100,
		TRACEMASK_HELPER = 0x200,
		TRACEMASK_THROUGH = 0x400,
		TRACEMASK_EFFECTMODEL = 0x800,
		TRACEMASK_VISUAL = TRACEMASK_MODEL | TRACEMASK_PARTICLE
			| TRACEMASK_LIGHT | TRACEMASK_SOUND | TRACEMASK_ACTOR 
			| TRACEMASK_TRIGGER | TRACEMASK_EFFECTMODEL,
	};
	
	// ����ֵ
	enum 
	{ 
		RESULT_SUCCESS = 0, 
		RESULT_CONTEXT_NOT_MATCH, 
		RESULT_ZONE_NOT_EXIST, 
		RESULT_VISUAL_HAD_EXIST, 
		RESULT_VISUAL_NOT_EXIST, 

		RESULT_UNKOWN,
	};
	
public:
	// ��ü��ذٷֱ�
	virtual float GetLoadPercent() = 0;

	// ��õ��α༭������
	virtual PERSISTID GetEditorID() = 0;
	// ��õ���ˢ�Ӷ���
	virtual PERSISTID GetBrushID() = 0;
	// ��õ��β��ʶ���
	virtual PERSISTID GetMaterialID() = 0;

	// ��ǰ��ҽ�ɫ����
	virtual void SetPlayerID(const PERSISTID& value) = 0;
	virtual PERSISTID GetPlayerID() const = 0;

	// ��ն���
	virtual void SetSkyID(const PERSISTID& value) = 0;
	virtual PERSISTID GetSkyID() const = 0;

	// ���ÿ����������Ķ���������
	virtual size_t GetZoneScale() const = 0;
	virtual int GetZoneScaleAttr() const = 0;
	// ���ÿ���ֿ�Ķ���������
	virtual size_t GetChunkScale() const = 0;
	virtual int GetChunkScaleAttr() const = 0;
	// ��õ�����ͼ�Ĵ�С
	virtual int GetTextureScale() const = 0;
	// �����ͼ�ظ��ĸ��ӷ�Χ
	virtual int GetTextureUnits() const = 0;
	// ��ö���֮���ALPHAֵ����
	virtual int GetAlphaPerUnit() const = 0;
	// ���ÿ���ֿ��ALPHA��ͼ�ߴ�
	virtual size_t GetAlphaMapScale() const = 0;
	virtual int GetAlphaMapScaleAttr() const = 0;
	// ���ÿ�����������ALPHAֵ������
	virtual size_t GetAlphaScale() const = 0;
	virtual int GetAlphaScaleAttr() const = 0;
	// ���LOD������
	virtual size_t GetLodLevels() const = 0;
	virtual int GetLodLevelsAttr() const = 0;
	// �����ײ�ֱ���
	virtual size_t GetCollidePerUnit() const = 0;
	virtual int GetCollidePerUnitAttr() const = 0;
	// �����ײ��λ��ߴ�
	virtual float GetCollideUnitSize() const = 0;
	
	virtual int GetZoneRows() const= 0;
	virtual int GetZoneCols() const= 0;
	virtual int GetZoneOriginRow() const= 0;
	virtual int GetZoneOriginCol() const= 0;
	virtual int GetZoneRange() const= 0;
	
	// �༭ģʽ
	virtual void SetDesignMode(bool value) = 0;
	virtual bool GetDesignMode() const = 0;
	virtual bool GetDesignModeAttr() const = 0;

	// ��ʾ��������ķ�ʽ
	virtual void SetShowDesignLine(int value) = 0;
	virtual int GetShowDesignLine() const = 0;

	// �Ƿ���ʾ���߷�Χ
	virtual void SetShowWalkable(bool value) = 0;
	virtual bool GetShowWalkable() const = 0;

	// �Ƿ���ʾ���߿ռ�߶�
	virtual void SetShowSpaceHeight(bool value) = 0;
	virtual bool GetShowSpaceHeight() const = 0;
	// �Ƿ���ʾ���Դ��Χ
	virtual void SetShowLightRange(bool value) = 0;
	virtual bool GetShowLightRange() const = 0;
	
	// �Ƿ���ʾ��Χ���
	virtual void SetShowRegion(bool value) = 0;
	virtual bool GetShowRegion() const = 0;

	// ��ʾָ����Χ�����
	virtual void SetShowRegionName(const char* value) = 0;
	virtual const char* GetShowRegionName() const = 0;

	// �Ƿ���ʾ�������
	virtual void SetShowArea(bool value) = 0;
	virtual bool GetShowArea() const = 0;

	// �Ƿ���ʾͼ��ģ��
	virtual void SetShowIcon(bool value) = 0;
	virtual bool GetShowIcon() const = 0;

	// �Ƿ���ʾ��ײ����
	virtual void SetShowCollide(bool value) = 0;
	virtual bool GetShowCollide() const = 0;

	// �Ƿ�����ʾ�༭�÷���䲻����
	virtual void SetShowDesignNoEdge(bool value) = 0;
	virtual bool GetShowDesignNoEdge() const = 0;

	// �Ƿ���ʾ��
	virtual void SetShowTree(bool value) = 0;
	virtual bool GetShowTree() const = 0;

	// �Ƿ���ʾ������Ĳ����ṹ
	virtual void SetShowQuadTree(bool value) = 0;
	virtual bool GetShowQuadTree() const = 0;

	// Զ����ȱʡLOD����
	virtual void SetFarLodLevel(int value) = 0;
	virtual int GetFarLodLevel() const = 0;

	// ��Ҫ����LOD����ķ�Χ�뾶
	virtual void SetLodRadius(float value) = 0;
	virtual float GetLodRadius() const = 0;

	// ����Ҫ����LOD����ķ�Χ�뾶
	virtual void SetNoLodRadius(float value) = 0;
	virtual float GetNoLodRadius() const = 0;

	// �༭�뾶
	virtual void SetDesignRadius(float value)  = 0;
	virtual float GetDesignRadius() const = 0;

	// ��ײ��Ϣ��ʾ�뾶
	virtual void SetCollideRadius(float value) = 0;
	virtual float GetCollideRadius() const = 0;

	// ���ͼ�����뾶	
	virtual void SetAlphaFadeRadius(float value) = 0;
	virtual float GetAlphaFadeRadius() const = 0;
	virtual float GetAlphaFadeRadiusAttr() const = 0;
	
	// ���ͼ�����뾶������Ҫ�������ͼ��	
	virtual void SetAlphaHideRadius(float value)  = 0;
	virtual float GetAlphaHideRadius() const = 0;
	virtual float GetAlphaHideRadiusAttr() const = 0;

	// ���ݵ����뾶
	virtual void SetGrassRadius(float value) = 0;
	virtual float GetGrassRadius() const = 0;
	virtual float GetGrassRadiusAttr() const = 0;

	// ���ݾ���
	virtual void SetGrassLod(float value) = 0;
	virtual float GetGrassLod() const = 0;
	virtual float GetGrassLodAttr() const = 0;

	// �ɼ������LOD������ʼ����
	virtual void SetVisualLodBegin(float value) = 0;
	virtual float GetVisualLodBegin() const = 0;
	virtual float GetVisualLodBeginAttr() const = 0;
	
	// �ɼ������LOD�����������
	virtual void SetVisualLodEnd(float value) = 0;
	virtual float GetVisualLodEnd() const = 0;
	virtual float GetVisualLodEndAttr() const = 0;

	// �Ƿ�ɼ�
	virtual void SetWaterVisible(bool value) = 0;
	virtual bool GetWaterVisible() const = 0;
	virtual void SetGroundVisible(bool value) = 0;
	virtual bool GetGroundVisible() const = 0;
	virtual void SetVisualVisible(bool value) = 0;
	virtual bool GetVisualVisible() const = 0;
	virtual void SetHelperVisible(bool value) = 0;
	virtual bool GetHelperVisible() const = 0;
	
	// ֻ��ʾ��������
	virtual void SetWaterWire(bool value) = 0;
	virtual bool GetWaterWire() const  = 0;
	virtual void SetGroundWire(bool value) = 0;
	virtual bool GetGroundWire() const = 0;
	virtual void SetVisualWire(bool value) = 0;
	virtual bool GetVisualWire() const = 0;
	
	// �Ƿ񲻼��ع���
	virtual void SetNoLight(bool value) = 0;
	virtual bool GetNoLight() const = 0;

	// �Ƿ���Ƽ򻯵ر�
	virtual void SetSimpleGround(bool value) = 0;
	virtual bool GetSimpleGround() const = 0;

	// �Ƿ�ʹ�õ�ƽ�߼���
	virtual void SetHorizontalCulling(bool value) = 0;
	virtual bool GetHorizontalCulling() const = 0;

	// �Ƿ����������嵭��
	virtual void SetVisualFadeIn(bool value) = 0;
	virtual bool GetVisualFadeIn() const = 0;

	// �Ƿ���ƽ����ƽ��
	virtual void SetSmoothHorizontal(bool value) = 0;
	virtual bool GetSmoothHorizontal() const = 0;

	// �ر�LOD��������
	virtual void SetGroundLevelDown(int value) = 0;
	virtual int GetGroundLevelDown() const = 0;

	// ���ε�Ӱ����
	virtual void SetGroundEnvMap(int value) = 0;
	virtual int GetGroundEnvMap() const = 0;

	// �������嵹Ӱ����
	virtual void SetVisualEnvMap(int value) = 0;
	virtual int GetVisualEnvMap() const = 0;

	// �ؿ�����
	virtual void SetGroundSortOrder(int value) = 0;
	virtual int GetGroundSortOrder() const = 0;

	// ������������ʽ
	virtual void SetVisualSortOrder(int value) = 0;
	virtual int GetVisualSortOrder() const = 0;

	// ������ð뾶
	virtual void SetClipRadiusNear(float value) = 0;
	virtual float GetClipRadiusNear() const = 0;

	// ��Զ���ð뾶
	virtual void SetClipRadiusFar(float value) = 0;
	virtual float GetClipRadiusFar() const = 0;

	// ��С����뾶
	virtual void SetVisualRadiusSmall(float value) = 0;
	virtual float GetVisualRadiusSmall() const = 0;

	// �������뾶
	virtual void SetVisualRadiusBig(float value) = 0;
	virtual float GetVisualRadiusBig() const = 0;

	// ������صĸ��Ӿ���
	virtual void SetLoadRadiusAdd(float value) = 0;
	virtual float GetLoadRadiusAdd() const = 0;
	
	// ����ж�صĸ��Ӿ���
	virtual void SetUnloadRadiusAdd(float value) = 0;
	virtual float GetUnloadRadiusAdd() const = 0;

	// ���������ذ뾶
	virtual float GetLoadRadius() const = 0;
	// �������ж�ذ뾶
	virtual float GetUnloadRadius() const = 0;

	// ��Դ�ļ�����·��
	virtual void SetFilePath(const char* value) = 0;
	virtual const char* GetFilePath() const = 0;

	// ������ͼģ�͸���·��
	virtual void SetAppendPath(const char* value) = 0;
	virtual const char* GetAppendPath() const = 0;

	// ������������ļ�·��
	virtual void SetZoneLightPath(const char* value) = 0;
	virtual const char* GetZoneLightPath() const = 0;

	// ����ģ�͹����ļ�·��
	virtual void SetModelLightPath(const char* value) = 0;
	virtual const char* GetModelLightPath() const = 0;

	// �������ļ�·��
	virtual void SetWalkablePath(const char* value) = 0;
	virtual const char* GetWalkablePath() const = 0;

	// �����õĸ߶�ͼ
	virtual void SetHeightMap(const char* value) = 0;
	virtual const char* GetHeightMap() const = 0;

	// ���γ�ʼ�߶�
	virtual void SetInitHeight(float value) = 0;
	virtual float GetInitHeight() const = 0;
	
	// ���ر�LOD�������
	virtual void SetPixelError(float value) = 0;
	virtual float GetPixelError() const = 0;
	virtual float GetPixelErrorAttr() const = 0;

	// ����ͼ�ĳߴ�
	virtual void SetLightTexScale(int value) = 0;
	virtual int GetLightTexScale() const = 0;

	// ���������ɫ�ֱ���
	virtual void SetLightPerUnit(int value) = 0;
	virtual int GetLightPerUnit() const = 0;
	
	// ��λ�ߴ�
	virtual void SetUnitSize(float value) = 0;
	virtual float GetUnitSize() const = 0;
	virtual float GetUnitSizeAttr() const = 0;

	// �ƹ��־ģ��
	virtual void SetLightModelID(const PERSISTID& value) = 0;
	virtual PERSISTID GetLightModelID() const = 0;

	// ������־ģ��
	virtual void SetSoundModelID(const PERSISTID& value) = 0;
	virtual PERSISTID GetSoundModelID() const = 0;

	// ������Χ��־ģ��
	virtual void SetSoundRangeModelID(const PERSISTID& value) = 0;
	virtual PERSISTID GetSoundRangeModelID() const = 0;

	// ���ӱ�־ģ��
	virtual void SetParticleModelID(const PERSISTID& value) = 0;
	virtual PERSISTID GetParticleModelID() const = 0;

	// ��Ч��־ģ��
	virtual void SetEffectModelID(const PERSISTID& value) = 0;
	virtual PERSISTID GetEffectModelID() const = 0;

	// ��������־ģ��
	virtual void SetTriggerModelID(const PERSISTID& value) = 0;
	virtual PERSISTID GetTriggerModelID() const = 0;

	// ��ľ��־ģ��
	virtual void SetTreeModelID(const PERSISTID& value) = 0;
	virtual PERSISTID GetTreeModelID() const = 0;

	// �����
	virtual void Pick(const IVarList& args, IVarList& result) = 0;
	// ���������
	virtual void PickGround(const IVarList& args, IVarList& result) = 0;
	// ����ѡ��������
	virtual PERSISTID PickVisual(int x, int y, float distance) = 0; 
	// ����ѡ
	virtual bool InnerPick(int x, int y, float distance, trace_info_t& result) = 0;

	// ��ײ���Ի�ø߶�
	virtual float TraceHeight(float x, float z, float high_y, float low_y) = 0;
	// �������������Ķ����λ��
	virtual void TraceCollide(const IVarList& args, IVarList& result) = 0;
	// �������������Ķ���λ�ú���ͼ��
	virtual void TraceTexture(const IVarList& args, IVarList& result) = 0;
	// ���������Ƿ�����ײ
	virtual bool TraceHit(float x1, float y1, float z1, float x2, float y2, 
		float z2) = 0;
	// ���ָ��λ�õĸ߶�
	virtual float GetPosiY(float x, float z) = 0;

	// ��ײ�����Ƿ����
	virtual bool GetCollideEnable(float x, float z) = 0;
	// ���ָ��λ�õ����߶�
	virtual float GetApexHeight(float x, float z) = 0;
	// ���ָ��λ�õ����߶����ڵĲ�
	virtual int GetApexFloor(float x, float z) = 0;
	// �����Ϳ����߲�ĸ߶�
	virtual float GetBottomHeight(float x, float z) = 0;
	// �����Ϳ����߲�
	virtual int GetBottomFloor(float x, float z) = 0;
	// ����Ƿ������
	virtual bool GetWalkEnable(float x, float z) = 0;
	// ��ÿ����ߵĸ߶�
	virtual float GetWalkHeight(float x, float z) = 0;
	// ��ÿ����߲����
	virtual int GetWalkFloor(float x, float z) = 0;
	// ˮ���Ƿ����
	virtual bool GetWalkWaterExists(float x, float z) = 0;
	// ��þ�ȷ��ˮ��߶�
	virtual float GetWalkWaterHeight(float x, float z) = 0;
    // ���ˮ�沨�˵ľ�ȷ�߶�
	virtual float GetWaterWaveHeight(float x, float z) = 0;
	// ����ܵĲ�����
	virtual int GetFloorCount(float x, float z) = 0;
	// ��ò��Ƿ����
	virtual bool GetFloorExists(float x, float z, int floor) = 0;
	// ��ò��Ƿ���ƶ�
	virtual bool GetFloorCanMove(float x, float z, int floor) = 0;
	// ��ò��Ƿ��վ��
	virtual bool GetFloorCanStand(float x, float z, int floor) = 0;
	// ��ò�߶�
	virtual float GetFloorHeight(float x, float z, int floor) = 0;
	// ��ò�����ϰ��ռ�߶�
	virtual float GetFloorSpace(float x, float z, int floor) = 0;
	// ��ò��Ƿ���ǽ��
	virtual bool GetFloorHasWall(float x, float z, int floor) = 0;
	// ���ָ���߶�λ���Ƿ����ǽ��
	virtual bool GetWallExists(float x, float y, float z) = 0;
	// ���ǽ�ڷ��߷���
	virtual void GetWalkWallAngle(const IVarList& args, IVarList& result) = 0;
	// ���ǽ����ײ
	virtual void GetWalkWallCollide(const IVarList& args, IVarList& result) = 0;
	// ������߾�����������ײ�����������
	virtual void GetLineTraverse(const IVarList& args, IVarList& result) = 0;
	// �����������ײ����״��ཻλ��
	virtual void GetLineIntersect(const IVarList& args, IVarList& result) = 0;

	// ���ָ�����ֵĵ�������
	virtual PERSISTID GetVisual(const char* name) = 0;
	// ��鳡���Ƿ����ָ��������
	virtual bool GetVisualExists(const PERSISTID& id) = 0;

	// ���ָ���뾶�ڵ����е�������
	virtual void GetCircleVisualList(const IVarList& args, IVarList& result) = 0;

	// ��ӵ�������
	virtual bool AddVisual(const char* name, const PERSISTID& id) = 0;
	// ��������ɫ
	virtual bool AddVisualRole(const char* name, const PERSISTID& id) = 0;
	// �Ƴ���������
	virtual bool RemoveVisual(const PERSISTID& id) = 0;
	// ���¶�λ���������λ��
	virtual bool RelocateVisual(const PERSISTID& id, float x, float y, float z)= 0;
	// �������嵱ǰ�Ƿ��ڿɼ���Χ��
	virtual bool VisualInView(const PERSISTID& id)= 0;
	// ��������ļ��þ���
	virtual bool SetVisualClipRadius(const PERSISTID& id, float clip_radius) = 0;
	// ��ñ�����͸�������ָ����Χ�ڵ������������
	virtual void GetNearActorList(const IVarList& args, IVarList& result) = 0;
	// ������巶Χ�ڵĻ��������
	virtual int GetRolesInSphere(float x, float y, float z, float radius) = 0;
	
	// ����������ͼ·��
	virtual bool AddTexturePath(const char* value) = 0;
	virtual bool ClearTexturePath() = 0;
	virtual int GetTexturePathCount() const = 0;
	virtual void GetTexturePathList(const IVarList& args, IVarList& result) const = 0;
	virtual const char* GetTexturePathByIndex(size_t index) const = 0;
	virtual const char* GetTexturePathString() const = 0;
	
	// ������ͼ����
	virtual int GetBaseTexIndex(const char* name) = 0;
	virtual bool FindBaseTex(const char* name) = 0;
	virtual bool AddBaseTex(const char* name, const char* path) = 0;
	virtual bool ClearBaseTex() = 0;
	virtual int GetBaseTexCount() = 0;
	virtual void GetBaseTexList(const IVarList& args, IVarList& result) = 0;
	virtual const char* GetBaseTexName(int index) = 0;
	virtual const char* GetBaseTexPath(int index) = 0;
	virtual bool SetBaseTexSpecularLevel(int index, float value) = 0;
	virtual float GetBaseTexSpecularLevel(int index) = 0;
	virtual bool SetBaseTexSpecularPower(int index, float value) = 0;
	virtual float GetBaseTexSpecularPower(int index) = 0;
	virtual bool SetBaseTexHeightScale(int index, float value) = 0;
	virtual float GetBaseTexHeightScale(int index) = 0;

	// ������ͼ��ʽ����(ֻ��һ����ʽ).
	virtual int GetBaseFormatScaleU(const char* name, int index) = 0;
	virtual int GetBaseFormatScaleV(const char* name, int index) = 0;
	virtual int GetBaseFormatAngleX(const char* name, int index) = 0;
	virtual int GetBaseFormatAngleY(const char* name, int index) = 0;
	virtual int GetBaseFormatAngleZ(const char* name, int index) = 0;
	virtual bool SetBaseFormatScaleU(const char* name, int index, int value)  = 0;
	virtual bool SetBaseFormatScaleV(const char* name, int index, int value)  = 0;
	virtual bool SetBaseFormatAngleX(const char* name, int index, int value)  = 0;
	virtual bool SetBaseFormatAngleY(const char* name, int index, int value)  = 0;
	virtual bool SetBaseFormatAngleZ(const char* name, int index, int value) = 0;
	
	// �����ͼ����
	virtual int GetBlendTexIndex(const char* name) = 0;
	virtual bool FindBlendTex(const char* name) = 0;
	virtual bool AddBlendTex(const char* name, const char* path) = 0;
	virtual bool ClearBlendTex() = 0;
	virtual int GetBlendTexCount() = 0;
	virtual void GetBlendTexList(const IVarList& args, IVarList& result) = 0;
	virtual const char* GetBlendTexName(int index) = 0;
	virtual const char* GetBlendTexPath(int index) = 0;
	virtual bool SetBlendTexSpecularLevel(int index, float value) = 0;
	virtual float GetBlendTexSpecularLevel(int index) = 0;
	virtual bool SetBlendTexSpecularPower(int index, float value) = 0;
	virtual float GetBlendTexSpecularPower(int index) = 0;
	virtual bool SetBlendTexHeightScale(int index, float value) = 0;
	virtual float GetBlendTexHeightScale(int index) = 0;

	// �����ͼ��ʽ
	virtual bool AddBlendFormat(const char* name, int scale_u, int scale_v,
		int angle_x, int angle_y, int angle_z) = 0;
	virtual int GetBlendFormatCount(const char* name) = 0;
	virtual int GetBlendFormatScaleU(const char* name, int index) = 0;
	virtual int GetBlendFormatScaleV(const char* name, int index) = 0;
	virtual int GetBlendFormatAngleX(const char* name, int index) = 0;
	virtual int GetBlendFormatAngleY(const char* name, int index) = 0;
	virtual int GetBlendFormatAngleZ(const char* name, int index) = 0;
	virtual bool SetBlendFormatScaleU(const char* name, int index, int value) = 0;
	virtual bool SetBlendFormatScaleV(const char* name, int index, int value) = 0;
	virtual bool SetBlendFormatAngleX(const char* name, int index, int value) = 0;
	virtual bool SetBlendFormatAngleY(const char* name, int index, int value) = 0;
	virtual bool SetBlendFormatAngleZ(const char* name, int index, int value) = 0;

	// ȫ�ֻ�����ͼ
	virtual void SetGlobalEnvironmentMap(const char* name) = 0;
	virtual const char* GetGlobalEnvironmentMap() const = 0;

	// �Ƿ���ˮ�沨��
	virtual void SetWaterWave(bool value) = 0;
	virtual bool GetWaterWave() const = 0;

	// �Ƿ���ˮ�����
	virtual void SetWaterDepth(bool value) = 0;
	virtual bool GetWaterDepth() const = 0;
	
	// �Ƿ���ˮ�尶�߹���
	virtual void SetWaterBorder(bool value) = 0;
	virtual bool GetWaterBorder() const = 0;

	// �Ƿ���ˮ�淴��
	virtual void SetWaterReflect(bool value) = 0;
	virtual bool GetWaterReflect() const = 0;

	// ˮ�滷����ͼ�Ƿ�����ظ�ʹ��
	virtual void SetWaterEnvMapReusable(bool value) = 0;
	virtual bool GetWaterEnvMapReusable() const = 0;

	// �Ƿ���ˮ�����ⷴ��
	virtual void SetWaterSunReflect(bool value) = 0;
	virtual bool GetWaterSunReflect() const = 0;

	// �Ƿ�Ҫ�������������λ��
	virtual void SetUpdateFocus(bool value) = 0;
	virtual bool GetUpdateFocus() const = 0;

	// �Ƿ�����õ���
	virtual void SetOnlyCullTerrain(bool value) = 0;
	virtual bool GetOnlyCullTerrain() const = 0;

	// �Ƿ�ʹ�ð��򻷾���
	virtual void SetSphereAmbient(bool value) = 0;
	virtual bool GetSphereAmbient() const = 0;

	// �Ƿ����ײ���Ľ������У��
	virtual void SetVerifyTraceResult(bool value) = 0;
	virtual bool GetVerifyTraceResult() const = 0;

	// ������������ʱ���Ƿ���м�¼
	virtual void SetGenWalkTrace(bool value) = 0;
	virtual bool GetGenWalkTrace() const = 0;

	// �������������λ��
	virtual bool SetFocusPosition(float x, float y, float z) = 0;
	// �������������뾶
	virtual bool SetFocusRadius(float x, float y, float z) = 0;

	// ˮ�����͹���
	virtual PERSISTID CreateWater(const char* name) = 0;
	virtual PERSISTID GetWater(const char* name) = 0;
	virtual bool DeleteWater(const char* name) = 0;
	virtual bool ClearWater() = 0;
	virtual const char* GetWaterName(int index) = 0;
	virtual int GetWaterIndex(const char* name) = 0;
	virtual int GetWaterCount() const = 0;
	virtual void GetWaterList(const IVarList& args, IVarList& result) const = 0;
	virtual bool RefreshWater() = 0;
	virtual bool SetWaterCubeMapStatic(bool value) = 0;
	virtual bool DumpWaterCubeMap(int index, const char* file_name) = 0;
	
	// �������͹���
	virtual PERSISTID CreateGrass(const char* name) = 0;
	virtual PERSISTID GetGrass(const char* name) = 0;
	virtual bool DeleteGrass(const char* name) = 0;
	virtual bool ClearGrass() = 0;
	virtual const char* GetGrassName(int index) = 0;
	virtual int GetGrassIndex(const char* name) = 0;
	virtual int GetGrassCount() const = 0;
	virtual void GetGrassList(const IVarList& args, IVarList& result) const = 0;
	virtual bool RefreshGrass() = 0;

	// ˢ�µ���
	virtual bool RefreshGround() = 0; 
	// ˢ�µ�������
	virtual bool RefreshVisual() = 0; 
	
	// ��ײ�������
	virtual bool SetTraceMask(const char* type, bool value) = 0;
	virtual bool GetTraceMask(const char* type) = 0;
	virtual void SetTraceMaskValue(unsigned int value) = 0;
	virtual unsigned int GetTraceMaskValue() const = 0;

	// ��õ��������б�
	virtual void GetAreaList(const IVarList& args, IVarList& result) = 0;
	// ���ָ��λ�õĵ����������
	virtual const char* GetAreaName(float x, float z) = 0;

	// ��ӵ�������
	virtual int AddVisBase(const char* name, IVisBase* pVisBase, 
		bool is_role, float clip_radius) = 0;
	// �Ƴ���������
	virtual int RemoveVisBase(IVisBase* pVisBase) = 0;
	// ���¶�λλ�÷����ı�ĵ�������
	virtual int RelocateVisBase(IVisBase* pVisBase, float x, float y, 
		float z) = 0;

	// ��õ��������б�
	virtual void GetRegionList(const IVarList& args, IVarList& result) = 0;
	// ���ָ��λ�õķ�Χ����Ƿ���Ч
	virtual bool GetRegionEnable(const char* name, float x, float z) = 0;

	// ��Ӷ�̬���
	virtual bool AddDynamicWalkable(IVisBase* pVisBase) = 0;
	// ɾ����̬���
	virtual bool DeleteDynamicWalkable(IVisBase* pVisBase) = 0;

		// ���õ��β���
	virtual bool SetParameter(size_t zone_scale, size_t chunk_scale, size_t tex_scale, 
		size_t tex_units, size_t alpha_per_unit, size_t collide_per_unit) = 0;
	// ��ʼ������
	virtual bool InitZones(size_t rows, size_t cols, size_t origin_row, 
		size_t origin_col, size_t range) = 0;

	// ����ɾ��֮ǰɾ�����еؿ�ļ�����
	virtual bool ReleaseAllZoneLoader()= 0;
};

#endif // _VISUAL_I_TERRAIN_H
