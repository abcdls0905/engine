//--------------------------------------------------------------------
// �ļ���:		game_encoder.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��6��5��	
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _GAME_ENCODER_H
#define _GAME_ENCODER_H


#include "../public/module.h"
#include "../utils/lock_util.h"

/*
// ���ܷ�����Ϣ������ԭ��
typedef bool (__cdecl* SEND_ENCODE_API)(void* context, void* pack, size_t size);
// ���ܽ�����Ϣ������ԭ��
typedef bool (__cdecl* RECV_DECODE_API)(void* context, void* pack, size_t size);
// ������Ϣ���������ĺ���ԭ��
typedef bool (__cdecl* SEND_MESSAGE_API)(void* context, void* pack, size_t size);
*/
// ��Ϣ�ӽ���

class GameSock;

class CGameEncoder
{
public:
	CGameEncoder();
	~CGameEncoder();

	void SetGameSock(GameSock* value);
	
	// ���ؼ���DLL
	bool LoadEncodeDll(const char* name);
	// ��Ϣ����
	bool EncodeMessage(void* pdata, size_t len);
	// ��Ϣ����
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
