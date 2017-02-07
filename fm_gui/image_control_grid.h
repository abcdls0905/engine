//--------------------------------------------------------------------
// �ļ���:		ImageControlGrid.h
// ��  ��:		ͼƬ����ؼ�����չ�����Լ��������ؼ�
// ˵  ��:		
// ��������:	2009��3��3��
// ������:		����ƽ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _ImageControlGrid_h
#define _ImageControlGrid_h

#include "image_grid.h"

// ImageControlGrid

class ImageControlGrid: public ImageGrid
{
private:
	enum 
	{
		MAX_ADDBUTTON = 4,

		BUTTON_HIDE = 0,
		BUTTON_SHOW = 1,
		BUTTON_DISABLE = 2,
	};

	// ImageGridÿ����Ʒ�ϵĸ�����Ϣ
	struct AddImageItem
	{
		int index;
		int ButtonState[MAX_ADDBUTTON]; //��ť��ʾ״̬
        std::vector<bool> vbShowMultiTextBox;
        std::vector<bool> vbShowButton;
        std::vector<std::wstring> vwcsText;
	};

public:
    ImageControlGrid();
    
	// ʵ��IControl����麯��
    virtual bool Init(const IVarList& args);
    virtual bool Shut();

    virtual void PrepareDesignInfo(IDesignInfo* di);

    virtual void Paint(float seconds);

    // �����������һ����Ʒ,���ԭ���Ѵ������滻
    virtual bool AddItem(int index, const char* pImageName, 
		const wchar_t* pItemText = NULL, int ItemNumber = 1, int lMark = -1);
    // ������������
    virtual bool Clear();

    // ɾ��һ����Ʒ
    virtual bool DelItem(int index);

    // ������Ʒ��Ϣ
    virtual bool SetItemInfo(int index, const wchar_t *pText);
    // ����ÿ��ĸ��Ӱ�ť�Ƿ���ʾ
    virtual bool ShowItemAddButton(int index, int buttonindex, bool bShow);
    // ����ÿ��ĸ���ɨť�Ƿ����
    virtual bool DisableItemAddButton(int index, int buttonindex, 
		bool bDisable);

    // ����ÿ��ĸ�����Ϣ�Ƿ���ʾ
    virtual bool ShowItemAddInfo(int item_index, int info_index, bool bShow);

    // ���ø��Ӹ�����Ϣ
    bool SetItemAddInfo(int index, int controlindex, const wchar_t* pText);
    const wchar_t* GetItemAddText(int index, int controlindex);

    void SetButtonCount(int value); 
    int GetButtonCount();

    void SetHasMultiTextBox(bool bValue); 
    bool GetHasMultiTextBox();

    // 
    void SetMultiTextBoxCount(int value); 
    int GetMultiTextBoxCount();

    void SetButtonPos(const char *pValue);
    const char* GetButtonPos();

    PERSISTID GetButton1();
    PERSISTID GetButton2();
    PERSISTID GetButton3();
    PERSISTID GetButton4();
    PERSISTID GetButtonByIndex(int index);
    PERSISTID GetMultiTextBox1();
    PERSISTID GetMultiTextBox2();
    PERSISTID GetMultiTextBox3();
    PERSISTID GetMultiTextBox4();
    PERSISTID GetMultiTextBox5();
    PERSISTID GetMultiTextBox6();
    PERSISTID GetMultiTextBox7();
    PERSISTID GetMultiTextBox8();
    PERSISTID GetMultiTextBoxByIndex(int index);
    PERSISTID GetMultiTextBox();

    void SetMultiTextBoxPos(const char *pValue);
    const char* GetMultiTextBoxPos();

    void SetMultiTextBoxLeft(int value); 
    int GetMultiTextBoxLeft();

    void SetMultiTextBoxTop(int value); 
    int GetMultiTextBoxTop();

	virtual int OnMouseMove(int x, int y);
	virtual int OnLButtonDown(int x, int y);
	virtual int OnLButtonUp(int x, int y);

private:
    //
    bool DrawOneAddItem(const int item_index, const AddImageItem& item, 
        const int xbase, const int ybase, const int x, const int y, 
        const int GridWidth, const int GridHeight,
        const int nGridWidth, const int nGridHeight, 
        const float seconds);

    //
    bool DrawEditAddControl(const int xbase, const int ybase, 
        const int GridWidth, const int GridHeight, const float seconds);

    //
    void BeginShowAddControl();
    //
    void EndShowAddControl();

private:
    std::vector<PERSISTID> m_vButtons;
    std::vector<PERSISTID> m_vMultiTextBox;
    std::vector<point_t> m_vMultiTextBoxPos;
    std::string m_szButtonPos;
    std::string m_szMultiTextboxPos;
    std::vector<point_t> m_vPos;

    int m_nMouseInButtonGridIndex;
    int m_nMouseInButtinIndex;
    bool m_bPushSubButton;

    std::vector<AddImageItem> m_vAddItems;
};  

#endif //_ImageControlGrid_h
