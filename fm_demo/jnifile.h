#pragma once

#include "../public/i_file_sys.h"

class jniFile : public IFileSys
{
public:
	static jniFile* NewInstance();
public:
	jniFile();
	~jniFile();
	// 初始化
	virtual bool Init();
	// 关闭
	virtual bool Shut();
	// 释放
	virtual void Release();
	
	// 加载配置信息
	virtual bool LoadConfig(const char* config);
	// 文件是否存在
	virtual bool FileExists(const char* name);
	// 只在文件包中查找，未打包则等同FileExists
	virtual bool FileInPack(const char* name);
	// 删除文件
	virtual bool FileDelete(const char* name);
	// 获得文件大小
	virtual size_t FileSize(const char* name);
	// 获得文件日期
	virtual bool FileTime(const char* name, file_time_t* file_time);
	// 打开文件
	virtual void* FileOpen(const char* name, const char* mode);
	// 关闭文件
	virtual bool FileClose(void* handle);
	// 读文件
	virtual size_t FileRead(void* handle, void* pdata, size_t len);
	// 写文件
	virtual size_t FileWrite(void* handle, const void* pdata, size_t len);
	// 格式化写文件
	virtual size_t FilePrint(void* handle, const char* info, ...);
	// 格式化写文件
	virtual size_t FilePrintList(void* handle, const char* info, 
		va_list vlist);
	// 提交写入数据
	virtual bool FileFlush(void* handle);
	// 移动读写指针
	virtual bool FileSeek(void* handle, long offset, int origin);
	// 获得当前读写指针位置
	virtual size_t FileTell(void* handle);
	// 直接获得文件内容
	virtual const void* FileContent(void* handle);

	// 目录是否存在
	virtual bool DirExists(const char* name);
	// 创建目录
	virtual bool DirCreate(const char* name);
	// 删除目录
	virtual bool DirDelete(const char* name);
};