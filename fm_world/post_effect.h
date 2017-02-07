//--------------------------------------------------------------------
// �ļ���:		post_effect.h
// ��  ��:		
// ˵  ��:		
// ��������:	 
// ������:		�ſ�ʤ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------


#ifndef _POST_EFFECT_H_
#define _POST_EFFECT_H_

#include "../public/module.h"
#include "../public/core_type.h"
#include "../visual/i_render.h"
#include "../visual/i_vis_base.h"
// ����Ч��

class IRenderContext;
class ISceneView;

class CPostEffect: public IVisBase
{
public:


	CPostEffect();
	virtual ~CPostEffect();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void Update(float seconds);
	virtual void Realize();
protected:
    // ȫ�����
	virtual void DrawAxisAlignedQuad(IShaderProgram* pShader);
	// ����PS��VS����
    IShaderProgram* CreateShaderProgram(IVertexShader* pVS,IPixelShader* pPS);

	// ������Ⱦ��������
	void CopyBackColor(const char* ident);

protected:
	IRender* m_pRender;
	IShaderProgram* m_pShader;
	FmMat4          m_matProj;
	IColorRT*       m_pTexBackColor;
};




#endif