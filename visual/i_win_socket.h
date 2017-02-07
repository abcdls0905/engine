//--------------------------------------------------------------------
// 文件名:		i_win_socket.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年2月27日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _VISUAL_I_WIN_SOCKET_H
#define _VISUAL_I_WIN_SOCKET_H

#include "../public/module.h"

// 网络连接管理接口

class IWinSocket: public IInterface
{
public:
	// 获得WINDOWS消息编号
	virtual int GetSockMsg() = 0;
	// 释放WINDOWS消息编号
	virtual bool ReleaseSockMsg(int msg) = 0;
};

#endif // _VISUAL_I_WIN_SOCKET_H
