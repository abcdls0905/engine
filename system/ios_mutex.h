//--------------------------------------------------------------------
// �ļ���:		ios_mutex.h
// ��  ��:		
// ˵  ��:		
// ��������:	2010��12��15��	
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _SYSTEM_IOS_MUTEX_H
#define _SYSTEM_IOS_MUTEX_H

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

// ���̻�����

class CMutex
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
	
	// �Ƿ����ָ�����ֵĻ�����
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
	
		if (semget(key, 0, 0) == -1)
		{
			return false;
		}
	
		return true;
	}
	
	// ��֤Ψһ
	static bool Exclusive(const char* name)
	{
		// ��ֹ�ظ������ļ�
		if (!ConfirmFile(name))
		{
			return false;
		}
		
		// �򿪻����ļ�
		int fd = open(name, O_RDWR, 0666);
		
		if (fd < 0)
		{
			return false;
		}
		
		// ����ļ���״̬
		struct flock fl;
		
		memset(&fl, 0, sizeof(fl));
		fl.l_type = F_WRLCK;
		fl.l_whence = SEEK_SET;
		fl.l_start = 0;
		fl.l_len = 0;
		
		int res = fcntl(fd, F_GETLK, &fl);
		
		if (res < 0)
		{
			close(fd);
			return false;
		}
		
		if (fl.l_type != F_UNLCK)
		{
			// �ѱ�������������
			close(fd);
			return false;
		}
		
		// �����ļ�
		memset(&fl, 0, sizeof(fl));
		fl.l_type = F_WRLCK;
		fl.l_whence = SEEK_SET;
		fl.l_start = 0;
		fl.l_len = 0;
		
		res = fcntl(fd, F_SETLK, &fl);
		
		if (res < 0)
		{
			close(fd);
			return false;
		}
		
		// Ϊ�˱�֤�������ɹرմ��ļ�
		return true;
	}

public:
	CMutex()
	{
		m_pName = NULL;
		m_sem_id = -1;
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
		return -1 != m_sem_id;
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

		// �����ļ�
		if (!ConfirmFile(name))
		{
			return false;
		}
	
		key_t key = ftok(m_pName, 0);
	
		if (-1 == key)
		{
			return false;
		}
	
		m_sem_id = semget(key, 1, IPC_CREAT | IPC_EXCL | 00666);
		
		if (-1 == m_sem_id)
		{
			if (errno == EEXIST)
			{
				// �Ѵ���
				if (exists)
				{
					*exists = true;
				}
	
				m_sem_id = semget(key, 1, IPC_CREAT | 00666);
				
				if (-1 == m_sem_id)
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			// �½��������ź����ĳ�ʼֵ
			union semu { int val; };
	
			semu arg;
			
			arg.val = 1;
	
			if (semctl(m_sem_id, 0, SETVAL, arg) == -1)
			{
				// ʧ��
				semctl(m_sem_id, 0, IPC_RMID, NULL);
				m_sem_id = -1;
				return false;
			}
		}
		
		return true;
	}

	// ɾ����
	bool Destroy()
	{
		if (m_sem_id != -1)
		{
			semctl(m_sem_id, 0, IPC_RMID, NULL);
			m_sem_id = -1;
		}
		
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
		struct sembuf lock;
		
		lock.sem_num = 0;
		lock.sem_op = -1;
		lock.sem_flg = SEM_UNDO;
	
		semop(m_sem_id, &lock, 1);
	}

	// ����
	void Unlock()
	{
		struct sembuf unlock;
		
		unlock.sem_num = 0;
		unlock.sem_op = 1;
		unlock.sem_flg = SEM_UNDO;
	
		semop(m_sem_id, &unlock, 1);
	}

private:
	CMutex(const CMutex&);
	CMutex& operator=(const CMutex&);

private:
	char* m_pName;
	int m_sem_id;
};

#endif // _SYSTEM_IOS_MUTEX_H
