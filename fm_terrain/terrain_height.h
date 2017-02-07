//--------------------------------------------------------------------
// 文件名:		terrain_height.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月9日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _TERRAIN_HEIGHT_H
#define _TERRAIN_HEIGHT_H

#include "../public/macros.h"


#include "../visual/fm_define.h"

// 地形区域的高度信息

class Terrain;
class CTerrainZone;

class CTerrainHeight
{
public:
	CTerrainHeight(Terrain* pTerrain, CTerrainZone* pZone, size_t rows, 
		size_t cols);
	~CTerrainHeight();

	// 顶点行列数
	size_t GetRows() const { return m_nRows; }
	size_t GetCols() const { return m_nCols; }

	// 获得最低点
	float GetMinY() const { return m_fMinY; }
	// 获得最高点
	float GetMaxY() const { return m_fMaxY; }

	// 获得顶点高度
	float GetValue(int row, int col)
	{
		Assert(size_t(row) < m_nRows); 
		Assert(size_t(col) < m_nCols);

		return m_pValues[row * m_nCols + col];
	}

	float GetValueDefault(int row, int col, float def);
	// 设置高度
	bool SetValue(int row, int col, float value);
	// 改变高度并且释放相关的法线
	bool ChangeValue(int row, int col, float value);
	// 获得任意位置高度
	float GetY(float x, float z);
	// 设置任意位置高度
	bool SetY(int row, int col, float y);
	// 获得顶点坐标
	bool GetPoint(int row, int col, FmVec4& point);
	// 获得指定范围内的最低点
	float GetRegionMinY(int row, int col, int scale);
	// 获得获得指定范围内的最低点和最低点
	void GetBlockMinMaxY(int row, int col, int scale, float& min_y, 
		float& max_y);

	// 连接相邻的高度图
	bool ConnectValues();
	
	// 获得所有高度数据
	const float* GetHeightValues() const { return m_pValues; }
	
	// 重新生成指定位置的法线
	bool ReleaseNormal(int row, int col);
	// 获得顶点法线
	bool GetNormal(int row, int col, FmVec4& normal);
	
	// 获得整数形式的法线信息
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
	
	// 设置所有的高度和法线值
	bool Build(size_t rows, size_t cols, float min_y, float max_y, 
		float* heights, unsigned int* normals);
	// 设置局部块的最高和最低点
	bool BuildBlock(size_t block_scale, size_t block_dims, 
		float* block_min_y, float* block_max_y);
	
private:
	CTerrainHeight();
	CTerrainHeight(const CTerrainHeight&);
	CTerrainHeight& operator=(const CTerrainHeight&);

	void ReleaseAll();

	// 计算顶点法线
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
	float* m_pValues;			// 高度
	unsigned int* m_pNormals;	// 压缩的法线
	float* m_pBlockMinY;
	float* m_pBlockMaxY;
	size_t m_nBlockScale;
	size_t m_nBlockDims;
};

#endif // _TERRAIN_HEIGHT_H
