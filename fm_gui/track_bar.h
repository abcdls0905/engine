//--------------------------------------------------------------------
// �ļ���:		track_bar.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��15��	
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _TRACK_BAR_H
#define _TRACK_BAR_H

#include "i_control.h"

// ��������

class TrackBar: public IControl
{
private:
	enum ORIENT_TYPE
	{
		ORIENT_VERTICAL,	// ��ֱ����
		ORIENT_HORIZON,		// ˮƽ����
	};

	enum { MIN_TRACK_SIZE = 8 };

public:
	TrackBar();
	virtual ~TrackBar();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	// �Ƿ�������
	virtual bool GetLoadFinish();

	virtual int OnLButtonDown(int x, int y);
	virtual int OnWidthChanged(int old);
	virtual int OnHeightChanged(int old);
	virtual int OnSuborEvent(IControl* subor, 
		int event, const IVarList& args);

	// �õ������ӿؼ�
	PERSISTID GetTrackButton() const;

	// �õ�����TrackBar������
	void SetOrient(const char* value);
	const char* GetOrient() const;

	// �õ���������Сֵ
	void SetMinimum(int value);
	int GetMinimum() const;

	// �õ����������ֵ
	void SetMaximum(int value);
	int GetMaximum() const;
	
	// �õ������õ�ǰֵ
	void SetCurValue(int value);
	int GetCurValue() const;
	
	// ����ֵ
	void IncValue(int value);
	
private:
	void SwapWidthHeight();
	void CalcSuborRect();

	// �淶��ֵ
	int NormalizeValue(int value) const
	{
		if (value < m_nMinimum)
		{
			return m_nMinimum;
		}

		if (value > m_nMaximum)
		{
			return m_nMaximum;
		}

		return value;
	}

protected:
    virtual bool RefreshInnerAbsPos();

private:
	IControl* m_pTrackBtn;
	int m_nOrient;
	int m_nMinimum;
	int m_nMaximum;
	int m_nValue;
};

#endif // _TRACK_BAR_H
