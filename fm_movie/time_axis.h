//--------------------------------------------------------------------
// �ļ���:		time_axis.h
// ��  ��:		
// ˵  ��:		
// ��������:	2012��8��6��
// ������:		����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------
#ifndef __TIME_AXIS_H__
#define __TIME_AXIS_H__

#include "key_frame_controller.h"
#include "../visual/i_time_axis.h"
#include "../visual/i_render.h"
#include "../visual/i_res_loader.h"
#include "../utils/pod_hash_map.h"
#include "../utils/string_pod.h"

// ʱ����

class Pyramid;
class IDxDynamicVB;
class TiXmlDocument;
class TiXmlElement;

// һ�㶥������
struct vertex_t 
{
	FmVec3 pos;
};

// �ؼ�֡��������
struct vertex_key_frame_t
{
	FmVec3 pos;
	float u, v;
};

// ���Կ�������
typedef TPodHashMap<int, KeyFrameController*, TPodTraits<int>, TCoreAlloc> KeyFrameControllers;
//TPodHashMap
// ��������
struct sObjectData
{
	sObjectData();
	~sObjectData();
	//
	// ��������
	result_string		strName;
	PERSISTID			ObjectID;
	KeyFrameControllers controllers;		// �������ԵĿ�����
	bool				bShowPath;			// �Ƿ���ʾ·��

	bool				bLocalPosition;		// �Ƿ�洢�����������֡
	float				fOriginX;			// �������ϵ��ԭ��
	float				fOriginY;			// �������ϵ��ԭ��
	float				fOriginZ;			// �������ϵ��ԭ��
	FmVec3			    initPos;			// ��ʼ�����λ��

	bool				bRotation;			// �켣�Ƿ�����ת
	float				fRotationX;
	float				fRotationY;
	float				fRotationZ;
	float				fRAddY;				// Y��������ת����

	result_string		strExtProp;			// ����ĸ��Ӳ���

	// �Ƿ������ݸĶ�
	bool				bDataChanged;

	// �첽�������֮����Ҫˢ�µı��
	IResLoader*         pLoader;

    // ��ǰ֡��λ������ת
    float			    fPosInfo[6];

	// ���ڵ���ʱ���Ƶ�����
	vertex_t*           pFrameVBuffer;		        // ֡��������
#ifdef _DEBUG
	TArrayPod<vertex_t, 1> vFrameVBufferUsedDebug;	// �����õ�
#endif
	unsigned int        nFrameVBufferCount;		    // ֡����buffer�Ķ������
	unsigned int        nFrameVCountUsed;			// ʵ��ʹ�õ�֡���������ȥ��������һ����֡����

	vertex_key_frame_t* pKeyFrameVBuffer;	        // �ؼ�֡��������
	unsigned int        nKeyFrameVBufferCount;		// �ؼ�֡����buffer�Ķ������

    vertex_key_frame_t  currentFrame[8];           // ��ǰ֡��λ��

    void UpdateCurrentFrame();
};

class TimeAxis: public ITimeAxis
{
private:
	// ����������ж���
	typedef TStringPod<char, sObjectData*> Objects;

public:
	TimeAxis();
	virtual ~TimeAxis();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void Update(float seconds);
	virtual void Realize();

	// ����
	void Play();
	// ��ͣ��ָ��ʱ��
	void Pause(float time);
	// ��ͣ�ڵ�ǰʱ��
	void PauseNow();

	// �Ƿ�������
	bool GetLoadFinish();
	size_t GetLoadCount() const;
	// ��Ӷ���
	bool AddObject(const char* object_name);
	// ɾ������
	bool DeleteObject(const char* object_name);
	// �����Ƿ����
	bool IsObjectExist(const char* object_name);
	// ������������
	bool CopyObjectData(const char* src_object_name, const char* dst_object_name);
	// ����������
	bool ReName(const char* src_object_name, const char* new_object_name);
	// ��һ������������Ϣ��ֵ����һ������
	bool CopyControllerData(const char* src_object_name, int controller_type, 
		const char* dst_object_name);
	// �����ؼ�֡
	void CopyKeyTime(const char* object_name, float time, float new_time);
	// ����ĳ���������Ĺؼ�֡
	void CopyControllerKeyTime(const char* object_name, int controller_type,
		float time, float new_time);
	// �ؼ�֡�����Ƿ����
	bool IsKeyFrameExist(const char* object_name, int controller_type, float frame_time) const;

	// ����ID
	bool SetObjectID(const char* object_name, const PERSISTID& value);
	PERSISTID GetObjectID(const char* object_name) const;

	// ����Bezier�ؼ���
	bool AddBezierKey(const char* object_name, int controller_type, float time, float value);
	// �ı���Ƶ�ļ��㷽ʽ
	void ChangeComputeTypeKey(const char* object_name, int controller_type, float time, int type);
	// ���¼������еĿ��Ƶ�
	void CreateAllControlPoints(const char* object_name, int controller_type);
	// �ƶ�����ؼ�֡
	void MoveKeyTimes(const char* object_name, float time_add);
	// �ƶ��������Ĺؼ�֡
	void MoveControllerKeyTimes(const char* object_name, int controller_type, float time_add);
	// �ƶ�Bezier����
	void MoveAllKeyValue(const char* object_name, int controller_type, float value_add);
	// �ƶ��ؼ�֡
	void MoveKey(const char* object_name, float time, float new_time);
	// �ƶ��ؼ�֡
	void MoveControllerKey(const char* object_name, int controller_type, float time, float new_time);

	// ��ȡҪ��¼�ؼ����ǰһ���ؼ���
	void GetFrontBezierKey(const IVarList &param, IVarList &result);

	// ��ȡ���д���ؼ�֡��Ϣ�����ڹؼ�֡����(�뱣֤ʹ���귵�صĹؼ�ָ֡��ǰ���޸Ļ�ɾ���ؼ�֡��Ϣ)
	void GetAdjacencyBezierKeyData(const IVarList &args, IVarList &result) const;

	// ���ӻ��޸Ĺؼ�֡
	bool SetKeyFrameBezierMode(const char* object_name, int controller_type, 
		float time, float value, 
		float left_time, float left_value, 
		float right_time, float right_value);
	bool SetKeyFrameOnceMode(const char* object_name, int controller_type, 
		float time, const char* param);
	bool SetKeyFrameLinearMode(const char* object_name, int controller_type, 
		float time, float value);
	bool SetKeyFrameScaleMode(const char* object_name, int controller_type, 
		float time, const char* action_name, float time_length, float speed);

	// ɾ��ĳʱ�̵����йؼ�֡
	void DeleteKey(const char* object_name, float time);
	// ɾ���ؼ�֡
	bool DeleteControllerKey(const char* object_name, int controller_type, 
		float time);
	// ���ָ�������������йؼ�֡
	void ClearControllerKeyFrames(const char* object_name, int controller_type);
	// ��ն������п����������йؼ�֡
	void ClearControllers(const char* object_name);
	// ��ùؼ�֡�б�
	void GetControllerKeyTime(const IVarList& args, IVarList& result) const;
	int GetControllerKeyCount(const char* object_name, int controller_type) const;
	int GetKeyFrameListBezierMode(const IVarList& args, IVarList& result) const;
	int GetKeyFrameListOnceMode(const IVarList& args, IVarList& result) const;
	int GetKeyFrameListLinearMode(const IVarList& args, IVarList& result) const;
	int GetKeyFrameListScaleMode(const IVarList& args, IVarList& result) const;
	// ��ö������п�����ID
	int GetControllers(const IVarList& args, IVarList& result) const;
	// ��ȡָ��ʱ��Ŀ�����ֵ
	float GetValueBezierMode(const char* object_name, int controller_type, 
		float time) const;
	//const char* GetValueOnceMode(const char* object_name, int controller_type, 
	//	float time) const;
	// ��ȡĳһʱ�̹ؼ�֡֡����
	void GetKeyFrameValue(const IVarList& args, IVarList& result) const;
	// ��ȡ�洢�켣���������ͣ��Ƿ���������꣩
	bool GetIsLocalPosition(const char* object_name) const;
	// ��ȡ�����������ϵ������
	void GetLocalOrigin(const IVarList &args, IVarList& result) const;
	// ���ö����������ϵ������
	void SetLocalOrigin(const IVarList &args, IVarList& result);

	// ��ȡ�洢�켣�Ƿ񾭹�����ת
	bool GetIsRotation(const char* object_name) const;
	// ��ȡ�켣��ת
	float GetRotationY(const char* object_name) const;
	// ���ù켣��ת,�����actor��Ҫ��ת�켣180
	void SetRotationY(const char* object_name, float fAngle);
	// ��ȡ�켣������ϵ��ת�Ƕ�
	void GetRotation(const IVarList& args, IVarList& result) const;
	// ���ù켣������ϵ��ת�Ƕ�
	void SetRotation(const IVarList& args, IVarList& result);

	// ��ȡ����Ķ�������
	const char* GetExtProp(const char* object_name) const;
	// ��ȡ��ʼ״̬��λ��(��Ҫ���Ŀ�������δkey������)
	void GetInitialPosition(const IVarList &args, IVarList& result);


	// ����ת��
	void PositionToLocal(const IVarList& args, IVarList& result) const;
	// �Ƕ�ת��
	void AngleToLocal(const IVarList& args, IVarList& result) const;

	// �Ƿ�ѭ������
	void SetLoop(bool value) { m_bLoop = value; }
	bool GetLoop() const { return m_bLoop; }

	// ��ʼʱ��
	void SetBeginTime(float value) { m_fBeginTime = value; }
	float GetBeginTime() const { return m_fBeginTime; }
	// ����ʱ��
	void SetEndTime(float value) { m_fEndTime = value; }
	float GetEndTime() const { return m_fEndTime; }

	// ��֡��ʱ��
	void SetFrameInterval(float value) { if (value > 0.0001f) { m_fFrameInterval = value; } }
	float GetFrameInterval() const { return m_fFrameInterval; }

	// ·���Ƿ�ɼ�
	void SetPathVisible(const char* object_name, bool bVisible);
	bool GetPathVisible(const char* object_name) const;

	// ����������ʾ��·����
	bool BuildPathPoint(const char* object_name);

	// ���ض���ؼ�֡
	bool LoadObjectKeyFrame(const char* object_name, const char* file, 
		bool bAsync);

	// �������ؼ�֡
	bool SaveObjectKeyFrame(const char* object_name, const char* file);
		
	// �������ж���
	bool SaveAllObject(const char* path, bool bForce);

	// ���켣���ݵ���max��ʽ.
	bool ChangeToMaxPath(sObjectData* pObjectData, float x, float y, float z, float angle_y, int scale);

	// ��������ؼ�֡��max(��������)
	bool ExportToMaxPath(const char* object_name, const char* file_name,
		float x, float y, float z, float angle_y, int scale);

	bool ExportToMaxPathFrames(const char* object_name, const char* file_name,
		float x, float y, float z, float angle_y, int scale);

	// �𶯹ؼ�֡���������.
	bool LoadRandomData(const char *file_name);
	bool SaveRandomData(const char *file_name);

    // ���ûص�����
    void SetCallbackFunc(time_axis_run_callback func)
    {
        m_pRunCallbackFunc = func;
    }

    time_axis_run_callback GetCallbackFunc()
    {
        return m_pRunCallbackFunc;
    }
public:

	// ��ÿ�����
	KeyFrameController* GetController(const sObjectData& object_data, int controller_type);

	// ��ÿ�����
	const KeyFrameController* GetController(const sObjectData& object_data, int controller_type) const;

	// ��ӿ�����
	KeyFrameController* AddController(sObjectData& object_data, 
		int controller_type, int controller_usage_mode);

private:
	IVisBase* GetVisBase(const PERSISTID& id) const;

	sObjectData* InnerGetObjectData(const char* object_name);
	const sObjectData* InnerGetObjectData(const char* object_name) const;
	sObjectData* InnerAddObjectData(const char* object_name);
	sObjectData* InnerAddObjectDataNoInitCtrl(const char* object_name);

	bool InnerSaveObjectKeyFrame(sObjectData* pObjectData, 
		const char* object_name, const char* file_path);

	void InnerUpdate(float time);

	void RotationKey(FmVec3 *pPos, FmVec3 *pAngle, 
		const sObjectData& object_data);

	void GetWobbleInfo(FmVec3 &pos, FmVec3 &angle, float time,
		float *fWobbleInfo, size_t nInfoLen = 7);
	
	float GetRandValue(int index);

	bool GetKeyData(sObjectData &object_data, IVisBase *pObject,  float time,
		float *fPosInfo, size_t nInfoLen = 6);

    // ������Ⱦ����
    struct BatchData
    {
        IShaderProgram* pShader;
        IDynamicVB* pVBuffer;

        FmMat4 c_matWVP;
        FmVec4 c_vDiffuse;
        FmVec4 c_vTexelSize;

        fm_int hMatWVP;
        fm_int hDiffuse;
        fm_int hTexelSize;

        void* pVData;
        unsigned int vSize;

        IRenderDrawOp::DRAW_MODE drawMode;
        unsigned int primitiveCount;

        BatchData* pNext;

        BatchData()
        {
            pShader = NULL;
            pVBuffer = NULL;
            hMatWVP = 0;
            hDiffuse = 0;
            hTexelSize = 0;
            pVData = NULL;
            vSize = 0;
            primitiveCount = 0;
            pNext = NULL;
        }
    };

    BatchData* GetBatchData();

    void UpdateBatchData(BatchData* pBatchData, bool keyFrame, const sObjectData& objectData);

    // ��Ⱦ����
    static void RenderBatch(void* pdata);

private:
    static IRender* s_pRender;

	float m_fCurTime;			// ��ǰʱ��
	float m_fBeginTime;			// ��ʼʱ��
	float m_fEndTime;			// ����ʱ��
	bool m_bPause;				// �Ƿ���ͣ
	bool m_bLoop;				// �Ƿ�ѭ������
	int m_nLastFrame;

	Objects m_objects;			// ����������ж���

	float m_fFrameInterval;		// ��֡��ʱ��

	int m_nShader;
	int m_nShaderKeyFrame;		// ����KeyFrame��shader

	size_t m_nLoadCount;

    IShaderProgram* m_pShaderFrame;
    fm_int m_hMatWVP;
    fm_int m_hDiffuse;

    IShaderProgram* m_pShaderKeyFrame;
    fm_int m_hKeyMatWVP;
    fm_int m_hKeyDiffuse;
    fm_int m_hKeyTexelSize;

    IDynamicVB* m_pFrameVBuffer;
    unsigned int m_frameVBufferSize;

    IDynamicVB* m_pKeyFrameVBuffer;
    unsigned int m_keyFrameVBufferSize;

    // ��Ⱦ����
    BatchData* m_listBatchData;
    long m_nBatchCount;

    time_axis_run_callback m_pRunCallbackFunc;
};

#endif // __TIME_AXIS_H__
