//--------------------------------------------------------------------
// �ļ���:		picture.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��5��22��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _PICTURE_H
#define _PICTURE_H

#include "../public/core_type.h"
#include "i_control.h"

// ͼƬ��ʾ

class Picture: public IControl
{
public:
	Picture();
	virtual ~Picture();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);
	
	// �Ƿ�������
	virtual bool GetLoadFinish();

	virtual int OnWidthChanged(int old);
	virtual int OnHeightChanged(int old);

	virtual int OnLButtonDown(int x, int y);
	virtual int OnLButtonUp(int x, int y);
    virtual int OnRButtonDown(int x, int y);
    virtual int OnRButtonUp(int x, int y);
	virtual int OnLButtonDoubleClick(int x, int y);
    virtual int OnMouseMove(int x, int y);

	virtual int OnDragMove(int x, int y);
	virtual int OnDragLeave(int x, int y);

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

	// ͼƬ�ߴ�
	int GetImageWidth() const;
	int GetImageHeight() const;

    // �Ƿ�͸��
    void SetTransparent(bool value);
    bool GetTransparent() const;

	// ���ͼƬ��ʽ
	const char* GetImageFormat();

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
    bool m_bTransparent;

	//�϶����
	int m_nClickX;
	int m_nClickY;
};

#endif // _PICTURE_H
