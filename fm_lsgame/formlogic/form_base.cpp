
#include "form_base.h"
#include "../public/i_var_list.h"
#include "../fm_gui/form.h"
#include "../global.h"
#include "../helper.h"
#include <sstream>
#include "guimanager.h"
#include "../util/logic_util.h"
#include "../util/gui_utils.h"
#include "../../fm_gui/string_util.h"


//
FormBase::FormBase(/*const char*szPathFile, bool bCenterAlign*/)
{
	m_strFormFile = "";

	//m_bCenterAlign = bCenterAlign;
}

//
FormBase::~FormBase()
{
}

bool FormBase::Shut()
{
	g_core->DeleteEntity(m_formID);
	return true;
}

//
void FormBase::Load(bool bStatic, bool bAsyncLoad, bool bCenter)
{
	if(!g_core->GetEntity(m_formID))
	{
		Create(bStatic, bAsyncLoad, bCenter);
	}

	Form* pForm = (Form*)g_core->GetEntity(m_formID);
	if (pForm != NULL)
	{
		pForm->SetVisible(false);
	}
}

// 载入控件，但是不显示
void FormBase::LoadAndHide( bool bStatic, bool bShowModal, bool bAsyncLoad, bool bCenter)
{
	//
	GuiManager* pGuiManager = (GuiManager*)CHelper::GetGlobalEntity("GuiManager");
	if (pGuiManager == NULL) 
	{
		return;
	}

	//
	UIConfig uiConfig;
	std::string strEntName = GetEntInfo()->GetEntityName();
	if(pGuiManager->GetFormConfig(strEntName, uiConfig))
	{
		bStatic = uiConfig.bStatic;
		bAsyncLoad = uiConfig.bAsyncLoad;
		bCenter = uiConfig.bCenter;
	}

	if (m_strFormFile.empty())
	{
		m_strFormFile = uiConfig.strFormPath;
		if (m_strFormFile.empty())
		{
			TraceLog(LOG_ERROR, "[FormBase::Show] Form:%s Config File is NULL", strEntName.c_str());
			return;
		}
	}


	// 如果界面不存在，load界面
	Form* pForm = (Form*)g_core->GetEntity(m_formID);
	if(!pForm)
	{
		Load(bStatic, bAsyncLoad, bCenter);
	}
	
	// 如果界面load成功，加入UI体系
	pForm = (Form*)g_core->GetEntity(m_formID);
	if (pForm != NULL)
	{
		pForm->SetFormFile(m_strFormFile.c_str());
		pForm->SetFormName(StringUtil::FormFileToName(m_strFormFile).c_str());

		if (bShowModal)
		{
			pForm->ShowModal();
		}
		else
		{
			pForm->Show();
		}
		pForm->SetVisible(false);
	}
}

//
void FormBase::Show()
{
	bool bStatic = false;
	bool bAsyncLoad = false;

	//
	LoadAndHide(bStatic, false, bAsyncLoad);

	//Load(bStatic);
	//
	Form* pForm = (Form*)g_core->GetEntity(m_formID);
	if (pForm != NULL)
	{
		pForm->SetVisible(true);
		OnShow(true);
	}
}

void FormBase::OnShow(bool bShow)
{
}
void FormBase::OnHide()
{
}
//
void FormBase::ShowModal()
{
	bool bStatic = false;
	bool bAsyncLoad = false;

	//
	LoadAndHide(bStatic, true, bAsyncLoad);

	//Load(bStatic);
	//
	Form* pForm = (Form*)g_core->GetEntity(m_formID);
	if (pForm != NULL)
	{
		pForm->SetVisible(true);
		OnShow(true);
	}
}

//
void FormBase::Hide()
{
	Form* pForm = (Form*)g_core->GetEntity(m_formID);
	if (pForm != NULL)
	{
		pForm->SetVisible(false);
	}

	//
	OnHide();
}

//
void FormBase::Close()
{
	Form* pForm = (Form*)g_core->GetEntity(m_formID);
	if (NULL == pForm)
	{
		return;
	}

	//
	pForm->Close();

	//
	OnClosed();
    
    //
    g_core->DeleteEntity(m_formID);
}

//最大化（充满整个窗口）
void FormBase::Maximize()
{
	Form* pForm = (Form*)g_core->GetEntity(m_formID);
	if (NULL == pForm)
	{
		return;
	}

	GuiUtils::gui_maximize_form(pForm);
}

//
bool FormBase::IsShow()
{
	Form* pForm = (Form*)g_core->GetEntity(m_formID);
	if (NULL == pForm)
	{
		return false;
	}

	//
	return pForm->GetVisible();
}

//
Form* FormBase::GetMainForm()
{
	return (Form*)g_core->GetEntity(m_formID);
}


//
bool FormBase::Create(bool bStatic, bool bAsyncLoad, bool bCenter)
{
	if(NULL == g_core->GetEntity(m_formID))
	{
		m_formID = GuiUtils::gui_loadform(m_strFormFile.c_str(), bStatic, bAsyncLoad);
		if (m_formID.IsNull())
		{
			return false;
		}

		if (bCenter)
		{
			GuiUtils::gui_center((IControl*)g_core->GetEntity(m_formID));
		}

		//
		OnCreate();
	}

	//
	if (NULL == g_core->GetEntity(m_formID))
	{
		TraceLog(LOG_ERROR, "[FormBase::Create]  Create form:%s Fail", m_strFormFile.c_str());
		return false;
	}

	return true;
}

//
IControl* FormBase::GetControl(const char* szPathName)
{
	if(!szPathName || StringEmpty(szPathName))
	{
		return NULL;
	}

	//
	IControl* pControl = GetMainForm();
	if(!pControl)
	{
		return NULL;
	}

	char szControlName[512];
	std::stringstream strmTemp;
	strmTemp<<szPathName;

	//
	while ( !strmTemp.eof() )
	{
		strmTemp.getline(szControlName, sizeof(szControlName), '.');

		IControl* pChild = pControl->QueryControl(szControlName);
		if (NULL == pChild)
		{
			return NULL;
		}

		pControl = pChild;
	}

	return pControl;
}
