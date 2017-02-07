//--------------------------------------------------------------------
// �ļ���:		terrain_light.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��8��9��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _TERRAIN_LIGHT_H
#define _TERRAIN_LIGHT_H

#include "../public/macros.h"
#include "../visual/i_texture.h"
#include "../math/fm_vector.h"

// ���εĹ�����Ϣ

class Terrain;
class CTerrainZone;
class CTerrainHeight;
class CTexLightMap;
class IRender;

class CTerrainLight
{
public:
	CTerrainLight(Terrain* pTerrain, CTerrainZone* pZone, size_t rows, 
		size_t cols);
	~CTerrainLight();

	// ������
	size_t GetRows() const { return m_nRows; }
	size_t GetCols() const { return m_nCols; }

	// ��û����ɫ
	unsigned int* GetBlendColors();

	// ���ָ��λ�õĹ���ͼ����
	float GetDiffuse(float u, float v);

	// �Ƿ�Ϊ��ʼ�հ�LightMap
	bool GetIsNull() const { return m_bIsNull; }

	// ���õر�����ɫ
	bool SetBlendColor(int row, int col, unsigned int color);
	// ��õر�����ɫ
	unsigned int GetBlendColor(int row, int col);

	// ���ɿյĹ�������
	bool BuildNull();

	// ��������Ĺ�����ͼ
	CTexLightMap* CreateLightTex();
	
	// �������Ĺ�����ͼ
	CTexLightMap* GetLightTex() const { return m_pLightTex; }

	// ��������ͼ����ֵ
	void SetLightConfig(const FmVec3& value) { m_vHdrScale = value; }
	const FmVec3& GetLightConfig() const { return m_vHdrScale; }

	// ������ɫֵ���ɹ���ͼ
	bool BuildTexByColors(size_t rows, size_t cols, 
		const unsigned int* colors);
	
	// ʹ����ͼ�ļ�����������ͼ
	bool BuildTex(unsigned char* dds_data, size_t dds_size);
	bool BuildTex(ITexture* pTex);
	
	// �����ر�����ɫ
	bool BuildBlendColors(size_t rows, size_t cols, 
		const unsigned int* colors);

private:
	CTerrainLight();
	CTerrainLight(const CTerrainLight&);
	CTerrainLight& operator=(const CTerrainLight&);

	void ReleaseAll();
	// �ͷ�����Ĺ�����ͼ
	void ReleaseLightTex();
	// �����༭ģʽ�Ĺ���ͼ
	bool CreateDesignTex();

private:
	Terrain* m_pTerrain;
	CTerrainZone* m_pZone;
	IRender* m_pRender;
	size_t m_nRows;
	size_t m_nCols;
	unsigned int* m_pBlendColors;
	size_t m_nLightWidth;
	size_t m_nLightHeight;
	unsigned int* m_pLightColors;
	CTexLightMap* m_pLightTex;		// ������ͼ	
	FmVec3 m_vHdrScale;		// ����ͼ����
	ITexture* m_pDiffuseTex;
	bool m_bIsNull;
	bool m_bIsDesign;
};

#endif // _TERRAIN_LIGHT_H
