//--------------------------------------------------------------------
// �ļ���:		CacheIni.h
// ��  ��:		
// ˵  ��:		
// ��������:	2011��4��15��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _CACHEINI_H
#define _CACHEINI_H

#include "../public/macros.h"
#include "../visual/i_res_base.h"

// �����ļ�����

class IResLoader;
//class CReadIni;
class IIniFile;

class CCacheIni: public IResBase
{
public:
	enum STATE_ENUM
	{
		STATE_INIT,		// ��ʼ״̬
		STATE_LOADING,	// ���ڼ���
		STATE_READY,	// ����
		STATE_FAILED,	// ����ʧ��
	};

	static CCacheIni* NewInstace();

public:
	CCacheIni();
	virtual ~CCacheIni();

	// ͬ������
	bool Create(bool create_hash);
	// ��ʼ�첽���� 
	bool BeginCreate(bool create_hash);
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
	bool Build(IIniFile* pIni);
	
	// ����ļ�����
	IIniFile* GetIni() const { return m_pIni; }

private:
	CCacheIni(const CCacheIni&);
	CCacheIni& operator=(const CCacheIni&);

private:
	IResLoader* m_pLoader;
	int m_nState;
	IIniFile* m_pIni;
};

#endif // _CACHEINI_H

