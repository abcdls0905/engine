//--------------------------------------------------------------------
// �ļ���:		skeleton_loader.h
// ��  ��:		
// ˵  ��:		
// ��������:	2010��7��12��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _SKELETON_LOADER_H
#define _SKELETON_LOADER_H

#include "../public/macros.h"
#include "../public/fast_str.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"

#include "../visual/i_res_loader.h"
#include "skeleton_data.h"

// ��������������

class CSkeleton;

class CSkeletonLoader: public IResLoader
{
public:
	static CSkeletonLoader* NewInstance();

public:
	CSkeletonLoader();
	virtual ~CSkeletonLoader();

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
	void SetOwner(CSkeleton* pOwner);
	// ģ���ļ���
	void SetFileName(const char* file_name);
	
private:
	CSkeletonLoader(const CSkeletonLoader&);
	CSkeletonLoader& operator=(const CSkeletonLoader&);

private:
	CSkeleton* m_pOwner;
	core_string m_strFileName;
	skeleton_t* m_pSkeleton;
};

#endif // _SKELETON_LOADER_H
