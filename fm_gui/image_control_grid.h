//--------------------------------------------------------------------
// 文件名:		ImageControlGrid.h
// 内  容:		图片网格控件的扩展，可以加入其它控件
// 说  明:		
// 创建日期:	2009年3月3日
// 创建人:		杨利平
// 版权所有:	苏州蜗牛电子有限公司
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

	// ImageGrid每个物品上的附加信息
	struct AddImageItem
	{
		int index;
		int ButtonState[MAX_ADDBUTTON]; //按钮显示状态
        std::vector<bool> vbShowMultiTextBox;
        std::vector<bool> vbShowButton;
        std::vector<std::wstring> vwcsText;
	};

public:
    ImageControlGrid();
    
	// 实现IControl里的虚函数
    virtual bool Init(const IVarList& args);
    virtual bool Shut();

    virtual void PrepareDesignInfo(IDesignInfo* di);

    virtual void Paint(float seconds);

    // 向容器中添加一个物品,如果原来已存在则被替换
    virtual bool AddItem(int index, const char* pImageName, 
		const wchar_t* pItemText = NULL, int ItemNumber = 1, int lMark = -1);
    // 清理整个容器
    virtual bool Clear();

    // 删除一格物品
    virtual bool DelItem(int index);

    // 设置物品信息
    virtual bool SetItemInfo(int index, const wchar_t *pText);
    // 设置每格的附加按钮是否显示
    virtual bool ShowItemAddButton(int index, int buttonindex, bool bShow);
    // 设置每格的附加扫钮是否禁用
    virtual bool DisableItemAddButton(int index, int buttonindex, 
		bool bDisable);

    // 设置每格的附加信息是否显示
    virtual bool ShowItemAddInfo(int item_index, int info_index, bool bShow);

    // 设置格子附加信息
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
