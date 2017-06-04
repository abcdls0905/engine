
#ifndef _MODEL_LOADER_H
#define _MODEL_LOADER_H

#include "../public/macros.h"
#include "../public/fast_str.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"

#include "../visual/i_res_loader.h"
#include "model_data.h"

// ģ�ͼ�����

class CResModel;

class CModelLoader: public IResLoader
{
public:
	static CModelLoader* NewInstance();

public:
	CModelLoader();
	virtual ~CModelLoader();

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
	void SetOwner(CResModel* pOwner);
	// ģ���ļ���
	void SetFileName(const char* file_name);
	
private:
	CModelLoader(const CModelLoader&);
	CModelLoader& operator=(const CModelLoader&);

private:
	CResModel* m_pOwner;
	core_string m_strFileName;
	model_t* m_pModel;
};

#endif // _MODEL_LOADER_H
