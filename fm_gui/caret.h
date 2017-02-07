//--------------------------------------------------------------------
// �ļ���:		caret.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��15��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _CARET_H
#define _CARET_H

#include "../public/module.h"
#include "../public/core_type.h"

// ������

class IPainter;
class IControl;

class Caret: public IEntity
{
private:
	enum { MAX_CARET_WIDTH = 256 };
	enum { MAX_CARET_HEIGHT = 256 };
	
public:
	Caret();
	virtual ~Caret();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	// ����
	void SetName(const char* name);
	const char* GetName() const;
	unsigned int GetHash() const;
	
	// ��ʾ��ʱ����
	void SetShowInterval(float value);
	float GetShowInterval() const;
	
	// ���ص�ʱ����
	void SetHideInterval(float value);
	float GetHideInterval() const;

	// �������
	void SetWidthScale(float value);
	float GetWidthScale() const;

	// �����߶�
	void SetHeightScale(float value);
	float GetHeightScale() const;
	
	// �̶����
	void SetFixWidth(int value);
	int GetFixWidth() const;
	
	// �̶��߶�
	void SetFixHeight(int value);
	int GetFixHeight() const;
	
	// ��ɫ
	void SetColorString(const char* val);
	result_string GetColorString() const;

	// ���ʵ�ʵĳߴ�
	int GetWidth(int width);
	int GetHeight(int height);
	unsigned int GetActualColor(unsigned int color);

	// ����
	bool Create();
	// ����
	void Reset();
	// ˢ��
	void Flash(float time);
	
	// ��ָ��λ�û���
	bool Draw(int left, int bottom, int width, int height, unsigned int color);
	
private:
	IPainter* m_pPainter;
	core_string m_strName;
	unsigned int m_nHash;
	bool m_bDisplay;
	float m_fShowInterval;
	float m_fHideInterval;
	float m_fFlashCount;
	float m_fWidthScale;
	float m_fHeightScale;
	int m_nFixWidth;
	int m_nFixHeight;
	unsigned int m_nColor;
};

#endif // _CARET_H

