//--------------------------------------------------------------------
// �ļ���:		ent_manager.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��2��9��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _ENT_MANAGER_H
#define _ENT_MANAGER_H

#include "../public/macros.h"
#include "../utils/array_pod.h"

// ʵ�����

class IEntity;
class IEntCreator;
class CEntInfo;
class CEntInfoList;
class CCore;

class CEntManager
{
public:
	explicit CEntManager(CCore* pCore);
	~CEntManager();

	// ��Ӵ�����
	bool AddCreator(IEntCreator* pCreator);

	// �������еĸ�����Ϣ
	void SetAllParentInfo();

	// �������
	size_t GetCount() const;
	// ���ʵ����Ϣ
	CEntInfo* GetEntInfoByIndex(size_t index) const;
	// ����ʵ����Ϣ
	CEntInfo* GetEntInfo(const char* name) const;

	// ��ӵ�ÿ֡���ж���
	bool AddFrameExecute(IEntity* pEntity);
	// ��ÿ֡���ж���ɾ��
	bool RemoveFrameExecute(IEntity* pEntity);
	// ÿִ֡���߼�
	void ExecuteFrame(float seconds);
	
	// ��������ʵ����Ϣ���ļ�
	bool Dump(const char* file_name);

private:
	CEntManager();
	CEntManager(const CEntManager&);
	CEntManager& operator=(const CEntManager&);

private:
	CCore* m_pCore;
	bool m_bModified;
	TArrayPod<IEntity*, 1> m_FrameExecute;
	TArrayPod<IEntity*, 1> m_CurrentExecute;
	CEntInfoList* m_pEntInfoList;
};

#endif // _ENT_MANAGER_H
