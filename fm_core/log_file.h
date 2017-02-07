//--------------------------------------------------------------------
// �ļ���:		log_file.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��1��30��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _LOG_FILE_H
#define _LOG_FILE_H

#include "../public/macros.h"
#include "../public/fast_str.h"

// д��־�ļ�����

class CLogFile
{
public:
	CLogFile();
	CLogFile(const char* filename);
	~CLogFile();

	// �����ļ���
	void SetFileName(const char* filename);
	// ����ļ���
	const char* GetFileName() const;

	// д��־��Ϣ���Զ���ʱ�䣩
	bool SaveLog(const char* log) const;
	// �����־
	bool Clear() const;

private:
	CLogFile(const CLogFile&);
	CLogFile& operator=(const CLogFile&);

private:
	TFastStr<char, 128> m_strFileName;
};

#endif // _LOG_FILE_H
