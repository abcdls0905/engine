//--------------------------------------------------------------------
// �ļ���:		terrain_attr.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2007��8��9��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "terrain.h"

/// \file terrain_attr.cpp
/// \brief ����

 

/// entity: Terrain
/// \brief ����ʵ��
/// \see �̳���IVisBase
DECLARE_ENTITY(Terrain, 0, IVisBase);

/// readonly: object Editor
/// \brief ���α༭����
DECLARE_PROPERTY_GET(PERSISTID, Terrain, Editor, GetEditorID);
/// readonly: object Brush
/// \brief ����ˢ�Ӷ���
DECLARE_PROPERTY_GET(PERSISTID, Terrain, Brush, GetBrushID);
/// readonly: object Material
/// \brief ���β��ʶ���
DECLARE_PROPERTY_GET(PERSISTID, Terrain, Material, GetMaterialID);

/// readonly: int ZoneScale
/// \brief ��������ĳߴ�
DECLARE_PROPERTY_GET(int, Terrain, ZoneScale, GetZoneScaleAttr);
/// readonly: int ChunkScale
/// \brief �����ڷֿ�ĳߴ�
DECLARE_PROPERTY_GET(int, Terrain, ChunkScale, GetChunkScaleAttr);
/// readonly: int TextureScale
/// \brief ������ͼ�ĳߴ�
DECLARE_PROPERTY_GET(int, Terrain, TextureScale, GetTextureScale);
/// readonly: int TextureUnits
/// \brief ÿ��������ͼ���ǵĵ��ε�λ��������
DECLARE_PROPERTY_GET(int, Terrain, TextureUnits, GetTextureUnits);
/// readonly: int AlphaPerUnit
/// \brief ÿ�����ε�λ�����ڵ�ALPHAֵ����
DECLARE_PROPERTY_GET(int, Terrain, AlphaPerUnit, GetAlphaPerUnit);
/// readonly: int AlphaMapScale
/// \brief �����ͼ���ݵĳߴ�
DECLARE_PROPERTY_GET(int, Terrain, AlphaMapScale, GetAlphaMapScaleAttr);
/// readonly: int AlphaScale
/// \brief ÿ������Ļ�����ݳߴ�
DECLARE_PROPERTY_GET(int, Terrain, AlphaScale, GetAlphaScaleAttr);
/// readonly: int LodLevels
/// \brief LOD������
DECLARE_PROPERTY_GET(int, Terrain, LodLevels, GetLodLevelsAttr);
/// readonly: int CollidePerUnit
/// \brief ��ײ�ֱ���
DECLARE_PROPERTY_GET(int, Terrain, CollidePerUnit, GetCollidePerUnitAttr);

/// readonly: int ZoneRows
/// \brief ��������
DECLARE_PROPERTY_GET(int, Terrain, ZoneRows, GetZoneRows);
/// readonly: int ZoneCols
/// \brief ��������
DECLARE_PROPERTY_GET(int, Terrain, ZoneCols, GetZoneCols);
/// readonly: int ZoneOriginRows
/// \brief ԭ������������
DECLARE_PROPERTY_GET(int, Terrain, ZoneOriginRow, GetZoneOriginRow);
/// readonly: int ZoneOriginCols
/// \brief ԭ������������
DECLARE_PROPERTY_GET(int, Terrain, ZoneOriginCol, GetZoneOriginCol);
/// readonly: int ZoneRange
/// \brief �ɼ�����Χ
DECLARE_PROPERTY_GET(int, Terrain, ZoneRange, GetZoneRange);

/// proprety: object Player
/// \brief ��ǰ��Ҷ���
DECLARE_PROPERTY(PERSISTID, Terrain, Player, GetPlayerID, SetPlayerID);
/// proprety: object Sky
/// \brief ��ն���
DECLARE_PROPERTY(PERSISTID, Terrain, Sky, GetSkyID, SetSkyID);

/// property: int LightTexScale
/// \brief ����ͼ�ĳߴ�
DECLARE_PROPERTY(int, Terrain, LightTexScale, GetLightTexScale, 
	SetLightTexScale);
/// property: int LightPerUnit
/// \brief ���������ɫ�ֱ���
DECLARE_PROPERTY(int, Terrain, LightPerUnit, GetLightPerUnit, 
	SetLightPerUnit);
/// property: int CollideScale
/// \brief ��ײ����ĳߴ�
DECLARE_PROPERTY(int, Terrain, CollideScale, GetCollideScale, 
	SetCollideScale);
/// property: bool DesignMode
/// \brief �Ƿ����״̬
DECLARE_PROPERTY(bool, Terrain, DesignMode, GetDesignModeAttr, SetDesignMode);
/// property: int ShowDesignLine
/// \brief ��ʾ��������ķ�ʽ
/// \brief 0����ʾ��1ֻ��ʾ�����Ե�ߣ�2��ʾ�����Ե�ߺͷֿ��Ե�ߣ�3ȫ����ʾ
DECLARE_PROPERTY(int, Terrain, ShowDesignLine, GetShowDesignLine, 
	SetShowDesignLine);
/// property: bool ShowWalkable
/// \brief �Ƿ���ʾ���߷�Χ
DECLARE_PROPERTY(bool, Terrain, ShowWalkable, GetShowWalkable, 
	SetShowWalkable);
/// property: bool ShowSpaceHeight
/// \brief �Ƿ���ʾ���߿ռ�߶�
DECLARE_PROPERTY(bool, Terrain, ShowSpaceHeight, GetShowSpaceHeight, 
	SetShowSpaceHeight);
/// property: bool ShowLightRange
/// \brief �Ƿ���ʾ���Դ��Χ
DECLARE_PROPERTY(bool, Terrain, ShowLightRange, GetShowLightRange, 
	SetShowLightRange);
/// property: bool ShowRegion
/// \brief �Ƿ���ʾ��Χ���
DECLARE_PROPERTY(bool, Terrain, ShowRegion, GetShowRegion, SetShowRegion);
/// property: string ShowRegionName
/// \brief ��ʾָ����Χ�����
DECLARE_PROPERTY(const char*, Terrain, ShowRegionName, GetShowRegionName, 
	SetShowRegionName);
/// property: bool ShowArea
/// \brief �Ƿ���ʾ�������
DECLARE_PROPERTY(bool, Terrain, ShowArea, GetShowArea, SetShowArea);
/// property: bool ShowIcon
/// \brief �Ƿ���ʾͼ��ģ��
DECLARE_PROPERTY(bool, Terrain, ShowIcon, GetShowIcon, SetShowIcon);
/// property: bool ShowCollide
/// \brief �Ƿ���ʾ��ײ����
DECLARE_PROPERTY(bool, Terrain, ShowCollide, GetShowCollide, SetShowCollide);
/// property: bool ShowArea
/// \brief �Ƿ�����ʾ�༭�÷���䲻����
DECLARE_PROPERTY(bool, Terrain, ShowDesignNoEdge, GetShowDesignNoEdge, 
	SetShowDesignNoEdge);
/// property: bool ShowTree
/// \brief �Ƿ���ʾ��
DECLARE_PROPERTY(bool, Terrain, ShowTree, GetShowTree, SetShowTree);
/// property: bool ShowQuadTree
/// \brief �Ƿ���ʾ������Ĳ����ṹ
DECLARE_PROPERTY(bool, Terrain, ShowQuadTree, GetShowQuadTree, SetShowQuadTree);
/// property: int FarLodLevel
/// \brief ����Զ����ȱʡLOD����
DECLARE_PROPERTY(int, Terrain, FarLodLevel, GetFarLodLevel, SetFarLodLevel);
/// property: float LodRadius
/// \brief ��Ҫ����LOD����ķ�Χ�뾶��������Χ�������ֵ
DECLARE_PROPERTY(float, Terrain, LodRadius, GetLodRadius, SetLodRadius);
/// property: float NoLodRadius
/// \brief ����Ҫ����LOD����ķ�Χ�뾶��������Χ�����LOD����
DECLARE_PROPERTY(float, Terrain, NoLodRadius, GetNoLodRadius, SetNoLodRadius);
/// property: float DesignRadius
/// \brief ��ư뾶���˷�Χ�ڵķֿ鲻��LOD����
DECLARE_PROPERTY(float, Terrain, DesignRadius, GetDesignRadius, 
	SetDesignRadius);
/// property: float CollideRadius
/// \brief ��ײ��Ϣ��ʾ�뾶
DECLARE_PROPERTY(float, Terrain, CollideRadius, GetCollideRadius, 
	SetCollideRadius);
/// property: float AlphaFadeRadius
/// \brief ���ͼ�����뾶
DECLARE_PROPERTY(float, Terrain, AlphaFadeRadius, GetAlphaFadeRadiusAttr, 
	SetAlphaFadeRadius);
/// property: float AlphaHideRadius
/// \brief ���ͼ�����뾶���˷�Χ��ķֿ鲻��Ҫ�������ͼ 
DECLARE_PROPERTY(float, Terrain, AlphaHideRadius, GetAlphaHideRadiusAttr, 
	SetAlphaHideRadius);
/// property: float GrassRadius
/// \brief ���ݵ����뾶
DECLARE_PROPERTY(float, Terrain, GrassRadius, GetGrassRadiusAttr, 
	SetGrassRadius);
/// property: float GrassLod
/// \brief ���ݾ���
DECLARE_PROPERTY(float, Terrain, GrassLod, GetGrassLodAttr, SetGrassLod);
/// property: float VisualLodBegin
/// \brief �ɼ������LOD������ʼ����
DECLARE_PROPERTY(float, Terrain, VisualLodBegin, GetVisualLodBeginAttr, 
	SetVisualLodBegin);
/// property: float VisualLodEnd
/// \brief �ɼ������LOD�����������
DECLARE_PROPERTY(float, Terrain, VisualLodEnd, GetVisualLodEndAttr, 
	SetVisualLodEnd);

/// property: bool StopRefresh
/// \brief ֹͣˢ�µ��Σ�������
//DECLARE_PROPERTY(bool, Terrain, StopRefresh, GetStopRefresh, SetStopRefresh);

/// property: bool WaterVisible
/// \brief ˮ���Ƿ�ɼ�
DECLARE_PROPERTY(bool, Terrain, WaterVisible, GetWaterVisible, 
	SetWaterVisible);
/// property: bool GroundVisible
/// \brief �����Ƿ�ɼ�
DECLARE_PROPERTY(bool, Terrain, GroundVisible, GetGroundVisible, 
	SetGroundVisible);
/// property: bool VisualVisible
/// \brief ���������Ƿ�ɼ�
DECLARE_PROPERTY(bool, Terrain, VisualVisible, GetVisualVisible, 
	SetVisualVisible);
/// property: bool HelperVisible
/// \brief ���������Ƿ�ɼ�
DECLARE_PROPERTY(bool, Terrain, HelperVisible, GetHelperVisible, 
	SetHelperVisible);
/// property: bool WaterWire
/// \brief ˮ���Ƿ�ֻ��ʾ����
DECLARE_PROPERTY(bool, Terrain, WaterWire, GetWaterWire, SetWaterWire);
/// property: bool GroundWire
/// \brief �����Ƿ�ֻ��ʾ����
DECLARE_PROPERTY(bool, Terrain, GroundWire, GetGroundWire, SetGroundWire);
/// property: bool VisualWire
/// \brief �����Ƿ�ֻ��ʾ����
DECLARE_PROPERTY(bool, Terrain, VisualWire, GetVisualWire, SetVisualWire);

/// property: bool NoLight
/// \brief �Ƿ񲻼��ع���
DECLARE_PROPERTY(bool, Terrain, NoLight, GetNoLight, SetNoLight);
/// property: bool SpecularMaterial
/// \brief �Ƿ����߹����
//DECLARE_PROPERTY(bool, Terrain, SpecularMaterial, GetSpecularMaterial, 
//	SetSpecularMaterial);
/// property: bool SimpleGround
/// \brief �Ƿ���Ƽ򻯵ر�
DECLARE_PROPERTY(bool, Terrain, SimpleGround, GetSimpleGround, 
	SetSimpleGround);
/// property: bool HorizontalCulling
/// \brief �Ƿ�ʹ�õ�ƽ�߼���
DECLARE_PROPERTY(bool, Terrain, HorizontalCulling, GetHorizontalCulling, 
	SetHorizontalCulling);
/// property: bool VisualFadeIn
/// \brief �Ƿ����������嵭��
DECLARE_PROPERTY(bool, Terrain, VisualFadeIn, GetVisualFadeIn, 
	SetVisualFadeIn);
/// property: bool SmoothHorizontal
/// \brief �Ƿ���ƽ����ƽ��
DECLARE_PROPERTY(bool, Terrain, SmoothHorizontal, GetSmoothHorizontal, 
	SetSmoothHorizontal);

/// property: bool WaterWave
/// \brief �Ƿ���ˮ�沨��
DECLARE_PROPERTY(bool, Terrain, WaterWave, GetWaterWave, SetWaterWave);
/// property: bool WaterDepth
/// \brief �Ƿ���ˮ�����
DECLARE_PROPERTY(bool, Terrain, WaterDepth, GetWaterDepth, SetWaterDepth);
/// property: bool WaterBorder
/// \brief �Ƿ���ˮ�尶�߹���
DECLARE_PROPERTY(bool, Terrain, WaterBorder, GetWaterBorder, SetWaterBorder);
/// property: bool WaterReflect
/// \brief �Ƿ���ˮ�淴��
DECLARE_PROPERTY(bool, Terrain, WaterReflect, GetWaterReflect, 
	SetWaterReflect);
/// property: bool WaterEnvMapReusable
/// \brief ˮ�滷����ͼ�Ƿ�����ظ�ʹ��
DECLARE_PROPERTY(bool, Terrain, WaterEnvMapReusable, GetWaterEnvMapReusable, 
	SetWaterEnvMapReusable);
/// property: bool WaterSunReflect
/// \brief �Ƿ���ˮ�����ⷴ��
DECLARE_PROPERTY(bool, Terrain, WaterSunReflect, GetWaterSunReflect, 
	SetWaterSunReflect);

/// property: bool UpdateFocus
/// \brief �Ƿ�Ҫ�������������λ�úͰ뾶
DECLARE_PROPERTY(bool, Terrain, UpdateFocus, GetUpdateFocus, SetUpdateFocus);

/// property: bool OnlyCullTerrain
/// \brief �Ƿ�����õ���
DECLARE_PROPERTY(bool, Terrain, OnlyCullTerrain, GetOnlyCullTerrain, 
	SetOnlyCullTerrain);

/// property: bool SphereAmbient
/// \brief �Ƿ�ʹ�ð��򻷾���
DECLARE_PROPERTY(bool, Terrain, SphereAmbient, GetSphereAmbient, 
	SetSphereAmbient);

/// property: bool VerifyTraceResult
/// \brief �Ƿ����ײ���Ľ������У��
DECLARE_PROPERTY(bool, Terrain, VerifyTraceResult, GetVerifyTraceResult, 
	SetVerifyTraceResult);

/// property: bool GenWalkTrace
/// \brief ������������ʱ���Ƿ���м�¼
DECLARE_PROPERTY(bool, Terrain, GenWalkTrace, GetGenWalkTrace, SetGenWalkTrace);

/// property: int GroundLevelDown
/// \brief �ر�LOD��������
DECLARE_PROPERTY(int, Terrain, GroundLevelDown, GetGroundLevelDown, 
	SetGroundLevelDown);
/// property: int GroundEnvMap
/// \brief ���ε�Ӱ����(0-2)
DECLARE_PROPERTY(int, Terrain, GroundEnvMap, GetGroundEnvMap, 
	SetGroundEnvMap);
/// property: int VisualEnvMap
/// \brief �������嵹Ӱ����(0-2)
DECLARE_PROPERTY(int, Terrain, VisualEnvMap, GetVisualEnvMap, 
	SetVisualEnvMap);
/// property: int GroundSortOrder
/// \brief �ؿ�����(0-������1-�ӽ���Զ����)
DECLARE_PROPERTY(int, Terrain, GroundSortOrder, GetGroundSortOrder, 
	SetGroundSortOrder);
/// property: int VisualSortOrder
/// \brief ������������ʽ(0-������1-�ӽ���Զ����)
DECLARE_PROPERTY(int, Terrain, VisualSortOrder, GetVisualSortOrder, 
	SetVisualSortOrder);

/// property: float ClipRadiusNear
/// \brief ������ð뾶
DECLARE_PROPERTY(float, Terrain, ClipRadiusNear, GetClipRadiusNear, 
	SetClipRadiusNear);
/// property: float ClipRadiusFar
/// \brief ��Զ���ð뾶
DECLARE_PROPERTY(float, Terrain, ClipRadiusFar, GetClipRadiusFar, 
	SetClipRadiusFar);
/// property: float VisualRadiusSmall
/// \brief ��С����뾶
DECLARE_PROPERTY(float, Terrain, VisualRadiusSmall, GetVisualRadiusSmall, 
	SetVisualRadiusSmall);
/// property: float VisualRadiusBig
/// \brief �������뾶
DECLARE_PROPERTY(float, Terrain, VisualRadiusBig, GetVisualRadiusBig, 
	SetVisualRadiusBig);
/// property: float LoadRadiusAdd
/// \brief ������صĸ��Ӿ���
DECLARE_PROPERTY(float, Terrain, LoadRadiusAdd, GetLoadRadiusAdd, 
	SetLoadRadiusAdd);
/// property: float UnloadRadiusAdd
/// \brief ����ж�صĸ��Ӿ���
DECLARE_PROPERTY(float, Terrain, UnloadRadiusAdd, GetUnloadRadiusAdd, 
	SetUnloadRadiusAdd);

/// property: string RoleDiffuseFactor
/// \brief ������չ�Դ��ɫ��������
//DECLARE_PROPERTY(float, Terrain, RoleDiffuseFactor, GetRoleDiffuseFactor, 
//	SetRoleDiffuseFactor);
/// property: string RoleAmbientFactor
/// \brief ������ջ������������
//DECLARE_PROPERTY(float, Terrain, RoleAmbientFactor, GetRoleAmbientFactor, 
//	SetRoleAmbientFactor);
/// property: string RoleSpecularFactor
/// \brief ������շ�����������
//DECLARE_PROPERTY(float, Terrain, RoleSpecularFactor, GetRoleSpecularFactor, 
//	SetRoleSpecularFactor);

/// property: float PixelError
/// \brief LOD��ʾ�������������
DECLARE_PROPERTY(float, Terrain, PixelError, GetPixelErrorAttr, SetPixelError);

/// property: string FilePath
/// \brief ������Դ�ļ�����·��
DECLARE_PROPERTY(const char*, Terrain, FilePath, GetFilePath, SetFilePath);
/// property: string AppendPath
/// \brief ������ͼģ�͸���·��
DECLARE_PROPERTY(const char*, Terrain, AppendPath, GetAppendPath, 
	SetAppendPath);
/// property: string ZoneLightPath
/// \brief ������������ļ�·��
DECLARE_PROPERTY(const char*, Terrain, ZoneLightPath, GetZoneLightPath, 
	SetZoneLightPath);
/// property: string ModelLightPath
/// \brief ����ģ�͹����ļ�·��
DECLARE_PROPERTY(const char*, Terrain, ModelLightPath, GetModelLightPath, 
	SetModelLightPath);
/// property: string WalkablePath
/// \brief �������ļ�·��
DECLARE_PROPERTY(const char*, Terrain, WalkablePath, GetWalkablePath, 
	SetWalkablePath);
/// property: string HeightMap
/// \brief �����õĸ߶�ͼ�ļ�
DECLARE_PROPERTY(const char*, Terrain, HeightMap, GetHeightMap, SetHeightMap);
/// property: float InitHeight
/// \brief ���εĳ�ʼ�߶�
DECLARE_PROPERTY(float, Terrain, InitHeight, GetInitHeight, SetInitHeight);
/// property: float UnitSize
/// \brief ��λ���Ӵ�С
DECLARE_PROPERTY(float, Terrain, UnitSize, GetUnitSizeAttr, SetUnitSize);

/// property: object LightModel
/// \brief �ƹ��־ģ��
DECLARE_PROPERTY(PERSISTID, Terrain, LightModel, GetLightModelID, 
	SetLightModelID);
/// property: object SoundModel
/// \brief ������־ģ��
DECLARE_PROPERTY(PERSISTID, Terrain, SoundModel, GetSoundModelID, 
	SetSoundModelID);
/// property: object SoundRangeModel
/// \brief ������Χģ��
DECLARE_PROPERTY(PERSISTID, Terrain, SoundRangeModel, GetSoundRangeModelID, 
	SetSoundRangeModelID);
/// property: object ParticleModel
/// \brief ���ӱ�־ģ��
DECLARE_PROPERTY(PERSISTID, Terrain, ParticleModel, GetParticleModelID, 
	SetParticleModelID);
/// property: object EffectModel
/// \brief ��Ч��־ģ��
DECLARE_PROPERTY(PERSISTID, Terrain, EffectModel, GetEffectModelID, 
	SetEffectModelID);
/// property: object TriggerModel
/// \brief ��������־ģ��
DECLARE_PROPERTY(PERSISTID, Terrain, TriggerModel, GetTriggerModelID, 
	SetTriggerModelID);
/// property: object TreeModel
/// \brief ��ľ��־ģ��
DECLARE_PROPERTY(PERSISTID, Terrain, TreeModel, GetTreeModelID, 
	SetTreeModelID);

/// property: bool EnableNormalMap
/// \brief �Ƿ�ʹ�õ��淨����ͼ
DECLARE_PROPERTY(bool, Terrain, EnableNormalMap, GetEnableNormalMap, 
	SetEnableNormalMap);
/// property: bool EnableTessellation
/// \brief �Ƿ�ʹ�ñ���ϸ��
DECLARE_PROPERTY(bool, Terrain, EnableTessellation, GetEnableTessellation, 
	SetEnableTessellation);
/// property: float TessellationFactor
/// \brief ����ϸ�ֲ���
DECLARE_PROPERTY(float, Terrain, TessellationFactor, GetTessellationFactor, 
	SetTessellationFactor);
/// property: float NormalMapRadius
/// \brief ���淨����ͼ��Ч��Χ
DECLARE_PROPERTY(float, Terrain, NormalMapRadius, GetNormalMapRadius, 
	SetNormalMapRadius);

/// property: string GlobalEnvironmentMap
/// \brief ȫ�ֻ�����ͼ
DECLARE_PROPERTY(const char*, Terrain, GlobalEnvironmentMap, 
	GetGlobalEnvironmentMap, SetGlobalEnvironmentMap);

DECLARE_PROPERTY(const char*, Terrain, DynamicWalkablePath, GetDynamicWalkablePath, 
	SetDynamicWalkablePath); 
/*
/// method: float GetLoadPercent()
/// \brief ��ü��ذٷֱ�
DECLARE_METHOD_0(float, Terrain, GetLoadPercent);				 
/// method: [bool, float, float, float] Pick(int x, int y, float distance)
/// \brief ����������λ��
/// \param x,y ��Ļ����
/// \param distance ������
/// \return �������������
DECLARE_METHOD_N(Terrain, Pick);
/// method: [bool, float, float, float] PickGround(int x, int y, float distance)
/// \brief ���������ĵ���λ��
/// \param x,y ��Ļ����
/// \param distance ������
/// \return �������������
DECLARE_METHOD_N(Terrain, PickGround);

/// method: object PickVisual(int x, int y, float distance)
/// \brief ����ѡ��������
/// \param x,y ��Ļ����
/// \param distance ������
DECLARE_METHOD_3(PERSISTID, Terrain, PickVisual, int, int, float);
/// method: float GetPosiY(float x, float z)
/// \brief ���ָ�����Y���꣨����٣�
/// \param x,y ��Ļ����
DECLARE_METHOD_2(float, Terrain, GetPosiY, float, float);
/// method: float GetGroundHeight(float x, float z)
/// \brief ��þ�ȷ�ĵ���߶�
/// \param x,z ����
DECLARE_METHOD_2(float, Terrain, GetGroundHeight, float, float);
/// method: float TraceHeight(float beg_x, float beg_z, float high_y, float low_y)
/// \brief ���߶�
/// \param beg_x ��ʼX����
/// \param beg_z ��ʼZ����
/// \param high_y ��ߵ�
/// \param low_y ��͵�
/// \return 0��1.0��ʾ����ײ
DECLARE_METHOD_4(float, Terrain, TraceHeight, float, float, float, float);
/// method: [object,x,y,z] TraceCollide(float beg_x, float beg_y, float beg_z, 
///		float end_x, float end_y, float end_z)
/// \brief ���������ײ���Ķ����λ��
/// \param beg_x,beg_y,beg_z ��ʼ������
/// \param end_x,end_y,end_z ��ֹ������
/// \return ���������Ķ���ID����ײλ������
DECLARE_METHOD_N(Terrain, TraceCollide);
/// method: [tex_name,object,x,y,z] TraceTexture(float beg_x, float beg_y, float beg_z, 
///		float end_x, float end_y, float end_z)
/// \brief ���������ײ���Ķ���λ�ú���ͼ��
/// \param beg_x,beg_y,beg_z ��ʼ������
/// \param end_x,end_y,end_z ��ֹ������
/// \return ������������ͼ��,����ID,��ײλ������
DECLARE_METHOD_N(Terrain, TraceTexture);
/// method: bool TraceHit(float beg_x, float beg_y, float beg_z, 
///		float end_x, float end_y, float end_z)
/// \brief ���������Ƿ�����ײ
/// \param beg_x,beg_y,beg_z ��ʼ������
/// \param end_x,end_y,end_z ��ֹ������
DECLARE_METHOD_6(bool, Terrain, TraceHit, float, float, float, float, float, 
	float);

/// method: bool GetCollideEnable(float x, float z)
/// \brief ��ײ�����Ƿ����
/// \param x,z ����
DECLARE_METHOD_2(bool, Terrain, GetCollideEnable, float, float);
/// method: float GetApexHeight(float x, float z)
/// \brief ���ָ��λ�õ����߶�
/// \param x,z ����
DECLARE_METHOD_2(float, Terrain, GetApexHeight, float, float);
/// method: int GetApexFloor(float x, float z)
/// \brief ���ָ��λ�õ����߶����ڵĲ�
/// \param x,z ����
DECLARE_METHOD_2(int, Terrain, GetApexFloor, float, float);
/// method: float GetBottomHeight(float x, float z)
/// \brief �����Ϳ����߲�ĸ߶�
/// \param x,z ����
DECLARE_METHOD_2(float, Terrain, GetBottomHeight, float, float);
/// method: int GetBottomFloor(float x, float z)
/// \brief �����Ϳ����߲�
/// \param x,z ����
DECLARE_METHOD_2(int, Terrain, GetBottomFloor, float, float);
/// method: bool GetWalkEnable(float x, float z)
/// \brief ����Ƿ������
/// \param x,z ����
DECLARE_METHOD_2(bool, Terrain, GetWalkEnable, float, float);
/// method: float GetWalkHeight(float x, float z)
/// \brief ��ÿ����ߵĸ߶�
/// \param x,z ����
DECLARE_METHOD_2(float, Terrain, GetWalkHeight, float, float);
/// method: int GetWalkFloor(float x, float z)
/// \brief ��ÿ����߲�����
/// \param x,z ����
DECLARE_METHOD_2(int, Terrain, GetWalkFloor, float, float);

/// method: bool GetWalkWaterExists(float x, float z)
/// \brief ��ÿ�����ˮ���Ƿ����
/// \param x,z ����
DECLARE_METHOD_2(bool, Terrain, GetWalkWaterExists, float, float);
/// method: float GetWalkWaterHeight(float x, float z)
/// \brief ��ÿ�����ˮ��ĸ߶�
/// \param x,z ����
DECLARE_METHOD_2(float, Terrain, GetWalkWaterHeight, float, float);
/// method: float GetWaterWaveHeight(float x, float z)
/// \brief ���ˮ�沨�˵ľ�ȷ�߶�
/// \param x,z ����
DECLARE_METHOD_2(float, Terrain, GetWaterWaveHeight, float, float);

/// method: int GetFloorCount(float x, float z)
/// \brief ��ò�����
/// \param x,z ����
DECLARE_METHOD_2(int, Terrain, GetFloorCount, float, float);
/// method: bool GetFloorExists(float x, float z, int floor)
/// \brief ��ò��Ƿ����
/// \param x,z ����
/// \param floor ����ţ�1��GetFloorCount() - 1��
DECLARE_METHOD_3(bool, Terrain, GetFloorExists, float, float, int);
/// method: bool GetFloorCanMove(float x, float z, int floor)
/// \brief ��ò��Ƿ���ƶ�
/// \param x,z ����
/// \param floor ����ţ�0��GetFloorCount() - 1��
DECLARE_METHOD_3(bool, Terrain, GetFloorCanMove, float, float, int);
/// method: bool GetFloorCanStand(float x, float z, int floor)
/// \brief ��ò��Ƿ��վ��
/// \param x,z ����
/// \param floor ����ţ�0��GetFloorCount() - 1��
DECLARE_METHOD_3(bool, Terrain, GetFloorCanStand, float, float, int);
/// method: float GetFloorHeight(float x, float z, int floor)
/// \brief ��ò�ĵĸ߶�
/// \param x,z ����
/// \param floor ����ţ�1��GetFloorCount() - 1��
DECLARE_METHOD_3(float, Terrain, GetFloorHeight, float, float, int);
/// method: float GetFloorSpace(float x, float z, int floor)
/// \brief ��ò�ĵ����ϰ��ռ�߶�
/// \param x,z ����
/// \param floor ����ţ�1��GetFloorCount() - 1��
DECLARE_METHOD_3(float, Terrain, GetFloorSpace, float, float, int);
/// method: bool GetFloorHasWall(float x, float z, int floor)
/// \brief ��ò��Ƿ���ǽ��
/// \param x,z ����
/// \param floor ����ţ�1��GetFloorCount() - 1��
DECLARE_METHOD_3(bool, Terrain, GetFloorHasWall, float, float, int);
/// method: bool GetWallExists(float x, float y, float z)
/// \brief ���ָ���ռ�λ���Ƿ���ǽ��
/// \param x,y,z ����
DECLARE_METHOD_3(bool, Terrain, GetWallExists, float, float, float);

/// method: [posi_x, posi_y, posi_z, angle_x, angle_y, angle_z] GetWalkWallAngle(
///		float beg_x, float beg_y, float beg_z, float end_x, float end_y, float_z)
/// \brief ��ÿ�����ǽ��λ�úͽǶ�
DECLARE_METHOD_N(Terrain, GetWalkWallAngle);
/// method: [posi_x, posi_y, posi_z, angle_y] GetWalkWallCollide(
///		float beg_x, float beg_y, float beg_z, float end_x, float end_y, float_z)
/// \brief ��ÿ�����ǽ����ײ
DECLARE_METHOD_N(Terrain, GetWalkWallCollide);

/// method: table GetLineTraverse(float src_x, float src_z, float dst_x, float dst_z)
/// \brief ������߾�����������ײ�����������
/// \param src_x,src_z ������ʼ������
/// \param dst_x,dst_z ������ֹ������
DECLARE_METHOD_T(Terrain, GetLineTraverse);
/// method: [hit_x, hit_z] GetLineIntersect(float src_x, float src_z, 
///		float dst_x, float dst_z, float grid_x, float grid_z)
/// \brief �����������ײ����״��ཻλ��
/// \param src_x,src_z ������ʼ������
/// \param dst_x,dst_z ������ֹ������
/// \param grid_x,grid_z ��ײ������
/// \return hit_x,hit_z �״��ཻ������λ��
DECLARE_METHOD_N(Terrain, GetLineIntersect);

/// method: object GetVisual(string name)
/// \brief ���ָ�����ֵĵ�������
/// \param name ��������
DECLARE_METHOD_1(PERSISTID, Terrain, GetVisual, const char*);
/// method: object GetVisualExists(object obj)
/// \brief ��鳡���Ƿ����ָ��������
/// \param obj �����
DECLARE_METHOD_1(bool, Terrain, GetVisualExists, const PERSISTID&);
/// method: bool AddVisual(string name, object obj)
/// \brief ��ӵ�������
/// \param name ������
/// \param obj �����
DECLARE_METHOD_2(bool, Terrain, AddVisual, const char*, const PERSISTID&);
/// method: bool AddVisualRole(string name, object obj)
/// \brief �������
/// \param name ������
/// \param obj �����
DECLARE_METHOD_2(bool, Terrain, AddVisualRole, const char*, const PERSISTID&);
/// method: bool RemoveVisual(object obj)
/// \brief �Ƴ���������
/// \param obj �����
DECLARE_METHOD_1(bool, Terrain, RemoveVisual, const PERSISTID&);
/// method: bool RelocateVisual(object obj, float x, float y, float z)
/// \brief ���¶�λ���������λ��
/// \param obj �����
/// \param x λ��X
/// \param y λ��Y
/// \param z λ��Z
DECLARE_METHOD_4(bool, Terrain, RelocateVisual, const PERSISTID&, float, 
	float, float)
/// method: bool VisualInView(object obj)
/// \brief �жϵ������嵱ǰ�Ƿ��ڿɼ���Χ��
/// \param obj �����
DECLARE_METHOD_1(bool, Terrain, VisualInView, const PERSISTID&);
/// method: bool SetVisualClipRadius(object obj, float clip_radius)
/// \brief ���õ���������þ���
/// \param obj �����
/// \param clip_radius ����
DECLARE_METHOD_2(bool, Terrain, SetVisualClipRadius, const PERSISTID&, float);
/// method: [table] GetNearActorList(float beg_x, float beg_z, float end_x, float end_z)
/// \brief ��ñ�����͸�������ָ����Χ�ڵ������������
/// \param beg_x,beg_z,end_x,end_z ��Χ
DECLARE_METHOD_T(Terrain, GetNearActorList);
/// method: int GetRolesInSphere(float x, float y, float z, float radius)
/// \brief ������巶Χ�ڵĻ��������
/// \param x,y,z �������ĵ�����
/// \param radius ����뾶
DECLARE_METHOD_4(int, Terrain, GetRolesInSphere, float, float, float, float);

/// method: bool AddTexturePath(string path)
/// \brief ��ӵ���������ͼ�ļ�·��
/// \param path �ļ�·�������·��֮����'|'�ָ�
DECLARE_METHOD_1(bool, Terrain, AddTexturePath, const char*);
/// method: bool ClearTexturePath()
/// \brief ��յ���������ͼ�ļ�·��
DECLARE_METHOD_0(bool, Terrain, ClearTexturePath);
/// method: int GetTexturePathCount()
/// \brief ��õ���������ͼ�ļ�·������
DECLARE_METHOD_0(int, Terrain, GetTexturePathCount);
/// method: table GetTexturePathList()
/// \brief ��õ���������ͼ�ļ�·����
DECLARE_METHOD_T(Terrain, GetTexturePathList);
/// method: string GetTexturePathString()
/// \brief ��õ���������ͼ�ļ�·����
DECLARE_METHOD_0(const char*, Terrain, GetTexturePathString);

/// method: int GetBaseTexIndex(string tex_name)
/// \brief ��û�����ͼ���ֶ�Ӧ������ֵ
/// \param tex_name ��ͼ��
DECLARE_METHOD_1(int, Terrain, GetBaseTexIndex, const char*);
/// method: bool FindBaseTex(string tex_name)
/// \brief ����ָ�����ֵĻ�����ͼ
/// \param tex_name ��ͼ��
DECLARE_METHOD_1(bool, Terrain, FindBaseTex, const char*);
/// method: bool AddBaseTex(string tex_name, string tex_path)
/// \brief ��ӻ�����ͼ
/// \param tex_name ��ͼ��
/// \param tex_path ��ͼ�ļ�·��
DECLARE_METHOD_2(bool, Terrain, AddBaseTex, const char*, const char*);
/// method: bool ClearBaseTex()
/// \brief ������л�����ͼ
DECLARE_METHOD_0(bool, Terrain, ClearBaseTex);
/// method: int GetBaseTexCount()
/// \brief ��û�����ͼ����
DECLARE_METHOD_0(int, Terrain, GetBaseTexCount);
/// method: table GetBaseTexList()
/// \brief ��û�����ͼ�����б�
DECLARE_METHOD_T(Terrain, GetBaseTexList);
/// method: string GetBaseTexName(int index)
/// \brief ���ָ�������Ļ�����ͼ��
/// \param index ����ֵ
DECLARE_METHOD_1(const char*, Terrain, GetBaseTexName, int);
/// method: string GetBaseTexPath(int index)
/// \brief ���ָ�������Ļ�����ͼ�ļ�·��
/// \param index ����ֵ
DECLARE_METHOD_1(const char*, Terrain, GetBaseTexPath, int);
/// method: bool SetBaseTexSpecularLevel(int index, float value)
/// \brief ����ָ������������ͼ�ĸ߹����� 
/// \param index ����ֵ
/// \param value ��ֵ
DECLARE_METHOD_2(bool, Terrain, SetBaseTexSpecularLevel, int, float);
/// method: float GetBaseTexSpecularLevel(int index)
/// \brief ���ָ������������ͼ�ĸ߹�����
/// \param index ����ֵ
DECLARE_METHOD_1(float, Terrain, GetBaseTexSpecularLevel, int);
/// method: bool SetBaseTexSpecularPower(int index, float value)
/// \brief ����ָ������������ͼ�Ĺ���� 
/// \param index ����ֵ
/// \param value ��ֵ
DECLARE_METHOD_2(bool, Terrain, SetBaseTexSpecularPower, int, float);
/// method: float GetBaseTexSpecularPower(int index)
/// \brief ���ָ������������ͼ�Ĺ���� 
/// \param index ����ֵ
DECLARE_METHOD_1(float, Terrain, GetBaseTexSpecularPower, int);
/// method: bool SetBaseTexHeightScale(int index, float value)
/// \brief ����ָ������������ͼ�ı���ϸ�ָ߶� 
/// \param index ����ֵ
/// \param value ��ֵ
DECLARE_METHOD_2(bool, Terrain, SetBaseTexHeightScale, int, float);
/// method: float GetBaseTexHeightScale(int index)
/// \brief ���ָ������������ͼ�ı���ϸ�ָ߶� 
/// \param index ����ֵ
DECLARE_METHOD_1(float, Terrain, GetBaseTexHeightScale, int);

/// method: int GetBlendTexIndex(string tex_name)
/// \brief ��û����ͼ���ֶ�Ӧ������ֵ
/// \param tex_name ��ͼ��
DECLARE_METHOD_1(int, Terrain, GetBlendTexIndex, const char*);
/// method: bool FindBlendTex(string tex_name)
/// \brief ����ָ�����ֵĻ����ͼ
/// \param tex_name ��ͼ��
DECLARE_METHOD_1(bool, Terrain, FindBlendTex, const char*);
/// method: bool AddBlendTex(string tex_name, string tex_path)
/// \brief ��ӻ����ͼ
/// \param tex_name ��ͼ��
/// \param tex_path ��ͼ�ļ�·��
DECLARE_METHOD_2(bool, Terrain, AddBlendTex, const char*, const char*);
/// method: bool ClearBlendTex()
/// \brief ������л����ͼ
DECLARE_METHOD_0(bool, Terrain, ClearBlendTex);
/// method: int GetBlendTexCount()
/// \brief ��û����ͼ����
DECLARE_METHOD_0(int, Terrain, GetBlendTexCount);
/// method: table GetBlendTexList()
/// \brief ��û����ͼ�����б�
DECLARE_METHOD_T(Terrain, GetBlendTexList);
/// method: string GetBlendTexName(int index)
/// \brief ���ָ�������Ļ����ͼ��
/// \brief index ����ֵ
DECLARE_METHOD_1(const char*, Terrain, GetBlendTexName, int);
/// method: string GetBlendTexPath(int index)
/// \brief ���ָ�������Ļ����ͼ�ļ�·��
/// \brief index ����ֵ
DECLARE_METHOD_1(const char*, Terrain, GetBlendTexPath, int);
/// method: bool SetBlendTexSpecularLevel(int index, float value)
/// \brief ����ָ�����������ͼ�ĸ߹����� 
/// \param index ����ֵ
/// \param value ��ֵ
DECLARE_METHOD_2(bool, Terrain, SetBlendTexSpecularLevel, int, float);
/// method: float GetBlendTexSpecularLevel(int index)
/// \brief ���ָ�����������ͼ�ĸ߹����� 
/// \param index ����ֵ
DECLARE_METHOD_1(float, Terrain, GetBlendTexSpecularLevel, int);
/// method: bool SetBlendTexSpecularPower(int index, float value)
/// \brief ����ָ�����������ͼ�Ĺ���� 
/// \param index ����ֵ
/// \param value ��ֵ
DECLARE_METHOD_2(bool, Terrain, SetBlendTexSpecularPower, int, float);
/// method: float GetBlendTexSpecularPower(int index)
/// \brief ���ָ�����������ͼ�Ĺ���� 
/// \param index ����ֵ
DECLARE_METHOD_1(float, Terrain, GetBlendTexSpecularPower, int);
/// method: bool SetBlendTexHeightScale(int index, float value)
/// \brief ����ָ�����������ͼ�Ĺ���� 
/// \param index ����ֵ
/// \param value ��ֵ
DECLARE_METHOD_2(bool, Terrain, SetBlendTexHeightScale, int, float);
/// method: float GetBlendTexHeightScale(int index)
/// \brief ���ָ�����������ͼ�ı���ϸ�ָ߶� 
/// \param index ����ֵ
DECLARE_METHOD_1(float, Terrain, GetBlendTexHeightScale, int);

/// method: bool AddBlendFormat(string name, int scale_u, int scale_v,
///		int angle_x, int angle_y, int angle_z)
/// \brief ���ӻ����ͼ��ʽ
/// \param name �����ͼ��
/// \param scale_u U�����ܶ�
/// \param scale_v V�����ܶ�
/// \param angle_x X����Ƕ�
/// \param angle_y Y����Ƕ�
/// \param angle_z Z����Ƕ�
DECLARE_METHOD_6(bool, Terrain, AddBlendFormat, const char*, int, int, int, 
	int, int);
/// method: int GetBlendFormatCount(string name)
/// \brief ���ָ�������ͼ�ĸ�ʽ����
/// \param name �����ͼ��
DECLARE_METHOD_1(int, Terrain, GetBlendFormatCount, const char*);
/// method: int GetBlendFormatScaleU(string name, int index)
/// \brief ���ָ�������ͼ��U�����ܶ�
/// \param name �����ͼ��
/// \param index ��ʽ����
DECLARE_METHOD_2(int, Terrain, GetBlendFormatScaleU, const char*, int);
/// method: int GetBlendFormatScaleV(string name, int index)
/// \brief ���ָ�������ͼ��V�����ܶ�
/// \param name �����ͼ��
/// \param index ��ʽ����
DECLARE_METHOD_2(int, Terrain, GetBlendFormatScaleV, const char*, int);
/// method: int GetBlendFormatAngleX(string name, int index)
/// \brief ���ָ�������ͼ��X����Ƕ�
/// \param name �����ͼ��
/// \param index ��ʽ����
DECLARE_METHOD_2(int, Terrain, GetBlendFormatAngleX, const char*, int);
/// method: int GetBlendFormatAngleY(string name, int index)
/// \brief ���ָ�������ͼ��Y����Ƕ�
/// \param name �����ͼ��
/// \param index ��ʽ����
DECLARE_METHOD_2(int, Terrain, GetBlendFormatAngleY, const char*, int);
/// method: int GetBlendFormatAngleZ(string name, int index)
/// \brief ���ָ�������ͼ��Z����Ƕ�
/// \param name �����ͼ��
/// \param index ��ʽ����
DECLARE_METHOD_2(int, Terrain, GetBlendFormatAngleZ, const char*, int);
/// method: bool SetBlendFormatScaleU(string name, int index, int value)
/// \brief ����ָ�������ͼ��U�����ܶ�
/// \param name �����ͼ��
/// \param index ��ʽ����
/// \param value ֵ
DECLARE_METHOD_3(bool, Terrain, SetBlendFormatScaleU, const char*, int, int);
/// method: bool SetBlendFormatScaleV(string name, int index, int value)
/// \brief ����ָ�������ͼ��V�����ܶ�
/// \param name �����ͼ��
/// \param index ��ʽ����
/// \param value ֵ
DECLARE_METHOD_3(bool, Terrain, SetBlendFormatScaleV, const char*, int, int);
/// method: bool SetBlendFormatAngleX(string name, int index, int value)
/// \brief ����ָ�������ͼ��X����Ƕ�
/// \param name �����ͼ��
/// \param index ��ʽ����
/// \param value ֵ
DECLARE_METHOD_3(bool, Terrain, SetBlendFormatAngleX, const char*, int, int);
/// method: bool SetBlendFormatAngleY(string name, int index, int value)
/// \brief ����ָ�������ͼ��Y����Ƕ�
/// \param name �����ͼ��
/// \param index ��ʽ����
/// \param value ֵ
DECLARE_METHOD_3(bool, Terrain, SetBlendFormatAngleY, const char*, int, int);
/// method: bool SetBlendFormatAngleZ(string name, int index, int value)
/// \brief ����ָ�������ͼ��Z����Ƕ�
/// \param name �����ͼ��
/// \param index ��ʽ����
/// \param value ֵ
DECLARE_METHOD_3(bool, Terrain, SetBlendFormatAngleZ, const char*, int, int);

/// method: bool SetFocusPosition(float x, float y, float z)
/// \brief �������������λ��
DECLARE_METHOD_3(bool, Terrain, SetFocusPosition, float, float, float);
/// method: bool SetFocusRadius(float x, float y, float z)
/// \brief �������������뾶
DECLARE_METHOD_3(bool, Terrain, SetFocusRadius, float, float, float);

/// method: object CreateWater(string name)
/// \brief ����ˮ������
/// \param name ��������
DECLARE_METHOD_1(PERSISTID, Terrain, CreateWater, const char*);
/// method: object GetWater(string name)
/// \brief ���ˮ������
/// \param name ��������
DECLARE_METHOD_1(PERSISTID, Terrain, GetWater, const char*);
/// method: bool DeleteWater(string name)
/// \brief ɾ��ˮ������
/// \param name ��������
DECLARE_METHOD_1(bool, Terrain, DeleteWater, const char*);
/// method: bool ClearWater()
/// \brief ɾ������ˮ������
DECLARE_METHOD_0(bool, Terrain, ClearWater);
/// method: string GetWaterName(int index)
/// \brief ���ָ��������ˮ����������
/// \param index ����
DECLARE_METHOD_1(const char*, Terrain, GetWaterName, int);
/// method: int GetWaterIndex(string name)
/// \brief ���ˮ����������
/// \param name ��������
DECLARE_METHOD_1(int, Terrain, GetWaterIndex, const char*);
/// method: int GetWaterCount()
/// \brief ���ˮ����������
DECLARE_METHOD_0(size_t, Terrain, GetWaterCount);
/// method: table GetWaterList()
/// \brief ���ˮ�������б�
DECLARE_METHOD_T(Terrain, GetWaterList);
/// method: bool RefreshWater()
/// \brief ǿ��ˢ��ˮ����ʾ
DECLARE_METHOD_0(bool, Terrain, RefreshWater);
/// method: bool SetWaterCubeMapStatic(bool value)
/// \brief ����ˮ�滷����ͼ�Ƿ�ֹ
/// \param value �Ƿ�ֹ
DECLARE_METHOD_1(bool, Terrain, SetWaterCubeMapStatic, bool);
/// method: bool DumpWaterCubeMap(int index, string file_name)
/// \brief ����ָ��������ˮ��Ļ�����ͼ
/// \param index ����
/// \param file_name �ļ���
DECLARE_METHOD_2(bool, Terrain, DumpWaterCubeMap, int, const char*);

/// method: bool GetWaterCurrent(float x, float z)
/// \brief �������ˮ�������ֵ��û��ˮ���򷵻�-1��
/// \param x,z λ��
DECLARE_METHOD_2(int, Terrain, GetWaterCurrent, float, float);

/// method: object CreateGrass(string name)
/// \brief ������������
/// \param name ��������
DECLARE_METHOD_1(PERSISTID, Terrain, CreateGrass, const char*);
/// method: object GetGrass(string name)
/// \brief ��û�������
/// \param name ��������
DECLARE_METHOD_1(PERSISTID, Terrain, GetGrass, const char*);
/// method: bool DeleteGrass(string name)
/// \brief ɾ��������
/// \param name ��������
DECLARE_METHOD_1(bool, Terrain, DeleteGrass, const char*);
/// method: bool ClearGrass()
/// \brief ɾ�����л�������
DECLARE_METHOD_0(bool, Terrain, ClearGrass);
/// method: string GetGrassName(int index)
/// \brief ���ָ�������Ļ�����������
/// \param index ����
DECLARE_METHOD_1(const char*, Terrain, GetGrassName, int);
/// method: int GetGrassIndex(string name)
/// \brief ��û�����������
/// \param name ��������
DECLARE_METHOD_1(int, Terrain, GetGrassIndex, const char*);
/// method: int GetGrassCount()
/// \brief ��û�����������
DECLARE_METHOD_0(size_t, Terrain, GetGrassCount);
/// method: table GetGrassList()
/// \brief ��û��������б�
DECLARE_METHOD_T(Terrain, GetGrassList);
/// method: bool RefreshGrass()
/// \brief ǿ��ˢ�»�����ʾ
DECLARE_METHOD_0(bool, Terrain, RefreshGrass);

/// method: bool RefreshGround()
/// \brief ǿ��ˢ�µ�����ʾ
DECLARE_METHOD_0(bool, Terrain, RefreshGround);
/// method: bool RefreshVisual()
/// \brief ǿ��ˢ�µ���������ʾ
DECLARE_METHOD_0(bool, Terrain, RefreshVisual);

/// method: bool SetTraceMask(string type, bool value)
/// \brief ������ײ����״̬
/// \param type �������ͣ�"Ground", "Model", "Particle", "Light", "Sound", "Actor"��
/// \param value ����״̬
DECLARE_METHOD_2(bool, Terrain, SetTraceMask, const char*, bool);
/// method: bool GetTraceMask(string type)
/// \brief �����ײ����״̬
/// \param type �������ͣ�"Ground", "Model", "Particle", "Light", "Sound", "Actor"��
DECLARE_METHOD_1(bool, Terrain, GetTraceMask, const char*);

/// method: table GetRegionList()
/// \brief �������ʾ��ɫ�ķ�Χ�����б�
DECLARE_METHOD_T(Terrain, GetRegionList);
/// method: bool GetRegionEnable(string name, float x, float z)
/// \brief ���ָ��λ�õķ�Χ����Ƿ���Ч
/// \param name ��Χ����
/// \param x,z ����
DECLARE_METHOD_3(bool, Terrain, GetRegionEnable, const char*, float, float);

/// method: table GetAreaList()
/// \brief �������ʾ��ɫ�ĵ��������б�
DECLARE_METHOD_T(Terrain, GetAreaList);
/// method: string GetAreaName(float x, float z)
/// \brief ���ָ��λ�õĵ�������
/// \param x,z ����
DECLARE_METHOD_2(const char*, Terrain, GetAreaName, float, float);

/// method: bool SetParameter(int zone_scale, int chunk_scale, 
///		int tex_scale, int tex_units, int alpha_per_unit, int collide_per_unit)
/// \brief ���õ��β���
/// \param zone_scale ����ߴ�
/// \param chunk_scale �ֿ�ߴ�
/// \param tex_scale ������ͼ�ߴ�
/// \param tex_units ������ͼ���ǵĵ�λ�ߴ�
/// \param alpha_per_unit ÿ�����ε�λ�����ڵ�ALPHAֵ����
/// \param collide_per_unit ÿ�����ε�λ�����ڵ���ײ��Ϣ����
DECLARE_METHOD_6(bool, Terrain, SetParameter, size_t, size_t, size_t, size_t, 
	size_t, size_t);
/// method: bool InitZones(int zone_rows, int zone_cols, 
///		int origin_row, origin_col, int zone_range)
/// \brief ��ʼ����������
/// \param zone_rows �����������
/// \param zone_cols �����������
/// \param origin_row ԭ�����ڵ���������
/// \param origin_col ԭ�����ڵ���������
/// \param zone_range ��Ұ�ɼ�������Χ
DECLARE_METHOD_5(bool, Terrain, InitZones, int, int, int, int, int);

/// method: bool ReleaseAllZoneLoader()
/// \brief ����ɾ��֮ǰɾ�����еؿ�ļ�����
DECLARE_METHOD_0(bool, Terrain, ReleaseAllZoneLoader);

/// method: float GetDiffuse(float x, float z)
/// \brief ���ָ��λ�õĹ���ͼ���ȣ�����0.0-1.0��
/// \param x,z λ������
//DECLARE_METHOD_2(float, Terrain, GetDiffuse, float, float);
/// method: string GetMainTexture(float x, float z)
/// \brief ���ָ��λ�õĻ�ϱ���������ͼ����
/// \param x,z λ������
DECLARE_METHOD_2(const char*, Terrain, GetMainTexture, float, float);

/// method: bool IsViewClip(float x, float y, float z, float radius)
/// \brief ��Χ���Ƿ��ڼ�������
/// \param x,y,z ��Χ������
/// \param radius ��Χ��뾶
DECLARE_METHOD_4(bool, Terrain, IsViewClip, float, float, float, float);

/// method: bool DumpHorizon(string file_name)
/// \brief ������ƽ����Ϣ���ļ�
/// \param file_name �ļ���
DECLARE_METHOD_1(bool, Terrain, DumpHorizon, const char*);

/// method: bool DumpZones(string file_name)
/// \brief ���������б��ļ�
/// \param file_name �ļ���
DECLARE_METHOD_1(bool, Terrain, DumpZones, const char*);

*/