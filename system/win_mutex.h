//--------------------------------------------------------------------
// �ļ���:		win_mutex.h
// ��  ��:		
// ˵  ��:		
// ��������:	2006��8��23��	
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _SYSTEM_WIN_MUTEX_H
#define _SYSTEM_WIN_MUTEX_H

#include <windows.h>
#include <string.h>

// ���̻�����

class CMutex
{
public:
	// �Ƿ����ָ�����ֵĻ�����
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

	// ��֤Ψһ
	static bool Exclusive(const char* name)
	{
		HANDLE hMutex = CreateMutex(NULL, FALSE, name);

		if (NULL == hMutex)
		{
			return false;
		}
		
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			// �Ѿ�����
			return false;
		}
		
		// Ϊ�˱�֤�������ɹرջ�����
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

	// �������
	const char* GetName() const
	{
		if (NULL == m_pName)
		{
			return "";
		}
		
		return m_pName;
	}
	
	// �Ƿ���Ч
	bool IsValid() const
	{
		return (m_hMutex != NULL);
	}
	
	// ����������
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

	// ɾ����
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

	// ����
	void Lock()
	{
		WaitForSingleObject(m_hMutex, INFINITE);
	}

	// ����
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
