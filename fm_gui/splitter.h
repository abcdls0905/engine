//--------------------------------------------------------------------
// �ļ���:		splitter.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��4��23��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _SPLITTER_H
#define _SPLITTER_H

#include "i_control.h"

// ƽ��ָ���

class Adjuster;
class GroupBox;

class Splitter: public IControl
{
private:
	enum SPLITTER_ORIENTS
	{
		ORIENT_VERTICAL,	// ��ֱ�ָ�
		ORIENT_HORIZON,		// ˮƽ�ָ�
	};

	enum { MIN_SPLITTER_SIZE = 16 };

public:
	Splitter();
	virtual ~Splitter();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);
	
	// �Ƿ�������
	virtual bool GetLoadFinish();

	virtual int GetMinWidth() const { return MIN_SPLITTER_SIZE; }
	virtual int GetMinHeight() const { return MIN_SPLITTER_SIZE; }
	
	virtual int OnWidthChanged(int old);
	virtual int OnHeightChanged(int old);
	virtual int OnSuborEvent(IControl* subor, int event, const IVarList& args);
	
	// ��ø����ؼ�
	PERSISTID GetAdjuster() const;
	PERSISTID GetPanel1() const;
	PERSISTID GetPanel2() const;

	// ����
	void SetOrient(const char* value);
	const char* GetOrient() const;

	// ���ָ�����С�ߴ�
	void SetMinPanelSize(int value);
	int GetMinPanelSize() const;
	
	// ��õ���Ե�ľ���
	void SetPaddingWidth(int value);
	int GetPaddingWidth() const;

	// ��õ������Ŀ��
	void SetAdjusterWidth(int value);
	int GetAdjusterWidth() const;

	// �Ƿ�̶��ָ�
	void SetAdjusterFixed(bool value);
	bool GetAdjusterFixed() const;
	
	// ����Ƿ��ڷָ�����Χ��
	void SetMouseInSplitter(bool value);
	bool GetMouseInSplitter() const;

	// ���¼���ָ��Ĵ�С
	bool ResetPanelSize();

private:
	// ���ø����ؼ�����
	void CalcSuborRect();
	// ��ñ��ָ�ĳߴ�
	int GetSplitSize() const;

private:
	Adjuster* m_pAdjuster;
	GroupBox* m_pPanel1;
	GroupBox* m_pPanel2;
	int m_nOrient;
	int m_nMinPanelSize;
	int m_nPaddingWidth;
	int m_nAdjusterWidth;
	bool m_bAdjusterFixed;
};

#endif // _SPLITTER_H
