//--------------------------------------------------------------------
// �ļ���:		CacheFile.h
// ��  ��:		
// ˵  ��:		
// ��������:	2010��9��19��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _CACHEFILE_H
#define _CACHEFILE_H

#include "../public/macros.h"
#include "../visual/i_res_base.h"
#include "../visual/i_res_loader.h"

// �ļ�����

class IResLoader;

class CCacheFile: public IResBase
{
public:
	enum STATE_ENUM
	{
		STATE_INIT,		// ��ʼ״̬
		STATE_LOADING,	// ���ڼ���
		STATE_READY,	// ����
		STATE_FAILED,	// ����ʧ��
	};

	static CCacheFile* NewInstace();

public:
	CCacheFile();
	virtual ~CCacheFile();

	// ͬ������
	bool Create();
	// ��ʼ�첽���� 
	bool BeginCreate();
	// �����첽���� 
	bool EndCreate(int category, bool succeed);

	// �Ƿ����
	bool IsReady()
	{
		return (m_nState == STATE_READY);
	}

	// �Ƿ���ؽ������ɹ���ʧ�ܣ�
	bool IsLoadComplete()
	{
		return (m_nState == STATE_READY) || (m_nState == STATE_FAILED);
	}

	// �ͷ�
	virtual void Release();

	// ��ü�����
	IResLoader* GetLoader() const;

	// �������
	bool Build(unsigned char* pdata, size_t size);
	
	// ����ļ�����
	unsigned char* GetData() const { return m_pData; }
	// ����ļ�����
	size_t GetSize() const { return m_nSize; }

private:
	CCacheFile(const CCacheFile&);
	CCacheFile& operator=(const CCacheFile&);

private:
	IResLoader* m_pLoader;
	int m_nState;
	unsigned char* m_pData;
	size_t m_nSize;
};

#endif // _CACHEFILE_H

