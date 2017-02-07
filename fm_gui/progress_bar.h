//--------------------------------------------------------------------
// �ļ���:		progress_bar.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��15��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _PROGRESS_BAR_H
#define _PROGRESS_BAR_H

#include "../public/core_type.h"
#include "i_control.h"

// ������

class ProgressBar: public IControl
{
public:
	ProgressBar();
	virtual ~ProgressBar();
	
	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	// �Ƿ�������
	virtual bool GetLoadFinish();

	// �õ���������Сֵ
	void SetMinimum(int value);
	int GetMinimum() const;

	// �õ����������ֵ
	void SetMaximum(int value);
	int GetMaximum() const;

	// �õ������õ�ǰֵ
	void SetCurValue(int value);
	int GetCurValue() const;

    // ������ʾ�����ı�
    void SetTextVisible(bool value);
    bool GetTextVisible() const;

    // �����ı���ʾ��ʽ
    void SetTextPercent(bool value);
    bool GetTextPercent() const;

	// ���ý����ı���ɫ
	void SetTextColor(unsigned int val);
	unsigned int GetTextColor() const;
	void SetTextColorString(const char* val);
	result_string GetTextColorString() const;

	// ��ߵļ��
	void SetWidthGap(int val);
	int GetWidthGap() const;
	void SetHeightGap(int val);
	int GetHeightGap() const;

    // ������ʾģʽ
    void SetProgressMode(const char* mode);
    const char* GetProgressMode() const;

	// ������ͼ
	void SetProgressImage(const char* value);
	const char* GetProgressImage() const;

protected:
	int m_nMinimum;
	int m_nMaximum;
	int m_nValue;
	int m_nHeightGap;
	int m_nWidthGap;
    bool m_bTextVisible;
    bool m_bTextPercent;
	CColorData m_TextColor;

	CImagePlayer m_ProgressImage;

    enum ProgressMode
    {
        LEFT_TO_RIGHT = 0,
        RIGHT_TO_LEFT = 1,
        BOTTOM_TO_TOP = 2,
        TOP_TO_BOTTOM = 3,
        MAX_MODE = 4,
    };

    ProgressMode m_eProgressMode;
};

#endif // _PROGRESS_BAR_H
