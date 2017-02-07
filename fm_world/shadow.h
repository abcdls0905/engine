//--------------------------------------------------------------------
// �ļ���:		shadow.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��9��30��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
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
// Ӱ��

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

	// λ��
	virtual bool SetPosition(float x, float y, float z);
	virtual FmVec3 GetPosition() const;

	// �Ƿ�ɼ�
	virtual void SetVisible(bool value);
	virtual bool GetVisible() const;
	
	// �󶨵Ķ���
	void SetBindID(const PERSISTID& value);
	PERSISTID GetBindID() const;

	// ��Ӱģ����
	void SetLightDispersion(float value);
	float GetLightDispersion() const;

	// ��Ӱ������ɫ
	void SetShadowTopColorString(const char* value);
	result_string GetShadowTopColorString() const;

	// ��Ӱ�ײ���ɫ
	void SetShadowBottomColorString(const char* value);
	result_string GetShadowBottomColorString() const;

	// ʹ�ü�����Ӱ
	void SetUseSimpleShadow(bool value);
	bool GetUseSimpleShadow() const;

	// �Ƿ�����Ұ֮��
	bool IsViewClip();

	// ����Ӱ�ӹ�����
	void SetShadowManager(ShadowManager* value) { m_pShadowManager = value; }
	
private:
	// �����廭����ͼ��
	bool DrawModel(IVisBase* pVisBase, IColorRT* pShadowTex, 
		IDepthRT* pShadowDepth, const FmMat4& view, 
		const FmMat4& proj); 

	// ����Ӱ(�ɱ�̹���)
	void DrawShadowByShader(IColorRT* pShadowTex, IDynamicVB* pVBuffer);

	// ����ͶӰ��׶��
	void FindPlanes(FmPlane* planes, const FmMat4& view, 
		const FmMat4& proj);
	
	// ��Ӽ�ѡ����������
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
