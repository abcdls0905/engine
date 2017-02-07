//--------------------------------------------------------------------
// �ļ���:		store_archive.h
// ��  ��:		
// ˵  ��:		
// ����޸�:	2008��9��19��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _UTILS_STORE_ARCHIVE_H
#define _UTILS_STORE_ARCHIVE_H

#include "../public/macros.h"
#include "../public/persistid.h"
#include "../public/except_msg.h"
#include <assert.h>
class TStoreArchiveAlloc
{
public:
	TStoreArchiveAlloc() {}
	~TStoreArchiveAlloc() {}

	// �����ڴ�
	void* Alloc(size_t size) { return NEW char[size]; }
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) { delete[] (char*)ptr; }
	// ����
	void Swap(TStoreArchiveAlloc& src) {}
};

// ���л�����

template<typename ALLOC = TStoreArchiveAlloc>
class TStoreArchive
{
private:
	typedef TStoreArchive<ALLOC> self_type;
	
public:
	TStoreArchive(const void* pdata, size_t len)
	{
		Assert(pdata != NULL);
	
		m_pOriginData = (char*)pdata;
		m_nOriginLen = len;
		m_pData = (char*)pdata;
		m_nMaxLen = len;
		m_nPosi = 0;
	}
	
	~TStoreArchive()
	{
		if (m_pData != m_pOriginData)
		{
			m_Alloc.Free(m_pData, m_nMaxLen);
		}
	}

	// �ƶ�ָ��
	void Seek(size_t len)
	{
		Assert(len <= m_nMaxLen);

		m_nPosi = len;
	}

	// �������ָ��
	void* GetData() const
	{
		return m_pData;
	}
	
	// ���������ݳ���
	size_t GetMaxLen() const
	{
		return m_nMaxLen;
	}
	
	// ��õ�ǰ���ݳ���
	size_t GetLength() const
	{
		return m_nPosi;
	}
	
	// ���ʣ�����ݳ���
	size_t GetRemain() const
	{
		return m_nMaxLen - m_nPosi;
	}

	// Ԥ�����ݿռ�
	void* Forward(size_t len)
	{
		size_t need_size = m_nPosi + len;
		
		if (need_size > m_nMaxLen)
		{
			ExpandBuffer(need_size);
		}
		
		void* p = m_pData + m_nPosi;
		
		m_nPosi += len;
		
		return p;
	}

	// д������
	void Write(const void* buf, size_t len)
	{
		size_t need_size = m_nPosi + len;
		
		if (need_size > m_nMaxLen)
		{
			ExpandBuffer(need_size);
		}
		
		memcpy(m_pData + m_nPosi, buf, len);
		m_nPosi += len;
	}

	// д������
	void WriteBool(bool data)
	{
		size_t need_size = m_nPosi + sizeof(data);
		
		if (need_size > m_nMaxLen)
		{
			ExpandBuffer(need_size);
		}
		memcpy(m_pData + m_nPosi, &data, sizeof(bool));
		//*(bool*)(m_pData + m_nPosi) = data;
		m_nPosi += sizeof(data);
	}
	
	void WriteByte(unsigned char data)
	{
		size_t need_size = m_nPosi + sizeof(data);
		
		if (need_size > m_nMaxLen)
		{
			ExpandBuffer(need_size);
		}
		
		memcpy(m_pData + m_nPosi, &data, sizeof(char));
		//*(unsigned char*)(m_pData + m_nPosi) = data;
		m_nPosi += sizeof(data);
	}
	
	void WriteWord(unsigned short data)
	{
		size_t need_size = m_nPosi + sizeof(data);
		
		if (need_size > m_nMaxLen)
		{
			ExpandBuffer(need_size);
		}
		
		memcpy(m_pData + m_nPosi, &data, sizeof(unsigned short));
		//*(unsigned short*)(m_pData + m_nPosi) = data;
		m_nPosi += sizeof(data);
	}
	
	void WriteInt(int data)
	{
		size_t need_size = m_nPosi + sizeof(data);
		
		if (need_size > m_nMaxLen)
		{
			ExpandBuffer(need_size);
		}
		
		memcpy(m_pData + m_nPosi, &data, sizeof(int));
		//*(int*)(m_pData + m_nPosi) = data;
		m_nPosi += sizeof(data);
	}
	
	void WriteInt64(int64_t data)
	{
		size_t need_size = m_nPosi + sizeof(data);
		
		if (need_size > m_nMaxLen)
		{
			ExpandBuffer(need_size);
		}
		memcpy(m_pData + m_nPosi, &data, sizeof(int64_t));
		//*(int64_t*)(m_pData + m_nPosi) = data;
		m_nPosi += sizeof(data);
	}
	
	void WriteFloat(float data)
	{
		size_t need_size = m_nPosi + sizeof(data);
		
		if (need_size > m_nMaxLen)
		{
			ExpandBuffer(need_size);
		}
		
		memcpy(m_pData + m_nPosi, &data, sizeof(float));
		//*(float*)(m_pData + m_nPosi) = data;
		m_nPosi += sizeof(data);
	}
	
	void WriteDouble(double data)	
	{
		size_t need_size = m_nPosi + sizeof(data);
		
		if (need_size > m_nMaxLen)
		{
			ExpandBuffer(need_size);
		}
		
		memcpy(m_pData + m_nPosi, &data, sizeof(double));
		//*(double*)(m_pData + m_nPosi) = data;
		m_nPosi += sizeof(data);
	}
	
	void WriteString(const char* data)
	{
		unsigned int size = (unsigned int)(strlen(data) + 1);
		size_t need_size = m_nPosi + sizeof(size) + size;
		
		if (need_size > m_nMaxLen)
		{
			ExpandBuffer(need_size);
		}
		
		memcpy(m_pData + m_nPosi, &size, sizeof(unsigned int));
		//*(unsigned int*)(m_pData + m_nPosi) = size;
		m_nPosi += sizeof(size);
		memcpy(m_pData + m_nPosi, data, size);
		m_nPosi += size;
	}
	
	void WriteWideStr(const wchar_t* data)
	{
		int str_len = wcslen(data);
		unsigned int size = (unsigned int)(str_len + 1) * 2;
		size_t need_size = m_nPosi + sizeof(size) + size;
		
		if (need_size > m_nMaxLen)
		{
			ExpandBuffer(need_size);
		}
		
		memcpy(m_pData + m_nPosi, &size, sizeof(unsigned int));
		//*(unsigned int*)(m_pData + m_nPosi) = size;
		m_nPosi += sizeof(size);
		if(sizeof(wchar_t) == 2)
		{
			memcpy(m_pData + m_nPosi, data, size);
		}
		else if(sizeof(wchar_t) == 4)
		{
			unsigned int* src = (unsigned int*)data;
			unsigned short* dst = (unsigned short*)(m_pData + m_nPosi);
			unsigned int count = (unsigned int)(str_len);
			for(size_t i = 0; i < count; i++)
			{
				dst[i] = (unsigned short)src[i];
			}
			dst[count] = 0;
		}
		else
		{
			assert(0);
		}

		m_nPosi += size;
	}
	
	void WriteUCS2(const unsigned short* data)
	{
		const unsigned short* p = data;
		
		while (*p)
		{
			++p;
		}
		
		size_t ucs2_len = p - data;
		unsigned int size = (unsigned int)(ucs2_len + 1) * sizeof(unsigned short);
		size_t need_size = m_nPosi + sizeof(size) + size;
		
		if (need_size > m_nMaxLen)
		{
			ExpandBuffer(need_size);
		}
		
		memcpy(m_pData + m_nPosi, &size, sizeof(unsigned int));
		//*(unsigned int*)(m_pData + m_nPosi) = size;
		m_nPosi += sizeof(size);
		memcpy(m_pData + m_nPosi, data, size);
		m_nPosi += size;
	}
	
	void WriteObject(const PERSISTID& data)
	{
		size_t need_size = m_nPosi + sizeof(data);
		
		if (need_size > m_nMaxLen)
		{
			ExpandBuffer(need_size);
		}
		
		memcpy(m_pData + m_nPosi, &data, sizeof(PERSISTID));
		//*(PERSISTID*)(m_pData + m_nPosi) = data;
		m_nPosi += sizeof(data);
	}
	
	void WriteBinary(const void* pdata, size_t len)
	{
		unsigned int size = (unsigned int)len;
		size_t need_size = m_nPosi + sizeof(size) + size;
		
		if (need_size > m_nMaxLen)
		{
			ExpandBuffer(need_size);
		}
		
		memcpy(m_pData + m_nPosi, &size, sizeof(unsigned int));
		//*(unsigned int*)(m_pData + m_nPosi) = size;
		m_nPosi += sizeof(size);
		memcpy(m_pData + m_nPosi, pdata, size);
		m_nPosi += size;
	}
	
protected:
	TStoreArchive();
	TStoreArchive(const self_type&);
	self_type& operator=(const self_type&);
	
	// ���󻺳���
	void ExpandBuffer(size_t need_size)
	{
		// ���Ʋ��ܳ���16M
		if (m_nMaxLen > 0x1000000)
		{
			throw CExceptMsg("Out range");
		}
		
		// ���󻺳���
		size_t new_size = m_nMaxLen * 2;
		
		if (need_size > new_size)
		{
			new_size = need_size * 2;
		}
		
		char* p = (char*)m_Alloc.Alloc(new_size);

		memcpy(p, m_pData, m_nPosi);

		if (m_pData != m_pOriginData)
		{
			m_Alloc.Free(m_pData, m_nMaxLen);
		}

		m_pData = p;
		m_nMaxLen = new_size;
	}

private:
	ALLOC m_Alloc;
	char* m_pOriginData;
	size_t m_nOriginLen;
	char* m_pData;
	size_t m_nMaxLen;
	size_t m_nPosi;
};

typedef TStoreArchive<TStoreArchiveAlloc> CStoreArchive;

#endif // _UTILS_STORE_ARCHIVE_H
