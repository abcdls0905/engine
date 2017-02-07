#pragma once

#include "../utils/lock_util.h"

class IDeviceContext
{
public:
	virtual void Init(int width, int height) = 0;

	virtual void StartResourceContext() = 0;

	virtual void EndResourceContext() = 0;

	//线程端等待主线程端
	virtual void ResThreadWaitSync(CLockUtil& lock, void*& eglSync) = 0;
	
	//主线程等待子线程
	virtual bool MainThreadWaitSync(CLockUtil& lock, void* eglSync) = 0;
};

extern IDeviceContext* g_ResourceContext;