#include "node.h"
#include "../visual/i_render.h"
#include "shader.h"
#include "../visual/vis_utils.h"
#include "global.h"
//extern bool TestErr(const char* ident);
_UI3D_BEGIN


/*
inline bool GetRayTrace(IRenderContext* pContext, const FmMat4& matWorld, int screen_x, int screen_y, FmVec3* start, FmVec3* direction)
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
	::FmMat4Inverse( &m, NULL, &mWorldView );

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
*/

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
	FmMat4 mWorldView;
	mWorldView = matWorld * matView;
//	FmMatrixMultiply(&mWorldView, &matView, &matWorld);
	FmMat4 m;
	::FmMat4Inverse( &m, NULL, &mWorldView );
	
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
                        float* t, float* u, float* v )
{
    // Find vectors for two edges sharing vert0
    FmVec3 edge1 = v1 - v0;
    FmVec3 edge2 = v2 - v0;

    // Begin calculating determinant - also used to calculate U parameter
    FmVec3 pvec;
    FmVec3Cross( &pvec, &dir, &edge2 );

    // If determinant is near zero, ray lies in plane of triangle
    float det = FmVec3Dot( &edge1, &pvec );

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
        return false;

    // Calculate U parameter and test bounds
    *u = FmVec3Dot( &tvec, &pvec );
    if( *u < 0.0f || *u > det )
        return false;

    // Prepare to test V parameter
    FmVec3 qvec;
    FmVec3Cross( &qvec, &tvec, &edge1 );

    // Calculate V parameter and test bounds
    *v = FmVec3Dot( &dir, &qvec );
    if( *v < 0.0f || *u + *v > det )
        return false;

    // Calculate t, scale parameters, ray intersects triangle
    *t = FmVec3Dot( &edge2, &qvec );
    float fInvDet = 1.0f / det;
    *t *= fInvDet;
    *u *= fInvDet;
    *v *= fInvDet;

    return true;
}

inline FmVec3& GetValueVec3(char* data, int data_size, int index, int stride, int offset)
{
	int len = stride * index + offset;
	Assert(len < data_size);
	FmVec3* p = (FmVec3*)(data + len);
	return *p;
}

inline FmVec2& GetValueVec2(char* data, int data_size, int index, int stride, int offset)
{
	int len = stride * index + offset;
	Assert(len < data_size);
	FmVec2* p = (FmVec2*)(data + len);
	return *p;
}


Node::Node()
:m_vb(0)
,m_ib(0)
,m_vb_data(0)
,m_ib_data(0)
,m_ib_size(0)
,m_vb_size(0)
,m_stride(0)
,m_tex0(0)
{
	m_pos = FmVec3(0,0,0);
	m_angle = FmVec3(0,0,0);
	m_scale = FmVec3(1,1,1);
	BuildWorldMatrix();
}

Node::~Node()
{
	if(m_vb_data)
	{
		delete(m_vb_data);
	}

	if(m_ib_data)
	{
		delete(m_ib_data);
	}
}

void Node::Create(int fvf, char * vb_data, int vb_data_size, short* ib_data, int ib_data_size)
{
	int ps_flags = 0;
	int vs_flags = 0;

	m_shader = Shader::Inst().GetShader(0, 0);
	m_vb_data = vb_data;
	m_ib_data = ib_data;
	m_ib_size = ib_data_size;
	m_vb_size = vb_data_size;
	m_fvf = fvf;
	if(fvf & FVF_POS)
	{
		m_stride += sizeof(float)*3;
	}

	if(fvf & FVF_TEX0)
	{
		m_stride += sizeof(float)*2;
	}
}

void Node::CreateVBIB()
{
	if(m_vb == 0 && m_vb_data)
		m_vb = g_fm_ui3d_gloabl.pRender->CreateStaticVB(m_vb_data, m_vb_size);
	if(m_ib == 0 && m_ib_data)
		m_ib = g_fm_ui3d_gloabl.pRender->CreateStaticIB(m_ib_data, m_ib_size);
}

void Node::s_render(void* data)
{
	Node* pThis = (Node*)data;
	pThis->render();
}

void Node::render()
{
	CreateVBIB();

	int ps_flags = 0, vs_flags = 0;
	
	bool istex = m_tex0 != 0;
	ps_flags |= size_t(istex) << UI3D_PS_TEX;
	vs_flags |= size_t(istex) << UI3D_VS_TEX;
	m_shader = Shader::Inst().GetShader(vs_flags, ps_flags);

	m_shader->UsedShader();
	IRenderDrawOp* drawop = g_fm_ui3d_gloabl.pRender->GetRenderDrawOp();
	g_fm_ui3d_gloabl.pRender->GetRenderStateOp()->EnableCullFace(false);
	if(m_ib_data == 0)
	{
		drawop->SetVB(m_vb->GetBuffer());

		int offset = 0;

		//FVF
		if(m_fvf & FVF_POS)
		{
			drawop->EnableVertexAttribArray(0);
			drawop->SetVertexAttribPointer(0, 3, IRenderDrawOp::VERTEX_DATA_FLOAT, m_stride, (void*)offset);
			offset += sizeof(float)*3;
		}

		if(m_fvf & FVF_TEX0)
		{
			drawop->EnableVertexAttribArray(1);
			drawop->SetVertexAttribPointer(1, 2, IRenderDrawOp::VERTEX_DATA_FLOAT, m_stride, (void*) offset);
			offset += sizeof(float)*2;
		}

		//TEXTURE
		if(m_tex0 && m_tex0->IsReady())
		{
			fm_int id_texture = m_shader->GetParamOp()->FindParamIdByName("tex_BaseMap");
			if(id_texture > -1)
			{
				m_shader->GetParamOp()->SetTexture2D(id_texture, m_tex0->GetShaderTex()->GetTexture());
			}
			else
			{
				CORE_TRACE_EX("Node::render() set texture failed, can't find tex_basemap");
			}
		}

		//WVP
		IRenderContext* pContext = g_fm_ui3d_gloabl.pRender->GetContext();
		const FmVec3& refer_pos = pContext->GetReferPosition();
		FmMat4 mtxTemp = m_mtxWorld;
		FmMat4 mtxWVP;
		mtxTemp._41 -= refer_pos.x;
		mtxTemp._42 -= refer_pos.y;
		mtxTemp._43 -= refer_pos.z;
		FmMat4Multiply(&mtxWVP, &mtxTemp, &pContext->GetCamera().mtxViewProjRelative);
		FmMat4Transpose(&mtxWVP, &mtxWVP);
		fm_int id_mtxWVP = m_shader->GetParamOp()->FindParamIdByName("c_mtxWVP");
		m_shader->GetParamOp()->SetParamValue(id_mtxWVP, mtxWVP);


		//»æÖÆ
		drawop->DrawArrays(IRenderDrawOp::DRAW_TRIANGLES, 0, m_vb_size/m_stride);
	}
	g_fm_ui3d_gloabl.pRender->GetRenderStateOp()->EnableCullFace(true);
//	TestErr("");
}

void Node::Draw()
{
	IRenderContext* pContext = g_fm_ui3d_gloabl.pRender->GetContext();
	const FmMat4& mtxView = pContext->GetCamera().mtxView;
	FmMat4 mtxWorld  = m_mtxWorld;
	float camera_z = mtxView._13 * mtxWorld._41 
		+ mtxView._23 * mtxWorld._42 
		+ mtxView._33 * mtxWorld._43 + mtxView._43;
	//g_fm_ui3d_gloabl.pRender->GetSceneView()->AddSolidBatch(s_render, this, "Node::Draw");
	g_fm_ui3d_gloabl.pRender->GetSceneView()->AddBlendBatch(s_render, this, camera_z, "Node::Draw");
}

void Node::BuildWorldMatrix()
{
	FmQuat rotate;
	FmQuatRotationYawPitchRoll(&rotate, m_angle.y, m_angle.x, m_angle.z);
	VisUtil_MatrixTransform(&m_mtxWorld, &m_scale, &rotate, &m_pos);
	if(m_mtxGlobal)
		m_mtxWorld = *m_mtxGlobal * m_mtxWorld;
}

bool Node::Pick(IRenderContext* pContext, int screen_x, int screen_y, FmVec2& uv, float& t)
{
	FmVec3 start, dir;
	
	if(!GetRayTrace(pContext, m_mtxWorld, screen_x, screen_y, &start, &dir))
		return false;

	float max_t = 10000;
	FmVec2 _uv;
	bool bfind = false;
	if(this->m_ib_data == 0 && m_vb_data)
	{
		int count = m_vb_size / m_stride;
		for(int i = 0; i < count; i+= 6)
		{
			float u, v;
			int v1,v2,v3;
			FmVec3& pos1 = GetValueVec3(m_vb_data, m_vb_size, i+0, m_stride, 0);
			FmVec3& pos2 = GetValueVec3(m_vb_data, m_vb_size, i+1, m_stride, 0);
			FmVec3& pos3 = GetValueVec3(m_vb_data, m_vb_size, i+2, m_stride, 0);
			FmVec3& pos4 = GetValueVec3(m_vb_data, m_vb_size, i+3, m_stride, 0);
			FmVec3& pos5 = GetValueVec3(m_vb_data, m_vb_size, i+4, m_stride, 0);
			FmVec3& pos6 = GetValueVec3(m_vb_data, m_vb_size, i+5, m_stride, 0);
			
			if(IntersectTriangle(start, dir, pos1, pos2, pos3, &t, &u, &v))
			{
				v1 = 0;
				v2 = 1;
				v3 = 2;
			}
			else if(IntersectTriangle(start, dir, pos4, pos5, pos6, &t, &u, &v))
			{
				v1 = 3;
				v2 = 4;
				v3 = 5;
			}
			else
			{
				continue;
			}

			if(m_fvf & FVF_TEX0)
			{
				int offset = sizeof(FmVec3);
				FmVec2 uv1 = GetValueVec2(m_vb_data, m_vb_size, i+v1, this->m_stride, offset);
				FmVec2 uv2 = GetValueVec2(m_vb_data, m_vb_size, i+v2, this->m_stride, offset);
				FmVec2 uv3 = GetValueVec2(m_vb_data, m_vb_size, i+v3, this->m_stride, offset);
				_uv = uv1 * (1.0F - u - v) + uv2 * u + uv3 * v;
			}
		
			if(t < max_t)
			{
				bfind = true;
				max_t = t;
				_uv = _uv;
			}
		}
		
		if(bfind)
		{
			uv = _uv;
			t = max_t;
			return true;
		}
	}
	return false;
}

_UI3D_END