
#ifndef _PUBLIC_I_FILE_SYS_H
#define _PUBLIC_I_FILE_SYS_H

#include "i_interface.h"
#include <stdarg.h>

// �ļ�ϵͳ�ӿ�

class IFileSys: public IInterface
{
public:
	// �ļ�ʱ��
	struct file_time_t
	{
		int nYear;
		int nMonth;
		int nDay;
		int nHour;
		int nMinute;
		int nSecond;
	};
	
public:
	// ����������Ϣ
	virtual bool LoadConfig(const char* config) = 0;

	// �ļ��Ƿ����
	virtual bool FileExists(const char* name) = 0;
	// ֻ���ļ����в��ң�δ������ͬFileExists
	virtual bool FileInPack(const char* name) = 0;
	// ɾ���ļ�
	virtual bool FileDelete(const char* name) = 0;
	// ����ļ���С
	virtual size_t FileSize(const char* name) = 0;
	// ����ļ�����
	virtual bool FileTime(const char* name, file_time_t* file_time) = 0;
	// ���ļ�
	virtual void* FileOpen(const char* name, const char* mode) = 0;
	// �ر��ļ�
	virtual bool FileClose(void* handle) = 0;
	// ���ļ�
	virtual size_t FileRead(void* handle, void* pdata, size_t len) = 0;
	// д�ļ�
	virtual size_t FileWrite(void* handle, const void* pdata, size_t len) = 0;
	// ��ʽ��д�ļ�
	virtual size_t FilePrint(void* handle, const char* info, ...) = 0;
	// ��ʽ��д�ļ�
	virtual size_t FilePrintList(void* handle, const char* info, 
		va_list vlist) = 0;
	// �ύд������
	virtual bool FileFlush(void* handle) = 0;
	// �ƶ���дָ��
	virtual bool FileSeek(void* handle, long offset, int origin) = 0;
	// ��õ�ǰ��дָ��λ��
	virtual size_t FileTell(void* handle) = 0;
	// ֱ�ӻ���ļ�����
	virtual const void* FileContent(void* handle) = 0;

	// Ŀ¼�Ƿ����
	virtual bool DirExists(const char* name) = 0;
	// ����Ŀ¼
	virtual bool DirCreate(const char* name) = 0;
	// ɾ��Ŀ¼
	virtual bool DirDelete(const char* name) = 0;
};

#endif // _PUBLIC_I_FILE_SYS_H
