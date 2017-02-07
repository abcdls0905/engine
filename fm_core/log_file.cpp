//--------------------------------------------------------------------
// �ļ���:		log_file.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2007��1��30��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "log_file.h"
#include "../public/portable.h"
#include <string.h>
#include <stdio.h>

// CLogFile

CLogFile::CLogFile()
{
}

CLogFile::CLogFile(const char* filename)
{
	SetFileName(filename);
}

CLogFile::~CLogFile()
{
}

void CLogFile::SetFileName(const char* filename)
{
	Assert(filename != NULL);
	
	m_strFileName = filename;
}

const char* CLogFile::GetFileName() const
{
	return m_strFileName.c_str();
}

bool CLogFile::SaveLog(const char* log) const
{
	Assert(log != NULL);

	FILE* fp = Port_FileOpen(GetFileName(), "ab");

	if (NULL == fp)
	{
		return false;
	}

	port_date_time_t tm;

	Port_GetLocalTime(&tm);

	char buf[128];

	sprintf(buf, "[%04d-%02d-%02d %02d:%02d:%02d.%03d] ",
		tm.nYear, tm.nMonth, tm.nDay, tm.nHour, tm.nMinute, tm.nSecond,
		tm.nMilliseconds);
	fprintf(fp, "%s%s\r\n", buf, log);
	fclose(fp);

	return true;
}

bool CLogFile::Clear() const
{
	FILE* fp = Port_FileOpen(GetFileName(), "wb");

	if (NULL == fp)
	{
		return false;
	}

	fclose(fp);

	return true;
}
