//--------------------------------------------------------------------
// �ļ���:		block_grass.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��6��24��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _BLOCK_GRASS_H
#define _BLOCK_GRASS_H

#include "i_block_reader.h"
#include "i_block_writer.h"
#include "visual_loader.h"

// ��ȡ������Ϣ 

class CBlockGrassReader: public IBlockReader
{
public:
	static IBlockReader* NewInstance(CVisualLoader::grass_all_t* p);
	
public:
	CBlockGrassReader(CVisualLoader::grass_all_t* p);
	virtual ~CBlockGrassReader();

	// �ͷ�
	virtual void Release();
	// ��ߴ�
	virtual int GetSize() const;
	// ��ȡ
	virtual int Read(void* handle);

private:
	CBlockGrassReader();

private:
	CVisualLoader::grass_all_t* m_pData;
};

// д�뻨����Ϣ 

class CTerrainZone;

class CBlockGrassWriter: public IBlockWriter
{
public:
	CBlockGrassWriter(CTerrainZone* pZone);
	virtual ~CBlockGrassWriter();

	// ��ߴ�
	virtual int GetSize() const;
	// ��ȡ
	virtual int Write(void* handle);
	
private:
	CBlockGrassWriter();
	
private:
	CTerrainZone* m_pZone;
};

#endif // _BLOCK_GRASS_H

