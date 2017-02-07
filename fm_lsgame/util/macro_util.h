#pragma once

#ifndef assert
#define assert(content) 
#endif
#define	HALF_PI (1.570796325f)
#define	PI      (3.14159265f)
#define PI2     (6.2831853f)

#ifndef NULL_RETURN_VOID
#define NULL_RETURN_VOID(var) if(NULL==(var)) {return;}
#endif

//
#ifndef NULL_RETURN
#define NULL_RETURN(var,res) if(NULL==(var)) {return (res);}
#endif

#ifdef _DEBUG
#define CLIENT_NEW(TYPE) new(g_pCore->MemoryCheckAllocDebug(sizeof(TYPE),  __FILE__, __LINE__))TYPE
#define CLIENT_DELETE(ptr) core_mem::CoreDelete(ptr)
#else
#define CLIENT_NEW(TYPE) new(g_pCore->MemoryAlloc(sizeof(TYPE)))TYPE
#define CLIENT_DELETE(ptr) core_mem::CoreDelete(ptr)
#endif // _DEBUG

#ifndef IMODEL_ADV_NODERADIAN180
#define IMODEL_ADV_NODERADIAN180
#endif

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(types) \
    types(const types&); \
    types& operator=(types&) const;
#endif

#define BASE_STATE		"ground"            // 基本状态定义

#include "../public/fast_str.h"
typedef TFastStr<char, 32> fast_string;
typedef TFastStr<wchar_t, 32> fast_wstring;


#define CORE_ADD_EXECUTE(pEnt)  \
	g_pCore->RemoveExecute(pEnt); \
	g_pCore->AddExecute(pEnt);

#define CORE_REMOVE_EXECUTE(pEnt) \
	g_pCore->RemoveExecute(pEnt);