//--------------------------------------------------------------------
// �ļ���:		block_alpha_map.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��7��3��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _BLOCK_ALPHA_MAP_H
#define _BLOCK_ALPHA_MAP_H

#include "i_block_reader.h"
#include "i_block_writer.h"
#include "alpha_loader.h"

// ��ȡ�����ͼ��Ϣ 

class CBlockAlphaMapReader: public IBlockReader
{
public:
	static IBlockReader* NewInstance(CAlphaLoader::map_all_t* p, 
		size_t chunk_rows, size_t chunk_cols, size_t alpha_map_scale);
	
public:
	CBlockAlphaMapReader(CAlphaLoader::map_all_t* p, size_t chunk_rows, 
		size_t chunk_cols, size_t alpha_map_scale);
	virtual ~CBlockAlphaMapReader();

	// �ͷ�
	virtual void Release();
	// ��ߴ�
	virtual int GetSize() const;
	// ��ȡ
	virtual int Read(void* handle);

private:
	CBlockAlphaMapReader();

private:
	CAlphaLoader::map_all_t* m_pData;
	size_t m_nChunkRows;
	size_t m_nChunkCols;
	size_t m_nAlphaMapScale;
};

// д������ͼ��Ϣ 

class CTerrainZone;

class CBlockAlphaMapWriter: public IBlockWriter
{
public:
	CBlockAlphaMapWriter(CTerrainZone* pZone);
	virtual ~CBlockAlphaMapWriter();

	// ��ߴ�
	virtual int GetSize() const;
	// ��ȡ
	virtual int Write(void* handle);
	
private:
	CBlockAlphaMapWriter();
	
private:
	CTerrainZone* m_pZone;
};

#endif // _BLOCK_ALPHA_MAP_H
