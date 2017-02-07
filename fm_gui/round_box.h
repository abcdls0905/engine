//--------------------------------------------------------------------
// �ļ���:		round_box.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��3��20��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _ROUND_BOX_H
#define _ROUND_BOX_H

#include "../public/core_type.h"
#include "i_control.h"

// Բ�οؼ�

class RoundBox: public IControl
{
public:
	RoundBox();
	virtual ~RoundBox();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	// �Ƿ�������
	virtual bool GetLoadFinish();

    // ������
	virtual int OnLButtonDown(int x, int y);
    // ����ƶ�
    virtual int OnMouseMove(int x, int y);

	virtual IControl* OnGetInWhat(int x, int y);
	
	// ͼƬ��
	void SetImage(const char* value);
	const char* GetImage() const;

	// ������ű���
	void SetZoomWidth(double value);
	double GetZoomWidth() const;

	// �߶����ű���
	void SetZoomHeight(double value);
	double GetZoomHeight() const;

	// �ؼ����ĵ���ͼƬ�ϵ�λ��
	void SetCenterX(int value);
	int GetCenterX() const;
	void SetCenterY(int value);
	int GetCenterY() const;

	// ��������
	void SetFanCount(int value);
	int GetFanCount() const;

	// �Ƿ�͸��
	void SetTransparent(bool value);
	bool GetTransparent() const;

	// ͼƬ�ߴ�
	int GetImageWidth() const;
	int GetImageHeight() const;

private:
	// �ж�ָ��λ���Ƿ�͸��
	bool TestTransparent(int x, int y);
	
private:
	core_string m_strImage;
	PERSISTID m_TexId;
	double m_dZoomWidth;
	double m_dZoomHeight;
	int m_nCenterX;
	int m_nCenterY;
	int m_nFanCount;
	bool m_bTransparent;
};

#endif // _ROUND_BOX_H

