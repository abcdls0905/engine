//--------------------------------------------------------------------
// �ļ���:		i_vis_base.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��9��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _VISUAL_I_VIS_BASE_H
#define _VISUAL_I_VIS_BASE_H

#include "../public/module.h"


#include "vis_utils.h"

// ���Ӷ������

class IRenderContext;

class IVisBase: public IEntity
{
public:
	IVisBase()
	{
		m_pContext = NULL;
		m_pUserData = NULL;
	}

	virtual ~IVisBase() = 0;

	// ����״̬
	virtual void Update(float seconds) {}
	// ������Ⱦ
	virtual void Realize() {}
	// ��Ⱦ��Ӱ��ͼ
	virtual void RealizeShadowMap(){}

	// ��������
	virtual bool Load() { return true; }
	// ���������Ƿ�ɹ�
	virtual bool GetLoaded() { return true; }
	// �Ƿ���ɼ��ػ��ڼ��ع����У�ͬ�����첽��
	virtual bool GetLoadFinish() { return true; }

	// λ��
	virtual bool SetPosition(float x, float y, float z) { return false; }
	virtual FmVec3 GetPosition() const { return FmVec3(0.0F, 0.0F, 0.0F); }
	// ����
	virtual bool SetAngle(float x, float y, float z) { return false; }
	virtual FmVec3 GetAngle() const { return FmVec3(0.0F, 0.0F, 0.0F); }
	// ����ϵ��
	virtual bool SetScale(float x, float y, float z) { return false; }
	virtual FmVec3 GetScale() const { return FmVec3(1.0F, 1.0F, 1.0F); }
	
	// ���������������ĵ�
	virtual FmVec3 GetCenter() 
	{ 
		return GetPosition() + FmVec3(0.0F, GetRadius(), 0.0F); 
	}

	// ��ð�Χ�гߴ�
	virtual FmVec3 GetBoxSize() { return FmVec3(0.0F, 0.0F, 0.0F); }
	// ��ð뾶
	virtual float GetRadius() { return 0.0F; }
	
	// ����ת������
	virtual void SetWorldMatrix(const FmMat4& mat) {}
	virtual void GetWorldMatrix(FmMat4& mat)
	{
		FmVec3 position = GetPosition();
		FmVec3 angle = GetAngle();
		FmVec3 scale = GetScale();	
		FmQuat rotate;

		FmQuatRotationYawPitchRoll(&rotate, angle.y, angle.x, 
			angle.z);
		VisUtil_MatrixTransform(&mat, &scale, &rotate, &position);
	}

	// ��ײ������ϸ��Ϣ
	virtual bool TraceDetail(const FmVec3& src, const FmVec3& dst, 
		trace_info_t& result) { return false; }

	// ��ײ�����Ƿ����赲
	virtual bool TraceHitted(const FmVec3& src, const FmVec3& dst) 
	{ 
		trace_info_t ti;

		return TraceDetail(src, dst, ti);
	}

	// ���巶Χ��ײ
	virtual bool TraceSphere(const FmVec3& center, 
		float radius) { return false; }
	// ��������ײ���
	virtual bool TraceTriangle(const FmVec3& v0, 
        const FmVec3& v1, const FmVec3& v2) { return false; }

	// ��ѡ
	virtual bool Cull(const FmPlane* planes, size_t plane_num, 
		const FmVec3& center, float radius, CULL_FUNC cull_func, 
		void* context) { return false; }
		
	// �Ƿ�ɼ�
	virtual void SetVisible(bool value) {}
	virtual bool GetVisible() const { return true; }

	// �Ƿ���ˮ��
	virtual void SetUnderWater(bool value) {}
	virtual bool GetUnderWater() const { return false; }

	// ��ɫ
	virtual void SetColor(unsigned int value) {}
	virtual unsigned int GetColor() const { return 0; }

	// ϸ�ڵȼ�
	virtual void SetLevelOfDetail(float value) {}
	virtual float GetLevelOfDetail() const { return 1.0F; }

	// �Ƿ�����ײ
	virtual void SetTraceEnable(bool value) {}
	virtual bool GetTraceEnable() const { return true; }

	// �Ƿ���Լ���
	virtual void SetCullEnable(bool value) {}
	virtual bool GetCullEnable() const { return true; }

	// �Ƿ���Ҫˮ�淴��
	virtual void SetWaterReflect(bool value) {}
	virtual bool GetWaterReflect() const { return false; }

	// ��¼��ʱ��͸���ȱ�����ֻ������ϵͳʹ�ã�
	virtual void SetAlphaValue(float value) {}
	virtual float GetAlphaValue() const { return 1.0F; }

	// ��ҪͶ��Ӱ��
	virtual void SetCastShadow(bool value) {}
	virtual bool GetCastShadow() const { return false; }

	// ��Ҫ����Ͷ��Ӱ��
	virtual void SetReceiveShadow(bool value) {}
	virtual bool GetReceiveShadow() const { return false; }

	// Ϊ�˷����ڽű��ж�ȡ
	float GetPositionX() const { return GetPosition().x; }
	float GetPositionY() const { return GetPosition().y; }
	float GetPositionZ() const { return GetPosition().z; }
	float GetAngleX() const { return GetAngle().x; }
	float GetAngleY() const { return GetAngle().y; }
	float GetAngleZ() const { return GetAngle().z; }
	float GetCenterX() { return GetCenter().x; }
	float GetCenterY() { return GetCenter().y; }
	float GetCenterZ() { return GetCenter().z; }
	float GetBoxSizeX() { return GetBoxSize().x; }
	float GetBoxSizeY() { return GetBoxSize().y; }
	float GetBoxSizeZ() { return GetBoxSize().z; }

	// ��Ⱦ������
	virtual void SetContext(IRenderContext* value) { m_pContext = value; }
	IRenderContext* GetContext() const { return m_pContext; }

	// �û�����ָ��
	void SetUserData(void* value) { m_pUserData = value; }
	void* GetUserData() const { return m_pUserData; }

protected:
	IRenderContext* m_pContext;
	void* m_pUserData;
};

inline IVisBase::~IVisBase() {}

#endif // _VISUAL_I_VIS_BASE_H
