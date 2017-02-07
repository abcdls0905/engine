//--------------------------------------------------------------------
// �ļ���:		label.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��14��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _LABEL_H
#define _LABEL_H

#include "../public/core_type.h"
#include "i_control.h"

// ���ֱ�ǩ

class CMarkString;

class Label: public IControl
{
public:
	Label();
	virtual ~Label();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	virtual void SetText(const wchar_t* value);

	// ���������״
	virtual int OnSetCursor(int x, int y);
	virtual int OnLButtonDown(int x, int y);
	
	virtual IControl* OnGetInWhat(int x, int y);

	// ���뷽ʽ
	void SetAlignString(const char* value);
	const char* GetAlignString() const;

	// �������
	void SetSolid(bool value);
	bool GetSolid() const;

	// �Ƿ����ת�����
	void SetMarked(bool value);
	bool GetMarked() const;

	// �Ƿ�͸��
	void SetTransparent(bool value);
	bool GetTransparent() const;

	// �Ƿ�������ͼƬ�¼�
	void SetClickEvent(bool value);
	bool GetClickEvent() const;

	// �Ƿ���ת
	void SetRotate(bool value);
	bool GetRotate() const;

	// ��ת�Ƕ�
	void SetAngleX(float value);
	float GetAngleX() const;
	void SetAngleY(float value);
	float GetAngleY() const;
	void SetAngleZ(float value);
	float GetAngleZ() const;
	
	// ������������״
	void SetRefCursor(const char* value);
	const char* GetRefCursor() const;

    void SetIsLimitWidth(bool IsLimit);
    bool GetIsLimitWidth();

    void SetTextOffsetX(int offsetx);
    int GetTextOffsetX();

    void SetTextOffsetY(int offsety);
    int GetTextOffsetY();

    int GetTextWidth();

protected:
	// �ж�ָ��λ���Ƿ�͸��
	bool TestTransparent(int x, int y);

private:
	int m_nAlign;
    int m_nTextOffsetX;
    int m_nTextOffsetY;
	bool m_bSolid;
	bool m_bMarked;
	bool m_bTransparent;
	bool m_bClickEvent;
	bool m_bRotate;
    bool m_bLimitWidth;
	FmVec3 m_vAngle;
	core_string m_strRefCursor;
	CMarkString* m_pMarkStr;
};

#endif // _LABEL_H

