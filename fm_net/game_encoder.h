//--------------------------------------------------------------------
// 文件名:		game_encoder.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年6月5日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _GAME_ENCODER_H
#define _GAME_ENCODER_H


#include "../public/module.h"
#include "../utils/lock_util.h"

/*
// 加密发送消息函数的原型
typedef bool (__cdecl* SEND_ENCODE_API)(void* context, void* pack, size_t size);
// 解密接收消息函数的原型
typedef bool (__cdecl* RECV_DECODE_API)(void* context, void* pack, size_t size);
// 发送消息到服务器的函数原型
typedef bool (__cdecl* SEND_MESSAGE_API)(void* context, void* pack, size_t size);
*/
// 消息加解密

class GameSock;

class CGameEncoder
{
public:
	CGameEncoder();
	~CGameEncoder();

	void SetGameSock(GameSock* value);
	
	// 加载加密DLL
	bool LoadEncodeDll(const char* name);
	// 消息加密
	bool EncodeMessage(void* pdata, size_t len);
	// 消息解密
	bool DecodeMessage(void* pdata, size_t len);
	
private:
	static bool SendMessageApi(void* context, void* pack, size_t size);

private:
	GameSock* m_pGameSock;
//	CCoreApi* m_pCoreApi;
	//SEND_ENCODE_API m_pSendEncodeApi;
//	RECV_DECODE_API m_pRecvDecodeApi;
};

#endif // _GAME_ENCODER_H
