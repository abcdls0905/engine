//--------------------------------------------------------------------
// �ļ���:		height_loader.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��8��15��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _HEIGHT_LOADER_H
#define _HEIGHT_LOADER_H

#include "../public/macros.h"
#include "../public/core_type.h"
#include "../visual/fm_header.h"
#include "../visual/i_res_loader.h"
#include "name_list.h"

#define BLOCK_ID_HEIGHT_0 '0TGH'
#define BLOCK_ID_HEIGHT_2 '2TGH'
#define BLOCK_ID_CHUNK_0 '0KHC'
#define BLOCK_ID_WATER_0 '0RTW'

// ����߶ȼ�����

class CTerrainZone;
class IBlockReader;

class CHeightLoader: public IResLoader
{
public:
	static CHeightLoader* NewInstance();

	static IBlockReader* CreateBlockReader(int block_id, void* context);

	// ����ֿ��LOD���
	static void CalcChunkErrors(const float* height_values, size_t cols,
		size_t row_offset, size_t col_offset, size_t chunk_scale,
		size_t lod_levels, float* lod_errors);
	// ����ֿ�İ�Χ��
	static void CalcChunkBoundBox(const float* height_values, size_t cols,
		size_t row_offset, size_t col_offset, size_t chunk_scale, float left, 
		float top, float unit_size, FmVec3& v_min, FmVec3& v_max);
	static void CalcBlockMinMaxY(const float* height_values, size_t cols, 
		size_t row, size_t col, size_t scale, float& min_y, float& max_y);

	// �ֿ���Ϣ
	struct chunk_info_t
	{
		FmVec3 vMin;	// ��Χ��
		FmVec3 vMax;
		float fErrors[8];	// ÿ��LOD��������
	};
	
	struct height_all_t
	{
		float* pValues;
		unsigned int* pNormals;
	};
	
	struct chunk_all_t
	{
		chunk_info_t* pChunks;
	};

	struct water_all_t
	{
		size_t nWaterNum;
		CNameList WaterNames;
		unsigned char* pWaterIndex;
	};
	
private:
	// ��ͼ������ת
	enum MIRROR_TYPES
	{
		MIRROR_NONE,
		MIRROR_HORIZON,		// ˮƽ����
		MIRROR_VERTICAL,	// ��ֱ����
		MIRROR_H_V,			// ˮƽ�ʹ�ֱ����
		MIRROR_MAX,
	};
	
public:
	CHeightLoader();
	virtual ~CHeightLoader();

	// �ͷ�
	virtual void Release();
	// �������
	virtual const char* GetName();
	// ����
	virtual bool Load(bool async);
	// ����
	virtual bool Create(bool async);
	// �����첽���� 
	virtual bool EndCreate(int category, bool succeed);
	
	// ��Դ����
	void SetOwner(CTerrainZone* pOwner);
	// �߶��ļ���
	void SetFileName(const char* file_name);
	// λͼ�ļ���
	void SetBitmap(const char* file_name, int mirror);
	// ѹ���߶��ļ���
	void SetGroundFile(const char* file_name);
	// ˮ��������������ļ�
	void SetWaterFile(const char* file_name);
	// ������ײ�ؿ�ĳߴ�
	void SetCollideScale(size_t value);
	
	// ����߶��ļ�
	static bool SaveBlockHeight(const char* file_name, CTerrainZone* pZone);
	static bool SaveBlockHeightCompress(const char* file_name,
		CTerrainZone* pZone);
	// ����ˮ�������������
	static bool SaveBlockWater(const char* file_name, CTerrainZone* pZone);
	
private:
	CHeightLoader(const CHeightLoader&);
	CHeightLoader& operator=(const CHeightLoader&);
	
	// ��λͼ�ļ�����
	bool LoadBitmapFile(const char* file_name, int mirror);
	// ����Ϊ��ʼ�߶�
	bool LoadInitHeight();
	// ���ظ߶���Ϣ
	bool LoadBlockHeight(const char* file_name);
	// ����ˮ�������������
	bool LoadBlockWater(const char* file_name);

	// �������ķ���
	bool CalcNormals();
	// �������зֿ������
	bool CalcChunkInfo();
	// ��ʼ�����зֿ������
	bool InitChunkInfo();

private:
	CTerrainZone* m_pOwner;
	core_string m_strFileName;
	core_string m_strBitmap;
	core_string m_strGroundFile;
	core_string m_strWaterFile;
	int m_nMirror;
	size_t m_nRows;
	size_t m_nCols;
	size_t m_nCollideScale;
	size_t m_nLodLevels;
	size_t m_nChunkScale;
	size_t m_nChunkRows;
	size_t m_nChunkCols;
	float m_fInitHeight;
	float m_fLeft;
	float m_fTop;
	float m_fUnitSize;
	float m_fMinY;
	float m_fMaxY;
	
	height_all_t m_Height;
	chunk_all_t m_Chunk;
	water_all_t m_Water;
	float* m_pBlockMinY;
	float* m_pBlockMaxY;
};

#endif // _HEIGHT_LOADER_H
