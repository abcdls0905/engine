//--------------------------------------------------------------------
// 文件名:		terrain_brush.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月30日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _TERRAIN_BRUSH_H
#define _TERRAIN_BRUSH_H

#include "../public/module.h"
#include "../public/core_mem.h"
#include "../utils/array_pod.h"

#include "../visual/i_terrain_brush.h"
#include "../visual/i_render.h"

// 地形设计刷子

class TerrainBrush: public ITerrainBrush
{
private:
	enum
	{
		SHAPE_RECTANGLE,	// 方形
		SHAPE_CIRCLE,		// 圆形
	};

	enum 
	{
		TRIG_CONTINUE,		// 连续触发
		TRIG_ONCE,			// 一次性触发
	};
	
	struct vertex_line_t
	{
		float x;
		float y;
		float z;
		unsigned int diffuse;
	};

	// 刷子单元格
	struct unit_data_t
	{
		float fPosiX;
		float fPosiZ;
		unsigned int nColor;
		float fFactor;
		float fDist;
	};

public:
	TerrainBrush();
	virtual ~TerrainBrush();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	void SetTerrain(Terrain* value) { m_pTerrain = value; }

	// 是否可见
	void SetVisible(bool value) { m_bVisible = value; }
	bool GetVisible() const { return m_bVisible; }

	// 刷子高于地面的距离
	void SetDeltaY(float value) { m_fDeltaY = value; }
	float GetDeltaY() const { return m_fDeltaY; }
	
	// 中心位置
	void SetPosiX(float value);
	float GetPosiX() const;
	void SetPosiZ(float value);
	float GetPosiZ() const;

	// 单元格子尺寸
	void SetUnitSize(float value) { m_fUnitSize = value; }
	float GetUnitSize() const { return m_fUnitSize; }

	// 刷子范围
	void SetScale(int value) { m_nScale = value; }
	int GetScale() const { return m_nScale; }

	// 刷子形状
	void SetShapeString(const char* value);
	result_string GetShapeString() const;

	// 触发方式
	void SetTrigModeString(const char* value);
	result_string GetTrigModeString() const;

	// 刷子颜色
	void SetColorString(const char* value);
	result_string GetColorString() const;

	// 刷子中心部分颜色
	void SetCenterColorString(const char* value);
	result_string GetCenterColorString() const;
	
	// 执行动作的周期时间
	void SetActionPeriod(float value);
	float GetActionPeriod() const;

	// 是否开始动作
	void SetActive(bool value);
	bool GetActive() const;
	
	// 获得单元格数量
	int GetUnitCount() const;
	
	// 获得单元格位置
	float GetUnitPosiX(size_t index) const;
	float GetUnitPosiZ(size_t index) const;

	// 获得单元格乘法因子
	float GetUnitFactor(size_t index) const;
	float GetUnitDist(size_t index) const;

	// 创建乘法因子
	bool CreateFactors(size_t rows, size_t cols);
	// 清除乘法因子
	bool ClearFactors();
	// 从贴图加载乘法因子 
	bool LoadFactorMap(const char* file);
	// 设置指定位置的乘法因子
	bool SetFactorValue(size_t row, size_t col, float value);
	// 获得指定位置的乘法因子
	float GetFactorValue(size_t row, size_t col);
	
	// 用新的参数重新生成刷子
	bool Generate();
	
	// 更新
	void Update(float seconds);
	// 渲染
	void Realize(float offset_seconds);
	
private:
	TerrainBrush(const TerrainBrush&);
	TerrainBrush& operator=(const TerrainBrush&);

	bool CreateVertices();
	void ReleaseVertices();

	// 创建方形的单元格
	bool CreateRectUnits();
	// 创建圆形的单元格
	bool CreateCircleUnits();

private:
	Terrain* m_pTerrain;
	bool m_bVisible;
	bool m_bActive;
	bool m_bTrigged;
	float m_fDeltaY;
	float m_fPosiX;	// 中心位置
	float m_fPosiZ;	
	float m_fUnitSize;// 单元格子的尺寸
	int m_nScale;	// 范围
	int m_nShape;	// 形状
	int m_nTrigMode;
	unsigned int m_nColor;	// 颜色
	unsigned int m_nCenterColor;	// 刷子中心的颜色
	float m_fActionPeriod;
	float m_fActionCounter;
	TArrayPod<unit_data_t, 1, TCoreAlloc> m_BrushUnits;
	TArrayPod<unit_data_t, 1, TCoreAlloc> m_ShowUnits;
	vertex_line_t* m_pVertices;
	size_t m_nVertexNum;
	size_t m_nFactorRows;
	size_t m_nFactorCols;
	float* m_pFactorValues;
};

#endif // _TERRAIN_BRUSH_H
