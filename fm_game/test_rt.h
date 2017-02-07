//测试rt
#pragma once

#include "../visual/i_resource.h"
#include "../visual/i_render.h"
#include "../visual/i_scene.h"
#include "global.h"
#include "../visual/i_vis_base.h"
#include "../visual/i_terrain.h"
#include "../public/core_log.h"
#include "../fm_gui/gui.h"
#include "../fm_gui/i_control.h"

#ifdef FX_SYSTEM_IOS
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#else
#include <gles2/gl2.h>
#include <gles2/gl2ext.h>
#endif
#ifdef FX_SYSTEM_WINDOWS
#pragma comment(lib, "../extern/ogles2/lib/libegl.lib")
#pragma comment(lib, "../extern/ogles2/lib/libglesv2.lib")
#endif // FX_SYSTEM_WINDOWS
#include "../fm_gui/gui_input.h"

#include "../visual/i_painter.h"

extern PERSISTID gui_loadform(const char* file);
extern IShaderTex* g_pShaderTexture;
class TestRT : public IVisBase
{
	// gl es 错误检测函数
	inline bool TestErr(const char* ident)
	{
		int err = glGetError();	
		if(err == GL_NO_ERROR)
			return false;
		CORE_TRACE(ident);
		switch(err)
		{
		case GL_INVALID_ENUM:
			CORE_TRACE("Err: GL_INVALID_ENUM ");
			break;
		case GL_INVALID_VALUE:
			CORE_TRACE("Err: GL_INVALID_VALUE ");
			break;
		case GL_INVALID_OPERATION:
			CORE_TRACE("Err: GL_INVALID_OPERATION ");
			break;
		case GL_OUT_OF_MEMORY:
			CORE_TRACE("Err: GL_OUT_OF_MEMORY ");
			break;
		default:
			CORE_TRACE("Err: UnKnown ");
			break;
		}
		return true;
}

	ISceneView* m_pSceneView;
	IFrameRT*  m_pFrameRT;
	IColorRT* m_pColorRT;
	void ScreenShot()
	{
		return;
		m_pFrameRT->UsedFrameRT();
		int x=0, y=0, w=100, h=100;
		char* bits = new char[w*h*4];
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		::glReadPixels(x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, (void*)bits);
		::glBindFramebuffer(GL_FRAMEBUFFER, 0);
		TestErr("rt");
		
	}
	PERSISTID m_formid;
public:
	~TestRT()
	{
		SAFE_RELEASE(m_pColorRT);
		SAFE_RELEASE(m_pFrameRT);
	//	SAFE_RELEASE(m_pSceneView);
	}
	bool Init(const IVarList& args)
	{
		m_pColorRT = g_render->CreateColorRT(1024, 768, TEX_FORMAT_RGB565_UNORM, 0);
		m_pFrameRT = g_render->CreateFrameRT(m_pColorRT);
	//	m_pSceneView = g_render->CreateSceneView("TestRT");
		g_render->GetContext()->SetDebugColorRT(m_pColorRT);
		this->SetContext(g_render->GetContext());
		g_core->AddMsgProc(this, WM_LBUTTONDOWN);
		m_formid = gui_loadform("res/skin/form_stage_login/form_login.xml");
		return true;
	}

	void Realize()
	{
		Gui* pGui = (Gui*)g_core->LookupEntity("Gui");
		
		IPainter* painter = (IPainter*)g_core->GetInterface("Painter");
		IFrameRT* pFrameRT = painter->GetFrameRT();
	
		if(m_pFrameRT->GetColorRT() != m_pColorRT)
			m_pFrameRT->SetColorRT(this->m_pColorRT);
		painter->SetFrameRT(m_pFrameRT);
	//	painter->DrawRect(0,0, 512, 512, 0xffffff00);
		painter->FillRect(0, 0, 512, 768, 0xffff0000);
		painter->FillRect(512, 0, 512*2, 768, 0xffffff00);
		painter->WriteText(10, 10, 0xffffffff, L"Welcome");
		
		IControl* pForm = (IControl*)g_pCore->GetEntity(m_formid);
		int w = pForm->GetWidth();
		int h = pForm->GetHeight();
		pForm->Paint(0);
		painter->SetFrameRT(pFrameRT);
	
		return;
		/*
		ISceneView* pOldSceneView = g_render->GetSceneView();
		g_render->AddSceneView(m_pSceneView);
		g_render->SetSceneView(m_pSceneView);
		m_pSceneView->SetContext(g_render->GetContext());
		m_pSceneView->SetFrameRT(m_pFrameRT, FmVec4(1,0,0,0));
		g_terrain->Realize();
		g_render->SetSceneView(pOldSceneView);
		IPainter* painter = (IPainter*)g_core->GetInterface("Painter");
		painter->DrawRect(512, 0, 512+10, 10, 0xffffffff);*/
	}

	virtual bool Shut()
	{
		return true;
	}

		// 窗口消息处理
	bool MsgProc(unsigned int msg, size_t param1, size_t param2, 
		int& result)
	{
		switch(msg)
		{
		case WM_LBUTTONDOWN:
			//ScreenShot();
			break;
		}
		return false;
	}
};
