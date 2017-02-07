//--------------------------------------------------------------------
// �ļ���:		win_sharemem.h
// ��  ��:		
// ˵  ��:		
// ��������:	2006��8��23��	
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _SYSTEM_WIN_SHAREMEM_H
#define _SYSTEM_WIN_SHAREMEM_H

#include <windows.h>
#include <string.h>

// ���̼�Ĺ����ڴ�

class CShareMem
{
public:
	// �Ƿ����ָ�����ֵĹ����ڴ�
	static bool Exists(const char* name)
	{
		HANDLE hFile = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, name);

		if (INVALID_HANDLE_VALUE == hFile)
		{
			return false;
		}

		CloseHandle(hFile);

		return true;
	}

public:
	CShareMem()
	{
		m_pName = NULL;
		m_nSize = 0;
		m_hFile = INVALID_HANDLE_VALUE;
		m_pMem = NULL;
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
		return (m_hFile != INVALID_HANDLE_VALUE) && (m_pMem != NULL);
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

		m_hFile = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, name);

		if (NULL == m_hFile)
		{
			m_hFile = CreateFileMappingA(INVALID_HANDLE_VALUE,
				NULL, PAGE_READWRITE, 0, DWORD(m_nSize), name);

			if (INVALID_HANDLE_VALUE == m_hFile)
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

		m_pMem = MapViewOfFile(m_hFile, FILE_MAP_ALL_ACCESS, 0, 0, m_nSize);

		if (NULL == m_pMem)
		{
			CloseHandle(m_hFile);
			m_hFile = INVALID_HANDLE_VALUE;
			return false;
		}
		
		return true;
	}

	// ɾ�������ڴ�
	bool Destroy()
	{
		if (m_pMem != NULL)
		{
			UnmapViewOfFile(m_pMem);
			m_pMem = NULL;
		}

		if (m_hFile != INVALID_HANDLE_VALUE)
		{
			if (!CloseHandle(m_hFile))
			{
				return false;
			}
	
			m_hFile = INVALID_HANDLE_VALUE;
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
	HANDLE m_hFile;
	void* m_pMem;
};

#endif // _SYSTEM_WIN_SHAREMEM_H

