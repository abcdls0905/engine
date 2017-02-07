//--------------------------------------------------------------------
// �ļ���:		ActionTrack.h
// ��  ��:		�����ںϱ༭���еĻ���
// ˵  ��:		
// ��������:	2010��6��22��
// ������:		������
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _ACTIONTRACK_H
#define _ACTIONTRACK_H

#include "../public/core_type.h"
#include "i_control.h"

// ����

class ActionTrack: public IControl
{
public:
	ActionTrack();
	virtual ~ActionTrack();

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

	// ͼ������
	void SetIconName(const char* value);
	const char* GetIconName() const;

	 // �Ƿ��ڸı�ߴ��״̬
    void SetEditMode(int value);
    int GetEditMode() const;

	// ��ʼ֡
	void SetStartFrame(int value);
    int GetStartFrame() const;

	// ����֡
	void SetEndFrame(int value);
    int GetEndFrame() const;

	// ���֡
	void SetMaxFrame(int value);
    int GetMaxFrame() const;

	void SetFrameChange(int value);
    int GetFrameChange() const;

	// �Ƿ񱻷ָ�
	void SetIsSlide(bool value);
    bool GetIsSlide() const;

	// ����
	void SetFadeIn(float value);
	float GetFadeIn() const;

	// �ڳ�
	void SetFadeOut(float value);
	float GetFadeOut() const;

	// ����
	void SetScale(float value);
	float GetScale() const;

	// ������ɫ
	void SetFaceColor(unsigned int value);
	unsigned int GetFaceColor() const;

	// ȷ���༭ģʽ
	int CheckEditMode(int x, int y);

	// �Ƿ�����
	void SetIsLock(bool value);
    bool GetIsLock() const;

	// �Ƿ��¡�ctrl����
	void SetIsControl(bool value);
    bool GetIsControl() const;

    // ���ð���״̬
    void SetPressed(bool bValue);

private:
	// �Ƿ��ڰ���״̬
	bool GetPressed() const
	{
		return m_bPush;// && GetCapture();
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
	core_string m_strIcon;
	PERSISTID m_IconId;
	bool m_bIconLoaded;

	int m_nEditMode;
	int m_nStartFrame;
	int m_nEndFrame;
	int m_nMaxFrame;
	int m_nFrameChange;
	float m_fScale;

	int m_nFadeIn;
	int m_nFadeOut;
	int m_nFadeOutEnd;
	bool m_bSlide;
	bool m_bLock;
	bool m_bControl;

	unsigned int m_btnColor;
	unsigned int m_btnColorWhite;
};

#endif // _ACTIONTRACK_H
