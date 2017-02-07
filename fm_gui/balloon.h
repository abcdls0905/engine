//--------------------------------------------------------------------
// �ļ���:		balloon.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��3��21��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _BALLOON_H
#define _BALLOON_H

#include "../public/core_type.h"
#include "../public/module.h"

// ����

class IControl;

class Balloon: public IEntity
{
public:
	Balloon();
	virtual ~Balloon();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// �Ƿ�������
	bool GetLoadFinish();

    // ����
	void SetName(const char* val);
	const char* GetName() const;

	// ǰһ���ڵ�
	void SetPrevNode(Balloon* node) { m_pPrev = node; }
	Balloon* GetPrevNode() const { return m_pPrev; }
	
	// ��һ���ڵ�
	void SetNextNode(Balloon* node) { m_pNext = node; }
	Balloon* GetNextNode() const { return m_pNext; }
	
	// ��ָ���ڵ�֮�����
	void AttachTo(Balloon* link_after, Balloon** first, Balloon** last);
	// ����ڵ�
	void Attach(Balloon** first, Balloon** last);
	// �Ƴ��ڵ�
	void Detach(Balloon** first, Balloon** last);
	// �����Ա�֤��Ⱦ��˳��
	inline bool ProcessOrder(Balloon** first, Balloon** last)
    {
	    if (m_pNext) 
	    {
		    // Զ�����Ȼ�
		    if (m_fDepth < m_pNext->m_fDepth)
		    {
			    Detach(first, last);
			    AttachTo(m_pNext, first, last);
			    return true;
		    }
	    }

	    return false;
    }
	// �ؼ�����
	void SetControl(IControl* pControl);
	IControl* GetControl() const;
	PERSISTID GetControlID() const;

	// �󶨵�ģ�Ͷ���
	void SetModelID(const PERSISTID& value, const char* szNodeName = 0);
	PERSISTID GetModelID() const;
	const char* GetNodeName()const;

	// Z���
	void SetDepth(float value);
	float GetDepth() const;

	// ��λ��
	void SetBindX(float value);
	float GetBindX() const;
	void SetBindY(float value);
	float GetBindY() const;
	void SetBindZ(float value);
	float GetBindZ() const;

	// �ؼ�ƫ��
	void SetOffsetLeft(float value);
	float GetOffsetLeft() const;
	void SetOffsetTop(float value);
	float GetOffsetTop() const;

	// ���뷶Χ
	void SetNearDistance(float value);
	float GetNearDistance() const;
	void SetFarDistance(float value);
	float GetFarDistance() const;

	// �Ƿ�ǿ����ʾ
	void SetMustVisible(bool value);
	bool GetMustVisible() const;
	
    // ������������
    bool SetType(int type);
    int GetType();

	bool IsTemped() {return m_bTemped;}
	void SetTemped(bool value)
	{
		m_bTemped = value;
	}
private:
	Balloon* m_pPrev;
	Balloon* m_pNext;
    PERSISTID m_ControlID;  // �ؼ�
	PERSISTID m_ModelID;	// ģ�Ͷ���
	float m_fDepth;
	float m_fBindX;			// �󶨵�����λ��
	float m_fBindY;
	float m_fBindZ;
	float m_fOffsetLeft;	// �ؼ����ĵ���󶨵�����ƫ��
	float m_fOffsetTop;
	float m_fNearDistance;	// �������
	float m_fFarDistance;	// ��Զ����
    int m_nType;            // ����
	bool m_bMustVisible;	// ǿ����ʾ
	bool m_bTemped;			// �Ƿ���ɾ��,��������
	core_string m_strName;
	core_string m_strNodeName;//ģ�Ͷ���Ĺҽӵ�
};

#endif // _BALLOON_H

