//--------------------------------------------------------------------
// 文件名:		terrain_brush.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年8月30日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "terrain_brush.h"
#include "terrain.h"
#include "../visual/vis_utils.h"
//#include "../visual/tga_file.h"
//#include "../visual/i_image_access.h"
#include "../visual/i_image_access.h"
#include "../public/var_list.h"
#include "../public/inlines.h"
#include "../public/auto_mem.h"
#include "../public/core_log.h"


/// \file terrain_brush.cpp
/// \brief 地形刷子

/// entity: TerrainBrush
/// \brief 地形刷子实体
DECLARE_ENTITY(TerrainBrush, 0, IEntity);

/// property: bool Visible
/// \brief 是否可见
DECLARE_PROPERTY(bool, TerrainBrush, Visible, GetVisible, SetVisible);
/// property: bool Active
/// \brief 是否执行动作
DECLARE_PROPERTY(bool, TerrainBrush, Active, GetActive, SetActive);
/// property: float DeltaY
/// \brief 刷子高于地面的距离
DECLARE_PROPERTY(float, TerrainBrush, DeltaY, GetDeltaY, SetDeltaY);
/// property: float PosiX
/// \brief 中心位置X坐标
DECLARE_PROPERTY(float, TerrainBrush, PosiX, GetPosiX, SetPosiX);
/// property: float PosiZ
/// \brief 中心位置Z坐标
DECLARE_PROPERTY(float, TerrainBrush, PosiZ, GetPosiZ, SetPosiZ);
/// property: float UnitSize
/// \brief 单元格尺寸
DECLARE_PROPERTY(float, TerrainBrush, UnitSize, GetUnitSize, SetUnitSize);
/// property: int Scale
/// \brief 刷子的大小
DECLARE_PROPERTY(int, TerrainBrush, Scale, GetScale, SetScale);
/// property: string Shape
/// \brief 刷子的形状（"rectangle"-方形,"circle"-圆形）
DECLARE_PROPERTY(result_string, TerrainBrush, Shape, GetShapeString, 
	SetShapeString);
/// property: string TrigMode
/// \brief 刷子的触发方式（"continue"-连续触发,"once"-一次性触发）
DECLARE_PROPERTY(result_string, TerrainBrush, TrigMode, GetTrigModeString, 
	SetTrigModeString);
/// property: string Color
/// \brief 刷子的颜色
DECLARE_PROPERTY(result_string, TerrainBrush, Color, GetColorString, 
	SetColorString);
/// property: string CenterColor
/// \brief 刷子中心部分的颜色
DECLARE_PROPERTY(result_string, TerrainBrush, CenterColor, 
	GetCenterColorString, SetCenterColorString);
/// property: float ActionPeriod
/// \brief 执行动作的周期时间
DECLARE_PROPERTY(float, TerrainBrush, ActionPeriod, GetActionPeriod, 
	SetActionPeriod);

/*
/// method: int GetUnitCount()
/// \brief 获得单元格数量
DECLARE_METHOD_0(int, TerrainBrush, GetUnitCount);
/// method: float GetUnitPosiX()
/// \brief 获得单元格的位置X坐标
/// \param index 索引
DECLARE_METHOD_1(float, TerrainBrush, GetUnitPosiX, size_t);
/// method: float GetUnitPosiZ()
/// \brief 获得单元格的位置Z坐标
/// \param index 索引
DECLARE_METHOD_1(float, TerrainBrush, GetUnitPosiZ, size_t);
/// method: float GetUnitFactor()
/// \brief 获得单元格的乘法因子
/// \param index 索引
DECLARE_METHOD_1(float, TerrainBrush, GetUnitFactor, size_t);
/// method: float GetUnitDist()
/// \brief 获得单元格到中心的距离百分比
/// \param index 索引
DECLARE_METHOD_1(float, TerrainBrush, GetUnitDist, size_t);
/// method: bool Generate()
/// \brief 用新的参数重新生成刷子
DECLARE_METHOD_0(bool, TerrainBrush, Generate);

/// method: bool CreateFactors(int rows, int cols)
/// \brief 创建乘法因子表
/// \param rows 行数
/// \param cols 列数
DECLARE_METHOD_2(bool, TerrainBrush, CreateFactors, size_t, size_t);
/// method: bool ClearFactors()
/// \brief 清空乘法因子
DECLARE_METHOD_0(bool, TerrainBrush, ClearFactors);
/// method: bool LoadFactorMap(string file)
/// \brief 加载乘法因子图像文件
/// \param file 文件名
DECLARE_METHOD_1(bool, TerrainBrush, LoadFactorMap, const char*);
/// method: bool SetFactorVaue(int row, int col, float value)
/// \brief 设置乘法因子
/// \param row 行
/// \param col 列
/// \param value 值
DECLARE_METHOD_3(bool, TerrainBrush, SetFactorValue, size_t, size_t, float);
*/

#define INIT_TERRAIN_LINE(p, x1, y1, z1, color1) \
	{ (p)->x = x1; (p)->y = y1; (p)->z = z1; (p)->diffuse = color1; }

// TerrainBrush

TerrainBrush::TerrainBrush()
{
	m_pTerrain = NULL;
	m_bVisible = true;
	m_bActive = true;
	m_bTrigged = false;
	m_fDeltaY = 0.1F;
	m_fPosiX = 0.0F;
	m_fPosiZ = 0.0F;
	m_fUnitSize = 1.0F;
	m_nScale = 0;
	m_nShape = 0;
	m_nTrigMode = 0;
	m_nColor = 0xFFFFFFFF;
	m_nCenterColor = 0xFFFFFFFF;
	m_fActionPeriod = 0.5F;
	m_fActionCounter = 0.0F;
	m_pVertices = NULL;
	m_nVertexNum = 0;
	m_nFactorRows = 0;
	m_nFactorCols = 0;
	m_pFactorValues = NULL;
}

TerrainBrush::~TerrainBrush()
{
	ReleaseVertices();

	if (m_pFactorValues)
	{
		CORE_FREE(m_pFactorValues, 
			sizeof(float) * m_nFactorRows * m_nFactorCols);
	}
}

bool TerrainBrush::Init(const IVarList& args)
{
	return true;
}

bool TerrainBrush::Shut()
{
	return true;
}

void TerrainBrush::SetPosiX(float value)
{ 
	if (!FloatEqual(m_fPosiX, value))
	{
		if (m_nTrigMode == TRIG_ONCE)
		{
			// 位置改变时重新触发
			m_bTrigged = false;
		}
	}
	
	m_fPosiX = value; 
}

float TerrainBrush::GetPosiX() const
{ 
	return m_fPosiX; 
}

void TerrainBrush::SetPosiZ(float value) 
{ 
	if (!FloatEqual(m_fPosiZ, value))
	{
		if (m_nTrigMode == TRIG_ONCE)
		{
			// 位置改变时重新触发
			m_bTrigged = false;
		}
	}
	
	m_fPosiZ = value; 
}

float TerrainBrush::GetPosiZ() const 
{ 
	return m_fPosiZ; 
}

void TerrainBrush::SetShapeString(const char* value)
{
	Assert(value != NULL);

	if (stricmp(value, "rectangle") == 0)
	{
		m_nShape = SHAPE_RECTANGLE;
	}
	else if (stricmp(value, "circle") == 0)
	{
		m_nShape = SHAPE_CIRCLE;
	}
}

result_string TerrainBrush::GetShapeString() const
{
	switch (m_nShape)
	{
	case SHAPE_RECTANGLE:
		return result_string("rectangle");
	case SHAPE_CIRCLE:
		return result_string("circle");
	default:
		Assert(0);
		break;
	}

	return result_string();
}

void TerrainBrush::SetTrigModeString(const char* value)
{
	Assert(value != NULL);
	
	if (stricmp(value, "continue") == 0)
	{
		m_nTrigMode = TRIG_CONTINUE;
	}
	else if (stricmp(value, "once") == 0)
	{
		m_nTrigMode = TRIG_ONCE;
		m_bTrigged = false;
	}
}

result_string TerrainBrush::GetTrigModeString() const
{
	switch (m_nTrigMode)
	{
	case TRIG_CONTINUE:
		return result_string("continue");
	case TRIG_ONCE:
		return result_string("once");
	default:
		Assert(0);
		break;
	}
	
	return result_string();
}

void TerrainBrush::SetColorString(const char* value)
{
	Assert(value != NULL);

	m_nColor = VisUtil_ColorToInt(value);
}

result_string TerrainBrush::GetColorString() const
{
	return VisUtil_IntToColor(m_nColor);
}

void TerrainBrush::SetCenterColorString(const char* value)
{
	Assert(value != NULL);
	
	m_nCenterColor = VisUtil_ColorToInt(value);
}

result_string TerrainBrush::GetCenterColorString() const
{
	return VisUtil_IntToColor(m_nCenterColor);
}

void TerrainBrush::SetActionPeriod(float value)
{ 
	m_fActionPeriod = value; 
	m_fActionCounter = 0.0F;
}

float TerrainBrush::GetActionPeriod() const
{ 
	return m_fActionPeriod; 
}

void TerrainBrush::SetActive(bool value)
{ 
	m_bActive = value; 
}

bool TerrainBrush::GetActive() const 
{ 
	return m_bActive; 
}

int TerrainBrush::GetUnitCount() const
{
	return (int)m_BrushUnits.size();
}

float TerrainBrush::GetUnitPosiX(size_t index) const
{
	Assert(index < m_BrushUnits.size());

	return m_BrushUnits[index].fPosiX;
}

float TerrainBrush::GetUnitPosiZ(size_t index) const
{
	Assert(index < m_BrushUnits.size());
	
	return m_BrushUnits[index].fPosiZ;
}

float TerrainBrush::GetUnitFactor(size_t index) const
{
	Assert(index < m_BrushUnits.size());
	
	return m_BrushUnits[index].fFactor;
}

float TerrainBrush::GetUnitDist(size_t index) const
{
	Assert(index < m_BrushUnits.size());
	
	return m_BrushUnits[index].fDist;
}

bool TerrainBrush::CreateFactors(size_t rows, size_t cols)
{
	if (m_pFactorValues)
	{
		CORE_FREE(m_pFactorValues, 
			sizeof(float) * m_nFactorRows * m_nFactorCols);
		m_pFactorValues = NULL;
	}

	if ((rows == 0) || (cols == 0))
	{
		return false;
	}

	if ((rows > 4096) || (cols > 4096))
	{
		return false;
	}

	m_pFactorValues = (float*)CORE_ALLOC(sizeof(float) * rows * cols);
	m_nFactorRows = rows;
	m_nFactorCols = cols;
	memset(m_pFactorValues, 0, rows * cols * sizeof(float));

	return true;
}

bool TerrainBrush::ClearFactors()
{
	if (m_pFactorValues)
	{
		CORE_FREE(m_pFactorValues, 
			sizeof(float) * m_nFactorRows * m_nFactorCols);
		m_pFactorValues = NULL;
	}
	
	return true;
}

bool TerrainBrush::LoadFactorMap(const char* file)
{
	IImageAccess* pImage = m_pTerrain->GetRender()->CreateImageAccess();
	
	if (!pImage->LoadFile(file))
	{
		CORE_TRACE("(TerrainBrush::LoadFactorMap)load image failed");
		CORE_TRACE(file);
		pImage->Release();
		return false;
	}

	// 转成灰度
	if (!pImage->ConvertToGray8())
	{
		CORE_TRACE("(TerrainBrush::LoadFactorMap)convert to gray failed");
		CORE_TRACE(file);
		pImage->Release();
		return false;
	}
	
	size_t width = pImage->GetWidth();
	size_t height = pImage->GetHeight();
	size_t pixels = width * height;
	unsigned char* pdata = pImage->GetData();
	
	if (m_pFactorValues)
	{
		CORE_FREE(m_pFactorValues, 
			sizeof(float) * m_nFactorRows * m_nFactorCols);
	}
	
	m_pFactorValues = (float*)CORE_ALLOC(sizeof(float) * pixels);
	
	for (size_t i = 0; i < pixels; ++i)
	{
		m_pFactorValues[i] = float(pdata[i]) / 255.0F;
	}
	
	pImage->Release();

	m_nFactorCols = width;
	m_nFactorRows = height;
	
	return true;
}

bool TerrainBrush::SetFactorValue(size_t row, size_t col, float value)
{
	if (NULL == m_pFactorValues)
	{
		return false;
	}
	
	if ((row >= m_nFactorRows) || (col >= m_nFactorCols))
	{
		return false;
	}
	
	m_pFactorValues[row * m_nFactorCols + col] = value;

	return true;
}

float TerrainBrush::GetFactorValue(size_t row, size_t col)
{
	if (NULL == m_pFactorValues)
	{
		return 1.0F;
	}
	
	if ((row >= m_nFactorRows) || (col >= m_nFactorCols))
	{
		return 1.0F;
	}

	return m_pFactorValues[row * m_nFactorCols + col];
}

bool TerrainBrush::CreateVertices()
{
	ReleaseVertices();
	
	size_t unit_num = m_ShowUnits.size();

	m_nVertexNum = unit_num * 8;
	m_pVertices = (vertex_line_t*)CORE_ALLOC(
		sizeof(vertex_line_t) * m_nVertexNum);
	
	vertex_line_t* pv = m_pVertices;
	float half_unit_size = m_fUnitSize * 0.2F;
	
	// 高于地面
	float delta_y = m_fDeltaY;
	
	for (size_t i = 0; i < unit_num; ++i)
	{
		float posi_x = m_ShowUnits[i].fPosiX;
		float posi_z = m_ShowUnits[i].fPosiZ;
		
		float x0 = posi_x - half_unit_size;
		float z0 = posi_z - half_unit_size;
		float y0 = m_pTerrain->GetPosiY(x0, z0) + delta_y;

		float x1 = posi_x + half_unit_size;
		float z1 = posi_z - half_unit_size;
		float y1 = m_pTerrain->GetPosiY(x1, z1) + delta_y;

		float x2 = posi_x - half_unit_size;
		float z2 = posi_z + half_unit_size;
		float y2 = m_pTerrain->GetPosiY(x2, z2) + delta_y;
		
		float x3 = posi_x + half_unit_size;
		float z3 = posi_z + half_unit_size;
		float y3 = m_pTerrain->GetPosiY(x3, z3) + delta_y;
		
		float max_y = y0;

		if (y1 > max_y)
		{
			max_y = y1;
		}
		
		if (y2 > max_y)
		{
			max_y = y2;
		}
		
		if (y3 > max_y)
		{
			max_y = y3;
		}

		y0 = max_y;
		y1 = max_y;
		y2 = max_y;
		y3 = max_y;
		
		unsigned int color = m_ShowUnits[i].nColor;
		
		INIT_TERRAIN_LINE(pv, x0, y0, z0, color);
		pv++;
		INIT_TERRAIN_LINE(pv, x1, y1, z1, color);
		pv++;
		INIT_TERRAIN_LINE(pv, x1, y1, z1, color);
		pv++;
		INIT_TERRAIN_LINE(pv, x3, y3, z3, color);
		pv++;
		INIT_TERRAIN_LINE(pv, x3, y3, z3, color);
		pv++;
		INIT_TERRAIN_LINE(pv, x2, y2, z2, color);
		pv++;
		INIT_TERRAIN_LINE(pv, x2, y2, z2, color);
		pv++;
		INIT_TERRAIN_LINE(pv, x0, y0, z0, color);
		pv++;
	}
	
	return true;
}

void TerrainBrush::ReleaseVertices()
{
	if (m_pVertices)
	{
		CORE_FREE(m_pVertices, sizeof(vertex_line_t) * m_nVertexNum);
		m_pVertices = NULL;
		m_nVertexNum = 0;
	}
}

bool TerrainBrush::CreateRectUnits()
{
	int range = m_nScale;
	
	if ((range < 0) || (range > 256))
	{
		return false;
	}
	
	for (int r = -range; r <= range; ++r)
	{
		for (int c = -range; c <= range; ++c)
		{
			float factor = 1.0F;
			
			if (m_pFactorValues)
			{
				float row = float(r) / float(range + 1) * 0.5F + 0.5F;
				float col = float(c) / float(range + 1) * 0.5F + 0.5F;
				
				factor = GetFactorValue(size_t(row * m_nFactorRows), 
					size_t(col * m_nFactorCols));
			}

			float dist = FM_MAX(abs(r), abs(c)) / float(range);
			
			unit_data_t data;
			
			data.fPosiX = m_fPosiX + c * m_fUnitSize;
			data.fPosiZ = m_fPosiZ + r * m_fUnitSize;
			data.nColor = m_nColor;
			data.fFactor = factor;
			data.fDist = dist;

			if ((r == 0) && (c == 0))
			{
				data.nColor = m_nCenterColor;
			}
			
			m_BrushUnits.push_back(data);
			
			if (m_nScale > 10)
			{
				// 只显示边缘的格子
				if ((abs(c) != range) && (abs(r) != range))
				{
					if ((r != 0) || (c != 0))
					{
						continue;
					}
				}
			}
			
			m_ShowUnits.push_back(data);
		}
	}
	
	return true;
}

bool TerrainBrush::CreateCircleUnits()
{
	int range = m_nScale + 1;
	
	for (int r = -range; r <= range; ++r)
	{
		for (int c = -range; c <= range; ++c)
		{
			float sx = c * m_fUnitSize;
			float sz = r * m_fUnitSize;

			if ((size_t)(r * r + c * c) > (size_t)(m_nScale * m_nScale))
			{
				continue;
			}
			
			float factor = 1.0F;
			
			if (m_pFactorValues)
			{
				float row = float(r) / float(range + 1) * 0.5F + 0.5F;
				float col = float(c) / float(range + 1) * 0.5F + 0.5F;
				
				factor = GetFactorValue(size_t(row * m_nFactorRows), 
					size_t(col * m_nFactorCols));
			}

			float dist = sqrt((float)(r * r + c * c)) / float(m_nScale);
			
			unit_data_t data;
			
			data.fPosiX = m_fPosiX + sx;
			data.fPosiZ = m_fPosiZ + sz;
			data.nColor = m_nColor;
			data.fFactor = factor;
			data.fDist = dist;

			if ((r == 0) && (c == 0))
			{
				data.nColor = m_nCenterColor;
			}
			
			m_BrushUnits.push_back(data);

			if (m_nScale > 10)
			{
				// 只显示边缘的格子
				if ((size_t)(r * r + c * c) 
					< (size_t)((m_nScale - 1) * (m_nScale - 1)))
				{
					if ((r != 0) || (c != 0))
					{
						continue;
					}
				}
			}

			m_ShowUnits.push_back(data);
		}
	}
	
	return true;
}

bool TerrainBrush::Generate()
{
	m_BrushUnits.clear();
	m_ShowUnits.clear();

	if (m_nShape == SHAPE_RECTANGLE)
	{
		CreateRectUnits();
	}
	else if (m_nShape == SHAPE_CIRCLE)
	{
		CreateCircleUnits();
	}
	
	CreateVertices();
	
	return true;
}

void TerrainBrush::Update(float seconds)
{
	if (!m_bVisible)
	{
		return;
	}

	if (!m_bActive)
	{
		return;
	}

	if (m_fActionPeriod <= 0.0F)
	{
		return;
	}

	if (m_nTrigMode == TRIG_CONTINUE)
	{
		// 定时连续触发动作
		m_fActionCounter += seconds;

		if (m_fActionCounter >= m_fActionPeriod)
		{
//			if (VisUtil_RunCallback(this, "on_action", 
//				CVarList() << m_fActionCounter) == 1)
//			{
				// 返回1表示地形顶点有改变必须重新生成刷子
//				Generate();
//			}

			m_fActionCounter = 0.0F;
		}
	}
	else
	{
		// 一次性触发
		if (!m_bTrigged)
		{
//			if (VisUtil_RunCallback(this, "on_action", CVarList()) == 1)
//			{
//				// 返回1表示地形顶点有改变必须重新生成刷子
//				Generate();
//			}

			m_bTrigged = true;
		}
	}
}

void TerrainBrush::Realize(float offset_seconds)
{
	if (!m_bVisible)
	{
		return;
	}

	if (NULL == m_pVertices)
	{
		return;
	}
	
	size_t line_num = m_ShowUnits.size() * 4;
	FmMat4 mat;

	FmMat4Identity(&mat);
	m_pTerrain->GetRender()->DrawDesignLine(mat, PRIMITIVE_LINELIST, 
		(unsigned int)line_num, m_pVertices, sizeof(vertex_line_t));
}
