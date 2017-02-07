//--------------------------------------------------------------------
// �ļ���:		core_config.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��5��14��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _PUBLIC_CORE_CONFIG_H
#define _PUBLIC_CORE_CONFIG_H

#include "fast_str.h"

#define CORE_FLAG_USE_MEMORY_POOL 0x2 // �Ƿ�ʹ���ڴ��
#define CORE_FLAG_MEMORY_VALIDATE 0x4 // �Ƿ�У���ڴ�

// ������������

class CCoreConfig
{
public:
	CCoreConfig()
	{
		m_nSleep = 0;
		m_nFlags = 0;
	}
	
	// ��ѭ������ʱ��
	int m_nSleep;
	// ��־
	int m_nFlags;
	// ����·��
	TFastStr<char, 128> m_strWorkPath;
	// ������Դ·��
	TFastStr<char, 128> m_strProgramPath;
	// ��Դ�ļ�·��
	TFastStr<char, 128> m_strResourcePath;
	// �ļ�ϵͳ
	TFastStr<char, 128> m_strFileSystem;
	// �ļ�ϵͳ������Ϣ
	TFastStr<char, 128> m_strFileConfig;
	// ��ʵ������
	TFastStr<char, 128> m_strMainEntity;
};

#endif // _PUBLIC_CORE_CONFIG_H
