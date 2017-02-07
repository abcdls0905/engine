//--------------------------------------------------------------------
// �ļ���:		IFrameReceiverUser.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2012��10��16��
// ������:		����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------
#include "i_frame_receiver_user.h"
#include "../public/core_log.h"

IFrameReceiverUser::IFrameReceiverUser()
{ 
	m_pFrameReceiver = NULL;
	m_nFrameSize = 1024;
}

IFrameReceiverUser::~IFrameReceiverUser()
{
}

bool IFrameReceiverUser::Init(const IVarList& args)
{
	if (!IVisual::Init(args))
	{
		return false;
	}

	//// ֻ�������һ��ʵ��
	//IEntity* pEntity = GetCore()->LookupEntity("IFrameReceiverUser");
	//if (pEntity != this)
	//{
	//	CORE_TRACE("(IFrameReceiverUser::Init) IFrameReceiverUser already exists");
	//	return false;
	//}

	m_pFrameReceiver = (FrameReceiver*)CORE_ALLOC(sizeof(FrameReceiver));
	new (m_pFrameReceiver) FrameReceiver(m_nFrameSize);
	if (!m_pFrameReceiver->Init())
	{
		CORE_DELETE(m_pFrameReceiver);
		return false;
	}

	return true;
}

bool IFrameReceiverUser::Shut()
{
	CORE_DELETE(m_pFrameReceiver);
	return IVisual::Shut();
}

void IFrameReceiverUser::Update(float seconds)
{
	if (NULL == m_pFrameReceiver)
	{
		return;
	}

	char* pFrameBuffer = m_pFrameReceiver->PopFromWaitList();
	// û�յ�����
	if (NULL == pFrameBuffer)
	{
		return;
	}

	ProcessFrameData(pFrameBuffer);
	m_pFrameReceiver->PushToFreeList(pFrameBuffer);
}

bool IFrameReceiverUser::StartListen(const char* szMyIP, unsigned short nPort)
{
	if (NULL == m_pFrameReceiver)
	{
		return false;
	}
	return m_pFrameReceiver->StartListen(szMyIP, nPort);
}

bool IFrameReceiverUser::StopListen()
{
	if (NULL == m_pFrameReceiver)
	{
		return false;
	}
	return m_pFrameReceiver->StopListen();
}
