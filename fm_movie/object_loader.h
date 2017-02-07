#pragma once
#include "../visual/i_res_loader.h"
#include "../public/fast_str.h"

class TimeAxis;
struct sObjectData;
class KeyFrameController;

class CObjectLoader :
	public IResLoader
{
public:
	static CObjectLoader* NewInstance();

public:
	CObjectLoader();
	virtual ~CObjectLoader();

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
	void SetOwner(TimeAxis* pOwner);
	// ���ö���ģ���ļ���
	void SetFileName(sObjectData* pObject, const char* file_name);
private:
	CObjectLoader(const CObjectLoader&);
	CObjectLoader& operator=(const CObjectLoader&);

private:
	sObjectData* m_pObject;
	result_string m_strFileName;

	TimeAxis* m_pOwner;
};
