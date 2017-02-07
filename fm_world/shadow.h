//--------------------------------------------------------------------
// 文件名:		shadow.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年9月30日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _SHADOW_H
#define _SHADOW_H

#include "../visual/i_vis_base.h"
#include "../visual/shader_list.h"
#include "../visual/i_render.h"
#include "../visual/i_context.h"
#include "../visual/i_scene_view.h"
#include "../visual/i_shadow.h"

#include "shadow_manager.h"
// 影子

class IColorRT;
 
class Shadow: public IShadow
{
public:
	struct shadow_cb_object_t
	{
		FmMat4 mtxWVP;
		FmMat4 mtxLightMVP;
		float light_dispersion;
		FmVec4 top_color;
		FmVec4 bottom_color;
		IDynamicVB* pVBuffer;
	};

public:
	Shadow();
	virtual ~Shadow();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual bool Load();
	
	virtual void Update(float seconds);
	virtual void Realize(float offset_seconds);

	// 位置
	virtual bool SetPosition(float x, float y, float z);
	virtual FmVec3 GetPosition() const;

	// 是否可见
	virtual void SetVisible(bool value);
	virtual bool GetVisible() const;
	
	// 绑定的对象
	void SetBindID(const PERSISTID& value);
	PERSISTID GetBindID() const;

	// 阴影模糊度
	void SetLightDispersion(float value);
	float GetLightDispersion() const;

	// 阴影顶部颜色
	void SetShadowTopColorString(const char* value);
	result_string GetShadowTopColorString() const;

	// 阴影底部颜色
	void SetShadowBottomColorString(const char* value);
	result_string GetShadowBottomColorString() const;

	// 使用简易阴影
	void SetUseSimpleShadow(bool value);
	bool GetUseSimpleShadow() const;

	// 是否在视野之内
	bool IsViewClip();

	// 设置影子管理器
	void SetShadowManager(ShadowManager* value) { m_pShadowManager = value; }
	
private:
	// 将物体画到贴图上
	bool DrawModel(IVisBase* pVisBase, IColorRT* pShadowTex, 
		IDepthRT* pShadowDepth, const FmMat4& view, 
		const FmMat4& proj); 

	// 画阴影(可编程管线)
	void DrawShadowByShader(IColorRT* pShadowTex, IDynamicVB* pVBuffer);

	// 生成投影视锥体
	void FindPlanes(FmPlane* planes, const FmMat4& view, 
		const FmMat4& proj);
	
	// 添加拣选到的三角形
	static bool AddPoly(void* context, const FmMat4& mat, 
		const FmVec3* vr1, size_t nverts);

	FmVec3* LockDynamicVB();
	void UnlockDynamicVB();

	static void RenderShadow(void* pdata);

private:
	IRender*    m_pRender;
	ShadowManager* m_pShadowManager;
	FmVec3 m_vPosition;
	FmVec3* m_pVertices;
	size_t m_nVertexNum;
	FmMat4 m_mtxTexTransform;
	IColorRT* m_pShadowTex;
	IDynamicVB* m_pVBuffer;
	FmVec3*     m_pVBBufferData;
	IRenderContext* m_pShadowContext;
	bool m_bVisible;
	PERSISTID m_BindID;
	FmVec4 m_nTopColor;
	FmVec4 m_nBottomColor;
	float m_fLightDispersion;
	bool m_bUseSimpleShadow;
	bool m_bSimpleShadow;
	bool m_bShadowVisible;
	bool m_bInitShadowTex;

	Shadow::shadow_cb_object_t m_shadow_cb_object;
    ShadowManager::shadow_shaderhandle_t* m_pShaderHandle;
	IShaderProgram* m_pShadowShader;


};

#endif // _SHADOW_H
