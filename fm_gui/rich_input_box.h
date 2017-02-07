//--------------------------------------------------------------------
// �ļ���:		RichInputBox.h
// ��  ��:		���ʽ�ı��������(HTML����֧��)
// ˵  ��:		
// ��������:	2008��9��18��
// ������:		����ƽ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef RichInputBox_h_2008_9_18
#define RichInputBox_h_2008_9_18

#include "i_control.h"
#include "tinyhtml.h"
// RichEdit
// �����������

class HyperLinkStyleManager;
class ImageAnimationManager;
class RichInputBox: public IControl
{
private:
	// ��ʾ����
	enum
	{
		PARTTYPE_CHAR,	// �ַ�
		PARTTYPE_IMAGE,	// ͼԪ
	};
	
	typedef std::vector< HtmlItem* > PART_CONTAINER;
	
public:
	RichInputBox();
	virtual ~RichInputBox();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	virtual int OnWidthChanged(int old);
	virtual int OnLButtonDown(int x, int y);
	virtual int OnLButtonUp(int x, int y);
	virtual int OnDragMove(int x, int y);
	virtual int OnKeyDown(unsigned int vk);
    virtual int OnKeyUp(unsigned int vk);
	virtual int OnChar(unsigned int ch);
	virtual int OnGetFocus(const PERSISTID& lost_id);
	virtual int OnLostFocus(const PERSISTID& get_id);
	virtual int OnChanged(const PERSISTID& changed_id);

	virtual void SetText(const wchar_t* val);
	virtual const wchar_t* GetText() const;
	virtual point_t GetCaretPos();
	// ����������
	virtual void SetCaretName(const char* name);
	virtual const char* GetCaretName() const;

	// ֻ��״̬
	void SetReadOnly(bool value); 
	bool GetReadOnly() const;
	
	// ֻ������������
	void SetOnlyDigit(bool value);
	bool GetOnlyDigit() const;
	
	// �༭��ģʽ
	void SetEditMode(bool value); 
	bool GetEditMode() const;

	// �Ƿ����ת�����
	void SetSupportHtml(bool value);
	bool GetSupportHtml() const;
	
	// �Ƿ�����϶��¼�
	void SetDragEvent(bool value);
	bool GetDragEvent() const;
	
    // �Ƿ�����仯�¼�
    void SetChangedEvent(bool value);
    bool GetChangedEvent() const;

	// ��󳤶�
	void SetMaxLength(int value);
	int GetMaxLength() const;
	
	// ������ʼλ��
	void SetTextOffsetX(int value);
	int GetTextOffsetX() const;
	
	// ���뷽ʽ
	void SetAlignString(const char* value);
	result_string GetAlignString() const;
	
	// ������ʾ�ַ�
	void SetPasswordChar(const wchar_t* value);
	const wchar_t* GetPasswordChar() const;
	
	// ѡ��ʱ����ɫ
	void SetSelectForeColorString(const char* value);
	result_string GetSelectForeColorString() const;
	
	// ѡ��ʱ�ı�����ɫ
	void SetSelectBackColorString(const char* value);
	result_string GetSelectBackColorString() const;
	
    // ��¼����
    void SetRemember(bool bRemember);
    bool GetRemember() const;

    // ���ü�¼�����ı��������
    void SetMaxRemember(int max);
    int GetMaxRemember() const;

    // ��¼����
    void SetReturnFontFormat(bool bValue);
    bool GetReturnFontFormat();

	// ���ѡ�в��ֵ��ַ���
	std::wstring GetSelectText();
	// �Ƿ���ѡ�е�����
	bool HasSelectText() const;
	// ���ѡ��״̬
	void ClearSelect();



	// ɾ��
	bool Cut();
	// ����
	bool Copy();
	// ճ��
	bool Paste();

	// �������mode 0 ����ں�,1�����ǰ -1����ڵ�ǰ���λ��,��ǰ�޹������������
	bool Append(const wchar_t* text, int mode);
	
    // ����Խ��������ַ�����
    bool TryVietnamesInput(wchar_t ch);
    // ����Խ��������ַ�����
    bool TryThaiInput(wchar_t ch);

private:
	void ReleasePart(HtmlItem* part);
	void ReleaseAll();
	
	// ���ù��
	bool SetCaretID(const char* name);

	// ��õ�ǰ����
	int GetSize() const;
	// �����󳤶�
	int GetMaxSize() const;

	// �����ı�
	void GenerateText(PART_CONTAINER& parts, int pos, int num, 
		std::wstring& ws);
	
	// ����ɼ��ı�
	void CalcVisText();

	// ����ı�
	int AppendText(int pos, const wchar_t* ws, int size);
	// ��������
	int InsertText(int pos, const wchar_t* ws, int size);
	// ɾ������
	bool DeleteText(int pos, int num);
	
	// �ַ�����
	int InputChar(wchar_t ch);
	// ��������
	int InputKey(unsigned int ch);
	// ������ʾλ��
	void SetShowPos(int pos);
	// ��������λ��
	void SetInputPos(int pos);
	// �����
	int GetCaretWidth() const;
	// �����ָ������ڵĿɼ�����
	int GetVisualCount(int pos, int width);
	// ��ʾ���
	int GetShowWidth() const;
	// ������ݿ��
	int GetTotalWidth(PART_CONTAINER& parts, int pos, int num);

	// ��λ������
	void ResetCaret();

	// �淶λ��
	int NormalizePos(int pos) const;
	// ��ǰ��ʾ���ֵ���ʼλ��
	int GetVisTextStart();
	// ����ڵ�ǰ�����е�λ��
	int GetCursorInPos(int x, int y);
	
	// ѡ��������ʼλ��
	int GetSelectBeg() const;
	// ѡ�����ֽ���λ��
	int GetSelectEnd() const;
	// ����ѡ������
	bool IncSelect(int pos, int num);
	// ɾ��ѡ������
	bool DeleteSelectText();
    // ��ɾ��
    bool DeleteLeftText(int pos);
	// ����ѡ������
	void PaintSelectText(int x1, int y1);

    ImageAnimationManager* InnerGetAnimation();
    HyperLinkStyleManager* InnerGetHyperlinkMng();

private:
	bool m_bReadOnly;
    bool m_bEditMode;
	bool m_bOnlyDigit;
	bool m_bSupportHtml;
	bool m_bDragEvent;
    bool m_bChangedEvent;
    bool m_bReturnFontFormat;
	int m_nDragOldX;
	int m_nDragOldY;
	int m_nMaxLength;
	int m_nTextOffsetX;
	int m_nAlign;
	int m_nScrollStep;
	int m_nInputPos;
	int m_nShowPos;
	int m_nShowCount;
	int m_nSelectBeg;
	int m_nSelectEnd;
    bool m_bRemember;
    int m_nMaxRemember;
    int m_nCurRemember;
    point_t m_CaretPos;
	std::wstring m_wsPswdChar;
	std::wstring m_wsText;
	PART_CONTAINER m_Buffer;
	CColorData m_SelectForeColor;
	CColorData m_SelectBackColor;
	std::string m_strCaretName;
	PERSISTID m_CaretID;
    TinyHtml m_Html;
    std::vector<std::wstring> m_vRemembers;
};

#endif //RichInputBox_h_2008_9_18

