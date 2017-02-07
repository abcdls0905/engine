//--------------------------------------------------------------------
// �ļ���:		material_loader.h
// ��  ��:		
// ˵  ��:		
// ��������:	2010��3��8��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _MATERIAL_LOADER_H
#define _MATERIAL_LOADER_H

#include "../public/macros.h"
#include "../public/fast_str.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"

#include "../visual/i_res_loader.h"

// ģ�Ͳ��ʼ�����

class IIniFile;
class CModelPlayer;
class IRender;

class CMaterialLoader: public IResLoader
{
public:
	static CMaterialLoader* NewInstance();

public:
	CMaterialLoader();
	virtual ~CMaterialLoader();

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
	void SetOwner(CModelPlayer* pOwner);
	// ��ͼ�ļ���
	void SetFileName(const char* file_name);
	// ģ�Ͳ�������
	void SetMaterialCount(size_t value);
	
private:
	CMaterialLoader(const CMaterialLoader&);
	CMaterialLoader& operator=(const CMaterialLoader&);
	
private:
	CModelPlayer* m_pOwner;
	core_string m_strFileName;
	IIniFile* m_pIniFile;
};

#endif // _MATERIAL_LOADER_H
