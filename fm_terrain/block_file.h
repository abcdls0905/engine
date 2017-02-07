//--------------------------------------------------------------------
// �ļ���:		block_file.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��6��24��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _BLOCK_FILE_H
#define _BLOCK_FILE_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../public/fast_str.h"
#include "../public/core_type.h"

// ���ļ���ʽ

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

	// �����ļ�
	bool Load(const char* file_name);
	// ��ʼ�����ļ�
	void* SaveBegin(const char* file_name);
	// �����
	bool SaveBlock(void* handle, int chunk_id, IBlockWriter* writer);
	// ���������ļ�
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
