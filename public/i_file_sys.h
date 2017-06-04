
#ifndef _PUBLIC_I_FILE_SYS_H
#define _PUBLIC_I_FILE_SYS_H

#include "i_interface.h"
#include <stdarg.h>

// 文件系统接口

class IFileSys: public IInterface
{
public:
	// 文件时间
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
	// 加载配置信息
	virtual bool LoadConfig(const char* config) = 0;

	// 文件是否存在
	virtual bool FileExists(const char* name) = 0;
	// 只在文件包中查找，未打包则等同FileExists
	virtual bool FileInPack(const char* name) = 0;
	// 删除文件
	virtual bool FileDelete(const char* name) = 0;
	// 获得文件大小
	virtual size_t FileSize(const char* name) = 0;
	// 获得文件日期
	virtual bool FileTime(const char* name, file_time_t* file_time) = 0;
	// 打开文件
	virtual void* FileOpen(const char* name, const char* mode) = 0;
	// 关闭文件
	virtual bool FileClose(void* handle) = 0;
	// 读文件
	virtual size_t FileRead(void* handle, void* pdata, size_t len) = 0;
	// 写文件
	virtual size_t FileWrite(void* handle, const void* pdata, size_t len) = 0;
	// 格式化写文件
	virtual size_t FilePrint(void* handle, const char* info, ...) = 0;
	// 格式化写文件
	virtual size_t FilePrintList(void* handle, const char* info, 
		va_list vlist) = 0;
	// 提交写入数据
	virtual bool FileFlush(void* handle) = 0;
	// 移动读写指针
	virtual bool FileSeek(void* handle, long offset, int origin) = 0;
	// 获得当前读写指针位置
	virtual size_t FileTell(void* handle) = 0;
	// 直接获得文件内容
	virtual const void* FileContent(void* handle) = 0;

	// 目录是否存在
	virtual bool DirExists(const char* name) = 0;
	// 创建目录
	virtual bool DirCreate(const char* name) = 0;
	// 删除目录
	virtual bool DirDelete(const char* name) = 0;
};

#endif // _PUBLIC_I_FILE_SYS_H
