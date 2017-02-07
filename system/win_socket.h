//--------------------------------------------------------------------
// 文件名:		win_socket.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年12月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SYSTEM_WIN_SOCKET_H
#define _SYSTEM_WIN_SOCKET_H

#include <winsock2.h>
#include <windows.h>
#include <string.h>
#pragma comment(lib, "ws2_32.lib")

#define PORT_INVALID_SOCKET INVALID_SOCKET

typedef SOCKET port_socket_t;

// 初始化通讯
inline bool Port_CommSystemStartup()
{
	WSADATA wsa;	
	
	if (WSAStartup(0x202, &wsa) != 0)
	{
		return false;
	}
	
	return true;
}

// 结束通讯
inline bool Port_CommSystemCleanup()
{
	WSACleanup();
	
	return true;
}

// 创建TCP连接
inline bool Port_SocketOpenTCP(port_socket_t* pSockHandle)
{
	port_socket_t sock = socket(AF_INET, SOCK_STREAM, 0);
	
	if (INVALID_SOCKET == sock)
	{
		return false;
	}
	
	*pSockHandle = sock;
	
	return true;
}

// 创建UDP连接
inline bool Port_SocketOpenUDP(port_socket_t* pSockHandle)
{
	port_socket_t sock = socket(AF_INET, SOCK_DGRAM, 0);
	
	if (INVALID_SOCKET == sock)
	{
		return false;
	}
	
	*pSockHandle = sock;
	
	return true;
}

// 关闭连接
inline bool Port_SocketClose(port_socket_t sock_handle)
{
	int res = closesocket(sock_handle);
	
	if (SOCKET_ERROR == res)
	{
		return false;
	}
			
	return true;
}

// 停止收发
inline bool Port_SocketShutdown(port_socket_t sock_handle)
{
	return shutdown(sock_handle, SD_BOTH) != SOCKET_ERROR;
}

// 停止发送
inline bool Port_SocketShutdownSend(port_socket_t sock_handle)
{
	return shutdown(sock_handle, SD_SEND) != SOCKET_ERROR;
}

// 连接
inline bool Port_SocketConnect(port_socket_t sock_handle, const char* addr, 
	int port)
{
	sockaddr_in sa;		
	
	memset(&sa, 0, sizeof(sa));	
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr(addr);
	sa.sin_port = htons(port);
	
	int res = connect(sock_handle, (sockaddr*)&sa, sizeof(sockaddr));
	
	if (SOCKET_ERROR == res)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			return false;
		}
	}
	
	return true;
}

// 绑定端口
inline bool Port_SocketBind(port_socket_t sock_handle, const char* addr, 
	int port)
{
	sockaddr_in sa;

	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr(addr);
	sa.sin_port = htons((u_short)port);

	int res = bind(sock_handle, (const sockaddr*)&sa, sizeof(sa));

	if (SOCKET_ERROR == res)
	{
		return false;
	}
			
	return true;
}

// 侦听端口
inline bool Port_SocketListen(port_socket_t sock_handle, int backlog)
{
	int res = listen(sock_handle, backlog);

	if (SOCKET_ERROR == res)
	{
		return false;
	}
	
	return true;
}

// 接受连接
inline bool Port_SocketAccept(port_socket_t sock_handle, 
	port_socket_t* pNewSocket, char* remote_addr, 
	size_t remote_addr_size, int* remote_port)
{
	sockaddr_in	sa;
	int	len = sizeof(sockaddr);
	
	SOCKET sock = accept(sock_handle, (sockaddr*)&sa, &len);

	if (INVALID_SOCKET == sock)
	{
		return false;
	}
	
	*pNewSocket = sock;
	
	if (remote_addr)
	{
		char* addr = inet_ntoa(sa.sin_addr);
		size_t addr_size = strlen(addr);
		
		if (addr_size >= remote_addr_size)
		{
			memcpy(remote_addr, addr, remote_addr_size - 1);
			remote_addr[remote_addr_size - 1] = 0;
		}
		else
		{
			memcpy(remote_addr, addr, addr_size + 1);
		}
	}
	
	if (remote_port)
	{
		*remote_port = ntohs(sa.sin_port);
	}	
	
	return true;
}

// 获得连接的本地地址
inline bool Port_SocketGetSockName(port_socket_t sock_handle, char* addr, 
	size_t addr_size, int* port)
{
	sockaddr_in	sa;
	int	len = sizeof(sockaddr);

	int res = getsockname(sock_handle, (sockaddr*)&sa, &len);

	if (SOCKET_ERROR == res)
	{
		return false;
	}

	if (addr)
	{
		char* s = inet_ntoa(sa.sin_addr);
		size_t size = strlen(s);

		if (size >= addr_size)
		{
			memcpy(addr, s, addr_size - 1);
			addr[addr_size - 1] = 0;
		}
		else
		{
			memcpy(addr, s, size + 1);
		}
	}

	if (port)
	{
		*port = ntohs(sa.sin_port);
	}	

	return true;
}

// 获得连接的对端地址
inline bool Port_SocketGetPeerName(port_socket_t sock_handle, char* addr, 
	size_t addr_size, int* port)
{
	sockaddr_in	sa;
	int	len = sizeof(sockaddr);

	int res = getpeername(sock_handle, (sockaddr*)&sa, &len);

	if (SOCKET_ERROR == res)
	{
		return false;
	}

	if (addr)
	{
		char* s = inet_ntoa(sa.sin_addr);
		size_t size = strlen(s);

		if (size >= addr_size)
		{
			memcpy(addr, s, addr_size - 1);
			addr[addr_size - 1] = 0;
		}
		else
		{
			memcpy(addr, s, size + 1);
		}
	}

	if (port)
	{
		*port = ntohs(sa.sin_port);
	}	

	return true;
}

// 发送消息
inline int Port_SocketSend(port_socket_t sock_handle, const char* buffer, 
	size_t size)
{
	int res = send(sock_handle, buffer, (int)size, 0);
	
	if (SOCKET_ERROR == res)
	{
		return -1;
	}
	
	return res;
}

// 异步发送消息
inline int Port_SocketSendAsync(port_socket_t sock_handle, const char* buffer, 
	size_t size, bool* would_block)
{
	int res = send(sock_handle, buffer, (int)size, 0);
	
	if (SOCKET_ERROR == res)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK)
		{
			*would_block = true;
		}
		else
		{
			*would_block = false;
		}
		
		return -1;
	}
	else
	{
		*would_block = false;
	}
	
	return res;
}

// 接收消息
inline bool Port_SocketReceive(port_socket_t sock_handle, char* buffer,
	size_t size, size_t* pReadSize)
{
	int res = recv(sock_handle, buffer, (int)size, 0);
	
	if (SOCKET_ERROR == res)
	{
		return false;
	}
	
	*pReadSize = res;
	
	return true;
}

// 发送消息到指定地址
inline int Port_SocketSendTo(port_socket_t sock_handle, const char* buffer, 
	size_t size, const char* addr, int port)
{
	sockaddr_in	sa;
	
	memset(&sa, 0, sizeof(sa));	
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr(addr);
	sa.sin_port = htons(port);
	
	int res = sendto(sock_handle, buffer, (int)size, 0, (sockaddr*)&sa, 
		sizeof(sockaddr));
	
	if (SOCKET_ERROR == res)
	{
		return -1;
	}
	
	return res;
}
	
// 接收消息和地址
inline bool Port_SocketReceiveFrom(port_socket_t sock_handle, char* buffer,
	size_t size, char* remote_addr, size_t remote_addr_size, int* remote_port,
	size_t* pReadSize)
{
	sockaddr_in sa;
	int len = sizeof(sockaddr);

	int res = recvfrom(sock_handle, buffer, (int)size, 0, (sockaddr*)&sa, 
		&len);
	
	if (SOCKET_ERROR == res)
	{
		return false;
	}
	
	*pReadSize = res;

	if (remote_addr)
	{
		char* addr = inet_ntoa(sa.sin_addr);
		size_t addr_size = strlen(addr);
		
		if (addr_size >= remote_addr_size)
		{
			memcpy(remote_addr, addr, remote_addr_size - 1);
			remote_addr[remote_addr_size - 1] = 0;
		}
		else
		{
			memcpy(remote_addr, addr, addr_size + 1);
		}
	}
	
	if (remote_port)
	{
		*remote_port = ntohs(sa.sin_port);
	}
	
	return true;
}

// 监视读取事件
inline bool Port_SocketSelectRead(port_socket_t sock_handle, int wait_ms, 
	bool* pReadFlag, bool* pExceptFlag)
{
	fd_set read_fs;
	fd_set except_fs;

	FD_ZERO(&read_fs);
	FD_ZERO(&except_fs);
	FD_SET(sock_handle, &read_fs);
	FD_SET(sock_handle, &except_fs);
	
	timeval tv = { 0, wait_ms * 1000 };

	int res = select(2, &read_fs, NULL, &except_fs, &tv);

	if (SOCKET_ERROR == res)
	{
		return false;
	}
	
	*pReadFlag = false;
	*pExceptFlag = false;

	if (res > 0)
	{
		if (FD_ISSET(sock_handle, &except_fs))
		{
			*pExceptFlag = true;
		}
		
		if (FD_ISSET(sock_handle, &read_fs))
		{
			*pReadFlag = true;
		}
	}
	
	return true;
}

// 监视事件
inline bool Port_SocketSelect(port_socket_t sock_handle, int wait_ms, 
	bool* pReadFlag, bool* pWriteFlag, bool* pExceptFlag)
{
	fd_set read_fs;
	fd_set write_fs;
	fd_set except_fs;

	FD_ZERO(&read_fs);
	FD_ZERO(&write_fs);
	FD_ZERO(&except_fs);
	FD_SET(sock_handle, &read_fs);
	FD_SET(sock_handle, &write_fs);
	FD_SET(sock_handle, &except_fs);
	
	timeval tv = { 0, wait_ms * 1000 };

	int res = select(2, &read_fs, &write_fs, &except_fs, &tv);

	if (SOCKET_ERROR == res)
	{
		return false;
	}
	
	*pReadFlag = false;
	*pWriteFlag = false;
	*pExceptFlag = false;

	if (res > 0)
	{
		if (FD_ISSET(sock_handle, &except_fs))
		{
			*pExceptFlag = true;
		}
		
		if (FD_ISSET(sock_handle, &write_fs))
		{
			*pWriteFlag = true;
		}
		
		if (FD_ISSET(sock_handle, &read_fs))
		{
			*pReadFlag = true;
		}
	}
	
	return true;
}

// 设置为非阻塞模式
inline bool Port_SocketSetNonBlocking(port_socket_t sock_handle)
{
	u_long nb = 1;
	int res = ioctlsocket(sock_handle, FIONBIO, &nb);
	
	if (SOCKET_ERROR == res)
	{
		return false;
	}
	
	return true;
}

// 设置为阻塞模式
inline bool Port_SocketSetBlocking(port_socket_t sock_handle)
{
	u_long nb = 0;
	int res = ioctlsocket(sock_handle, FIONBIO, &nb);
	
	if (SOCKET_ERROR == res)
	{
		return false;
	}
	
	return true;
}

// 设置为可重复使用地址
inline bool Port_SocketSetReuseAddr(port_socket_t sock_handle)
{
	int flag = 1;
	int res = setsockopt(sock_handle, SOL_SOCKET, SO_REUSEADDR, 
		(const char*)&flag, sizeof(flag));
		
	if (SOCKET_ERROR == res)
	{
		return false;
	}
	
	return true;
}

// 设置为可广播
inline bool Port_SocketSetBroadcast(port_socket_t sock_handle)
{
	int flag = 1;
	int res = setsockopt(sock_handle, SOL_SOCKET, SO_BROADCAST, 
		(const char*)&flag, sizeof(flag));
		
	if (SOCKET_ERROR == res)
	{
		return false;
	}
	
	return true;
}

// 设置为立即发送
inline bool Port_SocketSetNoDelay(port_socket_t sock_handle)
{
	int flag = 1;
	int res = setsockopt(sock_handle, IPPROTO_TCP, TCP_NODELAY, 
		(const char*)&flag, sizeof(flag));
		
	if (SOCKET_ERROR == res)
	{
		return false;
	}
	
	return true;
}

// 获得在广播模式下应该绑定的地址
inline const char* Port_GetBroadcastBindAddr(const char* local_addr, 
	const char* broad_addr)
{
	return local_addr;
}

// 获得错误信息
inline const char* Port_SocketGetError(char* buffer, size_t size)
{
	int err = WSAGetLastError();
	
	if (FormatMessageA(
		FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
		0, err, 0, buffer, (DWORD)size, NULL) == 0)
	{
		buffer[0] = 0;
	}		
		
	return buffer;
}

#endif // _SYSTEM_WIN_SOCKET_H
