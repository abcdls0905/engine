//--------------------------------------------------------------------
// �ļ���:		terrain_copy.h
// ��  ��:		
// ˵  ��:		
// ��������:	2011��1��10��
// ������:		
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------
/*
#ifndef _TERRAIN_COPY_H
#define _TERRAIN_COPY_H

#include "../public/module.h"
#include "../public/core_mem.h"
#include "../public/fast_str.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"
#include "../utils/string_pod.h"
#include "../visual/dx_header.h"

// ���ο���

class Terrain;
class TerrainEditor;

class TerrainCopy: public IEntity
{
private:
	struct blend_t
	{
		core_string strName;
		float x;
		float z;
		float value;
		int format;
	};

	struct grass_t
	{
		core_string strName;
		float x;					// λ��
		float z;
		//unsigned int nGrassIndex;	// ��������
		unsigned int nScale;		// ��Χ(1-10)
		unsigned int nAmount;		// ����(1-15)
		unsigned int nSeed;			// �������
	};

	typedef TArrayPod<blend_t*, 1, TCoreAlloc> BLEND_CONTAINER;
	typedef TArrayPod<grass_t*, 1, TCoreAlloc> GRASS_CONTAINER;

public:
	TerrainCopy();
	virtual ~TerrainCopy();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	// ����
	bool SetTerrain(PERSISTID id);

	// ���ñ༭��Χ(�����������Ϊ1.0F)
	bool SetRect(float start_x, float start_z, float end_x, float end_z);
	// ��������ı༭���ģ�δ�����ΪRect���ģ������Ϊ���巶Χ���ģ�
	bool SetCenter(float center_x, float center_z);

	// ���õ�ǰ�ļ���
	bool SetCurrentFile(const char* file_name);

	// ������ת�Ƕ�
	bool SetAngle(float vlaue);

	// �Ƿ񱣴�����ͼ����
	void SetCopyBlend(bool value);
	bool GetCopyBlend() const;

	// �Ƿ����ָ����Χ��ԭ�����ͼ����
	void SetClearBlend(bool value);
	bool GetClearBlend() const;

	// �Ƿ񱣴滨������
	void SetCopyGrass(bool value);
	bool GetCopyGrass() const;

	// �Ƿ����ָ����Χ��ԭ��������
	void SetClearGrass(bool value);
	bool GetClearGrass() const;

	// ����ǰ�Ĵ���
	bool BeginSave(const char* file_name);
	// ����������Ҫ����������
	bool Save();

	// ��ǰ�����ͼ��������
	int GetBlendDataCount() const;

	// ��ǰ������������
	int GetGrassDataCount() const;

	// ʹ�ô洢�����޸�ָ����Χ�ĵ���
	bool ModifyTerrain();
	
private:
	TerrainCopy(const TerrainCopy&);
	TerrainCopy& operator=(const TerrainCopy&);

	// �����µĽڵ㣬����ɹ�������µ�ǰ�ڵ�
	bool InsertNewNode(const char* name);
	bool InsertNewBlendNode(const char* name);
	bool InsertNewGrassNode(const char* name);

	// �ռ������ͼ����
	bool CollectBlendData();
	// ��ǰָ�������Ļ����ͼ����
	const char* GetBlendDataName(int index);
	const float GetBlendDataX(int index);
	const float GetBlendDataZ(int index);
	const float GetBlendDataValue(int index);
	const int GetBlendDataFormat(int index);

	// ���ػ����ͼ����
	int LoadBlendData(const char* file_name);
	// ��������ͼ����
	int SaveBlendData(const char* file_name);

	// ʹ�ô洢�Ļ����ͼ�����޸�ָ���ĵ���
	bool UseBlendDataModifyTerrain();

	// �ռ���������
	bool CollectGrassData();
	// ��ǰָ�������Ļ�������
	const char* GetGrassDataName(int index);
	const float GetGrassDataX(int index);
	const float GetGrassDataZ(int index);
	const int GetGrassDataScale(int index);
	const int GetGrassDataAmount(int index);
	const int GetGrassDataSeed(int index);

	// ���ػ�������
	int LoadGrassData(const char* file_name);
	// ���滨������
	int SaveGrassData(const char* file_name);

	// ʹ�ô洢�Ļ��������޸�ָ���ĵ���
	bool UseGrassDataModifyTerrain();

	// �����������
	bool ClearOldData(const char* name);

	// ���ָ���ڵ�Ļ����ͼ����
	bool ClearBlendData(const char* name);
	// ������нڵ�Ļ����ͼ����
	bool ClearAllBlendData();

	// ���ָ���ڵ�Ļ�������
	bool ClearGrassData(const char* name);
	// ������нڵ�Ļ�������
	bool ClearAllGrassData();

private:
	Terrain* m_pTerrain;
	TerrainEditor* m_pEditor;
	core_string m_strFileName;
	FmVec4 m_vStartPos;
	FmVec4 m_vEndPos;
	FmVec4 m_vCenterPos;
	float m_fAngle;
	bool m_bCopyBlend;
	bool m_bClearBlend;
	BLEND_CONTAINER* m_CurBlend;
	TStringPod<char, BLEND_CONTAINER*, TStringTraits<char>, 
		TCoreAlloc> m_BlendAll;
	bool m_bCopyGrass;
	bool m_bClearGrass;
	GRASS_CONTAINER* m_CurGrass;
	TStringPod<char, GRASS_CONTAINER*, TStringTraits<char>, 
		TCoreAlloc> m_GrassAll;
};

#endif // _TERRAIN_COPY_H
*/