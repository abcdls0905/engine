//--------------------------------------------------------------------
// �ļ���:		FxVicon.h
// ��  ��:		
// ˵  ��:		
// ��������:	2012��5��10��
// ������:		����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef __FXVICON_H__
#define __FXVICON_H__

#include "client.h"
#include "../public/module.h"
#include "../visual/i_visual.h"
#include "../utils/lock_util.h"
#include <list>
#include <vector>
#include <string>

// ��Vicon������ģ��

using namespace ViconDataStreamSDK::CPP;

class Actor2;
//struct sFxFrame;
//struct sFxBone;
//struct sFxMarkerVertex;
//struct sFxBody;

class FxVicon: public IVisual
{
protected:
	//// ֡
	//struct sFxFrame
	//{
	//	bool m_bRender;			// ��֡�Ƿ���Ⱦ��
	//	sFrameOfData cortex_frame;		// cortex��֡����
	//};

	// marker����
#define MARKER_FVF (D3DFVF_XYZ|D3DFVF_DIFFUSE)
	struct sFxMarkerVertex
	{
		D3DVECTOR pos;
		unsigned int diffuse;
	};

	// ��������
	struct sFxBone
	{
		sFxBone() 
		{ 
			nParentBoneIndex = 0; 
			FmMatrixIdentity(&mat);
		}

		std::string strBoneName;
		FmMat4 mat;
		int nParentBoneIndex;
#ifdef _DEBUG
		D3DVECTOR debug_vTranslation;
		FmMat4 debug_local_mat;
#endif
		// ��Ϸ��actor��tpose��local matrix
		FmMat4 matActorLocalMatrix;

		// vicon��ģ�͵�tpose����Ϸ��tpose��ӳ�����
		FmMat4 matTposeViconToActor;
	};

	typedef std::vector<sFxBone> BONES_ARRAY;

	struct sFxMarker
	{
		std::string strMarkerName;
	};
	typedef std::vector<sFxMarker> MARKER_ARRAY;

	// body
	// ��ӦCortex������һ��MarkerSet
	struct sFxBody
	{
		sFxBody()
		{
			//nRootBoneIndex = 0;
			nMarkerOffset = 0;
			nMarkerCount = 0;
		}

		std::string strBodyName;

		// ��body��bone array
		BONES_ARRAY bonesArray;
		//int nRootBoneIndex;

		// ��body��marker array
		MARKER_ARRAY markersArray;

		// ��body��marker����marker���������ʼλ�ú�����
		unsigned int nMarkerOffset;
		unsigned int nMarkerCount;
	};
	//typedef std::list<sFxFrame*> FrameList;

public:
	FxVicon();
	virtual ~FxVicon();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void Update(float seconds);
	virtual void Realize(float offset_seconds);

	// ����Vicon
	bool ConnectVicon(const char* szViconIP);

	// �󶨵Ķ�������
	void SetActorBind(const PERSISTID& actor);
	PERSISTID GetActorBind() const;

	// �Ƿ����Vicon��֡��Ϣ
	bool GetReceiveViconFrame() const { return m_bReceiveViconFrame; }
	void SetReceiveViconFrame(bool value) { m_bReceiveViconFrame = value; }

	// �Ƿ��Cortex����
	bool GetConnected() const;

	// ��ø�������������Ϣ
	bool GetBodyDefs();

	// marker�����ɫ
	void SetMarkerColorString(const char* value);
	std::string GetMarkerColorString() const;

	// ��ȡ��ǰ֡����
	bool GetFrameOfData();

	// �����cortex��ģ��tpose�任����Ϸģ��tpose�ı任����
	// ������cortex��ģ�ͺ���Ϸ���ģ�Ͷ�����tpose״̬ʱ���ܵ���
	bool CalcTposeTransformMatrix();

protected:
	// �ͷ���Դ
	//void ReleaseResource();

	Actor2* InnerGetActor() const;

protected:
	// �󶨵Ķ�������
	PERSISTID m_ActorBind;

	// Vicon client class
	Client m_ViconClient;

	//// ��Cortex��õĸ����岿����������Ϣ
	//sBodyDefs* m_pBodyDefs;
	////BodyArray m_vBodys;

	// vicon��IP
	std::string m_strViconIP;

	// ����body��marker����
	int m_nMarkers;
	IDxDynamicVB* m_pMarkerDynamicVB;

	// body����
	std::vector<sFxBody> m_bodys;

	// �Ƿ����Vicon��֡��Ϣ
	bool m_bReceiveViconFrame;

	// marker�����ɫ
	unsigned int m_nMarkerColor;

	int m_nShader;

	// ��ǰ֡���
	int m_nFrameNumber;
};

#endif // __FXVICON_H__
