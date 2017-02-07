//--------------------------------------------------------------------
// 文件名:		win_mutex.h
// 内  容:		
// 说  明:		
// 创建日期:	2006年8月23日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SYSTEM_WIN_MUTEX_H
#define _SYSTEM_WIN_MUTEX_H

#include <windows.h>
#include <string.h>

// 进程互斥锁

class CMutex
{
public:
	// 是否存在指定名字的互斥锁
	static bool Exists(const char* name)
	{
		HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, name);

		if (NULL == mutex)
		{
			return false;
		}

		CloseHandle(mutex);

		return true;
	}

	// 保证唯一
	static bool Exclusive(const char* name)
	{
		HANDLE hMutex = CreateMutex(NULL, FALSE, name);

		if (NULL == hMutex)
		{
			return false;
		}
		
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			// 已经存在
			return false;
		}
		
		// 为了保证锁定不可关闭互斥体
		return true;
	}

public:
	CMutex()
	{
		m_pName = NULL;
		m_hMutex = NULL;
	}

	~CMutex()
	{
		Destroy();
	}

	// 获得名字
	const char* GetName() const
	{
		if (NULL == m_pName)
		{
			return "";
		}
		
		return m_pName;
	}
	
	// 是否有效
	bool IsValid() const
	{
		return (m_hMutex != NULL);
	}
	
	// 创建或获得锁
	bool Create(const char* name, bool* exists = NULL)
	{
		size_t name_size = strlen(name) + 1;
		char* pName = NEW char[name_size];
		
		memcpy(pName, name, name_size);
		
		if (m_pName)
		{
			delete[] m_pName;
		}
		
		m_pName = pName;
		
		if (exists)
		{
			*exists = false;
		}

		m_hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, name);

		if (NULL == m_hMutex)
		{
			m_hMutex = CreateMutex(NULL, FALSE, name);

			if (NULL == m_hMutex)
			{
				return false;
			}

			if (exists)
			{
				if (GetLastError() == ERROR_ALREADY_EXISTS)
				{
					*exists = true;
				}
			}
		}
		else
		{
			if (exists)
			{
				*exists = true;
			}
		}

		return true;
	}

	// 删除锁
	bool Destroy()
	{
		if (!CloseHandle(m_hMutex))
		{
			return false;
		}

		m_hMutex = NULL;
		
		if (m_pName)
		{
			delete[] m_pName;
			m_pName = NULL;
		}
		
		return true;
	}

	// 加锁
	void Lock()
	{
		WaitForSingleObject(m_hMutex, INFINITE);
	}

	// 解锁
	void Unlock()
	{
		ReleaseMutex(m_hMutex);
	}

private:
	CMutex(const CMutex&);
	CMutex& operator=(const CMutex&);

private:
	char* m_pName;
	HANDLE m_hMutex;
};

#endif // _SYSTEM_WIN_MUTEX_H
