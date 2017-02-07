//--------------------------------------------------------------------
// 文件名:		game_encoder.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2006年5月24日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#ifdef _WIN32
    #include <windows.h>
#endif
#include "game_encoder.h"
#include "game_sock.h"
//#include "core_api.h"
#include "../public/core_mem.h"

// CGameEncoder

CGameEncoder::CGameEncoder()
{
	m_pGameSock = NULL;
	/*
	m_pSendEncodeApi = NULL;
	m_pRecvDecodeApi = NULL;
	m_pCoreApi = CORE_NEW(CCoreApi);*/
}

CGameEncoder::~CGameEncoder()
{
	//CORE_DELETE(m_pCoreApi);
}

void CGameEncoder::SetGameSock(GameSock* value)
{
	Assert(value != NULL);

	m_pGameSock = value;
}

// 加密DLL

bool CGameEncoder::SendMessageApi(void* context, void* pack, size_t size)
{
	CGameEncoder* pthis = (CGameEncoder*)context;
	
	return pthis->m_pGameSock->SendData(pack, size);
}


/*
typedef const char* (__cdecl* DLL_GET_TYPE)();
typedef void (__cdecl* DLL_SET_API)(VAPI* core_api, SEND_MESSAGE_API send_msg);
typedef SEND_ENCODE_API (__cdecl* DLL_GET_SEND_API)();
typedef RECV_DECODE_API (__cdecl* DLL_GET_RECV_API)();*/

bool CGameEncoder::LoadEncodeDll(const char* name)
{
	return false;
	/*
	Assert(name != NULL);
	
	HMODULE handle = LoadLibrary(name);
	
	if (NULL == handle)
	{
		return false;
	}
	
	DLL_GET_TYPE get_type_func = (DLL_GET_TYPE)GetProcAddress(handle, 
		"FxModule_GetType");
	
	if (NULL == get_type_func)
	{
		FreeLibrary(handle);
		return false;
	}
	
	const char* dll_type = get_type_func();
	
	if (stricmp(dll_type, "FxClient") != 0)
	{
		FreeLibrary(handle);
		return false;
	}
	
	DLL_SET_API set_api_func = (DLL_SET_API)GetProcAddress(handle, 
		"FxClient_SetApi");
	DLL_GET_SEND_API get_send_api_func = (DLL_GET_SEND_API)GetProcAddress(
		handle, "FxClient_GetSendApi");
	DLL_GET_RECV_API get_recv_api_func = (DLL_GET_RECV_API)GetProcAddress(
		handle, "FxClient_GetRecvApi");
	
	if ((NULL == set_api_func)
		|| (NULL == get_send_api_func)
		|| (NULL == get_recv_api_func))
	{
		FreeLibrary(handle);
		return false;
	}
	
	set_api_func(m_pCoreApi, SendMessageApi);

	
	m_pSendEncodeApi = get_send_api_func();
	m_pRecvDecodeApi = get_recv_api_func();
	*/
	
	return true;
}

bool CGameEncoder::EncodeMessage(void* pdata, size_t len)
{
	return false;
	/*
	if (NULL == m_pSendEncodeApi)
	{
		return false;
	}

	return m_pSendEncodeApi(this, pdata, len);*/
}

bool CGameEncoder::DecodeMessage(void* pdata, size_t len)
{
	return false;
	/*
	if (NULL == m_pRecvDecodeApi)
	{
		return false;
	}

	return m_pRecvDecodeApi(this, pdata, len);*/
}

