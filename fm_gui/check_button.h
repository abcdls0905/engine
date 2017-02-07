//--------------------------------------------------------------------
// �ļ���:		check_button.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��15��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _CHECK_BUTTON_H
#define _CHECK_BUTTON_H

#include "../public/core_type.h"
#include "i_control.h"

// ��ѡ��

class CheckButton: public IControl
{
private:
	enum { MIN_BOX_SIZE = 4 };

public:
	CheckButton();
	virtual ~CheckButton();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	// �Ƿ�������
	virtual bool GetLoadFinish();

	virtual int OnKeyDown(unsigned int vk);
	virtual int OnLButtonDown(int x, int y);
	virtual int OnLButtonUp(int x, int y);

	virtual bool SetCheckValue(bool value);

	bool ApplyCheckValue(bool value);

	// ѡ��״̬
	void SetChecked(bool value);
	bool GetChecked() const;
	
	// ����ѡ�п�
	void SetHideBox(bool value);
	bool GetHideBox() const;

	// �Ƿ��������¼�
	void SetClickEvent(bool value);
	bool GetClickEvent() const;

	// ѡ�п�ĳߴ�
	void SetBoxSize(int value);
	int GetBoxSize() const;

	// ����״̬�µ���ͼ
	void SetNormalImage(const char* value);
	const char* GetNormalImage() const;

	// ��궨λʱ����ͼ
	void SetFocusImage(const char* value);
	const char* GetFocusImage() const;

	// ѡ��ʱ����ͼ
	void SetCheckedImage(const char* value);
	const char* GetCheckedImage() const;
	
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
	
private:
	// �Ƿ��ڰ���״̬
	bool GetPressed() const
	{
		return m_bPush ^ m_bChecked;
	}
	
private:
	bool m_bChecked;
	bool m_bHideBox;
	bool m_bClickEvent;
	bool m_bPush;
	int m_nBoxSize;
	int m_nClickX;
	int m_nClickY;
	CImagePlayer m_FocusImage;
	CImagePlayer m_CheckedImage;
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

#endif // _CHECK_BUTTON_H

