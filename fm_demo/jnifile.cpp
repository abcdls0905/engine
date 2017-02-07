#include "jnifile.h"

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include "jnifile.h"
#include "../public/i_core.h"
#include "../public/auto_mem.h"
#include "../public/inlines.h"
#include "../public/portable.h"
#include "../public/core_log.h"
#include <string.h>
#include <stdio.h>

extern ICore* g_pCore;
extern bool g_bLogFileIO;
extern AAssetManager* g_asset_manager;

inline char* process_string(const char* name, char* buf)
{
	strcpy(buf, name);
	for(int i = 0; buf[i]!=0; i++)
	{
		if(buf[i] == '\\')
			buf[i] = '/';
	}
}

// CFileSys
jniFile* jniFile::NewInstance()  	
{
	return NEW jniFile;
}

jniFile::jniFile()
{
}

jniFile::~jniFile()
{
}

bool jniFile::Init()
{
	CORE_TRACE("jniFile init");
	return true;
}

bool jniFile::Shut()
{
	return true;
}

void jniFile::Release()
{
	delete this;
}

bool jniFile::LoadConfig(const char* config)
{
	return true;
}

bool jniFile::FileExists(const char* name)
{
	char _name[256];
	process_string(name, _name);
	AAsset* asset = AAssetManager_open(g_asset_manager, _name, AASSET_MODE_UNKNOWN);
	if(asset)
	{
		AAsset_close(asset);
		return true;
	}
	
	return false;
}

bool jniFile::FileInPack(const char* name)
{
	return FileExists(name);
}

bool jniFile::FileDelete(const char* name)
{
	CORE_TRACE("not support FileDelete");
	return true;
}

size_t jniFile::FileSize(const char* name)
{
	char _name[256];
	process_string(name, _name);
	size_t s = 0;
	AAsset* asset = AAssetManager_open(g_asset_manager, _name, AASSET_MODE_UNKNOWN);
	if(asset)
	{
		s = AAsset_getLength(asset);
		AAsset_close(asset);
	}
	else
	{
		char buf[256];
		sprintf(buf, "can't get file size %s", name);
		CORE_TRACE(buf);
	}
	char buf[256];
	sprintf(buf, "get file size %s, %d", name, s);
	CORE_TRACE(buf);
	return s;
}

bool jniFile::FileTime(const char* name, file_time_t* file_time)
{
	memset(file_time, 0, sizeof(file_time_t));
	return true;
}

void* jniFile::FileOpen(const char* name, const char* mode)
{
	char _name[256];
	process_string(name, _name);
	void* asset = (void*)AAssetManager_open(g_asset_manager, _name, AASSET_MODE_UNKNOWN);
	if(asset == 0)
	{
		char buf[256];
		sprintf(buf, "()jniFile::FileOpen %s, %d", _name, (int)asset);
		CORE_TRACE(buf);
	}
	return asset;
}

bool jniFile::FileClose(void* handle)    		
{
	AAsset_close((AAsset*)handle);  
	return true;
}

size_t jniFile::FileRead(void* handle, void* pdata, size_t len)
{
	return AAsset_read((AAsset*)handle, pdata, len);
}

size_t jniFile::FileWrite(void* handle, const void* pdata, size_t len)
{    
	CORE_TRACE("not support FileWrite");
	return 0;
}

size_t jniFile::FilePrint(void* handle, const char* info, ...)
{
	CORE_TRACE("not support FilePrint");
	return 0;
}

size_t jniFile::FilePrintList(void* handle, const char* info, va_list vlist)
{
	CORE_TRACE("not support FilePrintList");
	return 0;		
}

bool jniFile::FileFlush(void* handle)
{
	CORE_TRACE("not support FileFlush");
	return true;
}

bool jniFile::FileSeek(void* handle, long offset, int origin)
{
	return AAsset_seek((AAsset*)handle, offset, origin) == 0;
}

size_t jniFile::FileTell(void* handle)
{     			
	return AAsset_getLength((AAsset*)handle) - AAsset_getRemainingLength((AAsset*)handle);
}

const void* jniFile::FileContent(void* handle)
{
	return NULL;   
}
         
bool jniFile::DirExists(const char* name)
{
	char _name[256];
	process_string(name, _name);
	AAssetDir* asset = AAssetManager_openDir(g_asset_manager, _name);
	if(asset)
	{
		AAssetDir_close(asset);
		return true;
	}
	return false;
}

bool jniFile::DirCreate(const char* name)
{
	CORE_TRACE("not support DirCreate");
	return false;
}

bool jniFile::DirDelete(const char* name)
{
	CORE_TRACE("not support DirDelete");
	return false;
}

#include "../public/module.h"
DECLARE_FUNCTION_INTERFACE(jniFile);
void lib_jni_file()
{
	#define REG_INTERFACE(p) IIntCreator& p##_IntCreator();LinkIntCreator(p##_IntCreator());
	REG_INTERFACE(jniFile);
}
