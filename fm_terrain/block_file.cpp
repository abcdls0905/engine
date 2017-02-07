//--------------------------------------------------------------------
// 文件名:		block_file.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2008年6月24日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "block_file.h"
#include "i_block_reader.h"
#include "i_block_writer.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/auto_mem.h"
#include "../public/core_file.h"

// CBlockFile

CBlockFile::CBlockFile(const char* signature, BLOCK_READER_CREATOR creator, 
					   void* context)
{
	Assert(signature != NULL);

	m_strSignature = signature;
	m_BlockCreator = creator;
	m_BlockContext = context;
}

CBlockFile::~CBlockFile()
{
}

bool CBlockFile::Load(const char* file_name)
{
	FILE* fp = core_file::fopen(file_name, "rb");
	
	if (NULL == fp)
	{
		return false;
	}
	
	// 文件标识符
	size_t sig_len = m_strSignature.length();
	TAutoMem<char, 16, TCoreAlloc> auto_buf(sig_len + 1);
	char* buf = auto_buf.GetBuffer();
	
	if (core_file::fread(buf, sizeof(char), sig_len, fp) != sig_len)
	{
		core_file::fclose(fp);
		CORE_TRACE("(CBlockFile::Load)read signature failed");
		CORE_TRACE(file_name);
		return false;
	}
	
	buf[sig_len] = 0;
	
	if (m_strSignature != buf)
	{
		core_file::fclose(fp);
		CORE_TRACE("(CBlockFile::Load)signature not match");
		CORE_TRACE(file_name);
		return false;
	}
	
	// 数据块
	while (true)
	{
		// 块长度
		int size;
		
		if (core_file::fread(&size, sizeof(size), 1, fp) != 1)
		{
			break;
		}

		if (size < 0)
		{
			CORE_TRACE("(CBlockFile::Load)block size error");
			CORE_TRACE(file_name);
			break;
		}
		
		// 块标志
		int id;
		
		if (core_file::fread(&id, sizeof(id), 1, fp) != 1)
		{
			CORE_TRACE("(CBlockFile::Load)read block id failed");
			CORE_TRACE(file_name);
			break;
		}
		
		IBlockReader* reader = m_BlockCreator(id, m_BlockContext);
		
		if (reader != NULL)
		{
			int len = reader->Read(fp);

			reader->Release();
			
			if (len != size)
			{
				CORE_TRACE("(CBlockFile::Load)read block size not match");
				CORE_TRACE(file_name);
			}
		}
		else
		{
			// 跳过不可识别的数据块
			core_file::fseek(fp, size, SEEK_CUR);
			
			CORE_TRACE("(CBlockFile::Load)block unknown");
			CORE_TRACE(file_name);
		}
		
		// 现在忽略校验码
		int crc;
		
		if (core_file::fread(&crc, sizeof(crc), 1, fp) != 1)
		{
			CORE_TRACE("(CBlockFile::Load)read block crc failed");
			CORE_TRACE(file_name);
			break;
		}
	}
	
	core_file::fclose(fp);
	
	return true;
}

void* CBlockFile::SaveBegin(const char* file_name)
{
	FILE* fp = core_file::fopen(file_name, "wb");
	
	if (NULL == fp)
	{
		return NULL;
	}
	
	// 写入文件标识符
	size_t len = m_strSignature.length();
	
	if (core_file::fwrite(m_strSignature.c_str(), sizeof(char), len, fp) 
		!= len)
	{
		core_file::fclose(fp);
		return NULL;
	}
	
	return fp;
}

bool CBlockFile::SaveBlock(void* handle, int block_id, IBlockWriter* writer)
{
	Assert(handle != NULL);
	Assert(writer != NULL);
	
	FILE* fp = (FILE*)handle;
	
	// 数据长度
	int size = writer->GetSize();
	
	long pos = core_file::ftell(fp);
	
	if (core_file::fwrite(&size, sizeof(size), 1, fp) != 1)
	{
		core_file::fclose(fp);
		return false;
	}
	
	// 块标志
	if (core_file::fwrite(&block_id, sizeof(block_id), 1, fp) != 1)
	{
		core_file::fclose(fp);
		return false;
	}
	
	int len = writer->Write(fp);

	if (len < 0)
	{
		core_file::fclose(fp);
		return false;
	}
	
	if (len != size)
	{
		// 写入正确的数据长度
		long cur = core_file::ftell(fp);
		
		core_file::fseek(fp, pos, SEEK_SET);
		
		if (core_file::fwrite(&len, sizeof(len), 1, fp) != 1)
		{
			core_file::fclose(fp);
			return false;
		}
		
		core_file::fseek(fp, cur, SEEK_SET);
	}
	
	// 校验码
	int crc = 0;
	
	if (core_file::fwrite(&crc, sizeof(crc), 1, fp) != 1)
	{
		core_file::fclose(fp);
		return false;
	}
	
	return true;
}

bool CBlockFile::SaveEnd(void* handle)
{
	Assert(handle != NULL);
	
	core_file::fclose((FILE*)handle);
	
	return true;
}

