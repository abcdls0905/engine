//--------------------------------------------------------------------
// �ļ���:		sock_buf.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��5��24��	
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _SOCK_BUF_H
#define _SOCK_BUF_H

#include "../public/macros.h"

// ��Ϣ����

class CSockBuf  
{
public:
	enum { DEF_MESSAGEDATA_NUM = 2048 };
	enum { MAX_MESSAGEDATA_NUM = 0x10000 }; 
	enum { DEF_MESSAGEBUF_LEN = 0x40000 };
	enum { MAX_MESSAGEBUF_LEN = 0x1000000 };
	
	struct msg_data_t
	{
		int nSrc;
		char* pData;
		size_t nLen;
	};

public:
	explicit CSockBuf(size_t data_num = DEF_MESSAGEDATA_NUM, 
		size_t buf_len = DEF_MESSAGEBUF_LEN);
	~CSockBuf();

	// ��ǰ�����ɵ���Ϣ����
	size_t GetDataNum() const;
	// ��ǰ����Ϣ��������С
	size_t GetBufLen() const;
	// �����Ϣ����
	void SetMaxDataNum(size_t value);
	size_t GetMaxDataNum() const;
	// �����Ϣ�����С
	void SetMaxBufLen(size_t value);
	size_t GetMaxBufLen() const;
	// �Ƿ���
	bool IsFull() const;
	// �Ƿ�Ϊ��
	bool IsEmpty() const
	{
		// �Ż�
		return (m_nGet == m_nPut);
	}
	// ��Ϣ����
	size_t GetCount() const;
	// �ܵ���Ϣ��С
	size_t GetMsgSize() const;
	// ���
	void Clear();
	// ������Ϣ
	bool Put(int src,const void* pdata, size_t len);
	bool Put2(int src,const void* pdata1, size_t len1, 
		const void* pdata2, size_t len2);
	bool Put3(int src,const void* pdata1, size_t len1, 
		const void* pdata2, size_t len2,
		const void* pdata3, size_t len3);
	// ��ȡ��Ϣ
	bool Peek(int &src,void*& pdata, size_t& len);
	// ɾ����ǰ��Ϣ
	void Erase();
	// ������Ϣ����������Ϊ��ʱ��
	bool ShrinkDataNum(size_t size);
	// ����������������Ϊ��ʱ��
	bool ShrinkBufLen(size_t size);

private:
	CSockBuf(const CSockBuf&);
	CSockBuf& operator=(const CSockBuf&);

	// ������Ϣ������
	bool Require(size_t len, size_t& in, size_t OUTPTR);
	// ������Ϣ����
	bool AddDataNum();
	// ������Ϣ������
	bool AddBufLen();

private:
	msg_data_t* m_pData;
	size_t m_nDataNum;
	size_t m_nMaxDataNum;
	char* m_pBuffer;
	size_t m_nBufLen;
	size_t m_nMaxBufLen;
	size_t m_nIn;
	size_t m_nOut;
	size_t m_nGet;
	size_t m_nPut;
	size_t m_nCount;
	size_t m_nMsgSize;
};

#endif // _SOCK_BUF_H

