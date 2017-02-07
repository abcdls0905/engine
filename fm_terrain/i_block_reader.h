//--------------------------------------------------------------------
// 文件名:		i_block_reader.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年6月24日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _I_BLOCK_READER_H
#define _I_BLOCK_READER_H

// 块文件读接口

class IBlockReader
{
public:
	virtual ~IBlockReader() = 0;

	// 释放
	virtual void Release() = 0;
	// 块尺寸
	virtual int GetSize() const = 0;
	// 读取
	virtual int Read(void* handle) = 0;
};

inline IBlockReader::~IBlockReader() {}

#endif // _I_BLOCK_READER_H
