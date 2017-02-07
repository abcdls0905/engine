//--------------------------------------------------------------------
// �ļ���:		button.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��14��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _BUTTON_H
#define _BUTTON_H

#include "../public/core_type.h"
#include "i_control.h"

// ��ť

class Button: public IControl
{
public:
	Button();
	virtual ~Button();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	virtual int GetMinWidth() const { return 1; }
	virtual int GetMinHeight() const { return 1; }
	
	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	// �Ƿ�������
	virtual bool GetLoadFinish();

	virtual int OnKeyDown(unsigned int vk);
	virtual int OnLButtonDown(int x, int y);
	virtual int OnLButtonUp(int x, int y);
	virtual int OnLButtonDoubleClick(int x, int y);
	virtual int OnLButtonHover(int x, int y);
	virtual int OnDragMove(int x, int y);
	
    // �ж�ָ��λ���Ƿ�͸��
    virtual bool TestTransparent(int x, int y);

	// ���뷽ʽ
	void SetAlignString(const char* value);
	const char* GetAlignString() const;

	// �����������Ƿ���Ϊ�ظ����
	void SetHoverRepeat(bool value);
	bool GetHoverRepeat() const;

	// ��갴��ʱ�Ƿ�����¼�
	void SetPushEvent(bool value);
	bool GetPushEvent() const;
	
    // ����Ƴ��ſ�ʱ���Ƿ��������¼�
    void SetOutUpEvent(bool value);
    bool GetOutUpEvent() const;

    // �����ק���Ƿ��������¼�
    void SetDragEvent(bool value);
    bool GetDragEvent() const;

	// ���õļ������
	void SetStyleString(const char* value);
	const char* GetStyleString() const;
	
	// ����״̬�µ���ͼ
	void SetNormalImage(const char* value);
	const char* GetNormalImage() const;
	
	// ��궨λʱ����ͼ
	void SetFocusImage(const char* value);
	const char* GetFocusImage() const;
	
	// ����ʱ����ͼ
	void SetPushImage(const char* value);
	const char* GetPushImage() const;
	
	// ����ֹʱ����ͼ
	void SetDisableImage(const char* value);
	const char* GetDisableImage() const;

	// ����״̬��������
	virtual void SetNormalFontName(const char* val);
	const char* GetNormalFontName() const;

	// ���ѡ��״̬��������
	virtual void SetFocusFontName(const char* val);
	const char* GetFocusFontName() const;

	// ������״̬��������
	virtual void SetPushFontName(const char* val);
	const char* GetPushFontName() const;

	// ����ֹ����״̬��������
	virtual void SetDisableFontName(const char* val);
	const char* GetDisableFontName() const;

	// �������״̬�Ļ����ɫ�ַ���
	void SetFocusBlendColorString(const char* val);
	result_string GetFocusBlendColorString() const;

	// ������״̬�Ļ����ɫ�ַ���
	void SetPushBlendColorString(const char* val);
	result_string GetPushBlendColorString() const;

	// ����ֹ����״̬�Ļ����ɫ�ַ���
	void SetDisableBlendColorString(const char* val);
	result_string GetDisableBlendColorString() const;

	// ����״̬����ɫ�ַ���
	void SetNormalColorString(const char* val);
	result_string GetNormalColorString() const;

	// ���ѡ��״̬����ɫ�ַ���
	void SetFocusColorString(const char* val);
	result_string GetFocusColorString() const;

	// ������״̬����ɫ�ַ���
	void SetPushColorString(const char* val);
	result_string GetPushColorString() const;

	// ����ֹ����״̬����ɫ�ַ���
	void SetDisableColorString(const char* val);
	result_string GetDisableColorString() const;

    // ���ð���״̬
    void SetPressed(bool bValue);

private:
	// �Ƿ��ڰ���״̬
	bool GetPressed() const
	{
		return m_bPush;
	}

private:
	bool m_bPush;
	bool m_bHoverRepeat;
	bool m_bPushEvent;
    bool m_bOutUpEvent;
    bool m_bDragEvent;
	int m_nAlign;
	int m_nStyle;
	int m_nClickX;
	int m_nClickY;
    int m_nDragOldX;
    int m_nDragOldY;
	CImagePlayer m_FocusImage;
	CImagePlayer m_PushImage;
	CImagePlayer m_DisableImage;
	// ��ť����״̬�µĻ����ɫ
	CColorData m_FocusBlendColor;
	CColorData m_PushBlendColor;
	CColorData m_DisableBlendColor;
	// ��ť����״̬�µ�������ɫ
	CColorData m_NormalColor;
	CColorData m_FocusColor;
	CColorData m_PushColor;
	CColorData m_DisableColor;
	// ��ť����״̬�µ�����
	core_string m_strNormalFontName;
	core_string m_strFocusFontName;
	core_string m_strPushFontName;
	core_string m_strDisableFontName;
};

#endif // _BUTTON_H
