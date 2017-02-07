//--------------------------------------------------------------------
// �ļ���:		terrain_brush.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��8��30��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _TERRAIN_BRUSH_H
#define _TERRAIN_BRUSH_H

#include "../public/module.h"
#include "../public/core_mem.h"
#include "../utils/array_pod.h"

#include "../visual/i_terrain_brush.h"
#include "../visual/i_render.h"

// �������ˢ��

class TerrainBrush: public ITerrainBrush
{
private:
	enum
	{
		SHAPE_RECTANGLE,	// ����
		SHAPE_CIRCLE,		// Բ��
	};

	enum 
	{
		TRIG_CONTINUE,		// ��������
		TRIG_ONCE,			// һ���Դ���
	};
	
	struct vertex_line_t
	{
		float x;
		float y;
		float z;
		unsigned int diffuse;
	};

	// ˢ�ӵ�Ԫ��
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

	// �Ƿ�ɼ�
	void SetVisible(bool value) { m_bVisible = value; }
	bool GetVisible() const { return m_bVisible; }

	// ˢ�Ӹ��ڵ���ľ���
	void SetDeltaY(float value) { m_fDeltaY = value; }
	float GetDeltaY() const { return m_fDeltaY; }
	
	// ����λ��
	void SetPosiX(float value);
	float GetPosiX() const;
	void SetPosiZ(float value);
	float GetPosiZ() const;

	// ��Ԫ���ӳߴ�
	void SetUnitSize(float value) { m_fUnitSize = value; }
	float GetUnitSize() const { return m_fUnitSize; }

	// ˢ�ӷ�Χ
	void SetScale(int value) { m_nScale = value; }
	int GetScale() const { return m_nScale; }

	// ˢ����״
	void SetShapeString(const char* value);
	result_string GetShapeString() const;

	// ������ʽ
	void SetTrigModeString(const char* value);
	result_string GetTrigModeString() const;

	// ˢ����ɫ
	void SetColorString(const char* value);
	result_string GetColorString() const;

	// ˢ�����Ĳ�����ɫ
	void SetCenterColorString(const char* value);
	result_string GetCenterColorString() const;
	
	// ִ�ж���������ʱ��
	void SetActionPeriod(float value);
	float GetActionPeriod() const;

	// �Ƿ�ʼ����
	void SetActive(bool value);
	bool GetActive() const;
	
	// ��õ�Ԫ������
	int GetUnitCount() const;
	
	// ��õ�Ԫ��λ��
	float GetUnitPosiX(size_t index) const;
	float GetUnitPosiZ(size_t index) const;

	// ��õ�Ԫ��˷�����
	float GetUnitFactor(size_t index) const;
	float GetUnitDist(size_t index) const;

	// �����˷�����
	bool CreateFactors(size_t rows, size_t cols);
	// ����˷�����
	bool ClearFactors();
	// ����ͼ���س˷����� 
	bool LoadFactorMap(const char* file);
	// ����ָ��λ�õĳ˷�����
	bool SetFactorValue(size_t row, size_t col, float value);
	// ���ָ��λ�õĳ˷�����
	float GetFactorValue(size_t row, size_t col);
	
	// ���µĲ�����������ˢ��
	bool Generate();
	
	// ����
	void Update(float seconds);
	// ��Ⱦ
	void Realize(float offset_seconds);
	
private:
	TerrainBrush(const TerrainBrush&);
	TerrainBrush& operator=(const TerrainBrush&);

	bool CreateVertices();
	void ReleaseVertices();

	// �������εĵ�Ԫ��
	bool CreateRectUnits();
	// ����Բ�εĵ�Ԫ��
	bool CreateCircleUnits();

private:
	Terrain* m_pTerrain;
	bool m_bVisible;
	bool m_bActive;
	bool m_bTrigged;
	float m_fDeltaY;
	float m_fPosiX;	// ����λ��
	float m_fPosiZ;	
	float m_fUnitSize;// ��Ԫ���ӵĳߴ�
	int m_nScale;	// ��Χ
	int m_nShape;	// ��״
	int m_nTrigMode;
	unsigned int m_nColor;	// ��ɫ
	unsigned int m_nCenterColor;	// ˢ�����ĵ���ɫ
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
