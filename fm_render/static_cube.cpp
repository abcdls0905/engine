//--------------------------------------------------------------------
// �ļ���:		static_cube.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2012��9��26��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "static_cube.h"
#include "render.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"

CStaticCube::CStaticCube(Render* pRender)
{
	Assert(pRender != NULL);
	
	m_pRender = pRender;
	m_nIndex = -1;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nLevels = 0;
	m_nFormat = TEX_FORMAT_NONE;
	m_nTexture = 0;
	m_pTexSampler = NULL;
}

CStaticCube::~CStaticCube()
{
	if (m_nTexture)
	{
		glDeleteTextures(1, &m_nTexture);
	}
}

void CStaticCube::Destroy()
{
	CORE_DELETE(this);
}

void CStaticCube::Release()
{
	SAFE_RELEASE(m_pTexSampler);
	m_pRender->ReleaseResource(m_nIndex);
}

bool CStaticCube::CreateFrom(fm_uint nTex)
{
	SAFE_RELEASE(m_pTexSampler);
	Assert(nTex != 0);

	m_nTexture = nTex;
    m_pTexSampler = m_pRender->CreateTextureSamper(this);
	return true;
}
