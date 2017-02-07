#pragma once
#include "../visual/i_render.h"
#include "../visual/i_resource.h"
#include "../visual/i_texture.h"
#include "../fm_gui/gui.h"
#include "../fm_gui/form.h"
#include "../fm_gui/designer.h"
#include <vector>
#include <string>
#include "../visual/i_vis_base.h"
#include "global.h"

class RenderUI : public IVisBase
{
	struct Item
	{
		std::string form_file;
		PERSISTID ui;
		ITexture* pRenderTexture;
		IFrameRT* pFrameRT;
		float ratio;
	};
	std::vector<Item> m_items;
public:
	virtual bool Init(const IVarList& args) { return true; }

	virtual bool Shut() { return true; }

	void Realize()
	{
		IFrameRT* old = g_fm_ui3d_gloabl.pRender->GetUsedFrameRT();
	 
		Gui* pGui = (Gui*)g_pCore->LookupEntity("Gui");
		IPainter* painter = (IPainter*)g_pCore->GetInterface("Painter");
		//画到rt上强制使用非拉伸方式
		IFrameRT* pOldFrameRT = old;
		bool scale_enable = pGui->GetScaleEnable();
		pGui->SetScaleEnable(false);

		
		for(size_t i = 0; i < m_items.size(); i++)
		{
			IFrameRT* pFrameRT = m_items[i].pFrameRT;
			painter->SetViewport(0, 0, pFrameRT->GetColorRT()->GetWidth(), pFrameRT->GetColorRT()->GetHeight(), false);
			painter->SetFrameRT(pFrameRT);
			IColorRT* pColorRT = (IColorRT*)m_items[i].pRenderTexture->GetShaderTex();
			IControl* pControl = (IControl*)g_pCore->GetEntity(m_items[i].ui);
			pControl->Paint(0);
			painter->ResetViewport();
		}
		pGui->SetScaleEnable(scale_enable);
		painter->SetFrameRT(pOldFrameRT);
        g_fm_ui3d_gloabl.pRender->UsedFrameRT(old);
	}

	void LoadUI(const char* form_file)
	{
		//加载ui
		Gui* pGui = (Gui*)g_pCore->LookupEntity("Gui");
		Designer* pDesigner = (Designer*)g_pCore->GetEntity(pGui->GetDesigner());
		PERSISTID form_id = pDesigner->LoadForm(g_pCore->GetWorkPath(),form_file);
		Form* pForm = (Form*)g_pCore->GetEntity(form_id);
		LoadForm(form_file, pForm);

		
	}

	long LoadForm(const char* form_file, Form* pForm)
	{
		Item item;

		item.pRenderTexture = g_fm_ui3d_gloabl.pRender->CreateRenderTexture(form_file, pForm->GetWidth(), pForm->GetHeight(), TEX_FORMAT_RGBA8888_UNORM);
		if(item.pRenderTexture == 0)
		{
			item.pRenderTexture = g_fm_ui3d_gloabl.pRender->CreateRenderTexture(form_file, pForm->GetWidth(), pForm->GetHeight(), TEX_FORMAT_RGBA4444_UNORM);
			if(item.pRenderTexture == 0)
			{
				CORE_TRACE("CreateRenderTexture TEX_FORMAT_RGBA4444_UNORM error");
			}
		}

		item.pFrameRT = g_fm_ui3d_gloabl.pRender->CreateFrameRT((IColorRT*)item.pRenderTexture->GetShaderTex());

		item.ratio = pForm->GetWidth() / float(pForm->GetHeight());
		item.ui = pForm->GetID();
		item.form_file = form_file;
		m_items.push_back(item);
		
	
		return (int)m_items.size() - 1;
	}

	void RemoveForm(const char* form_file)
	{
		for(size_t i = 0; i< m_items.size(); i++)
		{
			if(stricmp(form_file, m_items[i].form_file.c_str()) == 0)
			{
				SAFE_RELEASE(m_items[i].pFrameRT);
				SAFE_RELEASE(m_items[i].pRenderTexture);
				m_items.erase(m_items.begin() + i);
				i--;
			}
		}
	}

	inline ITexture* GetRenderTexture(int i)
	{
		return m_items[i].pRenderTexture;
	}

	inline const PERSISTID& GetUI(int i)const
	{
		return m_items[i].ui;
	}

	inline size_t GetCount()const
	{
		return m_items.size();
	}
};