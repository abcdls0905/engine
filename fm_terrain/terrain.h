//--------------------------------------------------------------------
// �ļ���:		terrain.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��8��9��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _TERRAIN_H
#define _TERRAIN_H

#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"
#include "../visual/i_resource.h"
#include "../visual/i_terrain.h"
#include "../visual/i_render.h"

#include "name_list.h"

// �����ļ���ʶ��
#define TERRAIN_FILE_SIGNATURE "FxTer\n\n\n"
// �༭�õĺ꿪��
//#define TERRAIN_EDIT

// ����

class TerrainEditor;
class TerrainBrush;
class TerrainMaterial;
class CChunkStyle;
class CBlendTexSet;
class CTerrainHeight;
class CTerrainZone;
class CTerrainChunk;
class CTerrainCulling;
class CTerrainPainter;
class CTerrainWater;
class CTerrainGrass;
class CQuadCollide;
class CZoneManager;
class CVisualManager;
class CChunkVB;
class CLightMapConfig;
class CDynamicWalkLoader;
class CDynamicWalkableManager;

class Terrain: public ITerrain
{
public:
	// ������ͼ������ͼ���������
	enum { MAX_TEX_NUM = 256 };

	struct material_t
	{
		FmVec4 vDiffuse;
		FmVec4 vAmbient;
		FmVec4 vSpecular;
		FmVec4 vEmissive;
		float fSpecularPower;
		bool bSpecularEnable;
	};

private:
	struct area_color_t
	{
		char* pName;
		unsigned int nColor;
	};

public:
	Terrain();
	virtual ~Terrain();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// ���ó���������
	virtual void SetContext(IRenderContext* value);
	virtual IRenderContext* GetContext() const;

	// �������ת������
	virtual void GetWorldMatrix(FmMat4& mat); 

	virtual bool Load();
	
	virtual void Update(float seconds);
	virtual void Realize();
	virtual void RealizeShadowMap();

	// �Ƿ���ɼ��أ�ͬ�����첽��
	virtual bool GetLoadFinish();
	// ��ײ������ϸ��Ϣ
	virtual bool TraceDetail(const FmVec3& src, const FmVec3& dst,
		trace_info_t& result);
	// ��ײ�����Ƿ����赲
	virtual bool TraceHitted(const FmVec3& src, const FmVec3& dst);
	// ���巶Χ��ײ
	virtual bool TraceSphere(const FmVec3& center, float radius);
	// ��ѡ
	virtual bool Cull(const FmPlane* planes, size_t plane_num, 
		const FmVec3& center, float radius, CULL_FUNC cull_func, 
		void* context);
	
	// ��ˮ�淴����ͼ
	void RealizeReflection(size_t water_index);

	// ��ü��ذٷֱ�
	float GetLoadPercent();

	// ��õ��α༭������
	PERSISTID GetEditorID();
	// ��õ���ˢ�Ӷ���
	PERSISTID GetBrushID();
	// ��õ��β��ʶ���
	PERSISTID GetMaterialID();

	// ��ǰ��ҽ�ɫ����
	void SetPlayerID(const PERSISTID& value) { m_PlayerID = value; }
	PERSISTID GetPlayerID() const { return m_PlayerID; }

	// ��ն���
	void SetSkyID(const PERSISTID& value) { m_SkyID = value; }
	PERSISTID GetSkyID() const { return m_SkyID; }

	// ���ÿ����������Ķ���������
	size_t GetZoneScale() const { return m_nZoneScale; }
	int GetZoneScaleAttr() const { return (int)m_nZoneScale; }
	// ���ÿ���ֿ�Ķ���������
	size_t GetChunkScale() const { return m_nChunkScale; }
	int GetChunkScaleAttr() const { return (int)m_nChunkScale; }
	// ��õ�����ͼ�Ĵ�С
	int GetTextureScale() const { return (int)m_nTextureScale; }
	// �����ͼ�ظ��ĸ��ӷ�Χ
	int GetTextureUnits() const { return (int)m_nTextureUnits; }
	// ��ö���֮���ALPHAֵ����
	int GetAlphaPerUnit() const { return (int)m_nAlphaPerUnit; }
	// ���ÿ���ֿ��ALPHA��ͼ�ߴ�
	size_t GetAlphaMapScale() const { return m_nAlphaMapScale; }
	int GetAlphaMapScaleAttr() const { return (int)m_nAlphaMapScale; }
	// ���ÿ�����������ALPHAֵ������
	size_t GetAlphaScale() const { return m_nAlphaScale; }
	int GetAlphaScaleAttr() const { return (int)m_nAlphaScale; }
	// ���LOD������
	size_t GetLodLevels() const { return m_nLodLevels; }
	int GetLodLevelsAttr() const { return (int)m_nLodLevels; }
	// �����ײ�ֱ���
	size_t GetCollidePerUnit() const { return m_nCollidePerUnit; }
	int GetCollidePerUnitAttr() const { return (int)m_nCollidePerUnit; }
	// �����ײ��λ��ߴ�
	float GetCollideUnitSize() const { return m_fCollideSize; }
	
	int GetZoneRows() const;
	int GetZoneCols() const;
	int GetZoneOriginRow() const;
	int GetZoneOriginCol() const;
	int GetZoneRange() const;
	
	// �༭ģʽ
	void SetDesignMode(bool value) { m_bDesignMode = value; }
	bool GetDesignMode() const { return m_bDesignMode; }
	bool GetDesignModeAttr() const { return m_bDesignMode; }

	// ��ʾ��������ķ�ʽ
	void SetShowDesignLine(int value) { m_nShowDesignLine = value; }
	int GetShowDesignLine() const { return m_nShowDesignLine; }

	// �Ƿ���ʾ���߷�Χ
	void SetShowWalkable(bool value) { m_bShowWalkable = value; }
	bool GetShowWalkable() const { return m_bShowWalkable; }

	// �Ƿ���ʾ���߿ռ�߶�
	void SetShowSpaceHeight(bool value) { m_bShowSpaceHeight = value; }
	bool GetShowSpaceHeight() const { return m_bShowSpaceHeight; }

	// �Ƿ���ʾ���Դ��Χ
	void SetShowLightRange(bool value) { m_bShowLightRange = value; }
	bool GetShowLightRange() const { return m_bShowLightRange; }
	
	// �Ƿ���ʾ��Χ���
	void SetShowRegion(bool value) { m_bShowRegion = value; }
	bool GetShowRegion() const { return m_bShowRegion; }

	// ��ʾָ����Χ�����
	void SetShowRegionName(const char* value) { m_strShowRegionName = value; }
	const char* GetShowRegionName() const { return m_strShowRegionName.c_str(); }

	// �Ƿ���ʾ�������
	void SetShowArea(bool value) { m_bShowArea = value; }
	bool GetShowArea() const { return m_bShowArea; }

	// �Ƿ���ʾͼ��ģ��
	void SetShowIcon(bool value) { m_bShowIcon = value; }
	bool GetShowIcon() const { return m_bShowIcon; }

	// �Ƿ���ʾ��ײ����
	void SetShowCollide(bool value) { m_bShowCollide = value; }
	bool GetShowCollide() const { return m_bShowCollide; }

	// �Ƿ�����ʾ�༭�÷���䲻����
	void SetShowDesignNoEdge(bool value) { m_bShowDesignNoEdge = value; }
	bool GetShowDesignNoEdge() const { return m_bShowDesignNoEdge; }

	// �Ƿ���ʾ��
	void SetShowTree(bool value) { m_bShowTree = value; }
	bool GetShowTree() const { return m_bShowTree; }

	// �Ƿ���ʾ������Ĳ����ṹ
	void SetShowQuadTree(bool value) { m_bShowQuadTree = value; }
	bool GetShowQuadTree() const { return m_bShowQuadTree; }

	// Զ����ȱʡLOD����
	void SetFarLodLevel(int value) { m_nFarLodLevel = value; }
	int GetFarLodLevel() const { return m_nFarLodLevel; }

	// ��Ҫ����LOD����ķ�Χ�뾶
	void SetLodRadius(float value) { m_fLodRadius = value; }
	float GetLodRadius() const { return m_fLodRadius; }

	// ����Ҫ����LOD����ķ�Χ�뾶
	void SetNoLodRadius(float value) { m_fNoLodRadius = value; }
	float GetNoLodRadius() const { return m_fNoLodRadius; }

	// �༭�뾶
	void SetDesignRadius(float value) { m_fDesignRadius = value; }
	float GetDesignRadius() const { return m_fDesignRadius; }

	// ��ײ��Ϣ��ʾ�뾶
	void SetCollideRadius(float value) { m_fCollideRadius = value; }
	float GetCollideRadius() const { return m_fCollideRadius; }

	// ���ͼ�����뾶	
	void SetAlphaFadeRadius(float value) { m_fAlphaFadeRadius = value; }
	float GetAlphaFadeRadius() const { return m_fAlphaFadeRadius; }
	float GetAlphaFadeRadiusAttr() const { return m_fAlphaFadeRadius; }
	
	// ���ͼ�����뾶������Ҫ�������ͼ��	
	void SetAlphaHideRadius(float value) { m_fAlphaHideRadius = value; }
	float GetAlphaHideRadius() const { return m_fAlphaHideRadius; }
	float GetAlphaHideRadiusAttr() const { return m_fAlphaHideRadius; }

	// ���ݵ����뾶
	void SetGrassRadius(float value) { m_fGrassRadius = value; }
	float GetGrassRadius() const { return m_fGrassRadius; }
	float GetGrassRadiusAttr() const { return m_fGrassRadius; }

	// ���ݾ���
	void SetGrassLod(float value) { m_fGrassLod = value; }
	float GetGrassLod() const { return m_fGrassLod; }
	float GetGrassLodAttr() const { return m_fGrassLod; }

	// �ɼ������LOD������ʼ����
	void SetVisualLodBegin(float value) { m_fVisualLodBegin = value; }
	float GetVisualLodBegin() const { return m_fVisualLodBegin; }
	float GetVisualLodBeginAttr() const { return m_fVisualLodBegin; }
	
	// �ɼ������LOD�����������
	void SetVisualLodEnd(float value) { m_fVisualLodEnd = value; }
	float GetVisualLodEnd() const { return m_fVisualLodEnd; }
	float GetVisualLodEndAttr() const { return m_fVisualLodEnd; }

	// �Ƿ�ɼ�
	void SetWaterVisible(bool value);// { m_bWaterVisible = value; }
	bool GetWaterVisible() const { return m_bWaterVisible; }
	void SetGroundVisible(bool value) { m_bGroundVisible = value; }
	bool GetGroundVisible() const { return m_bGroundVisible; }
	void SetVisualVisible(bool value) { m_bVisualVisible = value; }
	bool GetVisualVisible() const { return m_bVisualVisible; }
	void SetHelperVisible(bool value) { m_bHelperVisible = value; }
	bool GetHelperVisible() const { return m_bHelperVisible; }
	
	// ֻ��ʾ��������
	void SetWaterWire(bool value) { m_bWaterWire = value; }
	bool GetWaterWire() const { return m_bWaterWire; }
	void SetGroundWire(bool value) { m_bGroundWire = value; }
	bool GetGroundWire() const { return m_bGroundWire; }
	void SetVisualWire(bool value) { m_bVisualWire = value; }
	bool GetVisualWire() const { return m_bVisualWire; }
	
	// �Ƿ񲻼��ع���
	void SetNoLight(bool value) { m_bNoLight = value; }
	bool GetNoLight() const { return m_bNoLight; }

	// �Ƿ���Ƽ򻯵ر�
	void SetSimpleGround(bool value) { m_bSimpleGround = value; }
	bool GetSimpleGround() const { return m_bSimpleGround; }

	// �Ƿ�ʹ�õ�ƽ�߼���
	void SetHorizontalCulling(bool value) { m_bHorizontalCulling = value; }
	bool GetHorizontalCulling() const { return m_bHorizontalCulling; }

	// �Ƿ����������嵭��
	void SetVisualFadeIn(bool value) { m_bVisualFadeIn = value; }
	bool GetVisualFadeIn() const { return m_bVisualFadeIn; }

	// �Ƿ���ƽ����ƽ��
	void SetSmoothHorizontal(bool value) { m_bSmoothHorizontal = value; }
	bool GetSmoothHorizontal() const { return m_bSmoothHorizontal; }

	// �ر�LOD��������
	void SetGroundLevelDown(int value) { m_nGroundLevelDown = value; }
	int GetGroundLevelDown() const { return m_nGroundLevelDown; }

	// ���ε�Ӱ����
	void SetGroundEnvMap(int value) { m_nGroundEnvMap = value; }
	int GetGroundEnvMap() const { return m_nGroundEnvMap; }

	// �������嵹Ӱ����
	void SetVisualEnvMap(int value) { m_nVisualEnvMap = value; }
	int GetVisualEnvMap() const { return m_nVisualEnvMap; }

	// �ؿ�����
	void SetGroundSortOrder(int value) { m_nGroundSortOrder = value; }
	int GetGroundSortOrder() const { return m_nGroundSortOrder; }

	// ������������ʽ
	void SetVisualSortOrder(int value) { m_nVisualSortOrder = value; }
	int GetVisualSortOrder() const { return m_nVisualSortOrder; }

	// ������ð뾶
	void SetClipRadiusNear(float value) { m_fClipRadiusNear = value; }
	float GetClipRadiusNear() const { return m_fClipRadiusNear; }

	// ��Զ���ð뾶
	void SetClipRadiusFar(float value) { m_fClipRadiusFar = value; }
	float GetClipRadiusFar() const { return m_fClipRadiusFar; }

	// ��С����뾶
	void SetVisualRadiusSmall(float value) { m_fVisualRadiusSmall = value; }
	float GetVisualRadiusSmall() const { return m_fVisualRadiusSmall; }

	// �������뾶
	void SetVisualRadiusBig(float value) { m_fVisualRadiusBig = value; }
	float GetVisualRadiusBig() const { return m_fVisualRadiusBig; }

	// ������صĸ��Ӿ���
	void SetLoadRadiusAdd(float value) { m_fLoadRadiusAdd = value; }
	float GetLoadRadiusAdd() const { return m_fLoadRadiusAdd; }
	
	// ����ж�صĸ��Ӿ���
	void SetUnloadRadiusAdd(float value) { m_fUnloadRadiusAdd = value; }
	float GetUnloadRadiusAdd() const { return m_fUnloadRadiusAdd; }

	// ���������ذ뾶
	float GetLoadRadius() const { return m_fClipRadiusFar + m_fLoadRadiusAdd; }
	// �������ж�ذ뾶
	float GetUnloadRadius() const { return GetLoadRadius() + m_fUnloadRadiusAdd; }

	// ��Դ�ļ�����·��
	void SetFilePath(const char* value) { m_strFilePath = value; }
	const char* GetFilePath() const { return m_strFilePath.c_str(); }

	// ������ͼģ�͸���·��
	void SetAppendPath(const char* value) 
  {
    m_strAppendPath = value; 
  }
	const char* GetAppendPath() const { return m_strAppendPath.c_str(); }

	// ������������ļ�·��
	void SetZoneLightPath(const char* value) { m_strZoneLightPath = value; }
	const char* GetZoneLightPath() const { return m_strZoneLightPath.c_str(); }

	// ����ģ�͹����ļ�·��
	void SetModelLightPath(const char* value) { m_strModelLightPath = value; }
	const char* GetModelLightPath() const { return m_strModelLightPath.c_str(); }

	// �������ļ�·��
	void SetWalkablePath(const char* value) { m_strWalkablePath = value; }
	const char* GetWalkablePath() const { return m_strWalkablePath.c_str(); }

	// ��̬�������ļ�·��
	void SetDynamicWalkablePath(const char* value) { m_strDynamicWalkablePath = value; }
	const char* GetDynamicWalkablePath() const { return m_strDynamicWalkablePath.c_str(); }

	// �����õĸ߶�ͼ
	void SetHeightMap(const char* value) { m_strHeightMap = value; }
	const char* GetHeightMap() const { return m_strHeightMap.c_str(); }

	// ���γ�ʼ�߶�
	void SetInitHeight(float value) { m_fInitHeight = value; }
	float GetInitHeight() const { return m_fInitHeight; }
	
	// ���ر�LOD�������
	void SetPixelError(float value) { m_fPixelError = value; }
	float GetPixelError() const { return m_fPixelError; }
	float GetPixelErrorAttr() const { return m_fPixelError; }

	// ����ͼ�ĳߴ�
	void SetLightTexScale(int value) { m_nLightTexScale = value; }
	int GetLightTexScale() const { return (int)m_nLightTexScale; }

	// ���������ɫ�ֱ���
	void SetLightPerUnit(int value) { m_nLightPerUnit = value; }
	int GetLightPerUnit() const { return (int)m_nLightPerUnit; }
	
	// ��λ�ߴ�
	void SetUnitSize(float value);
	float GetUnitSize() const { return m_fUnitSize; }
	float GetUnitSizeAttr() const { return m_fUnitSize; }

	// �ƹ��־ģ��
	void SetLightModelID(const PERSISTID& value);
	PERSISTID GetLightModelID() const;

	// ������־ģ��
	void SetSoundModelID(const PERSISTID& value);
	PERSISTID GetSoundModelID() const;

	// ������Χ��־ģ��
	void SetSoundRangeModelID(const PERSISTID& value);
	PERSISTID GetSoundRangeModelID() const;

	// ���ӱ�־ģ��
	void SetParticleModelID(const PERSISTID& value);
	PERSISTID GetParticleModelID() const;

	// ��Ч��־ģ��
	void SetEffectModelID(const PERSISTID& value);
	PERSISTID GetEffectModelID() const;

	// ��������־ģ��
	void SetTriggerModelID(const PERSISTID& value);
	PERSISTID GetTriggerModelID() const;

	// ��ľ��־ģ��
	void SetTreeModelID(const PERSISTID& value);
	PERSISTID GetTreeModelID() const;

	// �����
	void Pick(const IVarList& args, IVarList& result);
	// ���������
	void PickGround(const IVarList& args, IVarList& result);
	// ����ѡ��������
	PERSISTID PickVisual(int x, int y, float distance); 
	
	// ��ײ���Ի�ø߶�
	virtual float TraceHeight(float x, float z, float high_y, float low_y);
	// �������������Ķ����λ��
	virtual void TraceCollide(const IVarList& args, IVarList& result);
	// �������������Ķ���λ�ú���ͼ��
	virtual void TraceTexture(const IVarList& args, IVarList& result);
	// ���������Ƿ�����ײ
	virtual bool TraceHit(float x1, float y1, float z1, float x2, float y2, float z2);
	// ���ָ��λ�õĸ߶�
	virtual float GetPosiY(float x, float z);

	// ��þ�ȷ�ĵ���߶ȣ���֤�����ڵ��棩
	float GetGroundHeight(float x, float z);
	// ˮ���Ƿ����
	bool GetWaterExists(float x, float z);
	// �������ˮ�������ֵ
	int GetWaterCurrent(float x, float z);
	// ���ˮ������߶�
	float GetWaterBaseHeight(float x, float z);
	// ���ˮ�����߶�
	float GetWaterMaxHeightDefault(float x, float z, float default_y);
	// ���ˮ�沨�˵ľ�ȷ�߶�
	float GetWaterWaveHeight(float x, float z);

	// ��ײ�����Ƿ����
	virtual bool GetCollideEnable(float x, float z);
	// ���ָ��λ�õ����߶�
	virtual float GetApexHeight(float x, float z);
	// ���ָ��λ�õ����߶����ڵĲ�
	virtual int GetApexFloor(float x, float z);
	// �����Ϳ����߲�ĸ߶�
	virtual float GetBottomHeight(float x, float z);
	// �����Ϳ����߲�
	virtual int GetBottomFloor(float x, float z);
	// ����Ƿ������
	virtual bool GetWalkEnable(float x, float z);
	// ��ÿ����ߵĸ߶�
	virtual float GetWalkHeight(float x, float z);
	// ��ÿ����߲����
	virtual int GetWalkFloor(float x, float z);
	// ˮ���Ƿ����
	virtual bool GetWalkWaterExists(float x, float z);
	// ��þ�ȷ��ˮ��߶�
	virtual float GetWalkWaterHeight(float x, float z);
	// ����ܵĲ�����
	virtual int GetFloorCount(float x, float z);
	// ��ò��Ƿ����
	virtual bool GetFloorExists(float x, float z, int floor);
	// ��ò��Ƿ���ƶ�
	virtual bool GetFloorCanMove(float x, float z, int floor);
	// ��ò��Ƿ��վ��
	virtual bool GetFloorCanStand(float x, float z, int floor);
	// ��ò�߶�
	virtual float GetFloorHeight(float x, float z, int floor);
	// ��ò�����ϰ��ռ�߶�
	virtual float GetFloorSpace(float x, float z, int floor);
	// ��ò��Ƿ���ǽ��
	virtual bool GetFloorHasWall(float x, float z, int floor);
	// ���ָ���߶�λ���Ƿ����ǽ��
	virtual bool GetWallExists(float x, float y, float z);
	// ���ǽ�ڷ��߷���
	virtual void GetWalkWallAngle(const IVarList& args, IVarList& result);
	// ���ǽ����ײ
	virtual void GetWalkWallCollide(const IVarList& args, IVarList& result);
	// ������߾�����������ײ�����������
	virtual void GetLineTraverse(const IVarList& args, IVarList& result);
	// �����������ײ����״��ཻλ��
	virtual void GetLineIntersect(const IVarList& args, IVarList& result);

	// ���ָ�����ֵĵ�������
	PERSISTID GetVisual(const char* name);
	// ��鳡���Ƿ����ָ��������
	bool GetVisualExists(const PERSISTID& id);
	// ��ӵ�������
	bool AddVisual(const char* name, const PERSISTID& id);
	// ��������ɫ
	bool AddVisualRole(const char* name, const PERSISTID& id);
	// �Ƴ���������
	bool RemoveVisual(const PERSISTID& id);
	// ���¶�λ���������λ��
	bool RelocateVisual(const PERSISTID& id, float x, float y, float z);
	// �������嵱ǰ�Ƿ��ڿɼ���Χ��
	bool VisualInView(const PERSISTID& id);
	// ��������ļ��þ���
	bool SetVisualClipRadius(const PERSISTID& id, float clip_radius);
	// ��ñ�����͸�������ָ����Χ�ڵ������������
	void GetNearActorList(const IVarList& args, IVarList& result);
	// ������巶Χ�ڵĻ��������
	int GetRolesInSphere(float x, float y, float z, float radius);
	
	// ����������ͼ·��
	bool AddTexturePath(const char* value);
	bool ClearTexturePath();
	int GetTexturePathCount() const;
	void GetTexturePathList(const IVarList& args, IVarList& result) const;
	const char* GetTexturePathByIndex(size_t index) const;
	const char* GetTexturePathString() const { return m_strTexPath.c_str(); }
	
	// ������ͼ����
	int GetBaseTexIndex(const char* name);
	bool FindBaseTex(const char* name);
	bool AddBaseTex(const char* name, const char* path);
	bool ClearBaseTex();
	int GetBaseTexCount();
	void GetBaseTexList(const IVarList& args, IVarList& result);
	const char* GetBaseTexName(int index);
	const char* GetBaseTexPath(int index);
	bool SetBaseTexSpecularLevel(int index, float value);
	float GetBaseTexSpecularLevel(int index);
	bool SetBaseTexSpecularPower(int index, float value);
	float GetBaseTexSpecularPower(int index);
	bool SetBaseTexHeightScale(int index, float value);
	float GetBaseTexHeightScale(int index);

	// ������ͼ��ʽ����(ֻ��һ����ʽ).
	int GetBaseFormatScaleU(const char* name, int index);
	int GetBaseFormatScaleV(const char* name, int index);
	int GetBaseFormatAngleX(const char* name, int index);
	int GetBaseFormatAngleY(const char* name, int index);
	int GetBaseFormatAngleZ(const char* name, int index);
	bool SetBaseFormatScaleU(const char* name, int index, int value);
	bool SetBaseFormatScaleV(const char* name, int index, int value);
	bool SetBaseFormatAngleX(const char* name, int index, int value);
	bool SetBaseFormatAngleY(const char* name, int index, int value);
	bool SetBaseFormatAngleZ(const char* name, int index, int value);
	
	// �����ͼ����
	int GetBlendTexIndex(const char* name);
	bool FindBlendTex(const char* name);
	bool AddBlendTex(const char* name, const char* path);
	bool ClearBlendTex();
	int GetBlendTexCount();
	void GetBlendTexList(const IVarList& args, IVarList& result);
	const char* GetBlendTexName(int index);
	const char* GetBlendTexPath(int index);
	bool SetBlendTexSpecularLevel(int index, float value);
	float GetBlendTexSpecularLevel(int index);
	bool SetBlendTexSpecularPower(int index, float value);
	float GetBlendTexSpecularPower(int index);
	bool SetBlendTexHeightScale(int index, float value);
	float GetBlendTexHeightScale(int index);

	// �����ͼ��ʽ
	bool AddBlendFormat(const char* name, int scale_u, int scale_v,
		int angle_x, int angle_y, int angle_z);
	int GetBlendFormatCount(const char* name);
	int GetBlendFormatScaleU(const char* name, int index);
	int GetBlendFormatScaleV(const char* name, int index);
	int GetBlendFormatAngleX(const char* name, int index);
	int GetBlendFormatAngleY(const char* name, int index);
	int GetBlendFormatAngleZ(const char* name, int index);
	bool SetBlendFormatScaleU(const char* name, int index, int value);
	bool SetBlendFormatScaleV(const char* name, int index, int value);
	bool SetBlendFormatAngleX(const char* name, int index, int value);
	bool SetBlendFormatAngleY(const char* name, int index, int value);
	bool SetBlendFormatAngleZ(const char* name, int index, int value);

	// ȫ�ֻ�����ͼ
	void SetGlobalEnvironmentMap(const char* name);
	const char* GetGlobalEnvironmentMap() const;

	// �Ƿ���ˮ�沨��
	void SetWaterWave(bool value) { m_bWaterWave = value; }
	bool GetWaterWave() const { return m_bWaterWave; }

	// �Ƿ���ˮ�����
	void SetWaterDepth(bool value) { m_bWaterDepth = value; }
	bool GetWaterDepth() const { return m_bWaterDepth; }
	
	// �Ƿ���ˮ�尶�߹���
	void SetWaterBorder(bool value) { m_bWaterBorder = value; }
	bool GetWaterBorder() const { return m_bWaterBorder; }

	// �Ƿ���ˮ�淴��
	void SetWaterReflect(bool value) { m_bWaterReflect = value; }
	bool GetWaterReflect() const { return m_bWaterReflect; }

	// ˮ�滷����ͼ�Ƿ�����ظ�ʹ��
	void SetWaterEnvMapReusable(bool value) { m_bWaterEnvMapReusable = value; }
	bool GetWaterEnvMapReusable() const { return m_bWaterEnvMapReusable; }

	// �Ƿ���ˮ�����ⷴ��
	void SetWaterSunReflect(bool value) { m_bWaterSunReflect = value; }
	bool GetWaterSunReflect() const { return m_bWaterSunReflect; }

	// �Ƿ�Ҫ�������������λ��
	void SetUpdateFocus(bool value) { m_bUpdateFocus = value; }
	bool GetUpdateFocus() const { return m_bUpdateFocus; }

	// �Ƿ�����õ���
	void SetOnlyCullTerrain(bool value) { m_bOnlyCullTerrain = value; }
	bool GetOnlyCullTerrain() const { return m_bOnlyCullTerrain; }

	// �Ƿ�ʹ�ð��򻷾���
	void SetSphereAmbient(bool value) { m_bSphereAmbient = value; }
	bool GetSphereAmbient() const { return m_bSphereAmbient; }

	// �Ƿ����ײ���Ľ������У��
	void SetVerifyTraceResult(bool value) { m_bVerifyTraceResult = value; }
	bool GetVerifyTraceResult() const { return m_bVerifyTraceResult; }

	// ������������ʱ���Ƿ���м�¼
	void SetGenWalkTrace(bool value) { m_bGenWalkTrace = value; }
	bool GetGenWalkTrace() const { return m_bGenWalkTrace; }

	// �������������λ��
	bool SetFocusPosition(float x, float y, float z);
	// �������������뾶
	bool SetFocusRadius(float x, float y, float z);

	// ˮ�����͹���
	PERSISTID CreateWater(const char* name);
	PERSISTID GetWater(const char* name);
	bool DeleteWater(const char* name);
	bool ClearWater();
	const char* GetWaterName(int index);
	int GetWaterIndex(const char* name);
	int GetWaterCount() const;
	void GetWaterList(const IVarList& args, IVarList& result) const;
	bool RefreshWater();
	bool SetWaterCubeMapStatic(bool value);
	bool DumpWaterCubeMap(int index, const char* file_name);
	
	// �������͹���
	PERSISTID CreateGrass(const char* name);
	PERSISTID GetGrass(const char* name);
	bool DeleteGrass(const char* name);
	bool ClearGrass();
	const char* GetGrassName(int index);
	int GetGrassIndex(const char* name);
	int GetGrassCount() const;
	void GetGrassList(const IVarList& args, IVarList& result) const;
	bool RefreshGrass();

	// ��̬���������
	CDynamicWalkableManager* GetDynamicWalkableManager() const 
	{ return m_pDynamicWalkableManager; }

	// ˢ�µ���
	bool RefreshGround(); 
	// ˢ�µ�������
	bool RefreshVisual(); 

	// ��ײ�������
	virtual bool SetTraceMask(const char* type, bool value);
	virtual bool GetTraceMask(const char* type);
	virtual void SetTraceMaskValue(unsigned int value) { m_nTraceMask = value; }
	virtual unsigned int GetTraceMaskValue() const { return m_nTraceMask; }

	// ��Χ�����ɫ
	bool SetRegionColor(const char* name, unsigned int value);
	unsigned int GetRegionColor(const char* name) const;
	
	// ��õ��������б�
	virtual void GetRegionList(const IVarList& args, IVarList& result);
	// ���ָ��λ�õķ�Χ����Ƿ���Ч
	virtual bool GetRegionEnable(const char* name, float x, float z);

	// �ı䷶Χ���ƣ��༭���ã�
	bool ChangeRegionName(const char* ori_name, const char* new_name);

	// ���������ɫ
	bool SetAreaColor(const char* name, unsigned int value);
	unsigned int GetAreaColor(const char* name) const;

	// ��õ��������б�
	virtual void GetAreaList(const IVarList& args, IVarList& result);
	// ���ָ��λ�õĵ����������
	virtual const char* GetAreaName(float x, float z);

	// �ı�������ƣ��༭���ã�
	bool ChangeAreaName(const char* ori_name, const char* new_name);
	// ɾ��ָ����������ɫ��Ϣ���༭���ã�
	bool RemoveAreaColor(const char* name);

	// ���õ��β���
	bool SetParameter(size_t zone_scale, size_t chunk_scale, size_t tex_scale, 
		size_t tex_units, size_t alpha_per_unit, size_t collide_per_unit);
	// ��ʼ������
	bool InitZones(size_t rows, size_t cols, size_t origin_row, 
		size_t origin_col, size_t range);

	// ����ɾ��֮ǰɾ�����еؿ�ļ�����
	bool ReleaseAllZoneLoader();
	
	// �����Ⱦ��
	IRender* GetRender() const { return m_pRender; }
	// ����
	Terrain::material_t& GetMaterial() { return m_Material; }
	// ���������
	CZoneManager* GetZoneManager() const { return m_pZoneManager; }
	// �������������
	CVisualManager* GetVisualManager() const { return m_pVisualManager; }
	// ��ײ������
	CQuadCollide* GetCollide() const { return m_pCollide; }
	// ���μ���
	CTerrainCulling* GetCulling() const { return m_pCulling; }
	// ������Ⱦ��
	CTerrainPainter* GetPainter() const { return m_pPainter; }
	// ˮ������
	CTerrainWater* GetWater() const { return m_pWater; }
	// ��������
	CTerrainGrass* GetGrass() const { return m_pGrass; }

	// �ƹ��־ģ��
	IVisBase* GetLightModel() const { return m_pLightModel; }
	// ������־ģ��
	IVisBase* GetSoundModel() const { return m_pSoundModel; }
	// ������Χģ��
	IVisBase* GetSoundRangeModel() const { return m_pSoundRangeModel; }
	// ���ӱ�־ģ��
	IVisBase* GetParticleModel() const { return m_pParticleModel; }
	// ��Ч��־ģ��
	IVisBase* GetEffectModel() const { return m_pEffectModel; }
	// ��������־ģ��
	IVisBase* GetTriggerModel() const { return m_pTriggerModel; }
	// ��ľ��־ģ��
	IVisBase* GetTreeModel() const { return m_pTreeModel; }
	
	// ��ײ����ĳߴ�
	void SetCollideScale(int value);
	int GetCollideScale() const { return (int)m_nCollideScale; }
	
	// ��������ĳߴ�
	float GetZoneWidth() const { return m_nZoneScale * m_fUnitSize; }
	float GetZoneHeight() const { return m_nZoneScale * m_fUnitSize; }

	// �������εĳߴ�
	float GetFullWidth() const;
	float GetFullHeight() const;

	// ���ȫ�����ж���ĸ߶�
	float GetGlobalHeightDefault(size_t row, size_t col, float def);
	// �ͷ�ȫ�����ж���ķ���
	bool ReleaseGlobalNormal(size_t row, size_t col);
	
	// ���η�Χ
	void SetLeft(float value) { m_fLeft = value; }
	float GetLeft() const { return m_fLeft; }
	void SetTop(float value) { m_fTop = value; }
	float GetTop() const { return m_fTop; }
	float GetRight() const { return m_fLeft + GetFullWidth(); }
	float GetBottom() const { return m_fTop + GetFullHeight(); }

	// ��ͼ������
	CBlendTexSet* GetBaseTexSet() const { return m_pBaseTexSet; }
	CBlendTexSet* GetBlendTexSet() const { return m_pBlendTexSet; }

	// �����ʽ
	CChunkStyle* GetChunkStyle(size_t style)
	{
		Assert(style < m_nStyleNum);

		return m_pStyles[style];
	}

	// ��û��ݵĶ��㶨��
//	IDxInputLayout* GetGrassInputLayout();
	// ��û��ݵ���������
	IStaticIB* GetGrassIndexBuffer(size_t grass_num);
	
	// ������ڵ�����
	CTerrainZone* GetLeftZone(CTerrainZone* zone);
	CTerrainZone* GetRightZone(CTerrainZone* zone);
	CTerrainZone* GetUpZone(CTerrainZone* zone);
	CTerrainZone* GetDownZone(CTerrainZone* zone);
	
	// ����������ڵķֿ�
	CTerrainChunk* GetInChunk(float x, float z);
	
	// ���ָ��λ������
	//float GetDiffuse(float x, float z);
	// ���ָ��λ�û�ϱ���������ͼ����
	const char* GetMainTexture(float x, float z);
	// ���ָ���뾶�ڵ����е�������
	virtual void GetCircleVisualList(const IVarList& args, IVarList& result);
	// ��ӵ�������
	virtual int AddVisBase(const char* name, IVisBase* pVisBase, 
		bool is_role, float clip_radius);
	// �Ƴ���������
	virtual int RemoveVisBase(IVisBase* pVisBase);
	// ���¶�λλ�÷����ı�ĵ�������
	virtual int RelocateVisBase(IVisBase* pVisBase, float x, float y, 
		float z);
	
	// �Ƿ�ʹ�õ��淨����ͼ
	void SetEnableNormalMap(bool value) { m_bEnableNormalMap = value; }
	bool GetEnableNormalMap() const { return m_bEnableNormalMap; }

	// �Ƿ�ʹ�ñ���ϸ��
	void SetEnableTessellation(bool value) { m_bEnableTessellation = value; }
	bool GetEnableTessellation() const { return m_bEnableTessellation; }

	// ����ϸ�ֲ���
	void SetTessellationFactor(float value) { m_fTessellationFactor = value; }
	float GetTessellationFactor() const { return m_fTessellationFactor; }

	// ���淨����ͼ��Ч��Χ
	void SetNormalMapRadius(float value) { m_fNormalMapRadius = value; }
	float GetNormalMapRadius() const { return m_fNormalMapRadius; }

	// ��Χ���Ƿ��ڼ�������
	bool IsViewClip(float x, float y, float z, float radius);
	// �Ƿ񱻵�ƽ�߼���
	bool IsHorizonClip(const FmVec3& v_min, const FmVec3& v_max);
	// ������ƽ����Ϣ���ļ�
	bool DumpHorizon(const char* file_name);

	// �����ڴ��е������б�
	bool DumpZones(const char* file_name);

	// ��Ӷ�̬���
	virtual bool AddDynamicWalkable(IVisBase* pVisBase);
	// ɾ����̬���
	virtual bool DeleteDynamicWalkable(IVisBase* pVisBase);
	// ���ض�̬�����ײ
	bool LoadDynamicWalkable(IVisBase* pVisBase);
	// ж�ض�̬�����ײ
	bool UnloadDynamicWalkable(IVisBase* pVisBase);

	// ��Ӷ�̬���
	bool AddDynamicWalkable(const PERSISTID& id);
	// ɾ����̬���
	bool DeleteDynamicWalkable(const PERSISTID& id);
	// ���ض�̬�����ײ
	bool LoadDynamicWalkable(const PERSISTID& id);
	// ж�ض�̬�����ײ
	bool UnloadDynamicWalkable(const PERSISTID& id);

//#ifdef TERRAIN_EDIT
//	// �Ƿ񱣴�ָ�������ڵ�Chunk�Ŀ����ߵ�����
//	bool SetSaveChunkWalkPos(float x, float z, bool value);
//	bool GetSaveChunkWalkPos(float x, float z);
//	// ����ָ�������ڵ�Chunk�Ŀ����ߵ�����
//	bool SaveChunkWalkPos(float x, float z, const char* file_name);
//	// �������п����ߵ㵽�ı��ļ����ļ���Ϊ����·����
//	bool DumpAllWalkPosToFile(const char* file_name);
//	// �������п����ߵ㵽ͼ���ļ����ļ���Ϊ����·����
//	bool DumpAllWalkPosToImage(const char* file_name);
//#endif

	// ��ȡ����ͼ����
	CLightMapConfig* GetLigthMapConfig() { return m_pLightMapConfig; }

private:
	// ����ÿ�ַ�����������
	bool CreateStyleIndexBuffer();
	// �ͷ�ÿ�ַ�����������
	void ReleaseStyleIndexBuffer();
	
	// ����ѡ
	bool InnerPick(int x, int y, float distance, trace_info_t& result);
	
	// ����ײ���Ľ������У��
	void VerifyTraceResult(const FmVec3& src, const FmVec3& dst,
		trace_info_t& result);

//#ifdef TERRAIN_EDIT
//	// ������п������ļ���Ϣ
//	bool InnerBeginDumpWalkInfo();
//	// �������п����ߵ㵽�ļ�
//	bool InnerDumpAllWalkPosToFile(const char* file_name);
//#endif

	// ��ҪͶ��Ӱ��
	virtual void SetCastShadow(bool value) { m_bCastShadow = value; }
	virtual bool GetCastShadow() const { return m_bCastShadow; }

	// ��Ҫ����Ͷ��Ӱ��
	virtual void SetReceiveShadow(bool value)  {m_bReceiveShadow = value; }
	virtual bool GetReceiveShadow() const { return m_bReceiveShadow; }

private:
	IRender* m_pRender;
	//IRenderContext* m_pContext;
	TerrainEditor* m_pEditor;
	TerrainBrush* m_pBrush;
	TerrainMaterial* m_pMaterial;
	CZoneManager* m_pZoneManager;		// �������ݹ�����
	CVisualManager* m_pVisualManager;	// �������������
	CTerrainCulling* m_pCulling;		// ���μ���
	CTerrainPainter* m_pPainter;		// ������Ⱦ��
	CTerrainWater* m_pWater;			// ˮ������
	CTerrainGrass* m_pGrass;			// ��������
	CQuadCollide* m_pCollide;			
	CLightMapConfig* m_pLightMapConfig; // ��������beast����ͼ��Ӧ���ع�ָ��
	CDynamicWalkableManager* m_pDynamicWalkableManager; // ��̬��ײ������

	PERSISTID m_PlayerID;
	PERSISTID m_SkyID;

	material_t m_Material;		// ����
	CNameList m_TexPaths;
	core_string m_strTexPath;
	
	size_t m_nZoneScale;		// ÿһ������ĸ���������
	size_t m_nChunkScale;		// ÿһ���ֿ�ĸ���������
	size_t m_nTextureScale;		// ������ͼ�Ĵ�С
	size_t m_nTextureUnits;		// ��ͼ�ظ��ĸ��ӷ�Χ
	size_t m_nAlphaPerUnit;		// ����֮���ALPHAֵ����
	size_t m_nAlphaMapScale;	// �ֿ��ALPHA��ͼ�ߴ�
	size_t m_nAlphaScale;		// ÿһ�������ALPHAֵ������
	size_t m_nLodLevels;		// LOD������
	size_t m_nLightTexScale;	// ����ͼ�ĳߴ�
	size_t m_nLightPerUnit;		// ���������ɫ�ֱ���
	size_t m_nCollidePerUnit;	// ��ײ�ֱ���
	size_t m_nCollideScale;		// ��ײ����ĳߴ�
	
	bool m_bDesignMode;			// �Ƿ����״̬
	int m_nShowDesignLine;		// ��ʾ��������ķ�ʽ
	bool m_bShowWalkable;		// �Ƿ���ʾ���߷�Χ
	bool m_bShowSpaceHeight;	// �Ƿ���ʾ���߿ռ�߶�
	bool m_bShowLightRange;		// �Ƿ���ʾ���Դ��Χ
	bool m_bShowRegion;			// �Ƿ���ʾ��Χ���
	core_string m_strShowRegionName;
	bool m_bShowArea;			// �Ƿ���ʾ�������
	bool m_bShowDesignNoEdge;	// �Ƿ�����ʾ�༭�÷���䲻����
	bool m_bShowIcon;			// �Ƿ���ʾͼ��ģ��
	bool m_bShowCollide;		// �Ƿ���ʾ��ײ����
	bool m_bShowTree;			// �Ƿ���ʾ��
	bool m_bShowQuadTree;		// �Ƿ���ʾ������Ĳ����ṹ
	int m_nFarLodLevel;			// Զ����ȱʡLOD����
	float m_fLodRadius;			// ��Ҫ����LOD����ķ�Χ�뾶
	float m_fNoLodRadius;		// ����Ҫ����LOD����ķ�Χ�뾶
	float m_fDesignRadius;		// �ɱ༭��Χ�뾶
	float m_fCollideRadius;		// ��ײ��Ϣ��ʾ�뾶
	float m_fAlphaFadeRadius;	// �����ͼ�����뾶
	float m_fAlphaHideRadius;	// �����ͼ�����뾶
	float m_fGrassRadius;		// ���ݵ����뾶
	float m_fGrassLod;			// ���ݾ���
	float m_fVisualLodBegin;	// �ɼ������LOD������ʼ����
	float m_fVisualLodEnd;		// �ɼ������LOD�����������
	bool m_bWaterVisible;		// �Ƿ�ɼ�
	bool m_bGroundVisible;
	bool m_bVisualVisible;
	bool m_bHelperVisible;
	bool m_bWaterWire;			// �Ƿ�ֻ��ʾ����
	bool m_bGroundWire;
	bool m_bVisualWire;
	bool m_bNoLight;			// �Ƿ񲻼��ع���
	bool m_bSimpleGround;		// �Ƿ���Ƽ򻯵ر�
	bool m_bHorizontalCulling;	// �Ƿ�ʹ�õ�ƽ�߼���
	bool m_bVisualFadeIn;		// �Ƿ�������ģ�͵���
	bool m_bSmoothHorizontal;	// �Ƿ�Ҫƽ����ƽ��
	bool m_bWaterWave;
	bool m_bWaterDepth;
	bool m_bWaterBorder;
	bool m_bWaterReflect;
	bool m_bWaterEnvMapReusable;
	bool m_bWaterSunReflect;
	bool m_bUpdateFocus;
	bool m_bOnlyCullTerrain;
	bool m_bSphereAmbient;
	bool m_bVerifyTraceResult;	// �Ƿ����ײ���Ľ������У��
	bool m_bGenWalkTrace;		// ������������ʱ���Ƿ���м�¼
	int m_nGroundLevelDown;		// �ر�LOD��������
	int m_nGroundEnvMap;
	int m_nVisualEnvMap;
	int m_nGroundSortOrder;		// �ؿ�����
	int m_nVisualSortOrder;		// ������������ʽ
	unsigned int m_nTraceMask;	// ��ײ����
	float m_fClipRadiusNear;	// ������ð뾶
	float m_fClipRadiusFar;		// ��Զ���ð뾶
	float m_fVisualRadiusSmall; // ��С����뾶
	float m_fVisualRadiusBig;	// �������뾶
	float m_fLoadRadiusAdd;		// ���ذ뾶
	float m_fUnloadRadiusAdd;	// ж�ذ뾶

	float m_fLeft;					// ���Ͻ�����
	float m_fTop;
	float m_fUnitSize;				// ��λ���Ӵ�С
	float m_fInvUnitSize;
	float m_fCollideSize;		
	float m_fInvCollideSize;
	float m_fPixelError;			// �������
	float m_fInitHeight;			// ��ʼ�߶�
	
	core_string m_strFilePath; // ��Դ�ļ�����·��
	core_string m_strAppendPath;
	core_string m_strZoneLightPath; // ������������ļ�·��
	core_string m_strModelLightPath; // ����ģ�͹����ļ�·��
	core_string m_strWalkablePath; // �������ļ�·��
	core_string m_strDynamicWalkablePath; // ��̬�������ļ�·��
	core_string m_strHeightMap; // �����õĸ߶�ͼ

	IVisBase* m_pLightModel;
	IVisBase* m_pSoundModel;
	IVisBase* m_pSoundRangeModel;
	IVisBase* m_pParticleModel;
	IVisBase* m_pEffectModel;
	IVisBase* m_pTriggerModel;
	IVisBase* m_pTreeModel;

	CChunkStyle** m_pStyles;		// �ֿ���ʽ
	size_t m_nStyleNum;

	CBlendTexSet* m_pBaseTexSet;	// ������ͼ����
	CBlendTexSet* m_pBlendTexSet;	// �����ͼ����

	TArrayPod<area_color_t, 1, TCoreAlloc> m_RegionColors; // ��Χ�����ɫ
	TArrayPod<area_color_t, 1, TCoreAlloc> m_AreaColors; // ���������ɫ

//	IDxInputLayout* m_pGrassInputLayout;
	IStaticIB* m_pGrassIndexBuffer;
	
	bool m_bEnableNormalMap;
	bool m_bEnableTessellation;
	float m_fTessellationFactor;
	float m_fNormalMapRadius;
	float m_offset_seconds;

	bool m_bDynamicWalkLoaded;

	bool m_bCastShadow;
	bool m_bReceiveShadow;
};

#endif // _TERRAIN_H
