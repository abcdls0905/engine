//--------------------------------------------------------------------
// �ļ���:		image_list.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2007��5��22��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "image_list.h"
#include "gui_utils.h"
#include "../public/core_log.h"
#include "../public/core_mem.h"
#include "../visual/i_painter.h"

/// \file image_list.cpp
/// \brief ͼԪ�б�

/// entity: ImageList
/// \brief ͼԪ�б�ؼ�
DECLARE_ENTITY(ImageList, 0, IEntity);

// ImageList

ImageList::ImageList()
{
	m_pPainter = NULL;
	m_pGui = NULL;
}

ImageList::~ImageList()
{
	ClearImage();
}

bool ImageList::Init(const IVarList& args)
{
	const char* painter_name = args.StringVal(0);
	
	m_pPainter = (IPainter*)GetCore()->GetInterface(painter_name);
	
	if (NULL == m_pPainter)
	{
		CORE_TRACE("(ImageList::Init)no painter");
		CORE_TRACE(painter_name);
		return false;
	}
	
	m_pGui = (Gui*)GetCore()->GetEntity(args.ObjectVal(1));
	
	if (NULL == m_pGui)
	{
		CORE_TRACE("(ImageList::Init)no Gui");
		return false;
	}
	
	return true;
}

bool ImageList::Shut()
{
	ClearImage();

	SAFE_RELEASE(m_pPainter);
	
	return true;
}

bool ImageList::AddImage(const char* name)
{
	Assert(name != NULL);

	CImagePlayer* data = CORE_NEW(CImagePlayer);

	data->SetName(name);
	data->Create(m_pPainter, m_pGui, false);
	
	if (!data->IsReady())
	{
		CORE_DELETE(data);
		return false;
	}

	m_Images.push_back(data);

	return true;
}

bool ImageList::RemoveImage(const char* name)
{
	Assert(name != NULL);
	
	for (size_t i = 0; i < m_Images.size(); i++)
	{
		if (strcmp(m_Images[i]->GetName(), name) == 0)
		{
			m_Images[i]->Release();
			CORE_DELETE(m_Images[i]);
			m_Images.remove(i);
			return true;
		}
	}

	return false;
}

bool ImageList::ClearImage()
{
	for (size_t i = 0; i < m_Images.size(); i++)
	{
		m_Images[i]->Release();
		CORE_DELETE(m_Images[i]);
	}

	m_Images.clear();

	return true;
}

int ImageList::GetImageCount() const
{
	return (int)m_Images.size();
}

void ImageList::GetImageNameList(const IVarList& args, 
	IVarList& result) const
{
	result.Clear();

	for (size_t i = 0; i < m_Images.size(); ++i)
	{
		result.AddString(m_Images[i]->GetName());
	}
}

CImagePlayer* ImageList::GetImageByIndex(size_t index)
{
	if (index >= m_Images.size())
	{
		return NULL;
	}

	return m_Images[index];
}

