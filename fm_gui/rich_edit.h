//--------------------------------------------------------------------
// �ļ���:		RichEdit.h
// ��  ��:		���ʽ�ı������ ���� (HTML����֧��)
// ˵  ��:		
// ��������:	2008��9��18��
// ������:		����ƽ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef RichEdit_h_2008_9_18
#define RichEdit_h_2008_9_18

#include "i_scrollable.h"
#include "tinyhtml.h"
//#include <windef.h>

// RichEdit
// ��������� ����

class ImageAnimationManager;
class HyperLinkStyleManager;


class RichEdit: public IScrollable
{
private:
	// ��ʾ����
	enum
	{
		PARTTYPE_CHAR,	// �ַ�
		PARTTYPE_IMAGE,	// ͼԪ
	};
	
	struct LineItem
	{
		int nLineHeight;
		int nStartPos;
		int nEndPos;
	};
	
	typedef std::vector<HtmlItem*> PART_CONTAINER;
	
public:
	RichEdit();
	virtual ~RichEdit();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	
	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	virtual int OnWidthChanged(int old);
	virtual int OnHeightChanged(int old);
	virtual int OnLButtonDown(int x, int y);
	virtual int OnLButtonUp(int x, int y);
	virtual int OnDragMove(int x, int y);
	virtual int OnKeyDown(unsigned int vk);
    virtual int OnKeyUp(unsigned int vk);
	virtual int OnChar(unsigned int ch);
    virtual int OnEnter();
	virtual int OnGetFocus(const PERSISTID& lost_id);
	virtual int OnLostFocus(const PERSISTID& get_id);
	virtual int OnChanged(const PERSISTID& changed_id);

	virtual void SetText(const wchar_t* val);
	virtual const wchar_t* GetText() const;
	virtual point_t GetCaretPos();
	// ����������
	virtual void SetCaretName(const char* name);
	virtual const char* GetCaretName() const;

    // ������ʾ������:0,0,120,40
    void SetViewRect(const char* RectStr);
	result_string GetViewRect();

	// ֻ��״̬
	void SetReadOnly(bool value); 
	bool GetReadOnly() const;
	
	// �༭��ģʽ
	void SetEditMode(bool value); 
	bool GetEditMode() const;

	// HTML֧��
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

    // �и�
    void SetLineHeight(int height);
    int GetLineHeight();

    // �Ƿ��ȡ������ʽ
    void SetReturnAllFormat(bool bFull);
    bool GetReturnAllFormat();

	// �Ƿ񷵻������ʽ
	void SetReturnFontFormat(bool bValue);
	bool GetReturnFontFormat();

	// ��¼����
	void SetRemember(bool bRemember);
	bool GetRemember() const;

	// ���ü�¼�����ı��������
	void SetMaxRemember(int max);
	int GetMaxRemember() const;

	// ���ѡ�в��ֵ��ַ���
	std::wstring GetSelectText();
	// �Ƿ���ѡ�е�����
	bool HasSelectText() const;
	// ���ѡ��״̬
	void ClearSelect();

    // ��ѡ�����ɫ
    bool ChangeSelectColor(const char* color);

    // ��ѡ���������
    bool ChangeSelectFont(const char*font);

	// ɾ��
	bool Cut();
	// ����
	bool Copy();
	// ճ��
	bool Paste();

    // �������mode 0 ����ں�,1�����ǰ -1����ڵ�ǰ���λ��,��ǰ�޹������������
    bool Append(const wchar_t* text, int mode);
	
    // ��ȡ����
    int GetLineNum();

	// ��õ�ǰ����
	int GetSize() const;
private:    
	// �̳�IScrollable���ģ�巽��
    // ��ֱ����������
    virtual int GetVerticalTotal() const;
    // ������ʾ������
    virtual int GetVerticalCanDisp() const;
    // ���õ�ǰֵ
    virtual void SetVerticalValue(int value);
    // ��ȡ��ǰֵ
    virtual int GetVerticalValue() const;

    // ��ȡ������ռ������
    virtual int GetContentLines() const;
       
    virtual bool UpdateScroll();

private:
    // ��ȡ��ǰ�������ڵ�ʵ��itemλ��
    // ͨ��m_Buffer[GetCareRealPos()]�õ����ǲ����֮���item
    // ��Ҫ��ò����֮ǰ��itemʱ����ҪGetCareRealPos() - 1,����ע��m_BufferΪ�յ����
    int GetCareRealPos(const int line, const int linepos);
    void RefreshScrollByCaret();
    
	// ���������ı����һ��һ���ǻ��б�־
    void CorrectFinalFlag();

	void ReleasePart(HtmlItem* part);
	void ReleaseAll();
	
	// ���ù��
	bool SetCaretID(const char* name);


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
    // ��ɾ������
    bool DeleteLeftText(int pos);
	// ɾ������
	bool DeleteText(int pos, int num);
	// �ַ�����
	int InputChar(wchar_t ch);
	// ��������
	int InputKey(unsigned int ch);
	// ��������λ��
    void SetInputPos(int line, int pos);
	void SetInputPos(int pos);
	// �����
	int GetCaretWidth() const;
	// �����ָ������ڵĿɼ�����
	int GetVisualCount(int pos, int width, int&height);
	// ��ʾ���
	int GetShowWidth() const;
	// ������ݿ��
	int GetTotalWidth(PART_CONTAINER& parts, int pos, int num);

	// ��λ������
	void ResetCaret();

	// �淶λ��
	int NormalizePos(int pos) const;
	// ��ǰ��ʾ���ֵ���ʼλ��
	int GetVisTextStart(int startpos, int endpos);
	// ����ڵ�ǰ�����е�λ��
	int GetCursorInPos(int x, int y, int&line, int&linepos);
	
	// ѡ��������ʼλ��
	int GetSelectBeg() const;
	// ѡ�����ֽ���λ��
	int GetSelectEnd() const;
	// ����ѡ������
	bool SetSelect(int begin,int end);
	// ɾ��ѡ������
	bool DeleteSelectText();
	// ����ѡ������
	void PaintSelectText();

    // ����Խ��������ַ�����
    bool TryVietnamesInput(wchar_t ch);
    // ����Խ��������ַ�����
    bool TryThaiInput(wchar_t ch);

	// ��¼����
	bool RememberInput();
	// ��ʾ�ϡ���������
	bool ShowLastText();
	bool ShowNextText();

    ImageAnimationManager* InnerGetAnimation();
    HyperLinkStyleManager* InnerGetHyperlinkMng();

    // ��ȡʵ�ʹ��λ��
    int get_real_pos();
private:
	int m_nTopIndex;        // �����õ�
	int m_nRealTopIndex;    // ����ʵ�ʵ���ʾ�ڿؼ��ϵĿ�ʼ��
	int m_nRealEndIndex;        
	int m_nCaretInLine;     // ���������
	int m_nCaretLinePos;    // ������������ϵ�λ�� 
	int m_nLineHeight;      // ���ڼ�����и�
	int m_TextWidth;

	std::vector<LineItem> m_vLineItems; // ʵ��ÿ�еĸ߶�

	bool m_bReturnFontFormat;	// �Ƿ񷵻������ʽ
    bool m_bOneLine;     // �������뻹�Ƕ���
	bool m_bReadOnly;
	bool m_bSupportHtml;
	bool m_bOnlyDigit;
	bool m_bMarked;
	bool m_bDragEvent;
    bool m_bChangedEvent;
    bool m_bReturnAllFormat;
    bool m_bEditMode;
	int m_nDragOldX;
	int m_nDragOldY;
	int m_nMaxLength;
	int m_nAlign;
	int m_nScrollStep;
	int m_nSelectBeg;
	int m_nSelectEnd;

	std::wstring m_wsPswdChar;
	std::wstring m_wsText;
	PART_CONTAINER m_Buffer;
    // ���rect�����˿�������,�൱�ڴ��ڵĴ�С
    rect_t m_ViewRect;
    point_t m_CaretPos;
	CColorData m_SelectForeColor;
	CColorData m_SelectBackColor;
	std::string m_strCaretName;
	PERSISTID m_CaretID;
    TinyHtml m_Html;
    std::vector< TextSegment > m_vSimpleText;

	// ��¼����
	bool m_bRemember;
	int m_nMaxRemember;
	int m_nCurRemember;
	std::vector<std::wstring> m_vRemembers;
};

#endif //RichEdit_h_2008_9_18

