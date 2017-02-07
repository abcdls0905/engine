//--------------------------------------------------------------------
// �ļ���:		frame.h
// ��  ��:		
// ˵  ��:		
// ��������:	2013��4��2��
// ������:		�ſ�ʤ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _FRAME_RT_H
#define _FRAME_RT_H

#include "../visual/i_resource.h"

// ��Ⱦ֡����

class Render;

/*
�������е�״̬
��ɫ��
color_rt 
color_tex 

��Ȱ�
depth_rt
depth_tex
depthstencil_tex

ģ���
stencil_tex
depthstencil_tex


*/



class CFrameRT: public IFrameRT
{
public:

public:
	// ��������ǰ���ϵķ�ʽ������
	CFrameRT(Render* pRender, IColorRT* pColorRT,IDepthRT* pDepthRT,IStencilTex* pStencilTex);

	// ��������ǰ���ϵķ�ʽ������
	CFrameRT(Render* pRender, IColorRT* pColorRT,IDepthTex* pDepthTex,IStencilTex* pStencilTex);

    // ����ֻҪ���
	CFrameRT(Render* pRender, IColorTex* pColorRT,IDepthRT* pDepthRT,IStencilTex* pStencilTex);
    
	// 
	CFrameRT(Render* pRender, IColorTex* pColorRT,IDepthStencilTex* pDepthStencilTex);

    // 
	CFrameRT(Render* pRender, IColorRT* pColorRT,IDepthStencilTex* pDepthStencilTex);

	CFrameRT(Render* pRender, IColorRT* pColorRT,IDepthStencilRT* pDepthStencilRT);

	CFrameRT(Render* pRender, IColorRT* pColorRT);

	virtual ~CFrameRT();

	// ��ȡIColorRT
    virtual IColorRT* GetColorRT();

	// ��ȡIDepthRT
	virtual IDepthRT* GetDepthRT();

	// ��ȡ�󶨵�IColorTex
	virtual IColorTex* GetColorTex();

	// ��ȡ�󶨵�DepthTex
	virtual IDepthTex* GetDepthTex();

	// ��ȡ�󶨵�ģ��
	virtual IStencilTex* GetStencilTex();

	// ��ȡ�󶨵����ģ����
	virtual IDepthStencilTex* GetDepthStencilTex();

	// ��ȡ�󶨵����ģ��
	virtual IDepthStencilRT* GetDepthStencilRT();

	// ����IColorRT
	virtual bool SetColorRT(IColorRT* pColorRT);

	// ����IDepthRT
	virtual bool SetDepthRT(IDepthRT* pDepthRT);

	virtual bool SetColorTex(IColorTex* pColorTex);

	virtual bool SetDepthTex(IDepthTex* pDepthTex);

	virtual bool SetStencilTex(IStencilTex* pStencil);

	virtual bool SetDepthStencilTex(IDepthStencilTex* pDepthStencilTex);

	virtual bool SetDepthStencilRT(IDepthStencilRT* pDepthStencilRT);

	// ��ѯ��Frame��ǰ�İ󶨵�״̬
	virtual IFrameRT::BIND_STATE GetColorState();
	virtual IFrameRT::BIND_STATE GetDepthState();
	virtual IFrameRT::BIND_STATE GetStencilState();

	// �Ƿ����DepthStencil
	virtual bool IsBindDepthStencil();

	// �ͷ�
	virtual void Release();

	// ����
	bool Create();

	// ʹ�õ�ǰFBO��Ϊ��Ⱦ����
    void UsedFrameRT();

	// ������Դ����
	void SetIndex(int value) { m_nIndex = value; }
	
private:
	CFrameRT();
	CFrameRT(const CFrameRT&);
	CFrameRT& operator=(const CFrameRT&);
	
	// ����
	virtual void Destroy();

	// ����IColorRT
	bool inSetColorRT(IColorRT* pColorRT);

	// ����IDepthRT
	bool inSetDepthRT(IDepthRT* pDepthRT);

	bool inSetColorTex(IColorTex* pColorTex);

	bool inSetDepthTex(IDepthTex* pDepthTex);

	bool inSetStencilTex(IStencilTex* pStencil);

	bool inSetDepthStencilTex(IDepthStencilTex* pDepthStencilTex);

	bool inSetDepthStencilRT(IDepthStencilRT* pDepthStencilRT);
private:
	Render* m_pRender;
	int m_nIndex;
	int m_nType;

	IFrameRT::BIND_STATE m_nBindColorState;
	IFrameRT::BIND_STATE m_nBindDepthState;
	IFrameRT::BIND_STATE m_nBindStencilState;

	bool   m_bIsBindDepthStencil;//�Ƿ�������ģ���ʽ

	//��ɫ
	union 
	{
		IColorRT* m_pColorRT;
		IColorTex* m_pColorTex;
	};

	//���
	union
	{
		IDepthRT* m_pDepthRT;
		IDepthTex* m_pDepthTex;
	};

	//ģ��
	IStencilTex* m_pStencilTex;

	//���ģ��
	union
	{
		IDepthStencilTex* m_pDepthStencilTex;
		IDepthStencilRT*  m_pDepthStencilRT;
	};
};

#endif // _DEPTH_RT_H

/*
CFrameRT 
��gles�е�FBO�ĳ���


��һ����ȾRT�Ĺ�������

color_point
depth_point
stencil_point

*/