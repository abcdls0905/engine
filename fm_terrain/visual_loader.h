//--------------------------------------------------------------------
// �ļ���:		visual_loader.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��11��1��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _VISUAL_LOADER_H
#define _VISUAL_LOADER_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../public/fast_str.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"


#include "../visual/i_res_loader.h"
#include "visual_info.h"
#include "name_list.h"

// ����������Ϣ������

class CTerrainZone;
class IBlockReader;

class CVisualLoader: public IResLoader
{
public:
	static CVisualLoader* NewInstance();
	
	static IBlockReader* CreateBlockReader(int block_id, void* context);
	
#pragma pack(push, 1)
	struct grass_info_t
	{
		unsigned short nPosiX;		// λ��
		unsigned short nPosiZ;
		unsigned char nGrassIndex;	// ��������
		unsigned char nScaleAmount;	// ��Χ(1-10),����(1-15)
		unsigned short nSeed;		// �������
	};
#pragma pack(pop)

	struct grass_all_t
	{
		size_t nGrassTypeNum;
		CNameList GrassNames;
		size_t nGrassDataNum;
		grass_info_t* pGrassData;
	};

public:
	CVisualLoader();
	virtual ~CVisualLoader();

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
	// ���������ļ���
	void SetFileName(const char* file_name);
	// װ�����ļ���
	void SetWidgetFile(const char* file_name);
	// ���������ļ���
	void SetHelperFile(const char* file_name);
	// ģ����ײ��Ϣ�����ļ���
	void SetCollideFile(const char* file_name);
	// ����������Ϣ�ļ���
	void SetDescFile(const char* file_name);
	// ����ͼ�ϲ���Ϣ�ļ���
	void SetAtlasFile(const char* file_name);
	// �����ļ���
	void SetSoundFile(const char* file_name);
	// �����ļ���
	void SetGrassFile(const char* file_name);

	// ���������������
	static bool SaveVisuals(const char* file_name, const char* widget_file,
		const char* helper_file, const char* collide_file, 
		const char* desc_file, const char* sound_file, CTerrainZone* pZone);
	static bool SaveBlockGrass(const char* file_name, CTerrainZone* pZone);
	
private:
	CVisualLoader(const CVisualLoader&);
	CVisualLoader& operator=(const CVisualLoader&);

	// ���ص�����������
	bool LoadVisuals(const char* file_name);
	// ����ģ����ײ�������
	bool LoadCollideInfo(const char* file_name);
	// ��������������Ϣ
	bool LoadDesc(const char* file_name);
	// ���ع���ͼ�ϲ���Ϣ 
	bool LoadAtlas(const char* file_name);
	// ���ػ�������
	bool LoadBlockGrass(const char* file_name);
	// ������ϣ����
	void CreateHashIndex();
	// ����������ģ����Ϣ
	bool FindVisualIndex(const char* name, size_t& index);

private:
	CTerrainZone* m_pOwner;
	bool m_bDesignMode;
	core_string m_strFileName;
	core_string m_strWidgetFile;
	core_string m_strHelperFile;
	core_string m_strCollideFile;
	core_string m_strDescFile;
	core_string m_strAtlasFile;
	core_string m_strSoundFile;
	core_string m_strGrassFile;
	grass_all_t m_Grass;
	TArrayPod<visual_info_t*, 1, TCoreAlloc> m_Visuals;
	int* m_pHashIndex;
	size_t m_nHashSize;
};

#endif // _VISUAL_LOADER_H
