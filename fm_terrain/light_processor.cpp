//--------------------------------------------------------------------
// 文件名:		light_processor.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年11月5日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
/*
#include "light_processor.h"
#include "light_geometry.h"
#include "light_set.h"
#include "light_oct_tree.h"
#include "lighter.h"
#include "../public/core_mem.h"

#define LIGHTPRC_TRACE_NUM 1000
#define LIGHTPRC_SMOOTH_NUM 1000
#define LIGHTPRC_BLUR_NUM 500

// CLightProcessor

CLightProcessor::CLightProcessor()
{
	m_pLighter = NULL;
	m_pGeometry = NULL;
	m_pLights = NULL;
	m_pOctTree = NULL;

	m_nShadowTriangleCount = -1;
	m_nSmoothVertexCount = -1;
	m_nBlurVertexCount = -1;
}

CLightProcessor::~CLightProcessor()
{
}

void CLightProcessor::Init(Lighter* lighter, CLightGeometry* g, 
						   CLightSet* lit, CLightOctTree* ot)
{
	m_pLighter = lighter;
	m_pGeometry = g;
	m_pLights = lit;
	m_pOctTree = ot;
}

void CLightProcessor::Process()
{
	if (m_nShadowTriangleCount >= 0)
	{
		if (m_pGeometry->GetTriangleCount() > 0)
		{
			CalcShadows();
		}
		else
		{
			m_nShadowTriangleCount = -1;		
		}

		if (m_nShadowTriangleCount == -1)
		{
			vertex_t** vertices = m_pGeometry->GetVertices();
			size_t vertex_num = m_pGeometry->GetVertexCount();
			size_t light_num = m_pLights->GetCount();

			for (size_t i = 0; i < vertex_num; i++)
			{
				vertex_t* v = vertices[i];

				for (size_t j = 0; j < light_num; j++)
				{
					if (v->shadow[j].nrm > 0.0)
					{
						v->shadow[j].v /= v->shadow[j].nrm;
					}
					else
					{
						v->shadow[j].v = 1.0;
					}

					v->shadow[j].sm = v->shadow[j].v;
				}
			}

			CalcLights();
		}
		
		return;
	}

	if (m_nSmoothVertexCount >= 0)
	{
		SmoothShadows();

		if (m_nSmoothVertexCount == -1)
		{
			CalcLights();
		}
	}

	if (m_nBlurVertexCount >= 0)
	{
		BlurLight();

		if (m_nBlurVertexCount == -1)
		{
			CalcLights();
		}
	}

	if (m_pLighter->GetTraceShadows())
	{
		m_nShadowTriangleCount = 0;

		m_pLighter->SetTraceShadows(false);
		
		vertex_t** vertices = m_pGeometry->GetVertices();
		size_t vertex_num = m_pGeometry->GetVertexCount();
		size_t light_num = m_pLights->GetCount();

		for (size_t i = 0; i < vertex_num; i++)
		{
			vertex_t* v = vertices[i];
			
			for (size_t j = 0; j < light_num; j++)
			{
				v->shadow[j].v = 0.0f;
				v->shadow[j].nrm = 0.0f;
				v->shadow[j].sm = 0.0f;
			}
		}
	}

	if (m_pLighter->GetSmoothShadows())
	{
		m_nSmoothVertexCount = 0;
		
		m_pLighter->SetSmoothShadows(false);
	}

	if (m_pLighter->GetBlurLight())
	{
		CalcLights();

		m_nBlurVertexCount = 0;

		m_pLighter->SetBlurLight(false);
	}

	if (m_pLighter->GetResetBlurLight())
	{
		vertex_t** vertices = m_pGeometry->GetVertices();
		size_t vertex_num = m_pGeometry->GetVertexCount();

		for (size_t i = 0; i < vertex_num; i++)
		{
			vertices[i]->bc = FmVec4(0.0f, 0.0f, 0.0f);
		}

		m_pLighter->SetResetBlurLight(false);
		
		CalcLights();
	}

	if (m_pLighter->GetUpdateLight())
	{		
		CalcLights();
		
		m_pLighter->SetUpdateLight(false);
	}
}

void CLightProcessor::UpdateLightsParam()
{
	float cs;
	//float att;
	float dst;
	FmVec4 nrm;
	FmVec4 local;

	size_t vertex_num = m_pGeometry->GetVertexCount();
	size_t light_num = m_pLights->GetCount();
	vertex_t** vertices = m_pGeometry->GetVertices();

	for (size_t i = 0; i < vertex_num; i++)
	{
		vertex_t* v = vertices[i];

		for (size_t j = 0; j < light_num; j++)
		{
			light_t* light = m_pLights->GetLight(j);
			
			shadow_t& swh = v->shadow[j];

			switch (light->type)
			{
			case light_t::t_amb:
				swh.cs = 1.0f;
				//swh.att = 1.0f;
				//swh.csatt = float(swh.cs * swh.att);
				swh.shw = 1.0f;
				break;
			case light_t::t_sun:
				cs = D3DXVec3Dot(&light->dir, &v->n);

				if (cs < 0.0f)
				{
					cs = 0.0f;
				}

				swh.cs = cs;
				//swh.att = 1.0f;
				//swh.csatt = float(swh.cs * swh.att);
				swh.shw = 1.0f;
				break;
			case light_t::t_sky:
				cs = v->n.y;

				if (cs < 0.0f)
				{
					cs = 0.0f;
				}

				swh.cs = v->n.y;
				//swh.att = 1.0f;
				//swh.csatt = float(swh.cs * swh.att);
				swh.shw = 1.0f;
				break;
			case light_t::t_point:
				if (!m_pLighter->GetUsePointLight())
				{
					break;
				}

				nrm = light->pos - v->p;
				dst = D3DXVec3Length(&nrm);

				if (dst > 0.0f)
				{
					nrm *= 1.0f / dst;
					cs = D3DXVec3Dot(&nrm, &v->n);
				}
				else
				{
					cs = 1.0f;
				}

				if (cs < 0.0f)
				{
					cs = 0.0f;
				}

				swh.cs = cs;
				//swh.att = att;
				swh.dst = dst;
				//swh.csatt = float(swh.cs * swh.att);
				swh.shw = 1.0f;
				break;
			case light_t::t_spot:
				if (!m_pLighter->GetUsePointLight())
				{
					break;
				}

				nrm = light->pos - v->p;
				dst = D3DXVec3Length(&nrm);

				if (dst > 0.0f)
				{
					nrm *= 1.0f / dst;
					cs = D3DXVec3Dot(&nrm, &v->n);
				}
				else
				{
					cs = 1.0f;
				}

				if (cs < 0.0f)
				{
					cs = 0.0f;
				}

				swh.cs = cs;
				swh.dst = dst;
				swh.shw = 1.0f;
				break;
			case light_t::t_box:
				if (!m_pLighter->GetUsePointLight())
				{
					break;
				}

				// 转换到光源的本地空间
				FmVec3TransformCoord(&local, &v->p, &light->mtxWorldInverse);

				local.x = fabsf(local.x);
				local.y = fabsf(local.y);
				local.z = fabsf(local.z);

				dst = max(local.x, local.y);
				dst = max(dst, local.z);

				swh.cs = 1.0f;
				swh.dst = dst;
				swh.shw = 1.0f;
				break;
			default:
				Assert(0);
				break;
			}

			//swh.csatt = float(swh.cs * swh.att);
			//swh.shw = 1.0f;
		}
	}	
}

void CLightProcessor::CalcShadows()
{
	size_t triangle_num = m_pGeometry->GetTriangleCount();
	triangle_t** triangles = m_pGeometry->GetTriangles();
	
	for (size_t i = 0; i < LIGHTPRC_TRACE_NUM; i++)
	{
		if (size_t(m_nShadowTriangleCount) >= triangle_num)
		{
			break;
		}
		
		ApplyTriangleShadows(*triangles[m_nShadowTriangleCount]);

		m_nShadowTriangleCount++;
	}

	if (m_nShadowTriangleCount == triangle_num)
	{
		m_nShadowTriangleCount = -1;
	}
}

void CLightProcessor::ApplyTriangleShadows(triangle_t& t)
{
	size_t light_num = m_pLights->GetCount();

	vertex_t** vertices = m_pGeometry->GetVertices();

	float geo_radius = m_pGeometry->GetRadius();
	float shadow_dist = m_pLighter->GetShadowDistance();

	for (size_t i = 0; i < light_num; i++)
	{
		light_t* light = m_pLights->GetLight(i);
		
		if ((light->type == light_t::t_none) 
			|| (light->type == light_t::t_amb))
		{
			continue;
		}

		vertex_t* v0 = vertices[t.i[0]];
		vertex_t* v1 = vertices[t.i[1]];
		vertex_t* v2 = vertices[t.i[2]];

		v0->shadow[i].nrm += t.sq;
		v1->shadow[i].nrm += t.sq;
		v2->shadow[i].nrm += t.sq;

		FmVec4 pnt = t.p;
		
		switch (light->type)
		{
		case light_t::t_sun:
			if (D3DXVec3Dot(&light->dir, &t.n) >= 0.0f)
			{
				//double val = t.sq;
				float val = t.sq;

				if (shadow_dist > 0.0f)
				{
					trace_info_t ti;

					if (m_pGeometry->TraceDetail(pnt, 
						pnt + light->dir * geo_radius, ti))
					{
						if (shadow_dist > 0)
						{
							float f = ti.fDistance * (geo_radius / shadow_dist);

							if (f > 1.0f)
							{
								f = 1.0f;
							}

							val *= f * 0.8f;
						}
						else
						{
							val = 0.0f;
						}
					}
				}
				else
				{
					if (m_pGeometry->TraceHitted(pnt, 
						pnt + light->dir * geo_radius))
					{
						val = 0.0f;
					}
				}

				v0->shadow[i].v += val;
				v1->shadow[i].v += val;
				v2->shadow[i].v += val;
			}
			break;
		case light_t::t_sky:
			if (!m_pLighter->GetUseSkyLight())
			{
				break;
			}
			
			if (t.n.y >= 0.0f)
			{
				float sky = 0.0;
				float rad = geo_radius;
				float rdx = geo_radius * 0.2F;

				if (!m_pGeometry->TraceHitted(
					pnt, pnt + FmVec4(0.0f, rad, 0.0f)))
				{
					sky += 1.0f / 5.0f;
				}

				if (!m_pGeometry->TraceHitted(
					pnt, pnt + FmVec4(rdx, rad, 0.0f)))
				{
					sky += 1.0f / 5.0f;
				}

				if (!m_pGeometry->TraceHitted(
					pnt, pnt + FmVec4(-rdx, rad, 0.0f)))
				{
					sky += 1.0f / 5.0f;
				}

				if (!m_pGeometry->TraceHitted(
					pnt, pnt + FmVec4(0.0f, rad, rdx)))
				{
					sky += 1.0f / 5.0f;
				}

				if (!m_pGeometry->TraceHitted(
					pnt, pnt + FmVec4(0.0f, rad, -rdx)))
				{
					sky += 1.0f / 5.0f;
				}

				sky *= t.sq;
				
				v0->shadow[i].v += sky;
				v1->shadow[i].v += sky;
				v2->shadow[i].v += sky;
			}
			break;
		case light_t::t_point:
			if (!m_pLighter->GetUsePointLight())
			{
				break;
			}

			if (D3DXVec3LengthSq(&(light->pos - pnt)) 
				> (light->range * light->range))
			{
				// 超过点光源范围
				break;
			}

			if (D3DXVec3Dot(&(light->pos - pnt), &t.n) >= 0.0f)
			{
				if (!m_pGeometry->TraceHitted(pnt, light->pos))
				{
					v0->shadow[i].v += t.sq;
					v1->shadow[i].v += t.sq;
					v2->shadow[i].v += t.sq;
				}
			}
			break;
		case light_t::t_spot:
			if (!m_pLighter->GetUsePointLight())
			{
				break;
			}

			if (D3DXVec3LengthSq(&(light->pos - pnt)) 
				> (light->range * light->range))
			{
				// 超过光源范围
				break;
			}

			if (D3DXVec3Dot(&(light->pos - pnt), &t.n) >= 0.0f)
			{
				if (!m_pGeometry->TraceHitted(pnt, light->pos))
				{
					v0->shadow[i].v += t.sq;
					v1->shadow[i].v += t.sq;
					v2->shadow[i].v += t.sq;
				}
			}
			break;
		case light_t::t_box:
			break;
		default:
			Assert(0);
			//vrt[t.i[0]].shadow[i].v += t.sq;
			//vrt[t.i[1]].shadow[i].v += t.sq;
			//vrt[t.i[2]].shadow[i].v += t.sq;
			break;
		}
	}
}

void CLightProcessor::SmoothShadows()
{
	bool look_normal = m_pLighter->GetSmoothNormal();
	float smooth_rad = m_pLighter->GetSmoothRadius();
	float k_smooth_rad = 1.0f / smooth_rad;

	CLightSet& ls = *m_pLights;

	size_t light_num = ls.GetCount();

	vertex_t** vertices = m_pGeometry->GetVertices();
	size_t vertex_num = m_pGeometry->GetVertexCount();

	for (size_t i = 0; i < LIGHTPRC_SMOOTH_NUM; i++)
	{
		if (size_t(m_nSmoothVertexCount) >= vertex_num)
		{
			break;
		}
		
		vertex_t* v = vertices[m_nSmoothVertexCount];

		m_pOctTree->FindVerts(v->p, smooth_rad);

		oct_find_vertex_t* verts = m_pOctTree->GetFindVertices();

		size_t find_num = m_pOctTree->GetFindVertexCount();
		
		for (size_t n = 0; n < light_num; n++)
		{
			//double sm = 0.0;
			//double k_norm = 0.0f;
			float sm = 0.0f;
			float k_norm = 0.0f;

			for (size_t j = 0; j < find_num; j++)
			{
				if (look_normal)
				{
					if (D3DXVec3Dot(&v->n, &verts[j].v->n) <= 0.6f)
					{
						continue;
					}
				}

				//double k = sqrt(verts[j].r2) * k_smooth_rad;
				float k = sqrtf(verts[j].r2) * k_smooth_rad;

				if (k < 0.0f)
				{
					k = 0.0f;
				}

				if (k > 1.0f)
				{
					k = 1.0f;
				}

				k = 1.0f - k;
				sm += verts[j].v->shadow[n].v * k;
				k_norm += k;
			}

			if (k_norm > 0.0f)
			{
				sm /= k_norm;
			}
			else
			{
				sm = v->shadow[n].v;
			}

			v->shadow[n].sm = sm;
		}

		m_nSmoothVertexCount++;
	}

	if (size_t(m_nSmoothVertexCount) >= vertex_num)
	{
		m_nSmoothVertexCount = -1;
	}
}

void CLightProcessor::BlurLight()
{
	bool is_trace = m_pLighter->GetBlurTrace();
	float blur_rad = m_pLighter->GetBlurRadius();
	float k_blur_rad = 1.0f / blur_rad;
	float pw = m_pLighter->GetBlurAtt();
	float k_cos = m_pLighter->GetBlurCos();
	float k_cos1 = 1.0f - k_cos;

	vertex_t** vertices = m_pGeometry->GetVertices();
	size_t vertex_num = m_pGeometry->GetVertexCount();
	
	for (size_t i = 0; i < LIGHTPRC_BLUR_NUM; i++)
	{
		if (size_t(m_nBlurVertexCount) >= vertex_num)
		{
			break;
		}
		
		vertex_t* v = vertices[m_nBlurVertexCount];

		m_pOctTree->FindVerts(v->p, blur_rad);
		
		oct_find_vertex_t* verts = m_pOctTree->GetFindVertices();
		size_t find_num = m_pOctTree->GetFindVertexCount();

		size_t step = (find_num + 8) >> 4;

		if (step < 1)
		{
			step = 1;
		}

		double r = 0.0;
		double g = 0.0;
		double b = 0.0;
		double sum = 0.0;

		for (size_t j = 0; j < find_num; j += step)
		{
			vertex_t& vs = *verts[j].v;

			if (vs.c.x + vs.c.y + vs.c.z <= 0.0f)
			{
				continue;
			}

			FmVec4 n = vs.p - v->p;
			
			double css = -D3DXVec3Dot(&n, &vs.n);
			
			if (css <= 0.0)
			{
				continue;
			}

			double csd = D3DXVec3Dot(&n, &v->n);

			if (csd <= 0.0)
			{
				continue;
			}

			double dst = D3DXVec3Length(&n);

			if (dst <= 0.0)
			{
				continue;
			}

			double k = dst * k_blur_rad;

			dst = 1.0f / dst;

			n *= float(dst * 0.001);

			if (is_trace)
			{
				if (m_pGeometry->TraceHitted(v->p + n, vs.p - n))
				{
					continue;
				}
			}

			css *= dst; 
			csd *= dst;			

			if (css > 1.0)
			{
				css = 1.0;
			}

			if (csd > 1.0)
			{
				csd = 1.0;
			}

			if (k <= 0.0)
			{
				continue;
			}

			if (k > 1.0)
			{
				k = 1.0;
			}

			k = pow(1.0 - k, (double)pw) * (css * csd * k_cos + k_cos1);

			r += vs.c.x * k;
			g += vs.c.y * k;
			b += vs.c.z * k;

			sum += 1.0f;
		}

		if (sum > 0.0)
		{ 
			sum = 1.0 / sum;
			
			r *= sum; 
			g *= sum; 
			b *= sum; 
		}

		double max = r > g ? r : g;

		if (max < b)
		{
			max = b;
		}

		max = 1.0;

		if (max > 0.0)
		{ 
			max = 1.0 / max; 
			
			r *= max; 
			g *= max; 
			b *= max; 
		}

		v->bc.x = float(r);
		v->bc.y = float(g);
		v->bc.z = float(b);
		
		m_nBlurVertexCount++;
	}
	
	if (size_t(m_nBlurVertexCount) >= vertex_num)
	{
		m_nBlurVertexCount = -1;
	}
}

void CLightProcessor::CalcLights(bool is_cos, bool is_att, bool is_shadow)
{
	size_t light_num = m_pLights->GetCount();

	for (size_t i = 0; i < light_num; i++)
	{
		light_t* light = m_pLights->GetLight(i);
		
		if (!light->enable)
		{
			continue;
		}

		if ((light->type != light_t::t_sun) 
			&& (light->type != light_t::t_sky) 
			&& (light->type != light_t::t_point)
			&& (light->type != light_t::t_spot)
			&& (light->type != light_t::t_box))
		{
			continue;
		}

		if (light->gamma >= 0.5f)
		{
			light->curgm = 1.0f + (light->gamma - 0.5f) * 12.0f;
		}
		else
		{
			light->curgm = light->gamma + 0.5f;
		}

		light->curgm = 1.0f / light->curgm;
			
		if (light->contr >= 0.5f)
		{
			light->curct = 1.0f + (light->contr - 0.5f) * 20.0f;
		}
		else
		{
			light->curct = 0.05f + light->contr * 2.0f * 0.95f;
		}
	}

	float k_blur = m_pLighter->GetBlurCoeff();

	vertex_t** vertices = m_pGeometry->GetVertices();
	size_t vertex_num = m_pGeometry->GetVertexCount();

	for (size_t n = 0; n < vertex_num; n++)
	{
		vertex_t* v = vertices[n];

		if (NULL == v->shadow)
		{
			continue;
		}

		FmVec4 c = v->bc * (k_blur * k_blur * 2.0f);
		FmVec4 shdow_color;
		
		for (size_t i = 0; i < light_num; i++)
		{
			light_t* light = m_pLights->GetLight(i);
			
			if (!light->enable)
			{
				continue;
			}

			shadow_t& shw = v->shadow[i];

			switch (light->type)
			{
			case light_t::t_amb:
				shdow_color = light->color;

				// 环境遮挡信息
				shdow_color.x *= v->ao.x;
				shdow_color.y *= v->ao.y;
				shdow_color.z *= v->ao.z;

				c += shdow_color;
				break;
			case light_t::t_sun:
				if (is_cos)
				{
					shw.csatt = light->cosine * shw.cs 
						+ (1.0f - light->cosine);
				}

				if (is_shadow)
				{
					double vl = (shw.sm - 0.5) * light->curct + 0.5;

					if (vl < 0.0)
					{
						vl = 0.0;
					}

					if (vl > 1.0)
					{
						vl = 1.0;
					}

					float sw = float(pow(vl, (double)light->curgm)) 
						+ (light->bright - 0.5f) * 1.8f;

					if (sw < 0.0f)
					{
						sw = 0.0f;
					}

					if (sw > 1.0f)
					{
						sw = 1.0f;
					}

					shw.shw = light->shadow * sw + (1.0f - light->shadow);
				}

				shdow_color = light->color * (shw.csatt * shw.shw);
				c += shdow_color;
				break;
			case light_t::t_sky:
				if (is_cos)
				{
					shw.csatt = light->cosine * shw.cs 
						+ (1.0f - light->cosine);
				}

				if (is_shadow)
				{
					double vl = (shw.sm - 0.5) * light->curct + 0.5;
					
					if (vl < 0.0)
					{
						vl = 0.0;
					}

					if (vl > 1.0)
					{
						vl = 1.0;
					}

					float sw = float(pow(vl, (double)light->curgm)) 
						+ (light->bright - 0.5f) * 1.8f;

					if (sw < 0.0f)
					{
						sw = 0.0f;
					}

					if (sw > 1.0f)
					{
						sw = 1.0f;
					}

					shw.shw = light->shadow * sw + (1.0f - light->shadow);
				}

				shdow_color = light->color * (shw.csatt * shw.shw);
				c += shdow_color;
				break;
			case light_t::t_point:
				if (!m_pLighter->GetUsePointLight())
				{
					break;
				}

				if (is_att)
				{
					if (shw.dst < light->range)
					{
						//float factor = light->att0 + shw.dst * light->att1 
						//	+ shw.dst * shw.dst * light->att2;
				
						//if (factor > 0.0f)
						//{
						//	shw.att = 1.0f / factor;
						//}
						//else
						//{
						//	shw.att = 0.0f;
						//}

						float d = max(0.0f, 
							shw.dst / light->range + light->att0);

						shw.att = max((1.0f - light->att1 * d), 0.0f) 
							/ max(light->att2 * d * d, 1.0f);
					}
					else
					{
						shw.att = 0.0f;
					}
				}
				
				if (is_cos || is_att)
				{
					shw.csatt = (light->cosine * shw.cs 
						+ (1.0f - light->cosine)) * shw.att;
				}

				if (is_shadow)
				{
					float sw = float(shw.sm);

					if (sw < 0.0f)
					{
						sw = 0.0f;
					}

					if (sw > 1.0f)
					{
						sw = 1.0f;
					}

					shw.shw = light->shadow * sw;
				}

				shdow_color = light->color * (shw.csatt * shw.shw);
				c += shdow_color;
				break;
			case light_t::t_spot:
				if (!m_pLighter->GetUsePointLight())
				{
					break;
				}

				if (is_att)
				{
					FmVec4 to_light = light->pos - v->p;
					
					FmVec3Normalize(&to_light, &to_light);

					// L(light direction vector) * D(direction to vertex)
					float rho = D3DXVec3Dot(&light->dir, &to_light);
					float cos_phi = cosf(light->outer * 0.5f);

					if ((shw.dst < light->range) && (rho >= cos_phi))
					{
						float cos_theta = cosf(light->inner * 0.5f);
						float d = max(0.0f, 
							shw.dst / light->range + light->att0);

						shw.att = max(0.0f, 
							1.0f - (light->att1 * d + light->att2 * d * d));
						// pow((L * D - cos(phi / 2)) / (cos(theta / 2) - cos(phi / 2)), falloff)
						shw.att *= pow((rho - cos_phi) / (cos_theta - cos_phi), 
							light->falloff);
					}
					else
					{
						shw.att = 0.0f;
					}
				}

				if (is_cos || is_att)
				{
					shw.csatt = (light->cosine * shw.cs 
						+ (1.0f - light->cosine)) * shw.att;
				}

				if (is_shadow)
				{
					float sw = float(shw.sm);

					if (sw < 0.0f)
					{
						sw = 0.0f;
					}

					if (sw > 1.0f)
					{
						sw = 1.0f;
					}

					shw.shw = light->shadow * sw;
				}

				shdow_color = light->color * (shw.csatt * shw.shw);
				c += shdow_color;
				break;
			case light_t::t_box:
				if (!m_pLighter->GetUsePointLight())
				{
					break;
				}

				if (is_att)
				{
					if (shw.dst < 1.0f)
					{
						float d = max(shw.dst + light->att0, 0.0f);

						shw.att = max(0.0f, 
							1.0f - (light->att1 * d + light->att2 * d * d));
					}
					else
					{
						shw.att = 0.0f;
					}
				}

				if (is_cos || is_att)
				{
					shw.csatt = (light->cosine * shw.cs 
						+ (1.0f - light->cosine)) * shw.att;
				}

				shdow_color = light->color * shw.csatt;
				c += shdow_color;
				break;
			default:
				Assert(0);
				break;
			}
		}

		v->c = c * 0.5f * 0.588f;
	}

	m_pGeometry->UpdateModelColors();
	m_pGeometry->UpdateZoneColors();
}
*/
