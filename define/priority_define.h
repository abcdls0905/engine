//----------------------------------------------------------
// �ļ���:		priority_define.h
// ��  ��:		���ȼ�����
// ˵  ��:		
// ������:		
// ��������:    2014/04/2
// ��Ȩ����:    ������ţ�������޹�˾
//----------------------------------------------------------

#ifndef __PRORIORITY_DEFINE_H_
#define __PRORIORITY_DEFINE_H_

//
enum PriorityScene
{
	Priority_Scene_LightManager		= 3, //�ƹ�
	Priority_Scene_Geometry			= 10,  //����ͼ��
	Priority_Scene_LinesDrawer		= 10, //�߻���
	Priority_Scene_UI3D				= 20,  //����3DUI
	Priority_Scene_TimeAxis			= 20,  //ʱ����
	Priority_Scene_Actor			= 20,  
	Priority_Scene_Model			= 20, 
	Priority_Scene_EffectModel		= 20, //��Чģ��
	Priority_Scene_SoundManager		= 27, //����
	Priority_Scene_Terrain			= 30, //����
	Priority_Scene_ShadowManager	= 70, //��Ӱ
	Priority_Scene_PostEffectFilter	= 80, //����
	Priority_Scene_SunGlow			= 80, //����
	Priority_Scene_ParticleManager	= 100, //����
	Priority_Scene_CameraWrapper	= 100, //�����װ
	Priority_Scene_DynamicWeather	= 200, //��̬����
	Priority_Scene_SaberArcManager	= 512, //��β
};
#endif