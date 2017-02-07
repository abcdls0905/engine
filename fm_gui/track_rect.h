//--------------------------------------------------------------------
// �ļ���:		track_rect.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��1��22��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _TRACK_RECT_H
#define _TRACK_RECT_H

#include "i_control.h"

// ��ֵ���ڿ�

class TrackRect: public IControl
{
public:
	TrackRect();
	virtual ~TrackRect();

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

	// ˮƽ������Сֵ
	void SetHorizonMin(int value);
	int GetHorizonMin() const;

	// ˮƽ�������ֵ
	void SetHorizonMax(int value);
	int GetHorizonMax() const;
	
	// ˮƽ����ǰֵ
	void SetHorizonValue(int value);
	int GetHorizonValue() const;
	
	// ˮƽ��������ֵ
	void IncHorizonValue(int value);
	
	// ��ֱ������Сֵ
	void SetVerticalMin(int value);
	int GetVerticalMin() const;

	// ��ֱ�������ֵ
	void SetVerticalMax(int value);
	int GetVerticalMax() const;
	
	// ��ֱ����ǰֵ
	void SetVerticalValue(int value);
	int GetVerticalValue() const;
	
	// ��ֱ��������ֵ
	void IncVerticalValue(int value);

protected:
    virtual bool RefreshInnerAbsPos();

private:
	void CalcSuborRect();

private:
	IControl* m_pTrackBtn;
	int m_nHorizonMin;
	int m_nHorizonMax;
	int m_nHorizonValue;
	int m_nVerticalMin;
	int m_nVerticalMax;
	int m_nVerticalValue;
};

#endif // _TRACK_RECT_H
