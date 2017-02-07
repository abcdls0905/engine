//--------------------------------------------------------------------
// �ļ���:		alpha_loader.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��8��15��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _ALPHA_LOADER_H
#define _ALPHA_LOADER_H

#include "../public/macros.h"
#include "../public/core_type.h"
#include "../visual/i_res_loader.h"
#include "name_list.h"

// �����ͼ��Ϣ������

class CTerrainZone;
class CAlphaDesign;
class IBlockReader;

class CAlphaLoader: public IResLoader
{
public:
	static CAlphaLoader* NewInstance();
	static IBlockReader* CreateBlockReader(int block_id, void* context);
	
#pragma pack(push, 1)
	// ���浽�ļ�����ͼ��ʽ
	struct blend_format_t
	{
		unsigned short nScaleU;
		unsigned short nScaleV;
		unsigned short nAngleX;
		unsigned short nAngleY;
		unsigned short nAngleZ;
		unsigned char nTexIndex;
		unsigned char nFmtIndex;
	};
	
	// ���浽�ļ���������ͼ��Ϣ
	struct map_alpha_t
	{
		unsigned short nChunkRow;
		unsigned short nChunkCol;
		unsigned char nTexIndex;
		unsigned char nFormat;
	};
#pragma pack(pop)
	
	struct base_all_t
	{
		size_t nBaseTexNum;
		CNameList BaseNames;
		unsigned char* pBaseTexIndex;
	};

	struct map_all_t
	{
		size_t nMapTexNum;
		CNameList MapNames;
		size_t nFormatNum;
		blend_format_t*	pBlendFormat;
		size_t nMapNum;
		map_alpha_t* pMapAlpha;
		unsigned char* pMapData;
		size_t nMapDataSize;
	};

public:
	CAlphaLoader();
	virtual ~CAlphaLoader();

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
	// ������ͼ��������ļ�
	void SetBaseFile(const char* file_name);
	// ���ջ����ͼ�ļ�
	void SetMapFile(const char* file_name);

	// ���������ͼ�������
	static bool SaveBlockBase(const char* file_name, CTerrainZone* pZone);
	// �������ջ����ͼ�ļ�
	static bool SaveBlockAlphaMap(const char* file_name, 
		CTerrainZone* pZone);

private:
	CAlphaLoader(const CAlphaLoader&);
	CAlphaLoader& operator=(const CAlphaLoader&);

	bool LoadBlockBase(const char* file_name);
	bool LoadBlockAlphaMap(const char* file_name);
	
private:
	CTerrainZone* m_pOwner;
	core_string m_strBaseFile;
	core_string m_strBlendFile;
	core_string m_strMapFile;
	size_t m_nChunkRows;
	size_t m_nChunkCols;
	size_t m_nAlphaRows;
	size_t m_nAlphaCols;
	size_t m_nAlphaMapScale;
	base_all_t m_Base;
	map_all_t m_Map;
};

#endif // _ALPHA_LOADER_H
