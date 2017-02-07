//--------------------------------------------------------------------
// �ļ���:		block_area.h
// ��  ��:		
// ˵  ��:		
// ��������:	2009��6��2��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _BLOCK_AREA_H
#define _BLOCK_AREA_H

#include "i_block_reader.h"
#include "i_block_writer.h"
#include "region_loader.h"

// ��ȡ���������Ϣ 

class CBlockAreaReader: public IBlockReader
{
public:
	static IBlockReader* NewInstance(CRegionLoader::area_all_t* p, 
		size_t rows, size_t cols);
	
public:
	CBlockAreaReader(CRegionLoader::area_all_t* p, size_t rows, 
		size_t cols);
	virtual ~CBlockAreaReader();

	// �ͷ�
	virtual void Release();
	// ��ߴ�
	virtual int GetSize() const;
	// ��ȡ
	virtual int Read(void* handle);

private:
	CBlockAreaReader();

private:
	CRegionLoader::area_all_t* m_pData;
	size_t m_nRows;
	size_t m_nCols;
};

// д����������Ϣ 

class CTerrainZone;

class CBlockAreaWriter: public IBlockWriter
{
public:
	CBlockAreaWriter(CTerrainZone* pZone);
	virtual ~CBlockAreaWriter();

	// ��ߴ�
	virtual int GetSize() const;
	// ��ȡ
	virtual int Write(void* handle);
	
private:
	CBlockAreaWriter();
	
private:
	CTerrainZone* m_pZone;
};

#endif // _BLOCK_AREA_H

