//--------------------------------------------------------------------
// 文件名:		post_effect.h
// 内  容:		
// 说  明:		
// 创建日期:	 
// 创建人:		张俊胜
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------


#ifndef _POST_EFFECT_H_
#define _POST_EFFECT_H_

#include "../public/module.h"
#include "../public/core_type.h"
#include "../visual/i_render.h"
#include "../visual/i_vis_base.h"
// 后处理效果

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
    // 全屏填充
	virtual void DrawAxisAlignedQuad(IShaderProgram* pShader);
	// 根据PS和VS创建
    IShaderProgram* CreateShaderProgram(IVertexShader* pVS,IPixelShader* pPS);

	// 拷贝渲染表面内容
	void CopyBackColor(const char* ident);

protected:
	IRender* m_pRender;
	IShaderProgram* m_pShader;
	FmMat4          m_matProj;
	IColorRT*       m_pTexBackColor;
};




#endif