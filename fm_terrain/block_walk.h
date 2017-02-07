//--------------------------------------------------------------------
// �ļ���:		block_walk.h
// ��  ��:		
// ˵  ��:		
// ��������:	2009��1��15��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _BLOCK_WALK_H
#define _BLOCK_WALK_H

#include "i_block_reader.h"
#include "i_block_writer.h"
#include "walk_loader.h"

// ��ȡ������Ϣ 

class CBlockWalkReader: public IBlockReader
{
public:
	static IBlockReader* NewInstance(CWalkLoader::walk_all_t* p, size_t rows, 
		size_t cols);
	
public:
	CBlockWalkReader(CWalkLoader::walk_all_t* p, size_t rows, size_t cols);
	virtual ~CBlockWalkReader();

	// �ͷ�
	virtual void Release();
	// ��ߴ�
	virtual int GetSize() const;
	// ��ȡ
	virtual int Read(void* handle);

private:
	CBlockWalkReader();

private:
	CWalkLoader::walk_all_t* m_pData;
	size_t m_nRows;
	size_t m_nCols;
};

// д��������Ϣ 

class CTerrainZone;

class CBlockWalkWriter: public IBlockWriter
{
public:
	CBlockWalkWriter(CTerrainZone* pZone);
	virtual ~CBlockWalkWriter();

	// ��ߴ�
	virtual int GetSize() const;
	// ��ȡ
	virtual int Write(void* handle);
	
private:
	CBlockWalkWriter();
	
private:
	CTerrainZone* m_pZone;
};

#endif // _BLOCK_WALK_H
