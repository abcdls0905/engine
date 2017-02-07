//--------------------------------------------------------------------
// �ļ���:		model_system.h
// ��  ��:		
// ˵  ��:		
// ��������:	2009��1��1��
// ������:		����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _MODEL_SYSTEM_H
#define _MODEL_SYSTEM_H

#include "../visual/i_model_system.h"

// ģ��ϵͳ

class IRender;
class IActionPlayer;
class CResManager;

class ModelSystem: public IModelSystem
{
public:
	ModelSystem();
	virtual ~ModelSystem();

	virtual bool Init();
	virtual bool Shut();

	// �Ƿ���ģ�ʹ�����Ч����
	virtual void SetCheckModelData(bool value);
	virtual bool GetCheckModelData();

	// �Ƿ���ʾģ�ͽڵ�İ�Χ��
	virtual void SetShowNodeBoundingBox(bool value);
	virtual bool GetShowNodeBoundingBox();
	
	// ��Ⱦ��Χ��
	virtual void DrawBoundingBox(const FmVec3* min, 
		const FmVec3* max, unsigned int cr_start, unsigned int cr_end, 
		float scale);
	// ��Ⱦһ��������
	virtual void DrawPoint(const FmVec3* position, unsigned int cr, 
		float size, bool balance);

	// ����ģ�Ͳ�����
	virtual IModelPlayer* CreateModelPlayer(const char* name, double beg_time,
		const char* tex_paths, bool async);
	// ��������������
	virtual IActionPlayer* CreateActionPlayer(const char* name, 
		const char* append_path, const char* def_action, void* trigger_proc, 
		void* user_pointer, bool async);

    // ������̬ģ��
    virtual CResModel* CreateResModel(const char* name, const char* tex_paths, bool async);
    // �ͷž�̬ģ��
    virtual void ReleaseResModel(CResModel* pResModel);

    // ������������
    virtual CSkeleton* CreateSkeleton(const char* name, bool async);
    // �ͷŹ�������
    virtual void ReleaseSkeleton(CSkeleton* pSkeleton);

    // ����������
    virtual CActionSet* CreateActionSet(const char* name, const char* append_path, bool async);
    // �ͷŶ�����
    virtual void ReleaseActionSet(CActionSet* pActionSet);

	// �������е���Դ��Ϣ
	virtual bool DumpResource(const char* file_name);
	// ���ճ�ʱ�䲻ʹ�õĶ�������
	virtual int CollectUnuseAction(int seconds);

	// �����Ⱦ��
	IRender* GetRender() const { return m_pRender; }
	// �����Դ������
	CResManager* GetResManager() const { return m_pResManager; }

private:
	IRender* m_pRender;
	CResManager* m_pResManager;
};

#endif // _MODEL_SYSTEM_H
