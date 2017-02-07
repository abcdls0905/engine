//--------------------------------------------------------------------
// �ļ���:		terrain_height.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��8��9��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _TERRAIN_HEIGHT_H
#define _TERRAIN_HEIGHT_H

#include "../public/macros.h"


#include "../visual/fm_define.h"

// ��������ĸ߶���Ϣ

class Terrain;
class CTerrainZone;

class CTerrainHeight
{
public:
	CTerrainHeight(Terrain* pTerrain, CTerrainZone* pZone, size_t rows, 
		size_t cols);
	~CTerrainHeight();

	// ����������
	size_t GetRows() const { return m_nRows; }
	size_t GetCols() const { return m_nCols; }

	// �����͵�
	float GetMinY() const { return m_fMinY; }
	// �����ߵ�
	float GetMaxY() const { return m_fMaxY; }

	// ��ö���߶�
	float GetValue(int row, int col)
	{
		Assert(size_t(row) < m_nRows); 
		Assert(size_t(col) < m_nCols);

		return m_pValues[row * m_nCols + col];
	}

	float GetValueDefault(int row, int col, float def);
	// ���ø߶�
	bool SetValue(int row, int col, float value);
	// �ı�߶Ȳ����ͷ���صķ���
	bool ChangeValue(int row, int col, float value);
	// �������λ�ø߶�
	float GetY(float x, float z);
	// ��������λ�ø߶�
	bool SetY(int row, int col, float y);
	// ��ö�������
	bool GetPoint(int row, int col, FmVec4& point);
	// ���ָ����Χ�ڵ���͵�
	float GetRegionMinY(int row, int col, int scale);
	// ��û��ָ����Χ�ڵ���͵����͵�
	void GetBlockMinMaxY(int row, int col, int scale, float& min_y, 
		float& max_y);

	// �������ڵĸ߶�ͼ
	bool ConnectValues();
	
	// ������и߶�����
	const float* GetHeightValues() const { return m_pValues; }
	
	// ��������ָ��λ�õķ���
	bool ReleaseNormal(int row, int col);
	// ��ö��㷨��
	bool GetNormal(int row, int col, FmVec4& normal);
	
	// ���������ʽ�ķ�����Ϣ
	unsigned int GetIntNormal(int row, int col)
	{
		Assert(size_t(row) < m_nRows); 
		Assert(size_t(col) < m_nCols);

		size_t index = row * m_nCols + col;

		if (0 == m_pNormals[index])
		{
			m_pNormals[index] = CalcVertexNormal(row, col);
		}

		return m_pNormals[index];
	}
	
	// �������еĸ߶Ⱥͷ���ֵ
	bool Build(size_t rows, size_t cols, float min_y, float max_y, 
		float* heights, unsigned int* normals);
	// ���þֲ������ߺ���͵�
	bool BuildBlock(size_t block_scale, size_t block_dims, 
		float* block_min_y, float* block_max_y);
	
private:
	CTerrainHeight();
	CTerrainHeight(const CTerrainHeight&);
	CTerrainHeight& operator=(const CTerrainHeight&);

	void ReleaseAll();

	// ���㶥�㷨��
	unsigned int CalcVertexNormal(int row, int col);

private:
	Terrain* m_pTerrain;
	CTerrainZone* m_pZone;
	size_t m_nRows;
	size_t m_nCols;
	float m_fLeft;
	float m_fTop;
	float m_fUnitSize;
	float m_fInvUnitSize;
	float m_fMinY;
	float m_fMaxY;
	float* m_pValues;			// �߶�
	unsigned int* m_pNormals;	// ѹ���ķ���
	float* m_pBlockMinY;
	float* m_pBlockMaxY;
	size_t m_nBlockScale;
	size_t m_nBlockDims;
};

#endif // _TERRAIN_HEIGHT_H
