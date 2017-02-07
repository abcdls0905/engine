//--------------------------------------------------------------------
// �ļ���:		FxCortex.h
// ��  ��:		
// ˵  ��:		
// ��������:	2012��3��26��
// ������:		����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef __FXCORTEX_H__
#define __FXCORTEX_H__

#include "cortex.h"
#include "../public/module.h"
#include "../visual/i_visual.h"
#include "../utils/lock_util.h"
#include <list>
#include <vector>
#include <string>

// ��Cortex������ģ��
// MotionCapture��д��Capture

class Actor2;
//struct sFxFrame;
//struct sFxBone;
//struct sFxMarkerVertex;
//struct sFxBody;

class FxCortex: public IVisual
{
protected:
	// ֡
	struct sFxFrame
	{
		bool m_bRender;			// ��֡�Ƿ���Ⱦ��
		sFrameOfData cortex_frame;		// cortex��֡����
	};

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

		// ������׽ģ�͵�Tpose�������
		FmMat4 mtxCaptureWorldMatrix;
		FmMat4 mtxCaptureWorldMatrixInverse;

		// ��Ϸ��actor��tpose��local matrix
		//FmMat4 matActorLocalMatrix;

		// cortex��ģ�͵�tpose����Ϸ��tpose��ӳ�����
		//FmMat4 matTposeCortexToActor;
//#ifdef _DEBUG
//		FmMat4 debug_matTposeCortexInverse;
//#endif
//
//#ifdef _DEBUG
//		D3DVECTOR debug_vTranslation;
//		FmMat4 debug_local_mat;
//#endif
//
//		// ��Ӧ��actor��������
//		std::string strActorBoneName;
	};

	typedef std::vector<sFxBone> BONES_ARRAY;

	// actor�����Ͷ�����׽�����Ķ�Ӧ��ϵ��
	struct sBoneMap
	{
		std::string strActorBoneName;		// actor�й���������
		std::string strActorBoneParentName;	// actor�й����ĸ���������
		FmMat4 mtxActorBoneLocalMatrix;	// actor�й�����local����

		std::string strCaptureBoneName;		// ������׽�Ĺ���������
		unsigned int nCaptureBodyIndex;		// ��ʾ���ĸ�body
		unsigned int nCaptureBoneIndex;		// ��ʾ��body���ĸ�����

		// ������׽��ģ��tpose��actor��tpose��ת������
		FmMat4 mtxTposeCaptureToActor;
	};
	typedef std::vector<sBoneMap> BONES_MAP_CONTAINER;

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

		// ��body��bone array
		BONES_ARRAY bonesArray;
		//int nRootBoneIndex;

		// ��body��marker����marker���������ʼλ�ú�����
		unsigned int nMarkerOffset;
		unsigned int nMarkerCount;
	};

	typedef std::list<sFxFrame*> FrameList;

public:
	FxCortex();
	virtual ~FxCortex();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void Update(float seconds);
	virtual void Realize(float offset_seconds);

	// ������Ϣ�Ļص�����
	static void CortexErrorMsgHandler(int iLevel, char *szMsg);
	// ֡��Ϣ�Ļص�����
	static void CortexFrameDataHandler(sFrameOfData* FrameOfData);

	// ����Cortex
	bool ConnectCortex(const char* szMyIP, const char* szCortexIP);

	// �󶨵Ķ�������
	void SetActorBind(const PERSISTID& actor);
	PERSISTID GetActorBind() const;

	// �Ƿ����Cortex��֡��Ϣ
	bool GetReceiveCortexFrame() const { return m_bReceiveCortexFrame; }
	void SetReceiveCortexFrame(bool value) { m_bReceiveCortexFrame = value; }

	// �Ƿ��Cortex����
	bool GetConnected() const;

	// ��Cortex��ø����岿����������Ϣ
	bool GetBodyDefs();

	// ��Ⱦʱ�Ƿ���Ҫ��ֵ
	bool GetInterpolation() const { return false;/*m_bInterpolation; Ŀǰ����Ҫ��ֵ*/ }
	void SetInterpolation(bool value) { m_bInterpolation = value; }

	// marker�����ɫ
	void SetMarkerColorString(const char* value);
	std::string GetMarkerColorString() const;

	// ��cortex��ȡ��ǰ֡����
	bool GetFrameOfData();

	// �����cortex��ģ��tpose�任����Ϸģ��tpose�ı任����
	// ������cortex��ģ�ͺ���Ϸ���ģ�Ͷ�����tpose״̬ʱ���ܵ���
	bool CalcTposeTransformMatrix();

	// ���ö�����׽ģ�ͺ�actor�Ĺ�����Ӧ��ϵ
	bool SetBoneMap(const char* szCaptureBoneName, const char* szActorBoneName);

	void ClearBoneMap();

protected:
	// �ͷ���Դ
	// ����bReleaseMemPoolΪtrue��ʾ�ڴ��ҲҪ�ͷ�
	// Ϊ�����Ч�ʣ�Ŀǰֻ�ڱ�ʵ������ʱ���ͷ��ڴ��
	void ReleaseResource(bool bReleaseMemPool = false);

	// �ӵȴ������֡������ȡ��һ֡
	sFxFrame* PopFromWaitList();
	// ���һ����֡���ȴ������֡������
	void PushToWaitList(sFxFrame* pFrame);

	// �ӿ���֡��������һ����֡
	sFxFrame* PopFromFreeList();
	// ���һ����֡������֡������
	void PushToFreeList(sFxFrame* pFrame);

	// �������֡
	void AllocFreeFrame(unsigned int nAddFrameCount);

	Actor2* InnerGetActor() const;

protected:
	// �󶨵Ķ�������
	PERSISTID m_ActorBind;

	// �Ƿ��Cortex����
	bool m_bConnected;

	// Cortex��������Ϣ
	sHostInfo m_CortexHostInfo;

	// ��Cortex��õĸ����岿����������Ϣ
	sBodyDefs* m_pBodyDefs;
	//BodyArray m_vBodys;

	// ��ǰ������Ⱦ�������ؼ�֡
	sFxFrame* m_pPrevFrame;
	sFxFrame* m_pNextFrame;

	// �ȴ���Ⱦ��֡����
	FrameList m_WaitRenderFrameData;
	// ����֡����
	FrameList m_FreeFrameData;
	// �ڴ�������
	std::vector<char*> m_Buffers;

	// ���ϸ��ؼ�֡�����ڵ�ʱ��
	float m_fSecondsFromPrevFrame;
	// �����ؼ�֡�����ʱ��
	float m_fSecondsBetweenTwoFrames;

	// Cortex_sdk�����紫���Ǹ�������̣߳���Щ�ط���Ҫ����
	CLockUtil m_Lock;

	// cortex��IP
	std::string m_strCortexIP;

	// ����body��marker����
	int m_nMarkers;
	IDxDynamicVB* m_pMarkerDynamicVB;

	// body����
	std::vector<sFxBody> m_bodys;

	// �Ƿ����Cortex��֡��Ϣ
	bool m_bReceiveCortexFrame;

	// ��Ⱦʱ�Ƿ���Ҫ��ֵ
	bool m_bInterpolation;

	// marker�����ɫ
	unsigned int m_nMarkerColor;

	int m_nShader;

	// actor�����Ͷ�����׽�����Ķ�Ӧ��ϵ��
	BONES_MAP_CONTAINER m_bonesMap;

#ifdef _DEBUG
	// ���Է����ڴ�Ĵ���
	// debug�汾���Խ���ǣ�
	//		ͨ��Ԥ����400��֡�ȸ���ʵ�鷢��:
	//			Ԥ�������֡����Ӱ��ʵ��ʹ��֡���ķ�ֵ����Ч��û����Ӱ��
	//			ÿ��֡����ʱһ�δ�����֡����������Ӱ��ʵ��ʹ��֡���ķ�ֵ����Ч��û����Ӱ��
	// debug�汾���ȴ��������֡����ֵ��300~400
	// ����Ŀǰ������ǣ�
	//		Ԥ�������ٵ�����ÿ�δ�������Ҳ����
	std::vector<unsigned int> m_vDebugAllocTime;
#endif
};

#endif // __FXCORTEX_H__
