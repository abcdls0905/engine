//--------------------------------------------------------------------
// �ļ���:		action_set_loader.h
// ��  ��:		
// ˵  ��:		
// ��������:	2010��7��13��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _ACTION_SET_LOADER_H
#define _ACTION_SET_LOADER_H

#include "../public/macros.h"
#include "../public/fast_str.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../visual/i_res_loader.h"
#include "skeleton_data.h"

// ������������

class CActionSet;
struct action_set_t;

class CActionSetLoader: public IResLoader
{
public:
	static CActionSetLoader* NewInstance();

public:
	CActionSetLoader();
	virtual ~CActionSetLoader();

	// �ͷ�
	virtual void Release();
	// �������
	virtual const char* GetName();
	// ����
	virtual bool Load(bool async);
	// ����
	virtual bool Create(bool async);
	// �����첽���� 
	virtual bool EndCreate(int category, bool succeed);

	// ��Դ����
	void SetOwner(CActionSet* pOwner);
	// ģ���ļ���
	void SetFileName(const char* file_name);
	// ���ø�����Դ·����
	void SetAppendPath(const char* append_path);
	
private:
	CActionSetLoader(const CActionSetLoader&);
	CActionSetLoader& operator=(const CActionSetLoader&);

private:
	CActionSet* m_pOwner;
	core_string m_strFileName;
	core_string m_strAppendPath;
	action_set_t* m_pActionSet;
	skeleton_t* m_pMainSkeleton;
};

#endif // _ACTION_SET_LOADER_H
