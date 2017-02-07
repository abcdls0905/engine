//--------------------------------------------------------------------
// �ļ���:		float_edit.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��11��20��
// ������:		
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _FLOAT_EDIT_H
#define _FLOAT_EDIT_H

#include "../public/core_type.h"
#include "edit.h"
#include "label.h"

// �����������

class FloatEdit : public Edit
{
public:
	FloatEdit();
	virtual ~FloatEdit();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void PrepareDesignInfo(IDesignInfo* di);

	virtual int OnLButtonDown(int x, int y);
	virtual int OnLButtonUp(int x, int y);
	virtual int OnRButtonDown(int x, int y);
	virtual int OnDragMove(int x, int y);
	virtual int OnKeyDown(unsigned int vk);
	virtual int OnKeyUp(unsigned int vk);
	virtual int OnChar(unsigned int ch);
	virtual int OnGetCapture(const PERSISTID& lost_id);
	virtual int OnLostCapture(const PERSISTID& get_id);

	// ����ֵ����
	void SetFormat(const wchar_t* value);
	const wchar_t* GetFormat() const;

	// �϶�ʱ�ı仯����
	void SetDragStep(float value);
	float GetDragStep() const;

	// Ĭ��ֵ
	void SetDefault(float value);
	float GetDefault() const;

	// ��Сֵ
	void SetMin(float value);
	float GetMin() const;

	// ���ֵ
	void SetMax(float value);
	float GetMax() const;

	// ѭ��
	void SetLoop(bool value);
	bool GetLoop() const;

	// �����Ƿ���ʾ������Ϣ
	void SetShowHintText(bool value);
	bool GetShowHintText() const;

	// ׷����ʾ��Ϣ
	void SetAppendHintText(const wchar_t* name);
	const wchar_t* GetAppendHintText() const;

	// ��⸡�����ı������Ƿ����仯
	int OnChanged(const PERSISTID& changed_id);

private:
	// ������ʾ��Ϣ
	void UpdateHintText(float min, float max);
	// ���ֵ�Ƿ���Ч
	bool IsValid();
	// ���ø������ı�ֵ
	void SetValue(float value);

private:
	point_t m_ptLButtonDownPos;		// �����϶��������޸�ֵ
	core_wstring m_wsFormat;		// ����ֵ����
	bool m_bIsInteger;				// �Ƿ�Ϊ����
	bool m_bIsDragMethod;			// �Ƿ����϶�״̬
	float m_fDragStep;				// �϶�ʱ�ı仯����
	float m_fOldDragStep;			// ����ԭ�仯����
	float m_fDefault;				// Ĭ��ֵ
	float m_fMin;					// ��Сֵ
	float m_fMax;					// ���ֵ
	bool m_bLoop;					// ѭ��
	bool m_bHadChangedStep_s;		// ѹ��KEY_SHIFT�Ƿ�ı��˲���
	bool m_bHadChangedStep_c;		// ѹ��KEY_CONTROL�Ƿ�ı��˲���
	point_t m_OldPoint;				// ���ǰһ�ε�λ��
		
	bool m_bShowHintText;			// �Ƿ���ʵ��ʾ�ı���Ϣ
	core_wstring m_wsHintText;		// ��ʾ�ı���Ϣ
	core_wstring m_wsAppendHintText;// ���ӵ���ʾ�ı���Ϣ
	Label* m_pHintTextLabel;		// ��ʾ�ı���ǩ
};

#endif // _FLOAT_EDIT_H
