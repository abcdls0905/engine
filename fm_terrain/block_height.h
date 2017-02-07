//--------------------------------------------------------------------
// �ļ���:		block_height.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��6��25��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _BLOCK_HEIGHT_H
#define _BLOCK_HEIGHT_H

#include "i_block_reader.h"
#include "i_block_writer.h"
#include "height_loader.h"

// ��ȡ�߶���Ϣ 

class CBlockHeightReader: public IBlockReader
{
public:
	static IBlockReader* NewInstance(CHeightLoader::height_all_t* p,
		size_t rows, size_t cols);
	
public:
	CBlockHeightReader(CHeightLoader::height_all_t* p, size_t rows, 
		size_t cols);
	virtual ~CBlockHeightReader();

	// �ͷ�
	virtual void Release();
	// ��ߴ�
	virtual int GetSize() const;
	// ��ȡ
	virtual int Read(void* handle);

private:
	CBlockHeightReader();

private:
	CHeightLoader::height_all_t* m_pData;
	size_t m_nRows;
	size_t m_nCols;
};

// д��߶���Ϣ 

class CTerrainZone;

class CBlockHeightWriter: public IBlockWriter
{
public:
	CBlockHeightWriter(CTerrainZone* pZone);
	virtual ~CBlockHeightWriter();

	// ��ߴ�
	virtual int GetSize() const;
	// ��ȡ
	virtual int Write(void* handle);
	
private:
	CBlockHeightWriter();
	
private:
	CTerrainZone* m_pZone;
};

#endif // _BLOCK_HEIGHT_H
