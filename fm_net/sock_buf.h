//--------------------------------------------------------------------
// 文件名:		sock_buf.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年5月24日	
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SOCK_BUF_H
#define _SOCK_BUF_H

#include "../public/macros.h"

// 消息缓冲

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

	// 当前可容纳的消息数量
	size_t GetDataNum() const;
	// 当前的消息缓冲区大小
	size_t GetBufLen() const;
	// 最大消息数量
	void SetMaxDataNum(size_t value);
	size_t GetMaxDataNum() const;
	// 最大消息缓冲大小
	void SetMaxBufLen(size_t value);
	size_t GetMaxBufLen() const;
	// 是否满
	bool IsFull() const;
	// 是否为空
	bool IsEmpty() const
	{
		// 优化
		return (m_nGet == m_nPut);
	}
	// 消息数量
	size_t GetCount() const;
	// 总的消息大小
	size_t GetMsgSize() const;
	// 清空
	void Clear();
	// 放置消息
	bool Put(int src,const void* pdata, size_t len);
	bool Put2(int src,const void* pdata1, size_t len1, 
		const void* pdata2, size_t len2);
	bool Put3(int src,const void* pdata1, size_t len1, 
		const void* pdata2, size_t len2,
		const void* pdata3, size_t len3);
	// 获取消息
	bool Peek(int &src,void*& pdata, size_t& len);
	// 删除当前消息
	void Erase();
	// 收缩消息容量（必须为空时）
	bool ShrinkDataNum(size_t size);
	// 收缩缓冲区（必须为空时）
	bool ShrinkBufLen(size_t size);

private:
	CSockBuf(const CSockBuf&);
	CSockBuf& operator=(const CSockBuf&);

	// 申请消息缓冲区
	bool Require(size_t len, size_t& in, size_t OUTPTR);
	// 扩充消息数量
	bool AddDataNum();
	// 扩充消息缓冲区
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

