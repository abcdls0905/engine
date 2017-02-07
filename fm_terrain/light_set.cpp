//--------------------------------------------------------------------
// 文件名:		light_set.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年11月5日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
/*
#include "light_set.h"
#include "light_info.h"

// CLightSet

CLightSet::CLightSet()
{
	//m_Lights.resize(3);
	m_Lights.push_back(CORE_NEW(light_t));
	m_Lights.push_back(CORE_NEW(light_t));
	m_Lights.push_back(CORE_NEW(light_t));

	for (size_t i = 0; i < m_Lights.size(); i++)
	{
		SetDefLightParam(i);	
	}

	m_Lights[0]->type = light_t::t_amb;
	m_Lights[0]->color = FmVec4(0.2f, 0.2f, 0.2f);
	m_Lights[0]->enable = false;

	m_Lights[1]->type = light_t::t_sun;
	m_Lights[1]->color = FmVec4(0.9f, 0.9f, 0.6f);
	//m_Lights[1].pos = FmVec4(1.0f, 1.0f, 1.0f);
	m_Lights[1]->dir = FmVec4(1.0f, 1.0f, 1.0f);
	m_Lights[1]->enable = false;

	m_Lights[2]->type = light_t::t_sky;
	m_Lights[2]->color = FmVec4(0.3f, 0.5f, 1.0f);
	m_Lights[2]->enable = false;
}

CLightSet::~CLightSet()
{
	for (size_t i = 0; i < m_Lights.size(); ++i)
	{
		CORE_DELETE(m_Lights[i]);
	}
}

void CLightSet::SetDefLightParam(int i)
{
	m_Lights[i]->cosine = 1.0f;
	m_Lights[i]->shadow = 0.8f;
	m_Lights[i]->bright = 0.5f;
	m_Lights[i]->contr = 0.5f;
	m_Lights[i]->gamma = 0.5f;
	m_Lights[i]->enable = false;
}

bool CLightSet::SetSunDirection(const FmVec4& dir)
{
	FmVec3Normalize(&m_Lights[1]->dir, &dir);
	return true;
}

bool CLightSet::AddLight(LightInfo* info)
{
	Assert(info != NULL);

	const light_data_t* pData = info->GetData();
	FmVec4 color;
	
	color.x = pData->fColorRed;
	color.y = pData->fColorGreen;
	color.z = pData->fColorBlue;
	color *= pData->fIntensity;
	
	if (stricmp(info->GetName(), "ambient") == 0)
	{
		m_Lights[0]->red = pData->fColorRed;
		m_Lights[0]->green = pData->fColorGreen;
		m_Lights[0]->blue = pData->fColorBlue;
		m_Lights[0]->color = color;
		m_Lights[0]->group = "ambient";
		m_Lights[0]->enable = pData->bTurnOn;
	}
	else if (stricmp(info->GetName(), "sun") == 0)
	{
		m_Lights[1]->red = pData->fColorRed;
		m_Lights[1]->green = pData->fColorGreen;
		m_Lights[1]->blue = pData->fColorBlue;
		m_Lights[1]->color = color;
		m_Lights[1]->cosine = pData->fCosine;
		m_Lights[1]->shadow = pData->fShadow;
		m_Lights[1]->bright = pData->fBright;
		m_Lights[1]->contr = pData->fContrast;
		m_Lights[1]->gamma = pData->fGamma;
		m_Lights[1]->group = "sun";
		m_Lights[1]->enable = pData->bTurnOn;
	}
	else if (stricmp(info->GetName(), "sky") == 0)
	{
		m_Lights[2]->red = pData->fColorRed;
		m_Lights[2]->green = pData->fColorGreen;
		m_Lights[2]->blue = pData->fColorBlue;
		m_Lights[2]->color = color;
		m_Lights[2]->cosine = pData->fCosine;
		m_Lights[2]->shadow = pData->fShadow;
		m_Lights[2]->bright = pData->fBright;
		m_Lights[2]->contr = pData->fContrast;
		m_Lights[2]->gamma = pData->fGamma;
		m_Lights[2]->group = "sky";
		m_Lights[2]->enable = pData->bTurnOn;
	}
	else
	{
		size_t light_num = m_Lights.size();
		
		m_Lights.push_back(CORE_NEW(light_t));
		
		SetDefLightParam(light_num);

		FmMat4 mtxRotate;

		FmMat4RotationYawPitchRoll(&mtxRotate, pData->fAngleY, 
			pData->fAngleX, pData->fAngleZ);

		FmVec4 dir(0.0f, 1.0f, 0.0f);

		D3DXVec3TransformNormal(&dir, &dir, &mtxRotate);

		FmVec4 pos(pData->fPosiX, pData->fPosiY, pData->fPosiZ);
		FmVec4 size(pData->fSizeX, pData->fSizeY, pData->fSizeZ);

		m_Lights[light_num]->red = pData->fColorRed;
		m_Lights[light_num]->green = pData->fColorGreen;
		m_Lights[light_num]->blue = pData->fColorBlue;
		m_Lights[light_num]->pos = pos;
		m_Lights[light_num]->dir = dir;
		m_Lights[light_num]->size = size;
		m_Lights[light_num]->color = color;
		m_Lights[light_num]->cosine = pData->fCosine;
		m_Lights[light_num]->shadow = pData->fShadow;
		m_Lights[light_num]->bright = pData->fBright;
		m_Lights[light_num]->contr = pData->fContrast;
		m_Lights[light_num]->gamma = pData->fGamma;
		m_Lights[light_num]->att0 = pData->fAtt0;
		m_Lights[light_num]->att1 = pData->fAtt1;
		m_Lights[light_num]->att2 = pData->fAtt2;
		m_Lights[light_num]->range = pData->fRange;
		m_Lights[light_num]->inner = D3DXToRadian(pData->fInnerDegree);
		m_Lights[light_num]->outer = D3DXToRadian(pData->fOuterDegree);
		m_Lights[light_num]->falloff = pData->fFalloff;
		m_Lights[light_num]->group = info->GetName();
		//m_Lights[light_num].type = light_t::t_point;
		m_Lights[light_num]->type = pData->nType;
		m_Lights[light_num]->enable = pData->bTurnOn;

		if (pData->nType == light_t::t_box)
		{
			m_Lights[light_num]->range = D3DXVec3Length(&size) * 0.5f;

			FmVec4 scale = size * 0.5f;
			FmMat4 mtxScaling;
			FmMat4 mtxWorld;

			FmMat4Scaling(&mtxScaling, scale.x, scale.y, scale.z);
			FmMat4Multiply(&mtxWorld, &mtxScaling, &mtxRotate);

			mtxWorld._41 = pos.x;
			mtxWorld._42 = pos.y;
			mtxWorld._43 = pos.z;

			FmMat4Inverse(&m_Lights[light_num]->mtxWorldInverse, NULL,
				&mtxWorld);
		}
	}
	
	return true;
}

bool CLightSet::SetLight(LightInfo* info)
{
	Assert(info != NULL);
	
	const light_data_t* pData = info->GetData();
	
	for (size_t i = 0; i < m_Lights.size(); ++i)
	{
		light_t* light = m_Lights[i];
		
		if (stricmp(info->GetName(), light->group.c_str()) == 0)
		{
			FmVec4 color;
			
			color.x = pData->fColorRed;
			color.y = pData->fColorGreen;
			color.z = pData->fColorBlue;
			color *= pData->fIntensity * 2.0f;
			
			light->color = color;
			light->cosine = pData->fCosine;
			light->shadow = pData->fShadow;
			light->bright = pData->fBright;
			light->contr = pData->fContrast;
			light->gamma = pData->fGamma;
			light->att0 = pData->fAtt0;
			light->att1 = pData->fAtt1;
			light->att2 = pData->fAtt2;
		}
	}

	return true;
}
*/
