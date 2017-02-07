//--------------------------------------------------------------------
// �ļ���:		i_terrain_brush.h
// ��  ��:		
// ˵  ��:		
// ��������:
// ������:
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _VISUAL_I_TERRAIN_BRUSH_H
#define _VISUAL_I_TERRAIN_BRUSH_H

#include "../public/module.h"

class Terrain;

class ITerrainBrush: public IEntity
{
public:
	// ��õ�Ԫ������
	virtual int GetUnitCount() const = 0;

	// ��õ�Ԫ��λ��
	virtual float GetUnitPosiX(size_t index) const = 0;
	virtual float GetUnitPosiZ(size_t index) const = 0;

	// ��õ�Ԫ��˷�����
	virtual float GetUnitFactor(size_t index) const = 0;
	virtual float GetUnitDist(size_t index) const = 0;

	// �����˷�����
	virtual bool CreateFactors(size_t rows, size_t cols) = 0;
	// ����˷�����
	virtual bool ClearFactors() = 0;
	// ����ͼ���س˷����� 
	virtual bool LoadFactorMap(const char* file) = 0;
	// ����ָ��λ�õĳ˷�����
	virtual bool SetFactorValue(size_t row, size_t col, float value) = 0;
	// ���ָ��λ�õĳ˷�����
	virtual float GetFactorValue(size_t row, size_t col) = 0;

	// ���µĲ�����������ˢ��
	virtual bool Generate() = 0;
};

#endif // _VISUAL_I_TERRAINv_BRUSH_H
