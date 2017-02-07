#pragma once

#include "../utils/lock_util.h"

class IDeviceContext
{
public:
	virtual void Init(int width, int height) = 0;

	virtual void StartResourceContext() = 0;

	virtual void EndResourceContext() = 0;

	//�̶߳˵ȴ����̶߳�
	virtual void ResThreadWaitSync(CLockUtil& lock, void*& eglSync) = 0;
	
	//���̵߳ȴ����߳�
	virtual bool MainThreadWaitSync(CLockUtil& lock, void* eglSync) = 0;
};

extern IDeviceContext* g_ResourceContext;