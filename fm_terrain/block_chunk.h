//--------------------------------------------------------------------
// �ļ���:		block_chunk.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��6��25��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _BLOCK_CHUNK_H
#define _BLOCK_CHUNK_H

#include "i_block_reader.h"
#include "i_block_writer.h"
#include "height_loader.h"

// ��ȡ�ֿ���Ϣ 

class CBlockChunkReader: public IBlockReader
{
public:
	static IBlockReader* NewInstance(CHeightLoader::chunk_all_t* p, 
		size_t chunk_rows, size_t chunk_cols, float left, float top);
	
public:
	CBlockChunkReader(CHeightLoader::chunk_all_t* p, size_t chunk_rows,
		size_t chunk_cols, float left, float top);
	virtual ~CBlockChunkReader();

	// �ͷ�
	virtual void Release();
	// ��ߴ�
	virtual int GetSize() const;
	// ��ȡ
	virtual int Read(void* handle);

private:
	CBlockChunkReader();

private:
	CHeightLoader::chunk_all_t* m_pData;
	size_t m_nChunkRows;
	size_t m_nChunkCols;
	float m_fLeft;
	float m_fTop;
};

// д��ֿ���Ϣ 

class CTerrainZone;

class CBlockChunkWriter: public IBlockWriter
{
public:
	CBlockChunkWriter(CTerrainZone* pZone);
	virtual ~CBlockChunkWriter();

	// ��ߴ�
	virtual int GetSize() const;
	// ��ȡ
	virtual int Write(void* handle);
	
private:
	CBlockChunkWriter();
	
private:
	CTerrainZone* m_pZone;
};

// ֱ��д��ֿ���Ϣ 

class CTerrainZone;

class CBlockChunkWriter2: public IBlockWriter
{
public:
	CBlockChunkWriter2(CHeightLoader::chunk_all_t* p, size_t chunk_rows,
		size_t chunk_cols, float left, float top);
	virtual ~CBlockChunkWriter2();

	// ��ߴ�
	virtual int GetSize() const;
	// ��ȡ
	virtual int Write(void* handle);

private:
	CBlockChunkWriter2();

private:
	CHeightLoader::chunk_all_t* m_pData;
	size_t m_nChunkRows;
	size_t m_nChunkCols;
	float m_fLeft;
	float m_fTop;
};

#endif // _BLOCK_CHUNK_H
