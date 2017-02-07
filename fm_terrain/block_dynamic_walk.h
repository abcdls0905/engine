//--------------------------------------------------------------------
// �ļ���:		BlockDynamicWalk.h
// ��  ��:		
// ˵  ��:		
// ��������:	2013��3��11��
// ������:		����ΰ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _BLOCKDYNAMICWALK_H
#define _BLOCKDYNAMICWALK_H

#include "i_block_reader.h"
#include "i_block_writer.h"
#include "dynamic_walk_loader.h"

// CBlockDynamicWalkReader
// ��ȡ������Ϣ 

class CBlockDynamicWalkReader: public IBlockReader
{
public:
	static IBlockReader* NewInstance(CDynamicWalkLoader::walk_all_t* p, size_t rows, 
		size_t cols);

public:
	CBlockDynamicWalkReader(CDynamicWalkLoader::walk_all_t* p, size_t rows, size_t cols);
	virtual ~CBlockDynamicWalkReader();

	// �ͷ�
	virtual void Release();
	// ��ߴ�
	virtual int GetSize() const;
	// ��ȡ
	virtual int Read(void* handle);

private:
	CBlockDynamicWalkReader();

private:
	CDynamicWalkLoader::walk_all_t* m_pData;
	size_t m_nRows;
	size_t m_nCols;
};

// CBlockWalkWriter
// д��������Ϣ 

class CTerrainZone;

class CBlockDynamicWalkWriter: public IBlockWriter
{
public:
	CBlockDynamicWalkWriter(CDynamicWalkable* pWalk);
	virtual ~CBlockDynamicWalkWriter();

	// ��ߴ�
	virtual int GetSize() const;
	// ��ȡ
	virtual int Write(void* handle);

private:
	CBlockDynamicWalkWriter();

private:
	CDynamicWalkable* m_pWalk;
};

#endif // _BLOCKDYNAMICWALK_H
