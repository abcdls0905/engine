//--------------------------------------------------------------------
// �ļ���:		load_archive.h
// ��  ��:		
// ˵  ��:		
// ����޸�:	2008��9��19��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _UTILS_LOAD_ARCHIVE_H
#define _UTILS_LOAD_ARCHIVE_H

#include "../public/macros.h"
#include "../public/persistid.h"
#include "../public/except_msg.h"
#include <string.h>

// ���л���ȡ

class CLoadArchive
{
	template <class T> inline void ReadValue(T& value, void* data)
	{
		memcpy(&value, data, sizeof(T));
	}
public:
	CLoadArchive(const void* pdata, size_t len)
	{
		Assert(pdata != NULL);
	
		m_pData = (char*)pdata;
		m_nMaxLen = len;
		m_nPosi = 0;
	}
	
	~CLoadArchive()
	{
	}

	// �ƶ�ָ��
	void Seek(size_t len)
	{
		Assert(len <= m_nMaxLen);

		m_nPosi = len;
	}

	// �������ָ��
	const void* GetData() const
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

	// ��ȡ����
	void Read(void* buf, size_t len)
	{
		Assert(buf != NULL);
		
		if ((m_nPosi + len) > m_nMaxLen)
		{
			throw CExceptMsg("Out range");
		}
		
		memcpy(buf, m_pData + m_nPosi, len);
		m_nPosi += len;
	}

	// ��ȡ����
	bool ReadBool()
	{
		bool data;
		
		if ((m_nPosi + sizeof(data)) > m_nMaxLen)
		{
			throw CExceptMsg("Out range");
		}
		
		//data = *(bool*)(m_pData + m_nPosi);
		
		m_nPosi += sizeof(data);
		
		return data;
	}
	
	unsigned char ReadByte()
	{
		unsigned char data;
		
		if ((m_nPosi + sizeof(data)) > m_nMaxLen)
		{
			throw CExceptMsg("Out range");
		}
		
		ReadValue(data, m_pData + m_nPosi);
		//data = *(unsigned char*)(m_pData + m_nPosi);
		m_nPosi += sizeof(data);
		
		return data;
	}
	
	unsigned short ReadWord()
	{
		unsigned short data;
		
		if ((m_nPosi + sizeof(data)) > m_nMaxLen)
		{
			throw CExceptMsg("Out range");
		}
		
		ReadValue(data, m_pData + m_nPosi);
		//data = *(unsigned short*)(m_pData + m_nPosi);
		m_nPosi += sizeof(data);
		
		return data;
	}
	
	int ReadInt()
	{
		int data;
		
		if ((m_nPosi + sizeof(data)) > m_nMaxLen)
		{
			throw CExceptMsg("Out range");
		}
		
		ReadValue(data, m_pData + m_nPosi);
		//data = *(int*)(m_pData + m_nPosi);
		m_nPosi += sizeof(data);
		
		return data;
	}
	
	int64_t ReadInt64()
	{
		int64_t data;
		
		if ((m_nPosi + sizeof(data)) > m_nMaxLen)
		{
			throw CExceptMsg("Out range");
		}
		
		ReadValue(data, m_pData + m_nPosi);
		//data = *(int64_t*)(m_pData + m_nPosi);
		m_nPosi += sizeof(data);
		
		return data;
	}
	
	float ReadFloat()
	{
		float data;
		
		if ((m_nPosi + sizeof(data)) > m_nMaxLen)
		{
			throw CExceptMsg("Out range");
		}
		
		ReadValue(data, m_pData + m_nPosi);
		//data = *(float*)(m_pData + m_nPosi);
		m_nPosi += sizeof(data);
		
		return data;
	}
	
	double ReadDouble()
	{
		double data;
		
		if ((m_nPosi + sizeof(data)) > m_nMaxLen)
		{
			throw CExceptMsg("Out range");
		}
		
		ReadValue(data, m_pData + m_nPosi);
		//data = *(double*)(m_pData + m_nPosi);
		m_nPosi += sizeof(data);
		
		return data;
	}
	
	const char* ReadString()
	{
		unsigned int len;
		
		if ((m_nPosi + sizeof(len)) > m_nMaxLen)
		{
			throw CExceptMsg("Out range");
		}
		
		ReadValue(len, m_pData + m_nPosi);
		//len = *(unsigned int*)(m_pData + m_nPosi);
		m_nPosi += sizeof(len);
		
		const char* p = m_pData + m_nPosi;
		
		if ((m_nPosi + len) > m_nMaxLen)
		{
			throw CExceptMsg("Out range");
		}
		
		m_nPosi += len;
		
		if (p[len - 1] != 0)
		{
			throw CExceptMsg("String no end symbol");
		}
		
		return p;
	}
	
	inline const unsigned short* ReadUtf16()
	{
		unsigned int len;
		
		if ((m_nPosi + sizeof(len)) > m_nMaxLen)
		{
			throw CExceptMsg("Out range");
		}
		
		ReadValue(len, m_pData + m_nPosi);
		//len = *(unsigned int*)(m_pData + m_nPosi);
		m_nPosi += sizeof(len);
		
		const unsigned short* p = (const unsigned short*)(m_pData + m_nPosi);
		
		if ((m_nPosi + len) > m_nMaxLen)
		{
			throw CExceptMsg("Out range");
		}
		
		m_nPosi += len;
		
		if (p[len / sizeof(unsigned short) - 1] != 0)
		{
			throw CExceptMsg("Wide string no end symbol");
		}
		
		return p;
	}

	const wchar_t* ReadWideStr()
	{
		if(sizeof(wchar_t) == 2)
		{
			return (const wchar_t*)ReadUtf16();
		}
		else
		{	
			//��Ϊ���ݳ�����16λ�ģ����������ַ����ֽڲ��ᳬ��65535
			//��ʱʹ�����ַ�������Ϊ�Ķ���С
			static wchar_t buffer[65536];
			const unsigned short* p = ReadUtf16();
			int i = 0;
			for(i = 0; p[i]!=0; i++)
			{
				buffer[i] = p[i];
			}
			buffer[i] = 0;
			return buffer;
		}
		
	}
	
	const unsigned short* ReadUCS2()
	{
		unsigned int len;
		
		if ((m_nPosi + sizeof(len)) > m_nMaxLen)
		{
			throw CExceptMsg("Out range");
		}
		
		len = *(unsigned int*)(m_pData + m_nPosi);
		m_nPosi += sizeof(len);
		
		const unsigned short* p = (const unsigned short*)(m_pData + m_nPosi);
		
		if ((m_nPosi + len) > m_nMaxLen)
		{
			throw CExceptMsg("Out range");
		}
		
		m_nPosi += len;
		
		if (p[len / sizeof(unsigned short) - 1] != 0)
		{
			throw CExceptMsg("UCS2 string no end symbol");
		}
		
		return p;
	}
	
	PERSISTID ReadObject()
	{
		PERSISTID data;
		
		if ((m_nPosi + sizeof(data)) > m_nMaxLen)
		{
			throw CExceptMsg("Out range");
		}
		
		ReadValue(data, m_pData + m_nPosi);
		//data = *(PERSISTID*)(m_pData + m_nPosi);
		m_nPosi += sizeof(data);
		
		return data;
	}
	
	void* ReadBinary(size_t& size)
	{
		unsigned int len;
		
		if ((m_nPosi + sizeof(len)) > m_nMaxLen)
		{
			throw CExceptMsg("Out range");
		}
		
		ReadValue(len, m_pData + m_nPosi);
	//	len = *(unsigned int*)(m_pData + m_nPosi);
		m_nPosi += sizeof(len);
		
		void* p = m_pData + m_nPosi;
		
		if ((m_nPosi + len) > m_nMaxLen)
		{
			throw CExceptMsg("Out range");
		}
		
		m_nPosi += len;
		size = len;
		
		return p;
	}
	
protected:
	CLoadArchive();
	CLoadArchive(const CLoadArchive&);
	CLoadArchive& operator=(const CLoadArchive&);

private:
	char* m_pData;
	size_t m_nMaxLen;
	size_t m_nPosi;
};

#endif // _UTILS_LOAD_ARCHIVE_H
