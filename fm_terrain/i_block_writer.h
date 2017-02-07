//--------------------------------------------------------------------
// 文件名:		i_block_writer.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年6月24日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _I_BLOCK_WRITER_H
#define _I_BLOCK_WRITER_H

// 块文件写接口

class IBlockWriter
{
public:
	virtual ~IBlockWriter() = 0;

	// 块尺寸
	virtual int GetSize() const = 0;
	// 写入
	virtual int Write(void* handle) = 0;
};

inline IBlockWriter::~IBlockWriter() {}

#endif // _I_BLOCK_WRITER_H
