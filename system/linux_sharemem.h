//--------------------------------------------------------------------
// �ļ���:		linux_sharemem.h
// ��  ��:		
// ˵  ��:		
// ��������:	2010��12��15��	
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _SYSTEM_LINUX_SHAREMEM_H
#define _SYSTEM_LINUX_SHAREMEM_H

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#ifdef __ANDROID__
#include <linux/shm.h>
#else
#include <sys/shm.h>
#endif
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

// ���̼�Ĺ����ڴ�


#ifndef __ANDROID__
class CShareMem
{
public:
	// ��֤�ļ�����
	static bool ConfirmFile(const char* name)
	{
		int fd = open(name, O_CREAT | O_EXCL | O_WRONLY, 0666);
		
		if (fd < 0)
		{
			if (errno != EEXIST)
			{
				return false;
			}
		}
		else
		{
			close(fd);
		}
		
		return true;
	}
	
	// �Ƿ����ָ�����ֵĹ����ڴ�
	static bool Exists(const char* name)
	{
		// �����ļ�
		if (!ConfirmFile(name))
		{
			return false;
		}
	
		key_t key = ftok(name, 0);
	
		if (-1 == key)
		{
			return false;
		}
	
		//if (shmget(key, 1, IPC_CREAT | IPC_EXCL) == -1)
		//{
		//	if (errno == EEXIST)
		//	{
		//		return true;
		//	}
		//}
		if (shmget(key, 0, 0) == -1)
		{
			return false;
		}
	
		return true;
	}

public:
	CShareMem()
	{
		m_pName = NULL;
		m_nSize = 0;
		m_pMem = NULL;
		m_shm_id = -1;
	}

	~CShareMem()
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

	// ��ó���
	size_t GetSize() const
	{
		return m_nSize;
	}

	// ����ڴ��ַ
	void* GetMem() const
	{
		return m_pMem;
	}

	// �Ƿ���Ч
	bool IsValid() const
	{
		return (m_shm_id != -1) && (m_pMem != NULL);
	}
	
	// ��û򴴽������ڴ�
	bool Create(const char* name, size_t size, bool* exists = NULL)
	{
		size_t name_size = strlen(name) + 1;
		
		char* pName = NEW char[name_size];
		
		memcpy(pName, name, name_size);
		
		if (m_pName)
		{
			delete[] m_pName;
		}
		
		m_pName = pName;
		m_nSize = size;

		if (exists)
		{
			*exists = false;
		}

		// �����ļ�
		if (!ConfirmFile(name))
		{
			return false;
		}
	
		key_t key = ftok(m_pName, 0);
	
		if (key == -1)
		{
			return false;
		}
	
		m_shm_id = shmget(key, size, IPC_CREAT | IPC_EXCL | 00666);
		
		if (m_shm_id == -1)
		{
			if (errno != EEXIST)
			{
				return false;
			}
	
			if (exists)
			{
				*exists = true;
			}
	
			m_shm_id = shmget(key, size, IPC_CREAT | 00666);
			
			if (m_shm_id == -1)
			{
				return false;
			}
		}
	
		// ӳ�䵽���̿ռ�
		m_pMem = shmat(m_shm_id, NULL, 0);
	
		if ((void*)-1 == m_pMem)
		{
			m_pMem = NULL;
			return false;
		}
		
		return true;
	}

	// ɾ�������ڴ�
	bool Destroy()
	{
		if (m_pMem)
		{
			shmdt(m_pMem);
			m_pMem = NULL;
		}
		
		if (m_shm_id != -1)
		{
			shmctl(m_shm_id, IPC_RMID, NULL);
			m_shm_id = -1;
		}
	
		if (m_pName)
		{
			delete[] m_pName;
			m_pName = NULL;
		}
		
		return true;
	}

private:
	CShareMem(const CShareMem&);
	CShareMem& operator=(const CShareMem&);

private:
	char* m_pName;
	size_t m_nSize;
	void* m_pMem;
	int	m_shm_id;
};
#endif

#endif // _SYSTEM_LINUX_SHAREMEM_H

