//--------------------------------------------------------------------
// �ļ���:		blend_tex.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��7��3��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _BLEND_TEX_H
#define _BLEND_TEX_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../public/fast_str.h"
#include "../public/core_type.h"
#include "../public/persistid.h"
#include "../utils/array_pod.h"
#include "../visual/fm_define.h"

// �����ͼ

class ITexture;
class Terrain;

class CBlendTex
{
public:
	// ��ͼ��ʽ
	struct format_t
	{
		unsigned int nScaleU;
		unsigned int nScaleV;
		unsigned int nAngleX;
		unsigned int nAngleY;
		unsigned int nAngleZ;
		FmMat4 mtxTex;
	};
	
public:
	CBlendTex();
	~CBlendTex();

	// ����
	void SetName(const char* value) { m_strName = value; }
	const char* GetName() const { return m_strName.c_str(); }
	
	// ��ϣֵ
	void SetHash(unsigned int value) { m_nHash = value; }
	unsigned int GetHash() const { return m_nHash; }
	
	// ��ͼ·��
	void SetPath(const char* value) { m_strPath = value; }
	const char* GetPath() const { return m_strPath.c_str(); }
	
	// ��ͼ��Դ
	void SetTex(ITexture* value) { m_pTex = value; }
	ITexture* GetTex() const { return m_pTex; }

	// ������ͼ
	void SetNormalMap(ITexture* value) { m_pNormalMap = value; }
	ITexture* GetNormalMap() const { return m_pNormalMap; }

	// �߶���ͼ
	void SetHeightMap(ITexture* value) { m_pHeightMap = value; }
	ITexture* GetHeightMap() const { return m_pHeightMap; }

	// ���
	bool AddFormat(unsigned int scale_u, unsigned int scale_v, 
		unsigned int angle_x, unsigned int angle_y, unsigned int angle_z);

	// �����ʽ����
	size_t GetFormatCount() const { return m_Formats.size(); }

	// ���ָ����������ʽ��Ϣ
	format_t* GetFormatByIndex(size_t index)
	{
		Assert(index < m_Formats.size());

		return &m_Formats[index];
	}

	// ������ͼ����
	bool UpdateMatrix(size_t index, Terrain* pTerrain);

	// �߹�����
	void SetSpecularLevel(float value) { m_fSpecularLevel = value; }
	float GetSpecularLevel() const { return m_fSpecularLevel; }

	// �����
	void SetSpecularPower(float value) { m_fSpecularPower = value; }
	float GetSpecularPower() const { return m_fSpecularPower; }

	// ����ϸ�ָ߶�
	void SetHeightScale(float value) { m_fHeightScale = value; }
	float GetHeightScale() const { return m_fHeightScale; }

private:
	CBlendTex(const CBlendTex&);
	CBlendTex& operator=(const CBlendTex&);

private:
	core_string m_strName; // ����
	unsigned int m_nHash;
	core_string m_strPath; // ��ͼ�ļ�·��
	ITexture* m_pTex;
	ITexture* m_pNormalMap;
	ITexture* m_pHeightMap;
	TArrayPod<format_t, 1, TCoreAlloc> m_Formats;
	float m_fSpecularLevel;
	float m_fSpecularPower;
	float m_fHeightScale;
};

#endif // _BLEND_TEX_H
