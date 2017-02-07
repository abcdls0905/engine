//--------------------------------------------------------------------
// 文件名:		ImageData.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月14日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "image_data.h"
#include "../public/core_mem.h"

// CImageData

CImageData::CImageData()
{
	m_nLeft = 0;
	m_nTop = 0;
	m_nRight = 0;
	m_nBottom = 0;
	m_pFlip = NULL;
}

CImageData::~CImageData()
{
	if (m_pFlip)
	{
		CORE_DELETE(m_pFlip);
	}
}

void CImageData::Copy(const CImageData& src)
{
	m_strTex = src.m_strTex;
	m_nLeft = src.m_nLeft;
	m_nTop = src.m_nTop;
	m_nRight = src.m_nRight;
	m_nBottom = src.m_nBottom;
	
	if (src.m_pFlip)
	{
		CreateFlip();
		
		m_pFlip->m_SubImages = src.m_pFlip->m_SubImages;
	}
	else
	{
		DeleteFlip();
	}
}

void CImageData::CreateFlip()
{
	if (NULL == m_pFlip)
	{
		m_pFlip = CORE_NEW(CImageFlip);
		m_pFlip->m_fCounter = 0.0f;
		m_pFlip->m_nCurrent = 0;
	}
}

void CImageData::DeleteFlip()
{
	if (m_pFlip)
	{
		CORE_DELETE(m_pFlip);
		m_pFlip = NULL;
	}
}

void CImageData::ResetFlip()
{
	if (m_pFlip)
	{
		m_pFlip->m_fCounter = 0.0f;
		m_pFlip->m_nCurrent = 0;
	}
}

void CImageData::ExecuteFlip(float seconds)
{
	if (NULL == m_pFlip)
	{
		return;
	}

	CImageFlip::SUB_CONTAINER& images = m_pFlip->m_SubImages;

	if (images.empty())
	{
		return;
	}

	size_t current = m_pFlip->m_nCurrent;

	if (current >= images.size())
	{
		m_pFlip->m_nCurrent = 0;
		return;
	}

	while (seconds > 0.0f)
	{
		float remain = images[current].fSeconds - m_pFlip->m_fCounter;

		if (seconds > remain)
		{
			current++;

			if (current >= images.size())
			{
				current = 0;
			}

			seconds -= remain;
			m_pFlip->m_nCurrent = current;
			m_pFlip->m_fCounter = 0.0f;
		}
		else
		{
			m_pFlip->m_fCounter += seconds;
			break;
		}
	}
}

void CImageData::GetCurrent(int& left, int& top, int& right, int& bottom)
{
	if (m_pFlip)
	{
		size_t current = m_pFlip->m_nCurrent;

		if (current < m_pFlip->m_SubImages.size())
		{
			left = m_pFlip->m_SubImages[current].nLeft;
			top = m_pFlip->m_SubImages[current].nTop;
			right = m_pFlip->m_SubImages[current].nRight;
			bottom = m_pFlip->m_SubImages[current].nBottom;
			return;
		}
	}

	left = m_nLeft;
	top = m_nTop;
	right = m_nRight;
	bottom = m_nBottom;
}

