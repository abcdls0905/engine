//--------------------------------------------------------------------
// 文件名:		block_file.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年6月24日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _BLOCK_FILE_H
#define _BLOCK_FILE_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../public/fast_str.h"
#include "../public/core_type.h"

// 块文件格式

class IBlockReader;
class IBlockWriter;

typedef IBlockReader* (__cdecl* BLOCK_READER_CREATOR)(int block_id, 
	void* context);

class CBlockFile
{
public:
	CBlockFile(const char* signature, BLOCK_READER_CREATOR creator, 
		void* context);
	~CBlockFile();

	// 加载文件
	bool Load(const char* file_name);
	// 开始保存文件
	void* SaveBegin(const char* file_name);
	// 保存块
	bool SaveBlock(void* handle, int chunk_id, IBlockWriter* writer);
	// 结束保存文件
	bool SaveEnd(void* handle);

private:
	CBlockFile();
	CBlockFile(const CBlockFile&);
	CBlockFile& operator=(const CBlockFile&);

private:
	BLOCK_READER_CREATOR m_BlockCreator;
	void* m_BlockContext;
	core_string m_strSignature;
};

#endif // _BLOCK_FILE_H
