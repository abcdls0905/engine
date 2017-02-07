//--------------------------------------------------------------------
// �ļ���:		CacheIniLoader.h
// ��  ��:		
// ˵  ��:		
// ��������:	2011��4��15��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _CACHEINILOADER_H
#define _CACHEINILOADER_H

#include "../public/macros.h"
#include "../public/fast_str.h"
#include "../public/core_mem.h"
#include "../visual/i_res_loader.h"

// ���������ļ�������

class CCacheIni;
class IIniFile;

class CCacheIniLoader: public IResLoader
{
public:
	static CCacheIniLoader* NewInstance();

public:
	CCacheIniLoader();
	virtual ~CCacheIniLoader();

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
	
	// ������
	void SetOwner(CCacheIni* pOwner);
	// �ļ���
	void SetFileName(const char* file_name);
	// �Ƿ񴴽���ϣ����
	void SetCreateHash(bool value);

private:
	CCacheIniLoader(const CCacheIniLoader&);
	CCacheIniLoader& operator=(const CCacheIniLoader&);
	
private:
	CCacheIni* m_pOwner;
	TFastStr<char, 16, TCharTraits<char>, TCoreAlloc> m_strFileName;
	bool m_bCreateHash;
	IIniFile* m_pIni;
};

#endif // _CACHEINILOADER_H
