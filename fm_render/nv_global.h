//----------------------------------------------------------------------------------
// File:        jni/nv_global.h
// SDK Version: v10.00 
// Email:       tegradev@nvidia.com
// Site:        http://developer.nvidia.com/
//
// Copyright (c) 2007-2012, NVIDIA CORPORATION.  All rights reserved.
//
// TO  THE MAXIMUM  EXTENT PERMITTED  BY APPLICABLE  LAW, THIS SOFTWARE  IS PROVIDED
// *AS IS*  AND NVIDIA AND  ITS SUPPLIERS DISCLAIM  ALL WARRANTIES,  EITHER  EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED  TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL  NVIDIA OR ITS SUPPLIERS
// BE  LIABLE  FOR  ANY  SPECIAL,  INCIDENTAL,  INDIRECT,  OR  CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION,  DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE  USE OF OR INABILITY  TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
//
//
//----------------------------------------------------------------------------------
#ifndef __INCLUDED_NV_GLOBAL_H
#define __INCLUDED_NV_GLOBAL_H
#include <stdio.h>
#include "../public/core_file.h"
#include "../public/core_log.h"

#define NV_LITTLE_ENDIAN 1

#define NvFile FILE

inline void NVLogDebug(const char* func, const char* format, ...)
{
	char buf[512];
	va_list args;
	va_start(args, format);
	strcpy(buf, func);
	int len = strlen(func);
	vsnprintf(buf + strlen(func), sizeof(buf) - len, format, args);
	CORE_TRACE(buf);
	va_end(args);
}

inline void NVLogWarn(const char* func, const char* format, ...)
{
	char buf[512];
	va_list args;
	va_start(args, format);
	strcpy(buf, func);
	size_t len = strlen(func);
	vsnprintf(buf + strlen(func), sizeof(buf) - len, format, args);
	CORE_TRACE(buf);
	va_end(args);
}

inline void NVLogError(const char* func, const char* format, ...)
{
	char buf[512];
	va_list args;
	va_start(args, format);
	strcpy(buf, func);
	size_t len = strlen(func);
	vsnprintf(buf + strlen(func), sizeof(buf) - len, format, args);
	CORE_TRACE(buf);
	va_end(args);
}

inline FILE* NvFOpen(const char* file) 
{
	return core_file::fopen(file, "rb");
}

inline void NvFRead(void* buf, size_t size, size_t count, NvFile* fp)
{
	core_file::fread(buf, size, count, fp);
}

inline void NvFClose(FILE* fp)
{
	core_file::fclose(fp);
}

#include "nv_types.h"

#endif
