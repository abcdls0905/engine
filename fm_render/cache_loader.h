//--------------------------------------------------------------------
// �ļ���:		CacheLoader.h
// ��  ��:		
// ˵  ��:		
// ��������:	2010��9��19��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _CACHELOADER_H
#define _CACHELOADER_H

#include "../public/macros.h"
#include "../public/fast_str.h"
#include "../public/core_mem.h"
#include "../visual/i_res_loader.h"

// �����ļ�������

class CCacheFile;

class CCacheLoader: public IResLoader
{
public:
	static CCacheLoader* NewInstance();

public:
	CCacheLoader();
	virtual ~CCacheLoader();

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
	void SetOwner(CCacheFile* pOwner);
	// �ļ���
	void SetFileName(const char* file_name);

private:
	CCacheLoader(const CCacheLoader&);
	CCacheLoader& operator=(const CCacheLoader&);
	
private:
	CCacheFile* m_pOwner;
	TFastStr<char, 16, TCharTraits<char>, TCoreAlloc> m_strFileName;
	unsigned char* m_pData;
	size_t m_nSize;
};

#endif // _CACHELOADER_H
