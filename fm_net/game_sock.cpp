//--------------------------------------------------------------------
// 文件名:		game_win_sock.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年2月27日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#include "../public/macros.h"

#ifdef FX_SYSTEM_WINDOWS
	#include "../system/win_socket.h"
#endif // FX_SYSTEM_WINDOWS

#ifdef FX_SYSTEM_LINUX
	#include "../system/linux_socket.h"
#endif // FX_SYSTEM_LINUX

#ifdef FX_SYSTEM_IOS
	#include "../system/ios_portable.h"
#endif // FX_SYSTEM_IOS

#ifdef _WIN32
	#define _WIN32_WINNT 0x0400
	#include <winsock2.h>
	#include <windows.h>
	#pragma comment(lib, "ws2_32.lib")
#else
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <sys/errno.h>
	#include <sys/select.h>
	#include <sys/ioctl.h>
	#include <errno.h>
	#include <string.h>
	#include <netdb.h>
	#include <netinet/in.h>
	#include <netinet/tcp.h>
	#include <arpa/inet.h>
	#include <signal.h>
	#include <unistd.h>

	#ifdef __ANDROID__
		#include <sys/endian.h>
		#include <fcntl.h>
	#else
		#include <sys/sockio.h>
		#include <sys/filio.h>
		#include <sys/fcntl.h>
	#endif

	#define SOCKET int
	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
	#define closesocket close
#endif

#include "game_sock.h"
#include "sock_buf.h"
#include "game_encoder.h"
#include "game_sender.h"
#include "game_receiver.h"
#include "../visual/i_win_socket.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"
#include "../public/inlines.h"
#include "../public/portable.h"
#include "../visual/i_event_manager.h"
#include "../public/portable.h"
#define performance_time Port_GetPerformanceTime
//DECLARE_MODULE("");

/*
// 导出内存分配情况
#ifdef _DEBUG 

#include "../utils/leak_checker.h"

//extern CLeakChecker g_LeakChecker;

// 导出DLL模块的内存使用情况
static bool ext_dump_memory_fx_net2()
{
	g_LeakChecker.DumpMemory();
	return true;
}

// 在脚本中可调用此扩展函数获得内存导出文件
DECLARE_FUNCTION_0(bool, ext_dump_memory_fx_net2);

#endif // _DEBUG
*/

/// \file game_win_sock.cpp
/// \brief 单线程游戏网络通讯

/// entity: GameSock
/// \brief 单线程游戏网络通讯实体
DECLARE_ENTITY(GameSock, NULL, IEntity);

/// readonly: object Sender
/// \brief 消息发送器
DECLARE_PROPERTY_GET(PERSISTID, GameSock, Sender, GetSender);
/// readonly: object Receiver
/// \brief 消息接收器
DECLARE_PROPERTY_GET(PERSISTID, GameSock, Receiver, GetReceiver);
/// readonly: int State
/// \brief 连接状态
DECLARE_PROPERTY_GET(int, GameSock, State, GetState);
/// readonly: bool Connected
/// \brief 是否连接成功
DECLARE_PROPERTY_GET(bool, GameSock, Connected, GetConnected);

/// property: double LimitTime
/// \brief 每帧允许处理消息的时间上限
DECLARE_PROPERTY(double, GameSock, LimitTime, GetLimitTime, SetLimitTime);

/*
/// method: bool LoadEncodeDll(string dll_name)
/// \brief 加载加密模块
/// \param dll_name 模块文件名
DECLARE_METHOD_1(bool, GameSock, LoadEncodeDll, const char*);

/// method: bool Connect(string addr, int port)
/// \brief 连接服务器
/// \param addr IP地址
/// \param port 端口号
DECLARE_METHOD_2(bool, GameSock, Connect, const char*, int);
/// method: bool Disconnect()
/// \brief 断开连接
DECLARE_METHOD_0(bool, GameSock, Disconnect);*/

const unsigned char SOCKETMESSAGE_ENDMARK = 0xEE;
const unsigned char SOCKETMESSAGE_ZERO = 0x00;

// 获得当前时刻
// 获得当前时刻
//extern double performance_time();


bool IsConnected()
{
#ifdef _WIN32
	return (WSAGetLastError() == WSAEWOULDBLOCK);
#else
	return (errno == EINPROGRESS);
#endif
}


// GameSock

GameSock::GameSock()
{
	m_SockMsgHandler = NULL;
	m_pWinSocket = NULL;
	m_pSender = NULL;
	m_pReceiver = NULL;
	m_hSocket = NULL;
	m_nMsgId = 0;
	m_nState = 0;
	m_pSendBuf = CORE_NEW(CSockBuf);
	m_pRecvBuf = CORE_NEW(CSockBuf);
	m_pOutBuf = (unsigned char*)CORE_ALLOC(BUFFER_LEN);
	m_bSendBroken = false;
	m_nSendOffset = 0;
	m_nSendRemain = 0;
	m_pInBuf = (unsigned char*)CORE_ALLOC(BUFFER_LEN);
	m_nInCount = 0;
	m_nInPrior = 0;
	m_nPort = 0;
	m_dLimitTime = 0.1;
	m_pEncoder = CORE_NEW(CGameEncoder);
	m_pEncoder->SetGameSock(this);
}

GameSock::~GameSock()
{
	CORE_FREE(m_pOutBuf, BUFFER_LEN);
	CORE_FREE(m_pInBuf, BUFFER_LEN);
	CORE_DELETE(m_pSendBuf);
	CORE_DELETE(m_pRecvBuf);
	CORE_DELETE(m_pEncoder);
}

bool GameSock::Init(const IVarList& args)
{
	m_pWinSocket = (IWinSocket*)GetCore()->GetInterface("WinSocket");

	if (NULL == m_pWinSocket)
	{
		CORE_TRACE("(GameSock::Init)no WinSocket");
		return false;
	}

	// 加载加密模块
	//m_pEncoder->LoadEncodeDll("fx_cli.dll");

	m_nMsgId = m_pWinSocket->GetSockMsg();

	if (0 == m_nMsgId)
	{
		CORE_TRACE("(GameSock::Init)no message id");
		return false;
	}
	
	m_pSender = (GameSender*)GetCore()->CreateEntity("GameSender");
	m_pSender->SetGameSock(this);
	m_pReceiver = (GameReceiver*)GetCore()->CreateEntity("GameReceiver");
	
	//GetCore()->AddMsgProc(this, m_nMsgId);
	GetCore()->AddExecute(this);
	
	return true;
}

bool GameSock::Shut()
{
	Disconnect();
	
	if (m_nMsgId != 0)
	{
		m_pWinSocket->ReleaseSockMsg(m_nMsgId);
		m_nMsgId = 0;
	}

	GetCore()->RemoveExecute(this);
	
	SAFE_RELEASE(m_pSender);
	SAFE_RELEASE(m_pReceiver);
	SAFE_RELEASE(m_pWinSocket);

	return true;
}

void GameSock::MainLoop()
{
	if(m_hSocket == 0)
        return;
    fd_set readable;
    fd_set writable;
    fd_set exception;

	timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

	FD_ZERO(&readable);
	FD_ZERO(&exception);
	FD_ZERO(&writable);
	FD_SET(m_hSocket, &readable);
	FD_SET(m_hSocket, &exception);
	FD_SET(m_hSocket, &writable);

	int nfds = select(m_hSocket+1, &readable, &writable, &exception, &timeout);
	if(FD_ISSET(m_hSocket, &exception))
	{
		if(m_SockMsgHandler)
			m_SockMsgHandler->OnClose();
	}
	
	if(FD_ISSET(m_hSocket, &readable))
	{
		ProcessRead();
	}

	if(FD_ISSET(m_hSocket, &writable))
	{
		ProcessConnect();
		ProcessWrite();
	}
}

void GameSock::ProcessRead()
{
	unsigned char buf[2048];
			
	while (true)
	{
		int len = recv(m_hSocket, (char*)buf, sizeof(buf), 0);

		if(len == 0)
		{
			ProcessClose();
			break;
		}

		if (SOCKET_ERROR == len)
		{	
			break;
		}

		for (int i = 0; i < len; ++i)
		{
			unsigned char ch = buf[i];
			
			if ((SOCKETMESSAGE_ENDMARK == ch)
				&& (SOCKETMESSAGE_ENDMARK == m_nInPrior))
			{
				m_nInCount--;
				
				if (m_nInCount > 0)
				{
					RecvData(m_pInBuf, m_nInCount);
				}
				
				m_nInPrior = 0;
				m_nInCount = 0;
				continue;
			}
			else if ((SOCKETMESSAGE_ZERO == ch) 
				&& (SOCKETMESSAGE_ENDMARK == m_nInPrior))
			{
			}
			else
			{
				if (m_nInCount < BUFFER_LEN)
				{
					m_pInBuf[m_nInCount++] = ch;
				}
			}

			m_nInPrior = ch;
		}

		if (len < sizeof(buf))
		{
			// 数据已经接收完
			break;
		}
	}
}

bool GameSock::ProcessWrite()
{
	m_bSendBroken = false;

	while (m_nSendRemain > 0)
	{
		if (!InnerSend())
		{
			return false;
		}

		if (m_bSendBroken)
		{
			// 无法继续发送
			return true;
		}
	}

	while (!m_pSendBuf->IsEmpty())
	{
		void* pdata;
		size_t len;
		int src;

		if (!m_pSendBuf->Peek(src,pdata, len))
		{
			CORE_TRACE("(GameSock::MsgProc)get message failed");
			return false;
		}

		m_nSendRemain = MakeSendData(pdata, len);
		m_nSendOffset = 0;
		m_pSendBuf->Erase();

		if (!InnerSend())
		{
			return false;
		}

		if (m_bSendBroken)
		{
			// 无法继续发送
			return true;
		}
	}

	if (!m_bSendBroken)
	{
		/*
		HWND hwnd = (HWND)GetCore()->GetWindowHandle();
		int res = WSAAsyncSelect(m_hSocket, hwnd, m_nMsgId, 
			FD_READ | FD_CLOSE);

		if (SOCKET_ERROR == res)
		{
			CORE_TRACE("(GameSock::MsgProc)WSAsyncSelect error");
			m_nState = WORKSTATE_FAILED;
			return false;
		}*/
	}
	return true;
}

void GameSock::ProcessConnect()
{
	if(m_nState == WORKSTATE_CONNECTED)
		return;

	if(m_hSocket == 0)
		return;

#ifdef _WIN32
	char error;
	int optlen = sizeof(error);
	getsockopt(m_hSocket, SOL_SOCKET, SO_ERROR, &error, &optlen);
#else
	int error;
	unsigned int optlen = sizeof(error);
	getsockopt(m_hSocket, SOL_SOCKET, SO_ERROR, &error, &optlen);
#endif
	if(error != 0)
		return;

	if (m_nState == WORKSTATE_CONNECTING)
	{
		m_nState = WORKSTATE_CONNECTED;
		/*
		if (WSAGETSELECTERROR(param2))
		{
			m_nState = WORKSTATE_FAILED;
	//		GetCore()->ExecCallback(this, "on_connect_fail", CVarList());
		}
		else
		{
			m_nState = WORKSTATE_CONNECTED;
		//	GetCore()->ExecCallback(this, "on_connected", CVarList());
		}*/
		if(m_SockMsgHandler)
		{
			m_SockMsgHandler->OnConnect();
		}
		else
		{
			event_send(GetID(), "on_connected", CVarList());
		}
	}
	else
	{
		CORE_TRACE("(GameSock::MsgProc)FD_CONNECT error");
	}
}

void GameSock::ProcessClose()
{
	Disconnect();
}

/*
bool GameSock::MsgProc(void* hwnd, unsigned int msg, size_t param1, 
	size_t param2, int& result)
{
	Assert(msg == m_nMsgId);
	
	if ((SOCKET)param1 != m_hSocket)
	{
		return false;
	}
	
	switch (WSAGETSELECTEVENT(param2))
	{
	case FD_CONNECT:
		if (m_nState == WORKSTATE_CONNECTING)
		{
			if (WSAGETSELECTERROR(param2))
			{
				m_nState = WORKSTATE_FAILED;
		//		GetCore()->ExecCallback(this, "on_connect_fail", CVarList());
			}
			else
			{
				m_nState = WORKSTATE_CONNECTED;
			//	GetCore()->ExecCallback(this, "on_connected", CVarList());
			}
		}
		else
		{
			CORE_TRACE("(GameSock::MsgProc)FD_CONNECT error");
		}
		break;
	case FD_READ:
		{
			unsigned char buf[2048];
			
			while (true)
			{
				int len = recv(m_hSocket, (char*)buf, sizeof(buf), 0);

				if (SOCKET_ERROR == len)
				{	
					break;
				}

				for (int i = 0; i < len; ++i)
				{
					unsigned char ch = buf[i];
					
					if ((SOCKETMESSAGE_ENDMARK == ch)
						&& (SOCKETMESSAGE_ENDMARK == m_nInPrior))
					{
						m_nInCount--;
						
						if (m_nInCount > 0)
						{
							RecvData(m_pInBuf, m_nInCount);
						}
						
						m_nInPrior = 0;
						m_nInCount = 0;
						continue;
					}
					else if ((SOCKETMESSAGE_ZERO == ch) 
						&& (SOCKETMESSAGE_ENDMARK == m_nInPrior))
					{
					}
					else
					{
						if (m_nInCount < BUFFER_LEN)
						{
							m_pInBuf[m_nInCount++] = ch;
						}
					}

					m_nInPrior = ch;
				}

				if (len < sizeof(buf))
				{
					// 数据已经接收完
					break;
				}
			}
		}
		break;
	case FD_WRITE:
		{
			m_bSendBroken = false;

			while (m_nSendRemain > 0)
			{
				if (!InnerSend())
				{
					return false;
				}

				if (m_bSendBroken)
				{
					// 无法继续发送
					return true;
				}
			}

			while (!m_pSendBuf->IsEmpty())
			{
				void* pdata;
				size_t len;

				if (!m_pSendBuf->Peek(pdata, len))
				{
					CORE_TRACE("(GameSock::MsgProc)get message failed");
					return false;
				}

				m_nSendRemain = MakeSendData(pdata, len);
				m_nSendOffset = 0;
				m_pSendBuf->Erase();

				if (!InnerSend())
				{
					return false;
				}

				if (m_bSendBroken)
				{
					// 无法继续发送
					return true;
				}
			}
		}
		break;
	case FD_CLOSE:
		{
			int err = WSAGETSELECTERROR(param2);
			
			//GetCore()->ExecCallback(this, "on_close", CVarList() << err);
			Disconnect();
		}
		break;
	default	:
		break;
	}			
	
	return true;
}
*/

void GameSock::Execute(float seconds)
{
	CAutoLock autolock(m_Lock);

	MainLoop();

	if (m_pRecvBuf->IsEmpty())
	{
		return;
	}

	// 每帧允许执行的最大时间
	double max_time = m_dLimitTime;
	double beg_time = performance_time();
	void* pdata;
	size_t len;
	int src;

	while (m_pRecvBuf->Peek(src,pdata, len))
	{
		m_pReceiver->ProcessMessage(src,pdata, len);
		m_pRecvBuf->Erase();

		if (max_time > 0.0)
		{
			double cur_time = performance_time();

			// 防止每帧处理消息的时间太长 
			if ((cur_time - beg_time) > max_time)
			{
				unsigned int msg_type = *(unsigned char*)pdata;
				char info[128];
				SafeSprintf(info, sizeof(info), 
					"(GameSock::Execute)process message type %d use %f seconds", 
					msg_type, cur_time - beg_time);
				CORE_TRACE(info);
				break;
			}
		}
	}
}

PERSISTID GameSock::GetSender() const
{
	if (NULL == m_pSender)
	{
		return PERSISTID();
	}
	
	return m_pSender->GetID();
}

PERSISTID GameSock::GetReceiver() const
{
	if (NULL == m_pReceiver)
	{
		return PERSISTID();
	}
	
	return m_pReceiver->GetID();
}

bool GameSock::GetConnected() const
{ 
	return (m_nState == WORKSTATE_CONNECTED); 
}

int GameSock::GetState() const
{ 
	return m_nState; 
}

bool GameSock::LoadEncodeDll(const char* dll_name)
{
	Assert(dll_name != NULL);

	return m_pEncoder->LoadEncodeDll(dll_name);
}

bool GameSock::Connect(const char* addr, int port, bool blocking)
{
	Assert(addr != NULL);
	
	CAutoLock autolock(m_Lock);

	if (m_nState == WORKSTATE_CONNECTING)
	{
		CORE_TRACE("(GameSock::Connect)in connecting");
		return false;
	}
	else if (m_nState != 0)
	{
		Disconnect();
	}
	
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	
	if (INVALID_SOCKET == sock)
	{
		CORE_TRACE("(GameSock::Connect)no resource");
#ifdef __ANDROID__
		char buf[256];
		Port_SocketGetError(buf, 256);		
		CORE_TRACE(buf);
#endif
		return false;
	}

	// 增加接收缓冲区
	int recv_buf_len = 0x10000;
	int res = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, 
		(const char*)&recv_buf_len, sizeof(recv_buf_len));

	if (SOCKET_ERROR == res)
	{
		CORE_TRACE("(GameSock::Connect)setsockopt error");
		closesocket(sock);
		return false;
	}

	/*
	HWND hwnd = (HWND)GetCore()->GetWindowHandle();
	// 进入异步状态
	res = WSAAsyncSelect(sock, hwnd, m_nMsgId, 
		FD_CONNECT | FD_READ | FD_CLOSE);*/
	
	if (SOCKET_ERROR == res)
	{
		CORE_TRACE("(GameSock::Connect)WSAsyncSelect error");
		closesocket(sock);
		return false;
	}
	
	// 连接
	sockaddr_in sa;

	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr(addr);
	sa.sin_port = htons(port);
	
//	Port_SocketSetNonBlocking((port_socket_t)sock);
	if(blocking)
	{
		res = connect(sock, (sockaddr*)&sa, sizeof(sockaddr));
		if (SOCKET_ERROR == res)
		{
			if (IsConnected())
			{
				CORE_TRACE("(GameSock::Connect)connect error");
				closesocket(sock);
				return false;
			}
		}
	}
	else
	{
		Port_SocketSetNonBlocking((port_socket_t)sock);
		res = connect(sock, (sockaddr*)&sa, sizeof(sockaddr));
	}
	
	m_hSocket = sock;
	m_strAddr = addr;
	m_nPort = port;
	m_pSendBuf->Clear();
	m_pRecvBuf->Clear();
	m_bSendBroken = false;
	m_nSendOffset = 0;
	m_nSendRemain = 0;
	
	if(blocking)
	{
		m_nState = WORKSTATE_CONNECTED;
	}
	else
	{
		m_nState = WORKSTATE_CONNECTING;
	}

	return true;
}

bool GameSock::Disconnect()
{
	CAutoLock autolock(m_Lock);

	if (m_hSocket != NULL)
	{
		closesocket(m_hSocket);
		m_hSocket = NULL;
	}

	// 连接关闭时要清空接收缓冲区
	m_pRecvBuf->Clear();
	m_nState = 0;
	event_send(GetID(), "on_connect_fail", CVarList());
	return true;
}

bool GameSock::InnerSend()
{
	int res = send(m_hSocket, (const char*)m_pOutBuf + m_nSendOffset, 
		(int)m_nSendRemain, 0);

	if (SOCKET_ERROR == res)
	{
		if (IsConnected())
		{
			// 消息未发送完成
			m_bSendBroken = true;
			return true;
		}

		CORE_TRACE("(GameSock::InnerSend)send failed");
		m_nState = WORKSTATE_FAILED;
		return false;
	}

	m_nSendOffset += res;
	m_nSendRemain -= res;

	return true;
}

size_t GameSock::MakeSendData(void* pdata, size_t len)
{
	// 发送前先编码和加密
	m_pEncoder->EncodeMessage(pdata, len);

	unsigned char* buf = (unsigned char*)pdata;
	size_t count = 0;

	for (size_t i = 0; i < len; ++i)
	{
		m_pOutBuf[count++] = buf[i];

		if (SOCKETMESSAGE_ENDMARK == buf[i])
		{
			m_pOutBuf[count++] = SOCKETMESSAGE_ZERO;
		}
	}

	m_pOutBuf[count++] = SOCKETMESSAGE_ENDMARK;
	m_pOutBuf[count++] = SOCKETMESSAGE_ENDMARK;

	return count;
}

bool GameSock::RecvData(void* pdata, size_t len)
{
	Assert(pdata != NULL);
	Assert(len > 0);
	
	// 接收消息解码和解密
	m_pEncoder->DecodeMessage(pdata, len);

	// 将消息放入缓冲区统一处理
	if (!m_pRecvBuf->Put(1,pdata, len))
	{
		CORE_TRACE("(GameSock::RecvData)receive buffer overflow");
		return false;
	}

	return true;
	//return m_pReceiver->ProcessMessage(pdata, len);
}	

bool GameSock::SendData(void* pdata, size_t len)
{
	Assert(pdata != NULL);
	Assert(len > 0);
	
	CAutoLock autolock(m_Lock);

	if (m_nState != WORKSTATE_CONNECTED)
	{
		char info[128];
		SafeSprintf(info, sizeof(info), 
			"(GameSock::SendData)not connected, state is %d", m_nState);
		CORE_TRACE(info);
		return false;
	}
	
	if (len > (BUFFER_LEN / 2 - 2))
	{
		CORE_TRACE("(GameSock::SendData)message too long");
		return false;
	}
	
	if ((!m_bSendBroken) && (0 == m_nSendRemain))
	{
		m_nSendRemain = MakeSendData(pdata, len);
		m_nSendOffset = 0;

		while (m_nSendRemain > 0)
		{
			if (!InnerSend())
			{
				return false;
			}

			if (m_bSendBroken)
			{
				/*
				HWND hwnd = (HWND)GetCore()->GetWindowHandle();
				int res = WSAAsyncSelect(m_hSocket, hwnd, m_nMsgId, 
					FD_WRITE | FD_READ | FD_CLOSE);

				if (SOCKET_ERROR == res)
				{
					CORE_TRACE("(GameSock::SendData)WSAsyncSelect error");
					m_nState = WORKSTATE_FAILED;
					return false;
				}*/
				
				break;
			}
		}
	}
	else
	{
		if (!m_pSendBuf->Put(1,pdata, len))
		{
			CORE_TRACE("(GameSock::SendData)send buffer overflow");
			// 缓冲区满
			m_nState = WORKSTATE_FAILED;
			return false;
		}
	}

	return true;
}

bool GameSock::PutData(void* pdata, size_t len)
{
	Assert(pdata != NULL);
	Assert(len > 0);

	CAutoLock autolock(m_Lock);

	// 将消息放入缓冲区统一处理
	if (!m_pRecvBuf->Put(0,pdata, len))
	{
		CORE_TRACE("(GameSock::PutData)receive buffer overflow");
		return false;
	}

	return true;
}
bool GameSock::PutData2(const void* pdata1, size_t len1, 
					   const void* pdata2,  size_t len2)
{
	Assert(pdata1 != NULL);
	Assert(len1 > 0);
	Assert(pdata2 != NULL);
	Assert(len2 > 0);

	CAutoLock autolock(m_Lock);

	// 将消息放入缓冲区统一处理
	if (!m_pRecvBuf->Put2(0,pdata1, len1,pdata2,len2))
	{
		CORE_TRACE("(GameSock::PutData2)receive buffer overflow");
		return false;
	}

	return true;
}