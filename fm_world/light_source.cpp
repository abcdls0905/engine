//--------------------------------------------------------------------
// 文件名:		light_source.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年9月29日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "light_source.h"
#include "../visual/vis_utils.h"
#include "../visual/i_context.h"
#include "../visual/i_texture.h"
#include "../fm_world/model.h"
#include "../visual/i_model_player.h"
#include "../public/core_log.h"
#include "../visual/i_render.h"

/// \file light_source.cpp
/// \brief 光源

/// entity: LightSource
/// \brief 光源实体
/// \see 继承自IVisBase
DECLARE_ENTITY(LightSource, 0, IVisBase);

/// readonly: object Bind
/// \brief 当前绑定的对象
DECLARE_PROPERTY_GET(PERSISTID, LightSource, Bind, GetBindID);
// property: bool TraceEanble
/// \brief 是否检测碰撞
DECLARE_PROPERTY(bool, LightSource, TraceEnable, GetTraceEnable, 
	SetTraceEnable);
/// property: bool Visible
/// \brief 是否可见
DECLARE_PROPERTY(bool, LightSource, Visible, GetVisible, SetVisible);
/// property: string Name
/// \brief 灯光名字
DECLARE_PROPERTY(const char*, LightSource, Name, GetName, SetName);
/// property: string Texture
/// \brief 灯光叠加贴图
DECLARE_PROPERTY(const char*, LightSource, Texture, GetTexture, SetTexture);
/// property: string LightType
/// \brief 光源类型（"point"-点光源，"spot"-聚光灯，"box"-盒子光源）
DECLARE_PROPERTY(result_string, LightSource, LightType, GetLightTypeString, 
	SetLightTypeString);
/// property: float Range
/// \brief 范围
DECLARE_PROPERTY(float, LightSource, Range, GetRange, SetRange);
/// property: string Color
/// \brief 灯光颜色
DECLARE_PROPERTY(result_string, LightSource, Color, GetColorString, 
	SetColorString);
/// property: float Intensity
/// \brief 亮度
DECLARE_PROPERTY(float, LightSource, Intensity, GetIntensity, SetIntensity);
/// property: float Attenu0
/// \brief 衰减系数0
DECLARE_PROPERTY(float, LightSource, Attenu0, GetAttenu0, SetAttenu0);
/// property: float Attenu1
/// \brief 衰减系数1
DECLARE_PROPERTY(float, LightSource, Attenu1, GetAttenu1, SetAttenu1);
/// property: float Attenu2
/// \brief 衰减系数2
DECLARE_PROPERTY(float, LightSource, Attenu2, GetAttenu2, SetAttenu2);
/// property: float InnerDegree
/// \brief 聚光灯内角度
DECLARE_PROPERTY(float, LightSource, InnerDegree, GetInnerDegree, 
	SetInnerDegree);
/// property: float OuterDegree
/// \brief 聚光灯外角度
DECLARE_PROPERTY(float, LightSource, OuterDegree, GetOuterDegree, 
	SetOuterDegree);
/// property: float Falloff
/// \brief 聚光灯下降系数
DECLARE_PROPERTY(float, LightSource, Falloff, GetFalloff, SetFalloff);
/// property: float BoxScaleX
/// \bried 盒子尺寸X
DECLARE_PROPERTY(float, LightSource, BoxScaleX, GetBoxScaleX, SetBoxScaleX);
/// property: float BoxScaleY
/// \bried 盒子尺寸Y
DECLARE_PROPERTY(float, LightSource, BoxScaleY, GetBoxScaleY, SetBoxScaleY);
/// property: float BoxScaleZ
/// \bried 盒子尺寸Z
DECLARE_PROPERTY(float, LightSource, BoxScaleZ, GetBoxScaleZ, SetBoxScaleZ);
/// property: float Blink
/// \brief 闪烁系数
DECLARE_PROPERTY(float, LightSource, Blink, GetBlink, SetBlink);
/// property: float BlinkPeriod
/// \brief 闪烁周期
DECLARE_PROPERTY(float, LightSource, BlinkPeriod, GetBlinkPeriod, 
	SetBlinkPeriod);
/// property: float BlinkTick
/// \brief 闪烁时刻
DECLARE_PROPERTY(float, LightSource, BlinkTick, GetBlinkTick, SetBlinkTick);
/// property: int ShadowMapSize
/// \brief 阴影图尺寸
DECLARE_PROPERTY(int, LightSource, ShadowMapSize, GetShadowMapSize, 
	SetShadowMapSize);
/// property: bool ShowBoundBox
/// \brief 是否显示包围盒
DECLARE_PROPERTY(bool, LightSource, ShowBoundBox, GetShowBoundBox, 
	SetShowBoundBox);
/// property: bool ShowRange
/// \brief 是否显示光照范围
DECLARE_PROPERTY(bool, LightSource, ShowRange, GetShowRange, SetShowRange);
/// property: bool AsyncLoad
/// \brief 是否异步加载
DECLARE_PROPERTY(bool, LightSource, AsyncLoad, GetAsyncLoad, SetAsyncLoad);
/// property: bool ShadowMapDynamic
/// \brief 是否强制每帧都要刷新阴影图
DECLARE_PROPERTY(bool, LightSource, ShadowMapDynamic, GetShadowMapDynamic, 
	SetShadowMapDynamic);

	 
// LightSource

LightSource::LightSource()
{
	m_vPosition = FmVec3(0.0F, 0.0F, 0.0F);
	m_vAngle = FmVec3(0.0F, 0.0F, 0.0F);
	m_vBoxScale = FmVec3(1.0F, 1.0F, 1.0F);
	m_nLightType = LIGHT_TYPE_POINT;
	m_nColor = 0xFFFFFFFF;
	m_fIntensity = 1.0F;
	m_fRange = 10.0F;
	m_fAttenu0 = 0.0F;
	m_fAttenu1 = 1.0F;
	m_fAttenu2 = 0.0F;
	m_fInnerDegree = 15.0F;
	m_fOuterDegree = 30.0F;
	m_fFalloff = 1.0F;

	m_fBlink = 1.05F;
	m_fCurIntensity = 1.0F;
	m_fDistance = 0.0F;
	m_fBlinkPeriod = 0.0F;
	m_fBlinkTick = 0.0F;
	m_fBlinkCounter = 0.0F;
	m_fDistTime = 0.0F;
	m_nHelperID = -1;
	m_strHelperName = "";

	m_nShadowMapSize = 0;
	m_bVisible = true;
	m_bTraceEnable = true;
	m_bComplete = false;
	m_bShowBoundBox = false;
	m_bShowRange = false;
	m_bAsyncLoad = false;
	m_bShadowMapDynamic = false;

	m_nActiveRoles = 0;
	m_pTexture = NULL;
//	memset(m_pCB, 0, sizeof(m_pCB));
//	memset(m_pShadowCB, 0, sizeof(m_pShadowCB));
	memset(m_pShadowTex, 0, sizeof(m_pShadowTex));
	memset(m_ShadowViewMatrix, 0, sizeof(m_ShadowViewMatrix));
	memset(m_ShadowProjMatrix, 0, sizeof(m_ShadowViewMatrix));
}

LightSource::~LightSource()
{
	ClearShadowTex();
	SAFE_RELEASE(m_pTexture);

	for (size_t i = 0; i < 6; ++i)
	{
//		SAFE_RELEASE(m_pCB[i]);
//		SAFE_RELEASE(m_pShadowCB[i]);
	}
}

bool LightSource::Init(const IVarList& args)
{
	m_pRender = (IRender*)GetCore()->GetInterface("Render");
	if (NULL == m_pRender)
	{
		return false;
	} 
	return true;
}

bool LightSource::Shut()
{
	return true;
}

void LightSource::Update(float seconds)
{
	UpdateBind();

	if (m_fBlinkPeriod > 0.0F)
	{
		if (m_fBlinkCounter < m_fBlinkPeriod)
		{
			m_fBlinkCounter += seconds;
		}
		else
		{
			m_fCurIntensity = m_fIntensity;
			m_fBlinkCounter = 0;
		}

		m_fDistTime += seconds;

		if (m_fDistTime > m_fBlinkTick)
		{
			m_fDistTime = 0;

			if (m_fBlinkCounter < (m_fBlinkPeriod * 0.5F))
			{
				m_fCurIntensity /= m_fBlink;
			}
			else 
			{
				m_fCurIntensity *= m_fBlink;
			}
		}
	}
	else
	{
		m_fCurIntensity = m_fIntensity;
	}
}

// 画光照范围球体
static void draw_range_sphere(IRender* pRender, const FmVec3& center, 
	float radius, unsigned int color, size_t ring_num, size_t section_num)
{
	struct vertex_line_t
	{
		FmVec3 pos;
		unsigned int diffuse;
	};

	// 缩放系数
	float sx = 1.0F;
	float sy = 1.0F;
	float sz = 1.0F;
	size_t triangle_num = (ring_num + 1) * section_num * 2;
	size_t vertex_num = (ring_num + 1) * section_num + 2;
	vertex_line_t* pVertices = (vertex_line_t*)CORE_ALLOC(
		sizeof(vertex_line_t) * vertex_num);
	vertex_line_t* pLines = (vertex_line_t*)CORE_ALLOC(
		sizeof(vertex_line_t) * triangle_num * 6);

	// Generate vertices at the top and bottom points.
	FmVec3 top = center + FmVec3(0.0F, sy * radius, 0.0F);
	FmVec3 bottom = center + FmVec3(0.0F, -sy * radius, 0.0F);

	pVertices[0].pos = top;
	pVertices[vertex_num - 1].pos = bottom;

	// Generate vertex points for rings
	float dtheta = (FLOAT_PI / (ring_num + 2)); // Angle between each ring
	float dphi = (2 * FLOAT_PI / section_num); // Angle between each section
	float theta = dtheta;
	size_t n = 1; // vertex being generated, begins at 1 to skip top point
	size_t i;

	for (i = 0; i < (ring_num + 1); ++i)
	{
		float y = radius * cosf(theta); // y is the same for each ring
		float v = theta / FLOAT_PI; // v is the same for each ring
		float rsintheta = radius * sinf(theta);
		float phi = 0.0F;

		for (size_t j = 0; j < section_num; ++j)
		{
			float x = rsintheta * sinf(phi);
			float z = rsintheta * cosf(phi);
			float u = 1.0F - (phi / (2 * FLOAT_PI));
			FmVec3 normal = FmVec3(x / radius, y / radius, 
				z / radius);
			
			FmVec3Normalize(&normal, &normal);
			pVertices[n].pos = center + FmVec3(sx * x, sy * y, sz * z);
			phi += dphi;
			++n;
		}

		theta += dtheta;
	}

	for (i = 0; i < vertex_num; ++i)
	{
		pVertices[i].diffuse = color;
	}

	// Generate triangles for top and bottom caps.
	for (i = 0; i < section_num; ++i)
	{
		pLines[6 * i + 0] = pVertices[0];
		pLines[6 * i + 1] = pVertices[i + 1];
		pLines[6 * i + 2] = pVertices[i + 1];
		pLines[6 * i + 3] = pVertices[1 + ((i + 1) % section_num)];
		pLines[6 * i + 4] = pVertices[1 + ((i + 1) % section_num)];
		pLines[6 * i + 5] = pVertices[0];
		pLines[6 * (triangle_num - section_num + i) + 0] = 
			pVertices[vertex_num - 1];
		pLines[6 * (triangle_num - section_num + i) + 1] = 
			pVertices[vertex_num - 2 - i];
		pLines[6 * (triangle_num - section_num + i) + 2] = 
			pVertices[vertex_num - 2 - i];
		pLines[6 * (triangle_num - section_num + i) + 3] = 
			pVertices[vertex_num - 2 - ((1 + i) % section_num)];
		pLines[6 * (triangle_num - section_num + i) + 4] = 
			pVertices[vertex_num - 2 - ((1 + i) % section_num)];
		pLines[6 * (triangle_num - section_num + i) + 5] = 
			pVertices[vertex_num - 1];
	}

	// Generate triangles for the rings
	size_t m = 1; // first vertex in current ring,begins at 1 to skip top point

	n = section_num; // triangle being generated, skip the top cap 

	for (i = 0; i < ring_num; ++i)
	{
		for (size_t j = 0; j < section_num; ++j)
		{
			pLines[6 * n + 0] = pVertices[m + j];
			pLines[6 * n + 1] = pVertices[m + section_num + j];
			pLines[6 * n + 2] = pVertices[m + section_num + j];
			pLines[6 * n + 3] = 
				pVertices[m + section_num + ((j + 1) % section_num)];
			pLines[6 * n + 4] = 
				pVertices[m + section_num + ((j + 1) % section_num)];
			pLines[6 * n + 5] = pVertices[m + j];
			pLines[6 * (n + 1) + 0] = pLines[6 * n + 0];
			pLines[6 * (n + 1) + 1] = pLines[6 * n + 3];
			pLines[6 * (n + 1) + 2] = pLines[6 * n + 3];
			pLines[6 * (n + 1) + 3] = 
				pVertices[m + ((j + 1) % section_num)];
			pLines[6 * (n + 1) + 4] = 
				pVertices[m + ((j + 1) % section_num)];
			pLines[6 * (n + 1) + 5] = pLines[6 * n + 0];
			n += 2;
		}

		m += section_num;
	}

	FmMat4 mat;

	FmMat4Identity(&mat);
	pRender->DrawDesignLine(mat, 0, 
		(unsigned int)triangle_num * 3, pLines, sizeof(vertex_line_t));

	CORE_FREE(pVertices, sizeof(vertex_line_t) * vertex_num);
	CORE_FREE(pLines, sizeof(vertex_line_t) * triangle_num * 6);
}

// 画光照范围锥体
static void draw_range_cone(IRender* pRender, const FmVec3& center, 
	float range, const FmVec3& angle, float fov, unsigned int color, 
	int section_num)
{
	struct vertex_line_t
	{
		FmVec3 pos;
		unsigned int diffuse;
	};

	int vertex_num = section_num + 1;
	int line_num = section_num * 2;
	vertex_line_t* pVertices = (vertex_line_t*)CORE_ALLOC(
		sizeof(vertex_line_t) * vertex_num);
	vertex_line_t* pLines = (vertex_line_t*)CORE_ALLOC(
		sizeof(vertex_line_t) * line_num * 2);

	pVertices[0].pos = FmVec3(0.0F, 0.0F, 0.0F);

	int i;
	float theta = 0.0F;
	float radius = range * tanf(fov * 0.5F);

	for (i = 0; i < section_num; ++i)
	{
		pVertices[i + 1].pos.x = radius * cosf(theta);
		pVertices[i + 1].pos.z = radius * sinf(theta);
		pVertices[i + 1].pos.y = -range;
		theta += (FLOAT_PI * 2.0F) / section_num;
	}

	for (i = 0; i < vertex_num; ++i)
	{
		pVertices[i].diffuse = color;
	}

	vertex_line_t* pv = pLines;
	
	for (i = 0; i < section_num; ++i)
	{
		*pv++ = pVertices[0];
		*pv++ = pVertices[i + 1];
	}

	for (i = 0; i < section_num; ++i)
	{
		*pv++ = pVertices[i + 1];
		*pv++ = pVertices[(i + 2) % section_num];
	}

	FmMat4 mtxRotate;
	FmMat4 mtxTrans;
	FmMat4 mtxWorld;

	FmMat4RotationYawPitchRoll(&mtxRotate, angle.y, angle.x, angle.z);
	FmMat4Translation(&mtxTrans, center.x, center.y, center.z);
	FmMat4Multiply(&mtxWorld, &mtxRotate, &mtxTrans);
	pRender->DrawDesignLine(mtxWorld, 0, line_num, 
		pLines, sizeof(vertex_line_t));

	CORE_FREE(pVertices, sizeof(vertex_line_t) * vertex_num);
	CORE_FREE(pLines, sizeof(vertex_line_t) * line_num * 2);
}

void LightSource::Realize(float offset_seconds)
{
	if (!m_bVisible)
	{
		return;
	}

	if (m_bShowBoundBox)
	{
		FmMat4 mat;

		VisUtil_MatrixRotationTranslation(&mat, &m_vAngle, &m_vPosition); 
	    m_pRender->DrawBoundBox(mat, FmVec3(0.0F, 0.0F, 0.0F), GetBoxSize(), (const float&)0xFFFFFFFF);
	}

	if (m_bShowRange || m_bShowBoundBox)
	{
		if (m_nLightType == LIGHT_TYPE_SPOT)
		{
			draw_range_cone(m_pRender, GetPosition(), GetRange(), GetAngle(),
				(float)FM_TO_RADIAN(GetOuterDegree()), m_nColor, 16);
		}
		else if (m_nLightType == LIGHT_TYPE_BOX)
		{
			FmMat4 mat;

			VisUtil_MatrixRotationTranslation(&mat, &m_vAngle, &m_vPosition);

			m_pRender->DrawBoundBox( mat, FmVec3(0.0F, 0.0F, 0.0F), 
				m_vBoxScale, (const float&)m_nColor);
		}
		else
		{
			draw_range_sphere(m_pRender, GetPosition(), GetRange(), m_nColor, 
				8, 16);
		}
	}
}

bool LightSource::GetLoadFinish()
{
	if (m_pTexture)
	{
		if (!m_pTexture->IsLoadComplete())
		{
			return false;
		}
	}
	
	return true;
}

bool LightSource::SetAngle(float x, float y, float z)
{
	m_vAngle.x = x;
	m_vAngle.y = y;
	m_vAngle.z = z;

	ClearShadowTex();

	return true;
}

FmVec3 LightSource::GetAngle() const
{ 
	return m_vAngle; 
}

bool LightSource::SetPosition(float x, float y, float z)
{
	m_vPosition.x = x;
	m_vPosition.y = y;
	m_vPosition.z = z;

	ClearShadowTex();

	m_nActiveRoles = 0;

	return true;
}

FmVec3 LightSource::GetPosition() const
{ 
	return m_vPosition; 
}

FmVec3 LightSource::GetBoxSize()
{
	if (m_nLightType == LIGHT_TYPE_BOX)
	{
		return m_vBoxScale;
	}
	else
	{
		float d = m_fRange * 2.0F;

		return FmVec3(d, d, d);
	}
}

FmVec3 LightSource::GetCenter()
{
	return m_vPosition;
}

float LightSource::GetRadius()
{
	if (m_nLightType == LIGHT_TYPE_BOX)
	{
		return VisUtil_Vec3Length(&m_vBoxScale) * 0.5F;
	}
	else
	{
		return m_fRange;
	}
}

bool LightSource::TraceDetail(const FmVec3& src, const FmVec3& dst, 
	trace_info_t& result)
{
	if (!m_bVisible)
	{
		return false;
	}
	
	if (!m_bTraceEnable)
	{
		return false;
	}

	FmVec3 position = GetPosition();
	FmMat4 mat;

	FmMat4Translation(&mat, position.x, position.y, position.z);

	float scale = this->GetRadius();

	if (scale < 0.1F)
	{
		scale = 0.1F;
	}

	if (scale > 1.0F)
	{
		scale = 1.0F;
	}
	
	int old_hit_count = result.nHitCount;
	
	bool succeed = VisUtil_TraceBoundBox(mat, 
		FmVec3(0.0F, scale * 0.5F, 0.0F), 
		FmVec3(scale, scale, scale), src, dst, result);
	
	if (succeed)
	{
		// 设置碰撞到的对象ID
		if (result.nHitMax > 1)
		{
			for (int t = old_hit_count; t < result.nHitCount; ++t)
			{
				result.Hits[t].TraceID = this->GetID();
			}
		}
		else
		{
			result.Hits[0].TraceID = this->GetID();
		}
	}
	
	return succeed;
}

void LightSource::SetInnerDegree(float value)
{
	m_fInnerDegree = value;

	ClearShadowTex();
}

void LightSource::SetOuterDegree(float value)
{
	m_fOuterDegree = value;

	ClearShadowTex();
}

void LightSource::SetShadowMapSize(int value)
{
	m_nShadowMapSize = value;

	ClearShadowTex();
}

void LightSource::ClearShadowTex()
{
	for (int i = 0; i < 6; ++i)
	{
		SAFE_RELEASE(m_pShadowTex[i]);
	}
}

void LightSource::SetShadowTex(int index, IDepthRT* value)
{ 
	Assert(size_t(index) < 6);

	SAFE_RELEASE(m_pShadowTex[index]);
	m_pShadowTex[index] = value; 
}

IDepthRT* LightSource::GetShadowTex(int index) const
{ 
	Assert(size_t(index) < 6);

	return m_pShadowTex[index]; 
}

void LightSource::SetShadowViewMatrix(int index, const FmMat4& value)
{
	Assert(size_t(index) < 6);

	m_ShadowViewMatrix[index] = value;
}

const FmMat4& LightSource::GetShadowViewMatrix(int index) const
{
	Assert(size_t(index) < 6);

	return m_ShadowViewMatrix[index];
}

void LightSource::SetShadowProjMatrix(int index, const FmMat4& value)
{
	Assert(size_t(index) < 6);

	m_ShadowProjMatrix[index] = value;
}

const FmMat4& LightSource::GetShadowProjMatrix(int index) const
{
	Assert(size_t(index) < 6);

	return m_ShadowProjMatrix[index];
}

ITexture* LightSource::GetOverlayTex() const
{
	return m_pTexture;
}

/*
IDxDynamicCB* LightSource::GetConstBuffer(size_t index, unsigned int size)
{
	Assert(index < 6);

	if (NULL == m_pCB[index])
	{
		m_pCB[index] = m_pRender->CreateDynamicCB(size);
	}

	return m_pCB[index];
}
*/

void LightSource::SetColorString(const char* value)
{ 
	Assert(value != NULL);

	m_nColor = VisUtil_ColorToInt(value);
}

result_string LightSource::GetColorString() const
{ 
	return VisUtil_IntToColor(m_nColor);
}

void LightSource::SetLightTypeString(const char* value)
{
	Assert(value != NULL);

	if (stricmp(value, "point") == 0)
	{
		m_nLightType = LIGHT_TYPE_POINT;
	}
	else if (stricmp(value, "spot") == 0)
	{
		m_nLightType = LIGHT_TYPE_SPOT;
	}
	else if (stricmp(value, "box") == 0)
	{
		m_nLightType = LIGHT_TYPE_BOX;
	}
}

result_string LightSource::GetLightTypeString() const
{
	switch (m_nLightType)
	{
	case LIGHT_TYPE_POINT:
		return "point";
	case LIGHT_TYPE_SPOT:
		return "spot";
	case LIGHT_TYPE_BOX:
		return "box";
	default:
		Assert(0);
		break;
	}
	
	return result_string();
}

void LightSource::SetTexture(const char* value)
{
	Assert(value != NULL);

	m_strTexture = value;
	
	SAFE_RELEASE(m_pTexture);

	m_pTexture = m_pRender->CreateTexture(m_strTexture.c_str(), m_bAsyncLoad);
}

const char* LightSource::GetTexture() const
{
	return m_strTexture.c_str();
}

bool LightSource::IsViewClip()
{
	IRenderContext* pContext = m_pRender->GetContext();
	
	if (pContext->IsFarClip(m_vPosition, m_fRange))
	{
		return true;
	}

	return pContext->IsViewClip(m_vPosition, m_fRange);
}

bool LightSource::UpdateBind() 
{ 
	if (!m_BindID.IsNull())
	{
		IVisBase* pVisBase = (IVisBase*)m_pRender->GetCore()->GetEntity(
			m_BindID);

		if (pVisBase != NULL)
		{
			if (m_nHelperID != -1)
			{
				Model* pModel = (Model*)pVisBase;
				IModelPlayer* pModelPlayer = pModel->GetModelPlayer();

				FmMat4 mtxNode;
				FmMat4 mtxModel;

				pModelPlayer->GetLabelMatrix(m_nHelperID, mtxNode);
				pModel->GetWorldMatrix(mtxModel);

				FmMat4 mtxLink = mtxNode * mtxModel;
				FmVec3 pos(mtxLink._41, mtxLink._42, mtxLink._43);

				m_vPosition = pos;

				VisUtil_GetMatrixAngle(&m_vAngle, &mtxLink);
			}
			else
			{
				FmMat4 mat;

				pVisBase->GetWorldMatrix(mat);

				FmVec3TransformCoord(&m_vPosition, &m_vBindPos, &mat);
			}
		}
		else
		{
			// 跟随绑定对象一起消失
			m_bComplete = true;
		}
	}

	return true;
}

bool LightSource::LinkToObject(const PERSISTID& id, float pos_x, float pos_y, 
	float pos_z)
{
	IEntity* pEntity = (IEntity*)m_pRender->GetCore()->GetEntity(id);

	if (NULL == pEntity)
	{
		return false;
	}

	if (!pEntity->GetEntInfo()->IsKindOf("IVisBase"))
	{
		return false;
	}

	m_BindID = id;
	m_vBindPos = FmVec3(pos_x, pos_y, pos_z);
	m_nHelperID = -1;
	m_strHelperName = "";

	UpdateBind();

	return true;
}

bool LightSource::LinkToPoint(const PERSISTID& id, const char* helper_name)
{
	if (id.IsNull())
	{
		return false;
	}

	IEntity* pLink = (IEntity*)GetCore()->GetEntity(id);

	if (NULL == pLink)
	{
		return false;
	}

	if (!pLink->GetEntInfo()->IsKindOf("Model"))
	{
		return false;
	}

	Model* pModel = (Model*)pLink;
	IModelPlayer* pModelPlayer = pModel->GetModelPlayer();

	if (!pModelPlayer->IsReady())
	{
		// 模型未加载完成
		CORE_TRACE("(LightSource::LinkToPoint)model not load complete");
		return false;
	}

	int helper_id = pModelPlayer->FindLabelNameIndex(helper_name);

	if (-1 == helper_id)
	{	
		CORE_TRACE("(LightSource::LinkToPoint)not find helper");
		CORE_TRACE(helper_name);
		CORE_TRACE(pModelPlayer->GetModelFileName());
		return false;
	}

	m_BindID = id;
	m_nHelperID = helper_id;
	m_strHelperName = helper_name;

	return false;
}
