//--------------------------------------------------------------------
// �ļ���:		light_source.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��9��29��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _LIGHT_SOURCE_H
#define _LIGHT_SOURCE_H

#include "../public/core_type.h"
#include "../visual/i_vis_base.h"
#include "../visual/i_resource.h"
#include "../visual/i_texture.h"
#include "../visual/i_render.h"
#include "../visual/i_light_source.h"

// ��Դ

class LightSource: public ILightSource
{
public:
	enum LIGHT_TYPE_ENUM
	{
		LIGHT_TYPE_POINT,
		LIGHT_TYPE_SPOT,
		LIGHT_TYPE_BOX,
	};

public:
	LightSource();
	virtual ~LightSource();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void Update(float seconds);
	virtual void Realize(float offset_seconds);

	// �Ƿ�������
	virtual bool GetLoadFinish(); 

	// λ��
	virtual bool SetPosition(float x, float y, float z);
	virtual FmVec3 GetPosition() const;

	// �Ƕ�
	virtual bool SetAngle(float x, float y, float z);
	virtual FmVec3 GetAngle() const;

	// ��Χ�гߴ�
	virtual FmVec3 GetBoxSize();
	// ���ĵ�λ��
	virtual FmVec3 GetCenter();
	// �뾶
	virtual float GetRadius();

	// ��ײ������ϸ��Ϣ
	virtual bool TraceDetail(const FmVec3& src, const FmVec3& dst, 
		trace_info_t& result);

	// �Ƿ�ɼ�
	virtual void SetVisible(bool value) { m_bVisible = value; }
	virtual bool GetVisible() const { return m_bVisible; }

	// �Ƿ�����ײ
	virtual void SetTraceEnable(bool value) { m_bTraceEnable = value; }
	virtual bool GetTraceEnable() const { return m_bTraceEnable; }

	// �ƹ�����
	void SetName(const char* value) { m_strName = value; }
	const char* GetName() const { return m_strName.c_str(); }

	// ��Դ����
	void SetLightType(int value) { m_nLightType = value; }
	int GetLightType() const { return m_nLightType; }

	// ��Χ
	void SetRange(float value) { m_fRange = value; }
	float GetRange() const { return m_fRange; }

	// ��ɫ
	void SetColor(unsigned int value) { m_nColor = value; }
	unsigned int GetColor() const { return m_nColor; }

	// ����
	void SetIntensity(float value) { m_fIntensity = value; }
	float GetIntensity() const { return m_fIntensity; }

	// ˥��ϵ��0
	void SetAttenu0(float value) { m_fAttenu0 = value; }
	float GetAttenu0() const { return m_fAttenu0; }

	// ˥��ϵ��1
	void SetAttenu1(float value) { m_fAttenu1 = value; }
	float GetAttenu1() const { return m_fAttenu1; }

	// ˥��ϵ��2
	void SetAttenu2(float value) { m_fAttenu2 = value; }
	float GetAttenu2() const { return m_fAttenu2; }

	// �۹���ڽǶ�
	void SetInnerDegree(float value);
	float GetInnerDegree() const { return m_fInnerDegree; }

	// �۹����Ƕ�
	void SetOuterDegree(float value);
	float GetOuterDegree() const { return m_fOuterDegree; }

	// �۹���½�ϵ��
	void SetFalloff(float value) { m_fFalloff = value; }
	float GetFalloff() const { return m_fFalloff; }

	// ���ӳߴ�
	const FmVec3& GetBoxScale() const { return m_vBoxScale; }
	void SetBoxScaleX(float value) { m_vBoxScale.x = value; }
	float GetBoxScaleX() const { return m_vBoxScale.x; }
	void SetBoxScaleY(float value) { m_vBoxScale.y = value; }
	float GetBoxScaleY() const { return m_vBoxScale.y; }
	void SetBoxScaleZ(float value) { m_vBoxScale.z = value; }
	float GetBoxScaleZ() const { return m_vBoxScale.z; }

	// �ƹ���˸ϵ��
	void SetBlink(float value) { m_fBlink = value; }
	float GetBlink() const { return m_fBlink; }

	// ��˸����
	void SetBlinkPeriod(float value) { m_fBlinkPeriod = value; }
	float GetBlinkPeriod() const { return m_fBlinkPeriod; }

	// ��˸ʱ��
	void SetBlinkTick(float value) { m_fBlinkTick = value; }
	float GetBlinkTick() const { return m_fBlinkTick; } 

	// ��ð󶨶���
	PERSISTID GetBindID() const { return m_BindID; }
	// ��ø�����
	const char* GetHelperName() const { return m_strHelperName.c_str(); }
	// �Ƿ���Ҫ��ɾ��
	bool GetComplete() const { return m_bComplete; }

	// ��Ӱͼ�ߴ�
	void SetShadowMapSize(int value);
	int GetShadowMapSize() const { return m_nShadowMapSize; }
	
	// �Ƿ�ǿ��ÿ֡��Ҫˢ����Ӱͼ
	void SetShadowMapDynamic(bool value) { m_bShadowMapDynamic = value; }
	bool GetShadowMapDynamic() const { return m_bShadowMapDynamic; }

	// �Ƿ���ʾ��Χ��
	void SetShowBoundBox(bool value) { m_bShowBoundBox = value; }
	bool GetShowBoundBox() const { return m_bShowBoundBox; }

	// �Ƿ���ʾ���շ�Χ
	void SetShowRange(bool value) { m_bShowRange = value; }
	bool GetShowRange() const { return m_bShowRange; }

	// �Ƿ��첽����
	void SetAsyncLoad(bool value) { m_bAsyncLoad = value; }
	bool GetAsyncLoad() const { return m_bAsyncLoad; }

	// ��ǰ�ڵƹⷶΧ�Ļ��������
	void SetActiveRoles(int value) { m_nActiveRoles = value; }
	int GetActiveRoles() const { return m_nActiveRoles; }

	// ��õ�ǰ������ֵ
	float GetCurIntensity() const { return m_fCurIntensity; }

	// �Ƿ�۹��
	//bool IsSpotLight() const
	//{
	//	return (m_fOuterDegree >= 1.0F) && (m_fOuterDegree <= 120.0F)
	//		&& (m_fInnerDegree <= m_fOuterDegree);
	//}

	// ��Ӱͼ
	void ClearShadowTex();
	void SetShadowTex(int index, IDepthRT* value);
	IDepthRT* GetShadowTex(int index) const;

	// ��ӰͼͶӰ����
	void SetShadowViewMatrix(int index, const FmMat4& value);
	const FmMat4& GetShadowViewMatrix(int index) const;
	void SetShadowProjMatrix(int index, const FmMat4& value);
	const FmMat4& GetShadowProjMatrix(int index) const;

	// ��õ�����ͼ
 	ITexture* GetOverlayTex() const;

	/*
	// ��ó�������
	IDynamicCB* GetConstBuffer(size_t index, unsigned int size);

	// ͶӰ��������
	void SetShadowCB(size_t index, IDxDynamicCB* value) 
	{ 
		Assert(index < 6);
		
		m_pShadowCB[index] = value; 
	}

	IDxDynamicCB* GetShadowCB(size_t index) const 
	{ 
		Assert(index < 6);

		return m_pShadowCB[index];
	}
	*/

	// ��Դ����
	void SetLightTypeString(const char* value);
	result_string GetLightTypeString() const;
	
	// ��ɫ
	void SetColorString(const char* value);
	result_string GetColorString() const;

	// ��ͼ
	void SetTexture(const char* value);
	const char* GetTexture() const;

	// �Ƿ�����Ұ֮��
	bool IsViewClip();
	// ���ӵ���������
	bool LinkToObject(const PERSISTID& id, float pos_x, float pos_y, 
		float pos_z);
	bool LinkToPoint(const PERSISTID& id, const char* helper_name);

private:
	// ���°�λ��
	bool UpdateBind();

private:
	core_string m_strName;
	core_string m_strTexture;
	FmVec3 m_vPosition;
	FmVec3 m_vAngle;
	FmVec3 m_vBoxScale;
	int m_nLightType;
	unsigned int m_nColor;
	float m_fIntensity;
	float m_fRange;
	float m_fAttenu0;
	float m_fAttenu1;
	float m_fAttenu2;
	float m_fInnerDegree;
	float m_fOuterDegree;
	float m_fFalloff;
	float m_fCurIntensity;
	float m_fBlink;
	float m_fDistance;
	float m_fBlinkPeriod;
	float m_fBlinkTick;
	float m_fBlinkCounter;
	float m_fDistTime;
	PERSISTID m_BindID;
	int m_nHelperID;
	core_string m_strHelperName;
	FmVec3 m_vBindPos;
	int m_nShadowMapSize;
	bool m_bVisible;
	bool m_bTraceEnable;
	bool m_bComplete;
	bool m_bShowBoundBox;
	bool m_bShowRange;
	bool m_bAsyncLoad;
	bool m_bShadowMapDynamic;
	int m_nActiveRoles;
	ITexture* m_pTexture;
	/*
	IDxDynamicCB* m_pCB[6];
	IDxDynamicCB* m_pShadowCB[6];
	*/
	IDepthRT* m_pShadowTex[6];
	FmMat4 m_ShadowViewMatrix[6];
	FmMat4 m_ShadowProjMatrix[6];
	IRender* m_pRender;
};

#endif // _LIGHT_SOURCE_H
