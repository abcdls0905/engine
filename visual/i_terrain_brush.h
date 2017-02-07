//--------------------------------------------------------------------
// 文件名:		i_terrain_brush.h
// 内  容:		
// 说  明:		
// 创建日期:
// 创建人:
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _VISUAL_I_TERRAIN_BRUSH_H
#define _VISUAL_I_TERRAIN_BRUSH_H

#include "../public/module.h"

class Terrain;

class ITerrainBrush: public IEntity
{
public:
	// 获得单元格数量
	virtual int GetUnitCount() const = 0;

	// 获得单元格位置
	virtual float GetUnitPosiX(size_t index) const = 0;
	virtual float GetUnitPosiZ(size_t index) const = 0;

	// 获得单元格乘法因子
	virtual float GetUnitFactor(size_t index) const = 0;
	virtual float GetUnitDist(size_t index) const = 0;

	// 创建乘法因子
	virtual bool CreateFactors(size_t rows, size_t cols) = 0;
	// 清除乘法因子
	virtual bool ClearFactors() = 0;
	// 从贴图加载乘法因子 
	virtual bool LoadFactorMap(const char* file) = 0;
	// 设置指定位置的乘法因子
	virtual bool SetFactorValue(size_t row, size_t col, float value) = 0;
	// 获得指定位置的乘法因子
	virtual float GetFactorValue(size_t row, size_t col) = 0;

	// 用新的参数重新生成刷子
	virtual bool Generate() = 0;
};

#endif // _VISUAL_I_TERRAINv_BRUSH_H
