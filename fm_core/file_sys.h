//--------------------------------------------------------------------
// �ļ���:		file_sys.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��5��15��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _FILE_SYS_H
#define _FILE_SYS_H

#include "../public/macros.h"
#include "../public/i_file_sys.h"

// ȱʡ���ļ�ϵͳ

class CFileSys: public IFileSys
{
public:
	static CFileSys* NewInstance();
	
public:
	CFileSys();
	virtual ~CFileSys();

	// ��ʼ��
	virtual bool Init();
	// �ر�
	virtual bool Shut();
	// �ͷ�
	virtual void Release();
	
	// ����������Ϣ
	virtual bool LoadConfig(const char* config);

	// �ļ��Ƿ����
	virtual bool FileExists(const char* name);
	// ֻ���ļ����в��ң�δ������ͬFileExists
	virtual bool FileInPack(const char* name);
	// ɾ���ļ�
	virtual bool FileDelete(const char* name);
	// ����ļ���С
	virtual size_t FileSize(const char* name);
	// ����ļ�����
	virtual bool FileTime(const char* name, file_time_t* file_time);
	// ���ļ�
	virtual void* FileOpen(const char* name, const char* mode);
	// �ر��ļ�
	virtual bool FileClose(void* handle);
	// ���ļ�
	virtual size_t FileRead(void* handle, void* pdata, size_t len);
	// д�ļ�
	virtual size_t FileWrite(void* handle, const void* pdata, size_t len);
	// ��ʽ��д�ļ�
	virtual size_t FilePrint(void* handle, const char* info, ...);
	// ��ʽ��д�ļ�
	virtual size_t FilePrintList(void* handle, const char* info, 
		va_list vlist);
	// �ύд������
	virtual bool FileFlush(void* handle);
	// �ƶ���дָ��
	virtual bool FileSeek(void* handle, long offset, int origin);
	// ��õ�ǰ��дָ��λ��
	virtual size_t FileTell(void* handle);
	// ֱ�ӻ���ļ�����
	virtual const void* FileContent(void* handle);

	// Ŀ¼�Ƿ����
	virtual bool DirExists(const char* name);
	// ����Ŀ¼
	virtual bool DirCreate(const char* name);
	// ɾ��Ŀ¼
	virtual bool DirDelete(const char* name);
};

#endif // _FILE_SYS_H

