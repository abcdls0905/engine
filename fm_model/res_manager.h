//--------------------------------------------------------------------
// �ļ���:		res_manager.h
// ��  ��:		
// ˵  ��:		
// ��������:	2009��5��4��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _RES_MANAGER_H
#define _RES_MANAGER_H

#include "../public/macros.h"
#include "../public/persistid.h"
#include "../public/core_mem.h"
#include "../utils/string_pod.h"
#include "../public/i_entity.h"

class TResManagerAlloc
{
public:
	TResManagerAlloc() {}
	~TResManagerAlloc() {}

	// �����ڴ�
	void* Alloc(size_t size) { return CORE_ALLOC(size); }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { CORE_FREE(ptr, size); }
	// ����
	void Swap(TResManagerAlloc& src) {}
};

// ��Դ����

class CResModel;
class CSkeleton;
class CActionSet;
class ModelSystem;
struct skeleton_t;

class CResManager
{
private:
	typedef TStringPod<char, CResModel*, TStringTraitsCI<char>, 
		TResManagerAlloc> model_container_t;
	typedef TStringPod<char, CSkeleton*, TStringTraitsCI<char>, 
		TResManagerAlloc> skeleton_container_t;
	typedef TStringPod<char, CActionSet*, TStringTraitsCI<char>, 
		TResManagerAlloc> action_set_container_t;

public:
	CResManager();
	~CResManager();

	// ��ʼ��
	bool Initialize(ModelSystem* pModelSystem);
	// �ͷ�������ͼ
	bool ReleaseAllTextures();

	// ������̬ģ��
	CResModel* CreateResModel(const char* name, const char* tex_paths, 
		bool async);
	// �ͷž�̬ģ��
	bool ReleaseResModel(CResModel* pResModel);
	
	// ���ݹ����������ݻ����Դ����
	CSkeleton* GetSkeleton(const char* name, skeleton_t* pSkeleton);
	// ������������
	CSkeleton* CreateSkeleton(const char* name, bool async);
	// �ͷŹ�������
	bool ReleaseSkeleton(CSkeleton* pSkeleton);

	// ����������
	CActionSet* CreateActionSet(const char* name, const char* append_path, 
		bool async);
	// �ͷŶ�����
	bool ReleaseActionSet(CActionSet* pActionSet);
	// ���ճ�ʱ�䲻ʹ�õĶ�������
	int CollectUnuseAction(int seconds);

	// �������е���Դ��Ϣ
	bool DumpAll(const char* file_name);

	// ��ʼ�����ɴ�����������ʵ�壩
	virtual bool Init(const IVarList& args);
	// �رգ���ɾ���������õĸ���ʵ�壬����Դ�ͷ�Ӧ�����������н��У�
	virtual bool Shut();

	// ÿִ֡���߼����������ӵ����ж��У�
	virtual void Execute(float seconds);
private:
	CResManager(const CResManager&);
	CResManager& operator=(const CResManager&);	

private:
	ModelSystem* m_pModelSystem;
	model_container_t m_ResModels;
	skeleton_container_t m_Skeletons;
	action_set_container_t m_ActionSets;
};

class CResManagerUpdate : public IEntity
{
public:
	CResManager* res_manager;
public:
	virtual bool Init(const IVarList& args);
	virtual bool Shut(){ return true;}
	virtual void Execute(float seconds);
};

#endif // _RES_MANAGER_H
