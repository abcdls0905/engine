//--------------------------------------------------------------------
// �ļ���:		block_height2.h
// ��  ��:		
// ˵  ��:		
// ��������:	2009��1��16��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _BLOCK_HEIGHT2_H
#define _BLOCK_HEIGHT2_H

#include "i_block_reader.h"
#include "i_block_writer.h"
#include "height_loader.h"

// ��ȡѹ���߶���Ϣ 

class CBlockHeight2Reader: public IBlockReader
{
public:
	static IBlockReader* NewInstance(CHeightLoader::height_all_t* p,
		size_t rows, size_t cols);
	
public:
	CBlockHeight2Reader(CHeightLoader::height_all_t* p, size_t rows, 
		size_t cols);
	virtual ~CBlockHeight2Reader();

	// �ͷ�
	virtual void Release();
	// ��ߴ�
	virtual int GetSize() const;
	// ��ȡ
	virtual int Read(void* handle);

private:
	CBlockHeight2Reader();

private:
	CHeightLoader::height_all_t* m_pData;
	size_t m_nRows;
	size_t m_nCols;
};

// д��ѹ���߶���Ϣ 

class CTerrainZone;

class CBlockHeight2Writer: public IBlockWriter
{
public:
	CBlockHeight2Writer(CTerrainZone* pZone);
	virtual ~CBlockHeight2Writer();

	// ��ߴ�
	virtual int GetSize() const;
	// ��ȡ
	virtual int Write(void* handle);
	
private:
	CBlockHeight2Writer();
	
private:
	CTerrainZone* m_pZone;
};

// ֱ��д��ѹ���߶���Ϣ 

class CBlockHeight2Writer2: public IBlockWriter
{
public:
	CBlockHeight2Writer2(CHeightLoader::height_all_t* p, size_t rows, 
		size_t cols);
	virtual ~CBlockHeight2Writer2();

	// ��ߴ�
	virtual int GetSize() const;
	// ��ȡ
	virtual int Write(void* handle);

private:
	CBlockHeight2Writer2();

private:
	CHeightLoader::height_all_t* m_pData;
	size_t m_nRows;
	size_t m_nCols;
};

#endif // _BLOCK_HEIGHT2_H
