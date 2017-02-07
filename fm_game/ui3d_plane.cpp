#include "ui3d_plane.h"

#define FLOAT float
#define FALSE false
extern IShaderTex* g_pShaderTexture;

inline bool GetRayTrace(IRenderContext* pContext, FmMat4& matWorld, int screen_x, int screen_y, FmVec3* start, FmVec3* direction)
{
	if (NULL == pContext)
		return false;

	int view_width = pContext->GetViewWidth();
	int view_height = pContext->GetViewHeight();
	const FmMat4& mtxProj = pContext->GetCamera().mtxProj;
	
	// Compute the vector of the pick ray in screen space
	FmVec3 v1;

	v1.x = (((2.0F * screen_x) / (float)view_width) - 1.0F) / mtxProj._11;
	v1.y = -(((2.0F * screen_y) / (float)view_height) - 1.0F) / mtxProj._22;
	v1.z = 1.0F;

	 // Get the inverse view matrix
	const FmMat4& matView = pContext->GetCamera().mtxView;
	FmMat4 mWorldView = matWorld * matView;
	FmMat4 m;
	::FmMat4Inverse( &m, NULL, &matWorld );

	//const FmMat4& m = pContext->GetCamera().mtxViewInverse;

	// Transform the screen space pick ray into 3D space
	direction->x = v1.x * m._11 + v1.y * m._21 + v1.z * m._31;
	direction->y = v1.x * m._12 + v1.y * m._22 + v1.z * m._32;
	direction->z = v1.x * m._13 + v1.y * m._23 + v1.z * m._33;
	FmVec3Normalize(direction, direction);


	start->x = m._41;
	start->y = m._42;
	start->z = m._43;

	return true;
}

inline bool IntersectTriangle( const FmVec3& orig, const FmVec3& dir,
                        FmVec3& v0, FmVec3& v1, FmVec3& v2,
                        FLOAT* t, FLOAT* u, FLOAT* v )
{
    // Find vectors for two edges sharing vert0
    FmVec3 edge1 = v1 - v0;
    FmVec3 edge2 = v2 - v0;

    // Begin calculating determinant - also used to calculate U parameter
    FmVec3 pvec;
    FmVec3Cross( &pvec, &dir, &edge2 );

    // If determinant is near zero, ray lies in plane of triangle
    FLOAT det = FmVec3Dot( &edge1, &pvec );

    FmVec3 tvec;
    if( det > 0 )
    {
        tvec = orig - v0;
    }
    else
    {
        tvec = v0 - orig;
        det = -det;
    }

    if( det < 0.0001f )
        return FALSE;

    // Calculate U parameter and test bounds
    *u = FmVec3Dot( &tvec, &pvec );
    if( *u < 0.0f || *u > det )
        return FALSE;

    // Prepare to test V parameter
    FmVec3 qvec;
    FmVec3Cross( &qvec, &tvec, &edge1 );

    // Calculate V parameter and test bounds
    *v = FmVec3Dot( &dir, &qvec );
    if( *v < 0.0f || *u + *v > det )
        return FALSE;

    // Calculate t, scale parameters, ray intersects triangle
    *t = FmVec3Dot( &edge2, &qvec );
    FLOAT fInvDet = 1.0f / det;
    *t *= fInvDet;
    *u *= fInvDet;
    *v *= fInvDet;

    return true;
}

void UIPlane::SetPosition(float x, float y, float z)
{
	m_pos = FmVec3(x, y, z); 
	BuildWorldMatrix();
}

void UIPlane::SetScale(float x, float y, float z)
{
	m_scale = FmVec3(x, y, z); 
	BuildWorldMatrix();
}

void UIPlane::SetAngle(float x, float y, float z)
{
	m_angle = FmVec3(x, y, z); 
	BuildWorldMatrix();
}


void UIPlane::SetTexture(ITexture* pTexture)
{
	if(pTexture != m_texture)
	{
		SAFE_RELEASE(m_texture);
		m_texture = pTexture;
	}
}

bool UIPlane::Pick(IRenderContext* pContext, int screen_x, int screen_y, FmVec2& uv, float& t)
{
	FmMat4 mtxWorld  = m_mtxWorldParent * m_mtxWorld;
	FmVec3 start, dir;
	if(!GetRayTrace(pContext, mtxWorld, screen_x, screen_y, &start, &dir))
		return false;
	
	float u, v;
	int v1,v2,v3;
	if(IntersectTriangle(start, dir, m_vb_data[0].pos, m_vb_data[1].pos, m_vb_data[2].pos, &t, &u, &v))
	{
		v1 = 0;
		v2 = 1;
		v3 = 2;
	}
	else if(IntersectTriangle(start, dir, m_vb_data[3].pos, m_vb_data[4].pos, m_vb_data[5].pos, &t, &u, &v))
	{
		v1 = 3;
		v2 = 4;
		v3 = 5;
	}
	else
	{
		return false;
	}

	uv = m_vb_data[v1].uv * (1.0F - u - v) + m_vb_data[v2].uv * u + m_vb_data[v3].uv * v;
	return true;
}

void UIPlane::Realize()
{
	IRenderContext* pContext = g_render->GetContext();
	const FmMat4& mtxView = pContext->GetCamera().mtxView;
	FmMat4 mtxWorld  = m_mtxWorldParent * m_mtxWorld;
	float camera_z = mtxView._13 * mtxWorld._41 
		+ mtxView._23 * mtxWorld._42 
		+ mtxView._33 * mtxWorld._43 + mtxView._43;
	g_render->GetSceneView()->AddBlendBatch(s_render, this, camera_z, "UIPlane::Realize");
}

void UIPlane::render()
{
	CreateVB();
	m_shader->UsedShader();
	
	IRenderDrawOp* drawop = g_render->GetRenderDrawOp();
	g_render->GetRenderStateOp()->EnableCullFace(false);
	
	if(m_texture)
	{
		fm_int id_texture = m_shader->GetParamOp()->FindParamIdByName("tex_BaseMap");
		if(g_pShaderTexture)
			m_shader->GetParamOp()->SetTexture2D(id_texture, g_pShaderTexture->GetTexture());
		else
			m_shader->GetParamOp()->SetTexture2D(id_texture, m_texture->GetShaderTex()->GetTexture());
	}
	drawop->SetVB(m_vb->GetBuffer());
	drawop->EnableVertexAttribArray(0);
	drawop->SetVertexAttribPointer(0, 3, IRenderDrawOp::VERTEX_DATA_FLOAT, sizeof(Vertex), 0);
	drawop->EnableVertexAttribArray(1);
	drawop->SetVertexAttribPointer(1, 2, IRenderDrawOp::VERTEX_DATA_FLOAT, sizeof(Vertex), (void*) (3 * sizeof(float)));
	bool depth_test = g_render->GetRenderStateOp()->IsEnableDepthTest();

	// ÉèÖÃWVP
	IRenderContext* pContext = g_render->GetContext();
	const FmVec3& refer_pos = pContext->GetReferPosition();

	
	FmMat4 mtxTemp = m_mtxWorldParent * m_mtxWorld;
	
	FmMat4 mtxWVP;

	mtxTemp._41 -= refer_pos.x;
	mtxTemp._42 -= refer_pos.y;
	mtxTemp._43 -= refer_pos.z;
	FmMat4Multiply(&mtxWVP, &mtxTemp, &pContext->GetCamera().mtxViewProjRelative);
	FmMat4Transpose(&mtxWVP, &mtxWVP);
	
	fm_int id_mtxWVP = m_shader->GetParamOp()->FindParamIdByName("c_mtxWVP");
	m_shader->GetParamOp()->SetParamValue(id_mtxWVP, mtxWVP);

	fm_int id_alpha = m_shader->GetParamOp()->FindParamIdByName("c_alpha");
	m_shader->GetParamOp()->SetParamValue(id_alpha, m_alpha);
	drawop->DrawArrays(IRenderDrawOp::DRAW_TRIANGLES, 0, 6);
}

void UIPlane::s_render(void* data)
{
	((UIPlane*)data)->render();
}

void UIPlane::CreateVB()
{
	if(NULL == m_vb)
	{
		m_vb = g_render->CreateStaticVB(m_vb_data, 6*sizeof(Vertex));
	}

}

void UIPlane::Build(float w, float h, float _maxu, float _maxv)
{
	float half_w = w/2.0f;
	float half_h = h/2.0f;

	FmVec3 v1(-half_w, half_h, 0.0f);
	FmVec3 v2(-half_w, -half_h, 0.0f);
	FmVec3 v3(half_w, half_h, 0.0f);
	FmVec3 v4(half_w, -half_h, 0.0f);

	float maxu = _maxu; 
	if(m_texture)
		maxu = m_texture->GetOriginWidth() / float(m_texture->GetWidth());
	float maxv = _maxv;
	if(m_texture)
		maxv = m_texture->GetOriginHeight() / float(m_texture->GetHeight());
	FmVec2 uv1(0.0f, 0.0f);
	FmVec2 uv2(0.0f, maxv);
	FmVec2 uv3(maxu, 0.0f);
	FmVec2 uv4(maxu, maxv);


	m_vb_data[0].pos = v2; 
	m_vb_data[1].pos = v1;
	m_vb_data[2].pos = v4;
	m_vb_data[3].pos = v4;
	m_vb_data[4].pos = v1;
	m_vb_data[5].pos = v3;

	m_vb_data[0].uv = uv2;
	m_vb_data[1].uv = uv1;
	m_vb_data[2].uv = uv4;
	m_vb_data[3].uv = uv4;
	m_vb_data[4].uv = uv1;
	m_vb_data[5].uv = uv3;
}