//--------------------------------------------------------------------
// �ļ���:		block_base.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��6��25��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _BLOCK_BASE_H
#define _BLOCK_BASE_H

#include "i_block_reader.h"
#include "i_block_writer.h"
#include "alpha_loader.h"

// ��ȡ������ͼ��Ϣ 

class CBlockBaseReader: public IBlockReader
{
public:
	static IBlockReader* NewInstance(CAlphaLoader::base_all_t* p, 
		size_t chunk_rows, size_t chunk_cols);
	
public:
	CBlockBaseReader(CAlphaLoader::base_all_t* p, size_t chunk_rows, 
		size_t chunk_cols);
	virtual ~CBlockBaseReader();

	// �ͷ�
	virtual void Release();
	// ��ߴ�
	virtual int GetSize() const;
	// ��ȡ
	virtual int Read(void* handle);

private:
	CBlockBaseReader();

private:
	CAlphaLoader::base_all_t* m_pData;
	size_t m_nChunkRows;
	size_t m_nChunkCols;
};

// д�������ͼ��Ϣ 

class CTerrainZone;

class CBlockBaseWriter: public IBlockWriter
{
public:
	CBlockBaseWriter(CTerrainZone* pZone);
	virtual ~CBlockBaseWriter();

	// ��ߴ�
	virtual int GetSize() const;
	// ��ȡ
	virtual int Write(void* handle);
	
private:
	CBlockBaseWriter();
	
private:
	CTerrainZone* m_pZone;
};

#endif // _BLOCK_BASE_H

