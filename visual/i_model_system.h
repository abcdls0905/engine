
#ifndef	_VISUAL_I_MODEL_SYSTEM_H
#define	_VISUAL_I_MODEL_SYSTEM_H

#include "../public/module.h"
#include "../math/fm_math.h"

#define IMODEL_ADV_NODERADIAN180

// �ڵ�����
enum FXNODE_TYPE
{
	FXNODE_TYPE_INVALID = 0x00000000,
	FXNODE_TYPE_MESH = 0x00000001,
	FXNODE_TYPE_SKIN = 0x00000002,
	FXNODE_TYPE_HELPER = 0x00000003,
	FXNODE_TYPE_BONE = 0x00000004,
	FXNODE_TYPE_CAMERA = 0x00000005,
};

// ���������ص�����
typedef bool (*ACTION_TRIGGER_PROC)(void* user_pointer,
	unsigned int nActionIndex, const char* pszTriggerName, 
	unsigned int nFrame);

// ģ��ϵͳ�ӿ�

class IModelPlayer;
class IActionPlayer;
class CResModel;
class CSkeleton;
class CActionSet;

class IModelSystem: public IInterface
{
public:
	// ָ���Ƿ���ģ�ʹ�����Ч����
	virtual void SetCheckModelData(bool value) = 0;
	// �жϵ�ǰ�Ƿ���ģ�ʹ�����Ч����
	virtual bool GetCheckModelData() = 0;
	
	// �Ƿ���ʾģ�ͽڵ�İ�Χ��
	virtual void SetShowNodeBoundingBox(bool value) = 0;
	virtual bool GetShowNodeBoundingBox() = 0;

	// ��Ⱦ��Χ��
	virtual void DrawBoundingBox(const FmVec3* min, 
		const FmVec3* max, unsigned int cr_start, unsigned int cr_end, 
		float scale) = 0;
	// ��Ⱦһ��������
	virtual void DrawPoint(const FmVec3* position, unsigned int cr, 
		float size, bool balance) = 0;
	
	// ����ģ�Ͳ�����
	virtual IModelPlayer* CreateModelPlayer(const char* name, double beg_time,
		const char* tex_paths,	bool async) = 0;
	// ��������������
	virtual IActionPlayer* CreateActionPlayer(const char* name, 
		const char* append_path, const char* def_action, void* trigger_proc, 
		void* user_pointer, bool async) = 0;
	
	// �������е���Դ��Ϣ
	virtual bool DumpResource(const char* file_name) = 0;
	// ���ճ�ʱ�䲻ʹ�õĶ�������
	virtual int CollectUnuseAction(int seconds) = 0;

    /* 
        ���½ӿں������ڶ��ļ���Դ��Ԥ����
    */

    // ������̬ģ��
    virtual CResModel* CreateResModel(const char* name, const char* tex_paths, bool async) = 0;
    // �ͷž�̬ģ��
    virtual void ReleaseResModel(CResModel* pResModel) = 0;

    // ������������
    virtual CSkeleton* CreateSkeleton(const char* name, bool async) = 0;
    // �ͷŹ�������
    virtual void ReleaseSkeleton(CSkeleton* pSkeleton) = 0;

    // ����������
    virtual CActionSet* CreateActionSet(const char* name, const char* append_path, bool async) = 0;
    // �ͷŶ�����
    virtual void ReleaseActionSet(CActionSet* pActionSet) = 0;

    /* 
        ���Ͻӿں������ڶ��ļ���Դ��Ԥ����
    */
};

#endif // _VISUAL_I_MODEL_SYSTEM_H
