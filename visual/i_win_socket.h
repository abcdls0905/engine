//--------------------------------------------------------------------
// �ļ���:		i_win_socket.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��2��27��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _VISUAL_I_WIN_SOCKET_H
#define _VISUAL_I_WIN_SOCKET_H

#include "../public/module.h"

// �������ӹ���ӿ�

class IWinSocket: public IInterface
{
public:
	// ���WINDOWS��Ϣ���
	virtual int GetSockMsg() = 0;
	// �ͷ�WINDOWS��Ϣ���
	virtual bool ReleaseSockMsg(int msg) = 0;
};

#endif // _VISUAL_I_WIN_SOCKET_H
