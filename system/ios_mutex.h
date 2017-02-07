//--------------------------------------------------------------------
// 文件名:		ios_mutex.h
// 内  容:		
// 说  明:		
// 创建日期:	2010年12月15日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
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

// 进程互斥锁

class CMutex
{
public:
	// 保证文件存在
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
	
	// 是否存在指定名字的互斥锁
	static bool Exists(const char* name)
	{
		// 共享文件
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
	
	// 保证唯一
	static bool Exclusive(const char* name)
	{
		// 防止重复创建文件
		if (!ConfirmFile(name))
		{
			return false;
		}
		
		// 打开互斥文件
		int fd = open(name, O_RDWR, 0666);
		
		if (fd < 0)
		{
			return false;
		}
		
		// 获得文件锁状态
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
			// 已被其他进程锁定
			close(fd);
			return false;
		}
		
		// 锁定文件
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
		
		// 为了保证锁定不可关闭此文件
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
		return -1 != m_sem_id;
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

		// 共享文件
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
				// 已存在
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
			// 新建，设置信号量的初始值
			union semu { int val; };
	
			semu arg;
			
			arg.val = 1;
	
			if (semctl(m_sem_id, 0, SETVAL, arg) == -1)
			{
				// 失败
				semctl(m_sem_id, 0, IPC_RMID, NULL);
				m_sem_id = -1;
				return false;
			}
		}
		
		return true;
	}

	// 删除锁
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

	// 加锁
	void Lock()
	{
		struct sembuf lock;
		
		lock.sem_num = 0;
		lock.sem_op = -1;
		lock.sem_flg = SEM_UNDO;
	
		semop(m_sem_id, &lock, 1);
	}

	// 解锁
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
