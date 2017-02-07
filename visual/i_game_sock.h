#ifndef _I_GAME_WIN_SOCK_H
#define _I_GAME_WIN_SOCK_H

#include "../public/module.h"
#include "../visual/i_game_client.h"

class IGameSock: public IEntity
{
public:
	// ��Ϣ������
	virtual PERSISTID GetSender() const = 0;
	// ��Ϣ������
	virtual PERSISTID GetReceiver() const = 0;
	
	// �Ƿ�������״̬
	virtual bool GetConnected() const = 0;
	// ���SOCKET״̬
	virtual int GetState() const = 0;
	
	// ���ؼ���ģ��
	virtual bool LoadEncodeDll(const char* dll_name) = 0;

	// ���ӷ�����
	virtual bool Connect(const char* addr, int port, bool blocking = true) = 0;

	// �Ͽ�������
	virtual bool Disconnect() = 0;

	// ������Ϣ
	virtual bool SendData(void* pdata, size_t len) = 0;

	// ÿ֡��������Ϣ��ʱ������
	virtual void SetLimitTime(double value) = 0;

	virtual double GetLimitTime() const = 0;

	// ֱ��Ͷ����Ϣ
	virtual bool PutData(void* pdata, size_t len) = 0;

	virtual bool PutData2(const void* pdata1, size_t len1, 
		const void* pdata2,  size_t len2) = 0;
};

#endif