//--------------------------------------------------------------------
// �ļ���:		cursor.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��15��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _CURSOR_H
#define _CURSOR_H

#include "../public/module.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"


// ������

class IPainter;

class Cursor: public IEntity
{
public:
	// ���״̬
	enum CURSOR_STATE
	{
		STATE_NORMAL,		// δ����
		STATE_LEFTDOWN,		// �������
		STATE_RIGHTDOWN,	// �Ҽ�����
		STATE_MAX,
	};

	// ���״̬������Ϣ
	struct image_info_t
	{
		core_string strName;
		PERSISTID TexId;
		float fInterval;
	};

public:
	Cursor();
	virtual ~Cursor();
	
	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	// ����
	void SetName(const char* value);
	const char* GetName() const;
	unsigned int GetHash() const;
	
	// ��ͼ
	void SetImage(const char* value);
	const char* GetImage() const;

	// �Ƿ���ϵͳ���
	bool GetIsWinCursor() const;

	// ��꽹��
	void SetHotSpotX(int value);
	int GetHotSpotX() const;
	void SetHotSpotY(int value);
	int GetHotSpotY() const;

	// ��óߴ�
	int GetWidth() const;
	int GetHeight() const;
	
	// ����״̬
	bool SetState(int value);
	int GetState() const;

	// ״̬������ͼ
	bool AddStateImage(int state, const char* image, float interval);
	// ����
	bool Create();
	// �Ƿ����
	bool IsReady() const;
	// ����
	void Reset();
	// ˢ��
	void Flash(float time);
	// Ӧ�ô�������
	bool Apply();
	// ��ʾ
	bool Show();
	// ����
	bool Hide();

    // ����������ɾ��
    bool Clear();

private:
	IPainter* m_pPainter;
	core_string m_strName;
	core_string m_strImage;
	//HCURSOR m_hWinCursor;
	//HCURSOR m_hOldCursor;
	PERSISTID m_TexId;
	unsigned int m_nHash;
	int m_nHotSpotX;
	int m_nHotSpotY;
	int m_nState;
	int m_nStateImage;
	float m_fTimeCount;
	TArrayPod<image_info_t*, 1, TCoreAlloc> m_StateImages[STATE_MAX];
};

#endif // _CURSOR_H

