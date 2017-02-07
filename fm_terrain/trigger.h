//--------------------------------------------------------------------
// �ļ���:		trigger.h
// ��  ��:		
// ˵  ��:		
// ��������:	2009��5��6��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _TRIGGER_H
#define _TRIGGER_H

#include "../public/core_type.h"
#include "../public/i_interface.h"
#include "../visual/i_render.h"
#include "../visual/i_vis_base.h"

// ������

class Terrain;

class Trigger: public IVisBase
{
public:
	Trigger();
	virtual ~Trigger();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
    
	virtual void Update(float seconds);
	virtual void Realize(float offset_seconds);
	
	// λ��
	virtual bool SetPosition(float x, float y, float z);
	virtual FmVec3 GetPosition() const;

	// ����
	virtual bool SetAngle(float x, float y, float z);
	virtual FmVec3 GetAngle() const;

	// ���� 
	virtual bool SetScale(float x, float y, float z);
	virtual FmVec3 GetScale() const;

	// ���������������ĵ�
	virtual FmVec3 GetCenter();
	// ��ð�Χ�гߴ�
	virtual FmVec3 GetBoxSize();
	// ��ð뾶
	virtual float GetRadius();

	// ��ײ������ϸ��Ϣ
	virtual bool TraceDetail(const FmVec4& src, const FmVec4& dst, 
		trace_info_t& result);

	// �Ƿ�ɼ�
	virtual void SetVisible(bool value);
	virtual bool GetVisible() const;

	// �Ƿ�����ײ
	virtual void SetTraceEnable(bool value) { m_bTraceEnable = value; }
	virtual bool GetTraceEnable() const { return m_bTraceEnable; }

	// �߿�ߴ�
	void SetSizeX(float value);
	float GetSizeX() const; 
	void SetSizeY(float value);
	float GetSizeY() const; 
	void SetSizeZ(float value);
	float GetSizeZ() const; 

	// ���������ĵ��ζ���
	void SetTerrain(Terrain* pTerrain) { m_pTerrain = pTerrain; }
	void SetTerrainID(const PERSISTID& value);
	PERSISTID GetTerrainID() const;
	
	// �Ƿ���ʾ�߿�
	void SetShowBound(bool value) { m_bShowBound = value; }
	bool GetShowBound() const { return m_bShowBound; }
	
	// �Ƿ���ʾ��Χ��
	void SetShowBoundBox(bool value) { m_bShowBoundBox = value; }
	bool GetShowBoundBox() const { return m_bShowBoundBox; }

	// �ű��ļ���
	void SetScriptName(const char* value);
	const char* GetScriptName() const;

	// �¼�����
	void SetParameter(const char* value);
	const char* GetParameter() const;

	// ��ʱ����ʱ��
	void SetPeriod(float value);
	float GetPeriod() const;

	// �󶨶���
	void SetBindID(const PERSISTID& value);
	PERSISTID GetBindID() const { return m_BindID; }
	
	// ��λ��
	float GetBindPositionX() const { return m_vBindPos.x; }
	float GetBindPositionY() const { return m_vBindPos.y; }
	float GetBindPositionZ() const { return m_vBindPos.z; }

	// �󶨵���������
	bool SetBindPosition(float x, float y, float z);

	// ��ʼ���¼�
	bool CheckInit();
	// ��鶨ʱ�¼�
	bool CheckTimer(float seconds);
	// �������¼�
	bool CheckPlayer(IVisBase* pPlayer);
	
private:
	// ��ҽ��뷶Χ
	bool PlayerEntry(IVisBase* pPlayer);
	// ����뿪��Χ
	bool PlayerLeave(IVisBase* pPlayer);
	// ���°�λ��
	bool UpdateBind();

private:
	IRender* m_pRender;
	Terrain* m_pTerrain;
	FmMat4 m_mtxWorld;
	FmVec3 m_vBoxSize;
	PERSISTID m_BindID;
	FmVec3 m_vBindPos;
	bool m_bVisible;
	bool m_bTraceEnable;
	bool m_bShowBound;
	bool m_bShowBoundBox;
	core_string m_strScriptName;
	core_string m_strParameter;
	float m_fPeriod;
	float m_fCounter;
	PERSISTID m_EntryPlayerID;
};

#endif // _TRIGGER_H
