//--------------------------------------------------------------------
// �ļ���:		block_region.h
// ��  ��:		
// ˵  ��:		
// ��������:	2009��5��23��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _BLOCK_REGION_H
#define _BLOCK_REGION_H

#include "i_block_reader.h"
#include "i_block_writer.h"
#include "region_loader.h"

// ��ȡ��Χ�����Ϣ 

class CBlockRegionReader: public IBlockReader
{
public:
	static IBlockReader* NewInstance(CRegionLoader::region_all_t* p, 
		size_t rows, size_t cols);

public:
	CBlockRegionReader(CRegionLoader::region_all_t* p, size_t rows, 
		size_t cols);
	virtual ~CBlockRegionReader();

	// �ͷ�
	virtual void Release();
	// ��ߴ�
	virtual int GetSize() const;
	// ��ȡ
	virtual int Read(void* handle);

private:
	CBlockRegionReader();

private:
	CRegionLoader::region_all_t* m_pData;
	size_t m_nRows;
	size_t m_nCols;
};

// д�뷶Χ�����Ϣ 

class CTerrainZone;

class CBlockRegionWriter: public IBlockWriter
{
public:
	CBlockRegionWriter(CTerrainZone* pZone);
	virtual ~CBlockRegionWriter();

	// ��ߴ�
	virtual int GetSize() const;
	// ��ȡ
	virtual int Write(void* handle);

private:
	CBlockRegionWriter();

private:
	CTerrainZone* m_pZone;
};

#endif // _BLOCK_REGION_H
