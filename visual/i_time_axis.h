//--------------------------------------------------------------------
// �ļ���:		i_time_axis.h
// ��  ��:		ʱ����ӿ�
// ˵  ��:		
// ��������:	2014��1��13��
// ������:		Ҷ��
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef __i_time_axis_H__
#define __i_time_axis_H__

#include "../visual/i_vis_base.h"
#include "../visual/i_res_loader.h"

typedef int (*time_axis_run_callback)(IEntity* pEntity, const char* event, const IVarList& args);

class ITimeAxis: public IVisBase
{
public:
	// ����
	virtual void Play() = 0;
	// ��ͣ��ָ��ʱ��
	virtual void Pause(float time) = 0;
	// ��ͣ�ڵ�ǰʱ��
	virtual void PauseNow() = 0;

	// �Ƿ�������
	virtual bool GetLoadFinish() = 0;
	virtual size_t GetLoadCount() const = 0;
	// ��Ӷ���
	virtual bool AddObject(const char* object_name) = 0;
	// ɾ������
	virtual bool DeleteObject(const char* object_name) = 0;
	// �����Ƿ����
	virtual bool IsObjectExist(const char* object_name) = 0;
	// ������������
	virtual bool CopyObjectData(const char* src_object_name, const char* dst_object_name) = 0;
	// ����������
	virtual bool ReName(const char* src_object_name, const char* new_object_name) = 0;
	// ��һ������������Ϣ��ֵ����һ������
	virtual bool CopyControllerData(const char* src_object_name, int controller_type, 
		const char* dst_object_name) = 0;
	// �����ؼ�֡
	virtual void CopyKeyTime(const char* object_name, float time, float new_time) = 0;
	// ����ĳ���������Ĺؼ�֡
	virtual void CopyControllerKeyTime(const char* object_name, int controller_type,
		float time, float new_time) = 0;
	// �ؼ�֡�����Ƿ����
	virtual bool IsKeyFrameExist(const char* object_name, int controller_type, float frame_time) const = 0;

	// ����ID
	virtual bool SetObjectID(const char* object_name, const PERSISTID& value) = 0;
	virtual PERSISTID GetObjectID(const char* object_name) const = 0;

	// ����Bezier�ؼ���
	virtual bool AddBezierKey(const char* object_name, int controller_type, float time, float value) = 0;
	// �ı���Ƶ�ļ��㷽ʽ
	virtual void ChangeComputeTypeKey(const char* object_name, int controller_type, float time, int type) = 0;
	// ���¼������еĿ��Ƶ�
	virtual void CreateAllControlPoints(const char* object_name, int controller_type) = 0;
	// �ƶ�����ؼ�֡
	virtual void MoveKeyTimes(const char* object_name, float time_add) = 0;
	// �ƶ��������Ĺؼ�֡
	virtual void MoveControllerKeyTimes(const char* object_name, int controller_type, float time_add) = 0;
	// �ƶ�Bezier����
	virtual void MoveAllKeyValue(const char* object_name, int controller_type, float value_add) = 0;
	// �ƶ��ؼ�֡
	virtual void MoveKey(const char* object_name, float time, float new_time) = 0;
	// �ƶ��ؼ�֡
	virtual void MoveControllerKey(const char* object_name, int controller_type, float time, float new_time) = 0;

	// ��ȡҪ��¼�ؼ����ǰһ���ؼ���
	virtual void GetFrontBezierKey(const IVarList &param, IVarList &result) = 0;

	// ��ȡ���д���ؼ�֡��Ϣ�����ڹؼ�֡����(�뱣֤ʹ���귵�صĹؼ�ָ֡��ǰ���޸Ļ�ɾ���ؼ�֡��Ϣ)
	virtual void GetAdjacencyBezierKeyData(const IVarList &args, IVarList &result) const = 0;

	// ���ӻ��޸Ĺؼ�֡
	virtual bool SetKeyFrameBezierMode(const char* object_name, int controller_type, 
		float time, float value, 
		float left_time, float left_value, 
		float right_time, float right_value) = 0;
	virtual bool SetKeyFrameOnceMode(const char* object_name, int controller_type, 
		float time, const char* param) = 0;
	virtual bool SetKeyFrameLinearMode(const char* object_name, int controller_type, 
		float time, float value) = 0;
	virtual bool SetKeyFrameScaleMode(const char* object_name, int controller_type, 
		float time, const char* action_name, float time_length, float speed) = 0;

	// ɾ��ĳʱ�̵����йؼ�֡
	virtual void DeleteKey(const char* object_name, float time) = 0;
	// ɾ���ؼ�֡
	virtual bool DeleteControllerKey(const char* object_name, int controller_type, 
		float time) = 0;
	// ���ָ�������������йؼ�֡
	virtual void ClearControllerKeyFrames(const char* object_name, int controller_type) = 0;
	// ��ն������п����������йؼ�֡
	virtual void ClearControllers(const char* object_name) = 0;
	// ��ùؼ�֡�б�
	virtual void GetControllerKeyTime(const IVarList& args, IVarList& result) const = 0;
	virtual int GetControllerKeyCount(const char* object_name, int controller_type) const = 0;
	virtual int GetKeyFrameListBezierMode(const IVarList& args, IVarList& result) const = 0;
	virtual int GetKeyFrameListOnceMode(const IVarList& args, IVarList& result) const = 0;
	virtual int GetKeyFrameListLinearMode(const IVarList& args, IVarList& result) const = 0;
	virtual int GetKeyFrameListScaleMode(const IVarList& args, IVarList& result) const = 0;
	// ��ö������п�����ID
	virtual int GetControllers(const IVarList& args, IVarList& result) const = 0;
	// ��ȡָ��ʱ��Ŀ�����ֵ
	virtual float GetValueBezierMode(const char* object_name, int controller_type, 
		float time) const = 0;
	//virtual const char* GetValueOnceMode(const char* object_name, int controller_type, 
	//	float time) const = 0;
	// ��ȡĳһʱ�̹ؼ�֡֡����
	virtual void GetKeyFrameValue(const IVarList& args, IVarList& result) const = 0;
	// ��ȡ�洢�켣���������ͣ��Ƿ���������꣩
	virtual bool GetIsLocalPosition(const char* object_name) const = 0;
	// ��ȡ�����������ϵ������
	virtual void GetLocalOrigin(const IVarList &args, IVarList& result) const = 0;
	// ���ö����������ϵ������
	virtual void SetLocalOrigin(const IVarList &args, IVarList& result) = 0;

	// ��ȡ�洢�켣�Ƿ񾭹�����ת
	virtual bool GetIsRotation(const char* object_name) const = 0;
	// ��ȡ�켣��ת
	virtual float GetRotationY(const char* object_name) const = 0;
	// ���ù켣��ת,�����actor��Ҫ��ת�켣180
	virtual void SetRotationY(const char* object_name, float fAngle) = 0;
	// ��ȡ�켣������ϵ��ת�Ƕ�
	virtual void GetRotation(const IVarList& args, IVarList& result) const = 0;
	// ���ù켣������ϵ��ת�Ƕ�
	virtual void SetRotation(const IVarList& args, IVarList& result) = 0;

	// ��ȡ����Ķ�������
	virtual const char* GetExtProp(const char* object_name) const = 0;
	// ��ȡ��ʼ״̬��λ��(��Ҫ���Ŀ�������δkey������)
	virtual void GetInitialPosition(const IVarList &args, IVarList& result) = 0;


	// ����ת��
	virtual void PositionToLocal(const IVarList& args, IVarList& result) const = 0;
	// �Ƕ�ת��
	virtual void AngleToLocal(const IVarList& args, IVarList& result) const = 0;

	// �Ƿ�ѭ������
	virtual void SetLoop(bool value) = 0;
	virtual bool GetLoop() const = 0;

	// ��ʼʱ��
	virtual void SetBeginTime(float value) = 0;
	virtual float GetBeginTime() const = 0;
	// ����ʱ��
	virtual void SetEndTime(float value) = 0;
	virtual float GetEndTime() const = 0;

	// ��֡��ʱ��
	virtual void SetFrameInterval(float value) = 0;
	virtual float GetFrameInterval() const = 0;

	// ·���Ƿ�ɼ�
	virtual void SetPathVisible(const char* object_name, bool bVisible) = 0;
	virtual bool GetPathVisible(const char* object_name) const = 0;

	// ����������ʾ��·����
	virtual bool BuildPathPoint(const char* object_name) = 0;

	// ���ض���ؼ�֡
	virtual bool LoadObjectKeyFrame(const char* object_name, const char* file, 
		bool bAsync) = 0;

	// �������ؼ�֡
	virtual bool SaveObjectKeyFrame(const char* object_name, const char* file) = 0;
		
	// �������ж���
	virtual bool SaveAllObject(const char* path, bool bForce) = 0;

	// ��������ؼ�֡��max(��������)
	virtual bool ExportToMaxPath(const char* object_name, const char* file_name,
		float x, float y, float z, float angle_y, int scale) = 0;

	virtual bool ExportToMaxPathFrames(const char* object_name, const char* file_name,
		float x, float y, float z, float angle_y, int scale) = 0;

	// �𶯹ؼ�֡���������.
	virtual bool LoadRandomData(const char *file_name) = 0;
	virtual bool SaveRandomData(const char *file_name) = 0;

    // ���ûص�����
    virtual void SetCallbackFunc(time_axis_run_callback func) = 0;
    virtual time_axis_run_callback GetCallbackFunc() = 0;
};

#endif // __i_time_axis_H__
