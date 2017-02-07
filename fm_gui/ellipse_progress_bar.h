//--------------------------------------------------------------------
// �ļ���:		EllipseProgressBar.h
// ��  ��:		
// ˵  ��:		
// ��������:	2010��11��5��
// ������:		����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _ELLIPSEPROGRESSBAR_H
#define _ELLIPSEPROGRESSBAR_H

#include "progress_bar.h"
#include <string>

// ������

class EllipseProgressBar: public ProgressBar
{
public:
	EllipseProgressBar();
	virtual ~EllipseProgressBar();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	// �Ƿ�������
	virtual bool GetLoadFinish();

	// ������ͷ��ͼ
	void SetProgressLabImage(const char* value);
	std::string GetProgressLabImage() const;

	virtual void SetProgressImage(const char* value);
	// 
	void SetBeginDelta(int val);
	int GetBeginDelta() const;

	void SetEndDelta(int val);
	int GetEndDelta() const;

	float GetDelta( );
protected:
	PERSISTID m_TexId;
	int m_nBeginDelta;
	int m_nEndDelta;
};

#endif // _ELLIPSEPROGRESSBAR_H
