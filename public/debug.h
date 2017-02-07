//-------------------------------------------------------------------- 
// 文件名:		debug.h 
// 内  容:		调试工具
// 说  明:		
// 创建日期:	2002年10月28日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _PUBLIC_DEBUG_H
#define _PUBLIC_DEBUG_H

#ifdef FX_SYSTEM_WINDOWS
	#ifdef _DEBUG
		#include <crtdbg.h>
		#define NEW ::new(_NORMAL_BLOCK, __FILE__, __LINE__)
	#else
		#define NEW ::new
	#endif // _DEBUG
#else
	#define NEW ::new
#endif // FX_SYSTEM_WINDOWS

#ifdef _DEBUG
	void _AssertionFail(const char* msg, const char* file, size_t line);
	#define Assert(p) if (!(p)) { _AssertionFail(#p, __FILE__, __LINE__); }
#else
	#define Assert(p) (void(0))
#endif // _DEBUG

#define ASSERTION_FAIL_DEFINED \
	void _AssertionFail(const char* msg, const char* file, size_t line) \
	{ \
		FILE* fp = fopen("assert.log", "ab"); \
		if (fp) \
		{ \
			fprintf(fp, \
				"Assert(%s), file: %s, line: %d\r\n", msg, file, line); \
			fclose(fp); \
		} \
		fflush(stdout); \
		fprintf(stderr, \
			"Assert(%s), file: %s, line: %d", msg, file, line); \
		fflush(stderr); \
		abort(); \
	}

#endif // _PUBLIC_DEBUG_H
