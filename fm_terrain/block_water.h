//--------------------------------------------------------------------
// �ļ���:		block_water.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��6��25��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _BLOCK_WATER_H
#define _BLOCK_WATER_H

#include "i_block_reader.h"
#include "i_block_writer.h"
#include "height_loader.h"

// ��ȡˮ����Ϣ 

class CBlockWaterReader: public IBlockReader
{
public:
	static IBlockReader* NewInstance(CHeightLoader::water_all_t* p, 
		size_t chunk_rows, size_t chunk_cols);
	
public:
	CBlockWaterReader(CHeightLoader::water_all_t* p, size_t chunk_rows, 
		size_t chunk_cols);
	virtual ~CBlockWaterReader();

	// �ͷ�
	virtual void Release();
	// ��ߴ�
	virtual int GetSize() const;
	// ��ȡ
	virtual int Read(void* handle);

private:
	CBlockWaterReader();

private:
	CHeightLoader::water_all_t*	m_pData;
	size_t m_nChunkRows;
	size_t m_nChunkCols;
};

// д��ˮ����Ϣ 

class CTerrainZone;

class CBlockWaterWriter: public IBlockWriter
{
public:
	CBlockWaterWriter(CTerrainZone* pZone);
	virtual ~CBlockWaterWriter();

	// ��ߴ�
	virtual int GetSize() const;
	// ��ȡ
	virtual int Write(void* handle);
	
private:
	CBlockWaterWriter();
	
private:
	CTerrainZone* m_pZone;
};

#endif // _BLOCK_WATER_H
