//--------------------------------------------------------------------
// 文件名:		FrameReceiverGroup.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2012年12月26日
// 创建人:		刘捷
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#include "frame_receiver_group.h"
#include "i_frame_receiver.h"
#include "frame_receiver_macro.h"
#include "../public/core_log.h"
#include <process.h>

/// entity: FrameReceiverGroup
/// \brief 帧数据接收器组
/// \see 继承自IVisBase
DECLARE_ENTITY("", FrameReceiverGroup, IVisBase);
/// property: object CameraFrameReceiver
/// \brief 摄像机帧接收器
DECLARE_PROPERTY(PERSISTID, FrameReceiverGroup, CameraFrameReceiver, GetCameraFrameReceiver, SetCameraFrameReceiver);
/// property: object MotionCaptureFrameReceiver
/// \brief 动作捕捉帧接收器
DECLARE_PROPERTY(PERSISTID, FrameReceiverGroup, MotionCaptureFrameReceiver, GetMotionCaptureFrameReceiver, SetMotionCaptureFrameReceiver);
/// property: object BoneFrameReceiver
/// \brief 骨骼动画帧接收器
DECLARE_PROPERTY(PERSISTID, FrameReceiverGroup, BoneFrameReceiver, GetBoneFrameReceiver, SetBoneFrameReceiver);

/// method: bool StartListen(const char* szMyIP, unsigned short nPort);
/// \brief 开始监听
/// \param szMyIP 本地IP
/// \param nPort 监听的端口
DECLARE_METHOD_2(bool, FrameReceiverGroup, StartListen, const char*, unsigned short);
/// method: bool StopListen();
/// \brief 停止监听
DECLARE_METHOD_0(bool, FrameReceiverGroup, StopListen);
/// property: state Listening
/// \brief 摄像机帧接收器的状态
DECLARE_PROPERTY_GET(bool, FrameReceiverGroup, Listening, GetListenState);
/// property: ip
/// \brief 本机的ip
DECLARE_PROPERTY_GET(const char*, FrameReceiverGroup, IP, GetHostIp);
/// method: bool VectorRotation(float x, float y, float z, float rx, float ry, float rz)
/// \brief 向量绕角度旋转
/// \param x x坐标
/// \param y y坐标
/// \param z z坐标
/// \param rx 绕x轴旋转的角度
/// \param ry 绕y轴旋转的角度
/// \param rz 绕z轴旋转的角度
DECLARE_METHOD_T(FrameReceiverGroup, VectorRotation);

#pragma comment(lib, "ws2_32.lib")

// 等待线程结束
static bool wait_thread_exit(HANDLE handle)
{
	for (;;)
	{
		DWORD res = WaitForSingleObject(handle, 1000);

		if (res == WAIT_OBJECT_0)
		{
			return true;
		}
		else if (res == WAIT_TIMEOUT)
		{
			DWORD exit_code;

			if (GetExitCodeThread(handle, &exit_code) == FALSE)
			{
				return false;
			}

			if (exit_code != STILL_ACTIVE)
			{
				break;
			}
		}
		else
		{
			return false;
		}
	}

	return true;
}

FrameReceiverGroup::FrameReceiverGroup()
{ 
	m_hThread = NULL;
	m_bQuit = true;
	m_nPort = 0;
	m_sock = INVALID_SOCKET;
}

FrameReceiverGroup::~FrameReceiverGroup() 
{
}

bool FrameReceiverGroup::Init(const IVarList& args)
{
	if (!IVisual::Init(args))
	{
		return false;
	}

	WSADATA wsa;	
	if (WSAStartup(0x101, &wsa) != 0)
	{
		CORE_TRACE("(FrameReceiverGroup::Init) WSAStartup failed.");
		return false;
	}

	GetHostIp();
	return true;
}

bool FrameReceiverGroup::Shut()
{
	StopListen();

	PERSISTID null_id = PERSISTID();
	for (size_t i = 0; i < RECEIVER_TYPE_MAX; ++i)
	{
		if (!m_Receivers[i].IsNull())
		{
			GetCore()->DeleteEntity(m_Receivers[i]);
			m_Receivers[i] = null_id;
		}
	}

	WSACleanup();
	return IVisual::Shut();
}

bool FrameReceiverGroup::StartListen(const char* szMyIP, unsigned short nPort)
{
	//if (m_hThread && (m_strIP == szMyIP) && m_nPort == nPort)
	//{
	//	return false;
	//}

	StopListen();

	m_sock = socket(AF_INET,SOCK_DGRAM, 0);
	if(INVALID_SOCKET == m_sock)
	{
		return false;
	}

	result_string ip = szMyIP;
	if (ip == "localhost")
	{
		ip = "127.0.0.1";
	}
 
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(nPort);
	addr.sin_addr.s_addr = inet_addr(ip.c_str());

	int nResult = bind(m_sock, (sockaddr*)&addr, sizeof(addr));
	if (nResult == SOCKET_ERROR)
	{
		CORE_TRACE("(FrameReceiverGroup::StartListen) bind failed.");
		return false;
	}

	m_strIP = ip;
	m_nPort = nPort;
	m_bQuit = false;

	if (NULL == m_hThread)
	{
		m_hThread = (HANDLE)_beginthread(WorkerProc, 0, this);
	}
	return true;
}

bool FrameReceiverGroup::StopListen()
{
	m_bQuit = true;

	::closesocket(m_sock);
	m_sock = INVALID_SOCKET;

	if (m_hThread)
	{
		wait_thread_exit(m_hThread);
		m_hThread = NULL;
	}

	//Reset();

	return true;
}

bool FrameReceiverGroup::GetListenState()
{
	if (!m_bQuit && m_hThread != NULL)
	{
		return true;
	}

	return false;
}

void __cdecl FrameReceiverGroup::WorkerProc(void* lpParameter)
{
	FrameReceiverGroup* pthis = (FrameReceiverGroup*)lpParameter;

	fd_set fdread;
	timeval val = {60,0};
	int len = sizeof(sockaddr_in);
	sockaddr_in server;
	int sock = pthis->m_sock;

	char buffer[2048];

	while (!pthis->m_bQuit)
	{
		FD_ZERO(&fdread);
		FD_SET(pthis->m_sock, &fdread);

		// 查看socket是否可读
		int Result = select(0, &fdread, 0, 0, &val);
		if(Result == SOCKET_ERROR)
		{
			// 连接错误
			break;
		}

		if (sock != pthis->m_sock)
		{
			// 已经切换了连接
			break;
		}

		if(Result == 0)
		{
			// 没有可读的套接字,重新判断
			continue;
		}

		int rec_size = recvfrom(sock, buffer, 2048, 0, (sockaddr*)&server, &len);
		if(rec_size > 0)
		{
			pthis->ProcessFrameData(buffer, rec_size);
		}
	}
}

void FrameReceiverGroup::ProcessFrameData(char* pBuffer, int nBufferSize)
{
	// 根据帧数据头上的标识把帧数据交给对应接收者处理
	char flag[5];
	memcpy(flag, pBuffer, 4);
	flag[4] = '\0';

	RECEIVER_TYPE receiver_type = RECEIVER_TYPE_INVALID;
	if (strcmp(flag, FRAME_FLAG_MOTION_CAPTURE) == 0)
	{
		receiver_type = RECEIVER_TYPE_MOTION_CAPTURE;
	}
	else if (strcmp(flag, FRAME_FLAG_CAMERA) == 0)
	{
		receiver_type = RECEIVER_TYPE_CAMERA;
	}
	else if (strcmp(flag, FRAME_FLAG_BONE) == 0)
	{
		receiver_type = RECEIVER_TYPE_BONE;
	}
	else
	{
		// 未识别的帧
		return;
	}

	Assert(receiver_type != RECEIVER_TYPE_INVALID);

	PERSISTID receiver_id = m_Receivers[receiver_type];

	// 没监控该类型的帧数据
	if (receiver_id.IsNull())
	{
		return;
	}

	IFrameReceiver* pReceiver = GetFrameReceiver(receiver_id);
	// 无效的id
	if (NULL == pReceiver)
	{
		Assert(0);
		CORE_TRACE("(FrameReceiverGroup::ProcessFrameData) Invalid Receiver.");
		m_Receivers[receiver_type] = PERSISTID();
		return;
	}

	// 接收到的数据尺寸应该小于接收者的最大帧尺寸
	// 如果接收到的数据大于接收者的最大帧尺寸，则表示接收者不具备处理这种帧的功能
	if ((int)pReceiver->GetFrameSize() < nBufferSize)
	{
		Assert(0);
		CORE_TRACE("(FrameReceiverGroup::ProcessFrameData) The size of frame is too big.");
		return;
	}

	char* pFrameBuffer = pReceiver->PopFromFreeList();

	// 内存不足
	if (NULL == pFrameBuffer)
	{
		Assert(0);
		CORE_TRACE("(FrameReceiverGroup::ProcessFrameData) No memory.");
		return;
	}

	memcpy(pFrameBuffer, pBuffer, nBufferSize);
	pReceiver->PushToWaitList(pFrameBuffer);
}

IFrameReceiver* FrameReceiverGroup::GetFrameReceiver(const PERSISTID& id)
{
	IEntity* pEntity = GetCore()->GetEntity(id);
	if ((NULL == pEntity) || !pEntity->GetEntInfo()->IsKindOf("IFrameReceiver"))
	{
		return NULL;
	}
	return (IFrameReceiver*)pEntity;
}

void FrameReceiverGroup::SetCameraFrameReceiver(const PERSISTID& id)
{
	m_Receivers[RECEIVER_TYPE_CAMERA] = id;
}

PERSISTID FrameReceiverGroup::GetCameraFrameReceiver() const
{
	return m_Receivers[RECEIVER_TYPE_CAMERA];
}

void FrameReceiverGroup::SetMotionCaptureFrameReceiver(const PERSISTID& id)
{
	m_Receivers[RECEIVER_TYPE_MOTION_CAPTURE] = id;
}

PERSISTID FrameReceiverGroup::GetMotionCaptureFrameReceiver() const
{
	return m_Receivers[RECEIVER_TYPE_MOTION_CAPTURE];
}

void FrameReceiverGroup::SetBoneFrameReceiver(const PERSISTID& id)
{
	m_Receivers[RECEIVER_TYPE_BONE] = id;
}

PERSISTID FrameReceiverGroup::GetBoneFrameReceiver() const
{
	return m_Receivers[RECEIVER_TYPE_BONE];
}

void FrameReceiverGroup::Update(float seconds)
{
	for (size_t i = 0; i < RECEIVER_TYPE_MAX; ++i)
	{
		IFrameReceiver* pFrameReceiver = GetFrameReceiver(m_Receivers[i]);
		if (pFrameReceiver)
		{
			pFrameReceiver->Update();
		}
	}
}

void FrameReceiverGroup::Reset()
{
	for (size_t i = 0; i < RECEIVER_TYPE_MAX; ++i)
	{
		IFrameReceiver* pFrameReceiver = GetFrameReceiver(m_Receivers[i]);
		if (pFrameReceiver)
		{
			pFrameReceiver->ClearWaitList();
			pFrameReceiver->Reset();
		}
	}
}

int FrameReceiverGroup::VectorRotation(const IVarList& args, IVarList& result) const
{

	if (args.GetCount() < 6)
	{
		return 0;
	}

	FmVec3 vec(args.FloatVal(0), args.FloatVal(1), args.FloatVal(2));
	FmMat4 mat;

	FmMat4RotationYawPitchRoll(&mat, args.FloatVal(4), args.FloatVal(3), args.FloatVal(5));
	FmVec3TransformCoord(&vec, &vec, &mat);

	result.Clear();
	result<<vec.x<<vec.y<<vec.z;

	return result.GetCount();
}

const char* FrameReceiverGroup::GetHostIp()
{

	static  char ip[20] = "";
	if (strlen(ip) > 0)
	{
		return ip;
	}

	hostent* pHost = NULL;
	char	 host_name[128] = "";
	in_addr  in;
	if (gethostname(host_name,sizeof(host_name)) == SOCKET_ERROR)
	{
		return "";
	}

	pHost = gethostbyname(host_name);
	if ( pHost == NULL)
	{
		return "";
	}

	in.s_addr = *(unsigned long*)pHost->h_addr_list[0];
	if (in.s_addr == NULL)
	{
		return "";
	}

	strcpy_s(ip, sizeof(ip), inet_ntoa(in));

	return ip;
}
