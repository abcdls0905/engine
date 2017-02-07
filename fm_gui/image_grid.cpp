//--------------------------------------------------------------------
// �ļ���:		ImageGrid.cpp
// ��  ��:		ͼƬ����ؼ�
// ˵  ��:		
// ��������:	2008��8��27��
// ������:		����ƽ
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#include "image_grid.h"
#include "cool_manager.h"
#include "gui_const.h"
#include "imageanimation_manager.h"
#include "scroll_bar.h"
#include "gui.h"
#include "gui_utils.h"
#include "../tools/string_util.h"
#include "../public/core_mem.h"

/// entity: ImageGrid
/// \brief ͼƬ����ؼ�
DECLARE_ENTITY(ImageGrid, 0,IScrollable);

/// property: string SelectColor
/// \brief ѡ�и��ӵ���ɫ,���û��DrawMouseSelect�ű�ʹ��
DECLARE_PROPERTY(result_string, ImageGrid, SelectColor, 
				 GetSelectColor, SetSelectColor);
/// property: string MouseInColor
/// \brief ���������ӵ���ɫ,���û��DrawMouseIn�ű�ʹ��
DECLARE_PROPERTY(result_string, ImageGrid, MouseInColor, 
				 GetMouseInColor, SetMouseInColor);
/// property: string CoverColor
/// \brief ��ס���ӵ���ɫ,���û��DrawCover�ű�ʹ��
DECLARE_PROPERTY(result_string, ImageGrid, CoverColor, 
				 GetCoverColor, SetCoverColor);
/// property: string LockColor
/// \brief ������������ɫ,���û��DrawLock�ű�ʹ��
DECLARE_PROPERTY(result_string, ImageGrid, LockColor, GetLockColor, SetLockColor);
/// property: string CoolColor
/// \brief ��ȴ����ɫ
DECLARE_PROPERTY(result_string, ImageGrid, CoolColor, GetCoolColor, SetCoolColor);
/// property: rect_t ViewRect
/// \brief ʵ�ʸ������ڵ�����,��ʹ��RowNum,ClomnNum��������򻮳�����
DECLARE_PROPERTY(result_string, ImageGrid, ViewRect, GetViewRect, SetViewRect);

/// property: int MaxSize
/// \brief ������Ҫ�ĸ�����������һ����RowNum * ClomnNum, ��Ҫ�̶�������Сʱʹ�ã���Ȼ���-1
DECLARE_PROPERTY(int, ImageGrid, MaxSize, GetMaxSize, SetMaxSize);

/// property: int RowNum
/// \brief ����
DECLARE_PROPERTY(int, ImageGrid, RowNum, GetRowNum, SetRowNum);
/// property: int ClomnNum
/// \brief ����
DECLARE_PROPERTY(int, ImageGrid, ClomnNum, GetClomnNum, SetClomnNum);

/// property: bool RoundGrid
/// \brief �Ƿ���ʾ�յĸ��ӣ����Ϊtrue,��յĸ��ӱ�����ѡ�лᱻ����
DECLARE_PROPERTY(bool, ImageGrid, RoundGrid, GetRoundGrid, SetRoundGrid);

/// property: bool ShowEmpty
/// \brief �Ƿ���ʾ�յĸ��ӣ����Ϊtrue,��յĸ��ӱ�����ѡ�лᱻ����
DECLARE_PROPERTY(bool, ImageGrid, ShowEmpty, GetShowEmpty, SetShowEmpty);

/// property: bool EmptyShowFade
/// \brief �ո���ʾʱ�Ƿ��뵭��
DECLARE_PROPERTY(bool, ImageGrid, EmptyShowFade, GetEmptyShowFade, SetEmptyShowFade);

/// property: bool IsEditMode
/// \brief �Ƿ�༭ģʽ,�༭ģʽ�»���ʾ��������
DECLARE_PROPERTY(bool, ImageGrid, IsEditMode, GetEditMode, SetEditMode);
/// property: bool Center
/// \brief ʵ��ͼƬ���������Ƿ����,Ϊtrueʱ����GridOffsetX,GridOffsetY��������
DECLARE_PROPERTY(bool, ImageGrid, Center, GetCenter, SetCenter);
// ���ҽ���CenterΪfalseʱ��Ч
/// property: int GridOffsetX
/// \brief ʵ��ͼƬ������������ڸ������Ͻǵ�Xƫ��,
DECLARE_PROPERTY(int, ImageGrid, GridOffsetX, GetGridOffsetX, SetGridOffsetX);
/// property: int GridOffsetY
/// \brief ʵ��ͼƬ������������ڸ������Ͻǵ�Yƫ��,
DECLARE_PROPERTY(int, ImageGrid, GridOffsetY, GetGridOffsetY, SetGridOffsetY);
/// property: int GridHeight
/// \brief �ո�ĸ߶�
DECLARE_PROPERTY(int, ImageGrid, GridHeight, GetGridHeight, SetGridHeight);
/// property: int GridWidth
/// \brief �ո�Ŀ��
DECLARE_PROPERTY(int, ImageGrid, GridWidth, GetGridWidth, SetGridWidth);
/// property: bool FitGrid
/// \brief �����Ӵ�С������Ʒ
DECLARE_PROPERTY(bool, ImageGrid, FitGrid, GetFitGrid, SetFitGrid);
/// property: int VScrollIndex
/// \brief �����Ӵ�С������Ʒ
DECLARE_PROPERTY(int, ImageGrid, VScrollIndex, GetVerticalValue, SetVerticalValue)
/// property: int GridOffsetY
/// \brief ��������ÿ�����ӵĲ�ͬλ��,�����Ϊ��ʱ��Ĭ����������
///        ������ʽ:x,y;x,y;x,y;x,y;... ��;�Ÿ�ÿ����,��,��x�����y����,������һ��x,y��Ͼͱ�ʾ��һ����������ڿؼ����Ͻǵ�λ��,
///        �����Զ������λ��,Ӧ��һЩ�����Ų�����������
DECLARE_PROPERTY(result_string, ImageGrid, GridsPos, GetGridsPos, SetGridsPos);
/// property: string DrawMouseIn
/// \brief �������ʱ��ͼƬ��������ʷ,�����ĸ�����һ�������÷����ڴ�һͬ����˵��:
/// 1.��rect_t,��ʾ�ƻ���һ����͸������;          2.����gui�е�һ������ͼԪ,Ҫд�����·��,
/// 3.����һ��tex�е�һ��ͼԪ,Ҫд�����·ô,   4.����һ������ͼƬ��,��ImageAnimations.xml�����е�
DECLARE_PROPERTY(result_string, ImageGrid, DrawMouseIn, 
				 GetDrawMouseIn, SetDrawMouseIn);
/// property: string DrawMouseSelect
/// \brief ���ѡ�е���ʾ
DECLARE_PROPERTY(result_string, ImageGrid, DrawMouseSelect, 
				 GetDrawMouseSelect, SetDrawMouseSelect);
/// property: string DrawCover
/// \brief �������ʾ
DECLARE_PROPERTY(result_string, ImageGrid, DrawCover, GetDrawCover, SetDrawCover);
/// property: string DrawLock
/// \brief ��������ʾ
DECLARE_PROPERTY(result_string, ImageGrid, DrawLock, GetDrawLock, SetDrawLock);
/// property: string DrawGridBack
/// \brief ���ӱ���ͼ
DECLARE_PROPERTY(result_string, ImageGrid, DrawGridBack, 
				 GetDrawGridBack, SetDrawGridBack);
/// property: string GridBackOffsetX
/// \brief ���ӱ���ͼ����ڸ������ϵ�Xƫ��
DECLARE_PROPERTY(int, ImageGrid, GridBackOffsetX, 
				 GetGridBackOffsetX, SetGridBackOffsetX);
/// property: string GridBackOffsetY
/// \brief ���ӱ���ͼ����ڸ������ϵ�Xƫ��
DECLARE_PROPERTY(int, ImageGrid, GridBackOffsetY, 
				 GetGridBackOffsetY, SetGridBackOffsetY);

/// property: string GridCoverOffsetX
/// \brief �����ڸ�ͼ����ڸ������ϵ�Xƫ��
DECLARE_PROPERTY(int, ImageGrid, GridCoverOffsetX, 
                GetGridCoverOffsetX, SetGridCoverOffsetX);
/// property: string GridCoverOffsetY
/// \brief �����ڸ�ͼ����ڸ������ϵ�Xƫ��
DECLARE_PROPERTY(int, ImageGrid, GridCoverOffsetY, 
                GetGridCoverOffsetY, SetGridCoverOffsetY);

/// property: string GridCoverOffsetY
/// \brief �����ڸ�ͼ����ڸ������ϵ�Xƫ��
DECLARE_PROPERTY(result_string, ImageGrid, CoolAnimation, 
                GetCoolAnimation, SetCoolAnimation);

// property: bool ShowMouseDownState
/// \brief �����״̬
DECLARE_PROPERTY(bool, ImageGrid, ShowMouseDownState, 
                GetShowMouseDownState, SetShowMouseDownState);

// property: float MouseDownScale
/// \brief �����״̬����
DECLARE_PROPERTY(float, ImageGrid, MouseDownScale, 
                GetMouseDownScale, SetMouseDownScale);

// property: float MouseDownScale
/// \brief �����״̬ͼ��ƫ��X
DECLARE_PROPERTY(int, ImageGrid, MouseDownOffsetX, 
                GetMouseDownOffsetX, SetMouseDownOffsetX);

// property: float MouseDownScale
/// \brief �����״̬ͼ��ƫ��Y
DECLARE_PROPERTY(int, ImageGrid, MouseDownOffsetY, 
                GetMouseDownOffsetY, SetMouseDownOffsetY);

// property: int MouseDownAlpha
/// \brief �����״̬ͼ��͸����
DECLARE_PROPERTY(int, ImageGrid, MouseDownAlpha, 
                GetMouseDownAlpha, SetMouseDownAlpha);

/// \brief ����ı�
/// \method AddItem(int index, const char*pImageName, const wchar_t*pItemName = NULL, 
///                 int ItemNumber = 1, int lMark = -1, int nImageWidthScale = 1, int nImageHeightScale = 1);
/// \param index �����е����
/// \param pImageName ͼԪ·��
/// \param pItemName ��Ʒ��
/// \param ItemNumber ��Ʒ����
/// \param lMark �û��Զ�������
/// \param nImageWidthScale ͼƬ�Ŀ�ȱ���
/// \param nImageHeightScale ͼƬ�ĸ߶ȱ���
/// \return �ɹ�����true, ʧ�ܷ���false
//DECLARE_METHOD_7(bool, ImageGrid, AddItemWithImageScale, int, const char*, const wchar_t*, int, int, int, int);
// property: int EmptyGridState
/// \brief ��ǰ�ո��ӵ�״̬
DECLARE_PROPERTY(int, ImageGrid, EmptyGridState, 
                GetEmptyGridState, SetEmptyGridState);

#if 0	//DECLARE_METHOD
/// \brief ����ı�
/// \method AddItem(int index, const char*pImageName, const wchar_t*pItemName = NULL, 
///                 int ItemNumber = 1, int lMark = -1);
/// \param index �����е����
/// \param pImageName ͼԪ·��
/// \param pItemName ��Ʒ��
/// \param ItemNumber ��Ʒ����
/// \param lMark �û��Զ�������
/// \return �ɹ�����true, ʧ�ܷ���false
DECLARE_METHOD_5(bool, ImageGrid, AddItem, int, const char*, const wchar_t*, int, int);
/// \brief ɾ��ĳһ����Ʒ
/// \method bool DelItem(int index);
/// \param index �������
/// \return �ɹ�ɾ������true
DECLARE_METHOD_1(bool, ImageGrid, DelItem, int);
/// \brief �ж�ĳ�������Ƿ�Ϊ��
/// \method bool IsEmpty(int index);
/// \param index �������
/// \return Ϊ�շ���true,��Ϊ�շ���false
DECLARE_METHOD_1(bool, ImageGrid, IsEmpty, int);
/// \brief ������������
/// \method bool Clear();
/// \return true
DECLARE_METHOD_0(bool, ImageGrid, Clear);
/// \brief ����ĳ�����ӵ�ͼƬ
/// \method bool SetItemImage(int index, const char*pImageName)
/// \param index �������
/// \param pImageName ͼƬ�������·��
/// \return true
DECLARE_METHOD_2(bool, ImageGrid, SetItemImage, int, const char*);
/// \brief ��ȡĳ�����ӵ�ͼƬ
/// \method const char*GetItemImage(int index);
/// \param index �������
/// \return ͼƬ����·��
DECLARE_METHOD_1(std::string, ImageGrid, GetItemImage, int);
/// \brief �������еĲ�ͼ�úڰ׻���
/// \method bool ChangeItemImageToBW(int index, bool bChange);
/// \param index �������
/// \param bChange�Ƿ�ı�
/// \return �ɹ�����true
DECLARE_METHOD_2(bool, ImageGrid, ChangeItemImageToBW, int, bool);
/// \brief ����ĳ�����ӵ�����
/// \method bool SetItemName(int index, const wchar_t*pItemName);
/// \param index �������
/// \param pItemName ��������Ʒ��
/// \return true
DECLARE_METHOD_2(bool, ImageGrid, SetItemName, int, const wchar_t*);
/// \brief ����ĳ�����ӵ�����
/// \method const wchar_t* GetItemName(int index);
/// \param index �������
/// \return ��Ʒ��
DECLARE_METHOD_1(std::wstring, ImageGrid, GetItemName, int);

/// \brief ͨ�����Ʋ�����Ʒ,�鲻������-1���鵽������Ʒ���
/// \method int FindItem(wstring item_name);
/// \param item_name ��Ʒ��
/// \return ���
DECLARE_METHOD_1(int, ImageGrid, FindItem, const wchar_t*);

/// \brief ����ĳ�������е���Ʒ����
/// \method bool SetItemNumber(int index, int ItemNumber);
/// \param index �������
/// \param ItemNumber ��Ʒ����
/// \return true
DECLARE_METHOD_2(bool, ImageGrid, SetItemNumber, int, int);
/// \brief ��ȡĳ�������е���Ʒ����
/// \method int GetItemNumber(int index);
/// \param index �������
/// \return ItemNumber
DECLARE_METHOD_1(int, ImageGrid, GetItemNumber, int);
/// \brief ����ĳ�����ӵı��(�û��Զ���)
/// \method bool SetItemMark(int index, int lMark);
/// \param index �������
/// \param lMark �Զ�����
/// \return true
DECLARE_METHOD_2(bool, ImageGrid, SetItemMark, int, int);
/// \brief ����ĳ�����ӵı��(�û��Զ���)
/// \method bool GetItemMark(int index);
/// \param index �������
/// \return mark
DECLARE_METHOD_1(int, ImageGrid, GetItemMark, int);
/// \brief ����ĳ�����ӵ���ȴ����
/// \method bool SetCoolType(int index, long lCoolType);
/// \param index �������
/// \param lCoolType ��ȴ����
/// \return true
DECLARE_METHOD_2(bool, ImageGrid, SetCoolType, int, long);
/// \brief ����ĳ�����ӵ���ȴ����
/// \method bool SetCoolTeam(int index, long lCoolTeam);
/// \param index �������
/// \param lCoolTeam ��ȴ������
/// \return true
DECLARE_METHOD_2(bool, ImageGrid, SetCoolTeam, int, long);

/// \brief ����ĳ�����ӵ���ȴ��Ч
/// \method bool AddCool(int index, long lcooltime,float bit);
/// \param index �������
/// \param lcooltime ��ȴʱ��
/// \param bit ��ȴ��ǰ����
/// \return true
DECLARE_METHOD_3(bool, ImageGrid, AddCool, int, long,float);
/// \brief �ر�ĳ�����ӵ���ȴ��Ч
/// \method bool StopCool(int index);
/// \param index �������
/// \return true
DECLARE_METHOD_1(bool, ImageGrid, StopCool, int);

/// \brief ����ĳ������͸����
/// \method bool SetItemAlpha(int index, int alpha)
/// \param index �������
/// \param bCover ͸����alpha
/// \return true
DECLARE_METHOD_2(bool, ImageGrid, SetItemAlpha, int, int);

/// \brief ��ȡĳ������͸����
/// \method bool GetItemAlpha(int index)
/// \param index �������
/// \return ͸����
DECLARE_METHOD_1(bool, ImageGrid, GetItemAlpha, int);

/// \brief ����ĳ��������Ʒ��ͼƬ��ס(�ɱ�ʾ���õ�)
/// \method bool CoverItem(int index, bool bCover)
/// \param index �������
/// \param bCover �Ƿ��ס
/// \return true
DECLARE_METHOD_2(bool, ImageGrid, CoverItem, int, bool);
/// \brief ����ĳ�����ӱ�����(�õ�ʱ����,�米������Ʒ����������)
/// \method bool LockItem(int index, bool bLock);
/// \param index �������
/// \param bLock �Ƿ�����
/// \return true
DECLARE_METHOD_2(bool, ImageGrid, LockItem, int, bool);
/// \brief ����ĳ�����ӵİ����
/// \method void SetBindIndex(int itemindex, int bindindex);
/// \param itemindex ������Ŵ�0��ʼ
/// \param bindindex ��index
/// \return �ɹ�����
DECLARE_METHOD_2(bool, ImageGrid, SetBindIndex, int, int);
/// \brief ��ȡĳ�����ӵİ����
/// \method int GetBindIndex(int itemindex);
/// \param itemindex ������Ŵ�0��ʼ
/// \param bindindex ��index
/// \return �ɹ������
DECLARE_METHOD_1(int, ImageGrid, GetBindIndex, int);
/// \brief ���õ�ǰ������ѡ�и������index
/// \method bool SetSelectItemIndex(int itemindex)
/// \return �������
DECLARE_METHOD_1(bool, ImageGrid, SetSelectItemIndex, int);
/// \brief ��ȡ��ǰ������ѡ�и������index
/// \method int GetSelectItemIndex()
/// \return �������
DECLARE_METHOD_0(int, ImageGrid, GetSelectItemIndex);
/// \brief ��ȡ��ǰ������ڵ��и������index
/// \method int GetMouseInItemIndex()
/// \return �������
DECLARE_METHOD_0(int, ImageGrid, GetMouseInItemIndex);
/// \brief ��ȡ��ǰ��˫���ĸ������index
/// \method int GetDBClickItemIndex()
/// \return �������
DECLARE_METHOD_0(int, ImageGrid, GetDBClickItemIndex);
/// \brief ��ȡ��ǰ���Ҽ�����ĸ������index
/// \method int GetRBClickItemIndex()
/// \return �������
DECLARE_METHOD_0(int, ImageGrid, GetRBClickItemIndex);
/// \brief ��ȡָ�����ӵ�x����
/// \method int GetItemLeft(int itemindex)
/// \return ָ�����ӵ�x����
DECLARE_METHOD_1(int, ImageGrid, GetItemLeft, int);
/// \brief ��ȡָ��������y����
/// \method int GetItemTop(int itemindex)
/// \return ָ�����ӵ�y����
DECLARE_METHOD_1(int, ImageGrid, GetItemTop, int);
/// \brief ��ȡ��ǰ����ĸ�����x����
/// \method int GetMouseInItemLeft()
/// \return ��ǰ����ĸ�����x����
DECLARE_METHOD_0(int, ImageGrid, GetMouseInItemLeft);
/// \brief ��ȡ��ǰ����ĸ�����y����
/// \method int GetMouseInItemTop()
/// \return ��ǰ����ĸ�����y����
DECLARE_METHOD_0(int, ImageGrid, GetMouseInItemTop);
/// \brief ��ȡ���񱳾�
/// \method string GetItemBackImage(int index)
/// \return ���񱳾�ͼ
DECLARE_METHOD_1(std::string, ImageGrid, GetItemBackImage, int);
/// \brief ���õ��񱳾�
/// \method bool SetItemBackImage(int index, string imagename)
/// \return ���񱳾�ͼ
DECLARE_METHOD_2(bool, ImageGrid, SetItemBackImage, int, const char*);
/// \brief ��ȡ�����ڸ�ͼ
/// \method string GetItemCoverImage(int index)
/// \return �����ڸ�ͼ
DECLARE_METHOD_1(std::string, ImageGrid, GetItemCoverImage, int);
/// \brief ���õ����ڸ�ͼ
/// \method bool SetItemCoverImage(int index, string imagename)
/// \return �����ڸ�ͼ
DECLARE_METHOD_2(bool, ImageGrid, SetItemCoverImage, int, const char*);

/// \brief �ֶ����õ���״̬
/// \method bool SetMouseDownState(int index, bool bDown)
/// \return true
DECLARE_METHOD_2(bool, ImageGrid, SetMouseDownState, int, bool);

/// \brief �ֶ��������״̬
/// \method bool SetMouseDownState(int index)
/// \return true
DECLARE_METHOD_0(bool, ImageGrid, ClearMouseDowState);

/// \brief �ֶ�����ǰ��ѡ��
/// \method bool ClearSelectedState()
/// \return true
DECLARE_METHOD_0(bool, ImageGrid, ClearSelectedState);

/// \brief �ֶ�����ѡ��״̬����ִ�лص�
/// \method bool SetSelectedState(int index)
/// \return true
DECLARE_METHOD_1(bool, ImageGrid, SetSelectedState, int);
#endif

/// event: int on_mousein_grid(object self, index)
/// \brief �������ĳ�������Ǵ���
/// \param self���ؼ�
/// \param index ������ӵ����index(ע����ʵ�����,������ʾ���,��Ϊ�����й���)
DECLARE_EVENT(on_mousein_grid);
/// event: int on_mouseout_grid(object self, index)
/// \brief ����Ƴ�ĳ������ʱ����
/// \param self���ؼ�
/// \param index �Ƴ����ӵ����index
DECLARE_EVENT(on_mouseout_grid);
/// event: int on_leftclick_up(object self, index)
/// \brief ����������ĳ������ʱ����
/// \param self���ؼ�
/// \param index ���ӵ����index
DECLARE_EVENT(on_leftclick_up);
/// event: int on_select_changed(object self, index)
/// \brief �����ĳ������ʱ����,ѡ�����ı�
/// \param self���ؼ�
/// \param index ���ӵ����index
DECLARE_EVENT(on_select_changed);
/// event: int on_doubleclick_grid(object self, index)
/// \brief ���˫��ĳ������ʱ����
/// \param self���ؼ�
/// \param index ���ӵ����index
DECLARE_EVENT(on_doubleclick_grid);
/// event: int on_rightclick_grid(object self, index)
/// \brief ����Ҽ����ĳ������ʱ����
/// \param self���ؼ�
/// \param index ���ӵ����index
DECLARE_EVENT(on_rightclick_grid);

/// event: int on_leftclick_down(object self, index)
/// \brief ����������ĳ������ʱ����
/// \param self���ؼ�
/// \param index ���ӵ����index
DECLARE_EVENT(on_leftclick_down);
/// event: int on_mouse_move(object self, x, y)
/// \brief ����ƶ�ʱ����
/// \param self���ؼ�
/// \param x, y �������
DECLARE_EVENT(on_mouse_move);
/// event: int on_drag_enter(object self, int index)
/// \brief ��ʼ�϶��Ļص�
/// \param self ���ؼ�
DECLARE_EVENT(on_drag_enter);

/// event: int on_drag_move(object self, int index, int drag_x, int drag_y)
/// \brief ��갴ס�϶��Ļص�
/// \param self ���ؼ�
/// \param drag_x X������϶�����
/// \param drag_y Y������϶�����
DECLARE_EVENT(on_drag_move);

/// event: int on_drag_leave(object self, int index)
/// \brief ��ʼ�϶��Ļص�
/// \param self ���ؼ�
DECLARE_EVENT(on_drag_leave);

/// event: int on_drag_leave(object self, int index)
/// \brief ��ʼ�϶��Ļص�
/// \param self ���ؼ�
DECLARE_EVENT(on_drop_in);

// ImageGrid

ImageGrid::ImageGrid()
{
	m_bEdit = false;
	m_bCenter = false;
    m_bFitGrid = true;
    m_IsFirstTime = true;
    m_bRoundGrid = false;
    m_bShowEmpty = true;
    m_bShowEmptyFade = false;
	m_nRow = 1;
	m_nClomn = 1;
    m_nMaxSize = -1;

	GuiUtil_StringToRect("0,0,32,32", m_ViewRect);

    m_nGridOffsetX = 0;
    m_nGridOffsetY = 0;
    m_nGridHeight = 32;
    m_nGridWidth = 32;     
    m_nGridBackOffsetX = 0;    // ����ͼƬ����ڸ��ӵ�Xƫ��
    m_nGridBackOffsetY = 0;    // ����ͼƬ����ڸ��ӵ�Yƫ��
    m_nGridCoverOffsetX = 0;    // ����ͼƬ����ڸ��ӵ�Xƫ��
    m_nGridCoverOffsetY = 0;    // ����ͼƬ����ڸ��ӵ�Yƫ��
	// ѡ�к�����״̬����ʾ�Ŀ��ͼƬ
	m_CoolColor = GuiUtil_ColorToInt("100,0,0,0");
	m_SelectColor = GuiUtil_ColorToInt("80,0,255,0");
	m_MouseInColor = GuiUtil_ColorToInt("80,255,255,0");
    m_CoverColor = GuiUtil_ColorToInt("80,255,0,0");
    m_LockColor = GuiUtil_ColorToInt("80,255,255,0");


	m_SelectImage.name = "rect_t";		// ����Ƴ�
	m_MouseInImage.name = "rect_t";		// �������
	m_CoverImage.name = "rect_t";			// ����
	m_LockImage.name = "rect_t";			// ����
	m_SelectImage.mode = HELP_RECT;
	m_MouseInImage.mode = HELP_RECT;
	m_CoverImage.mode = HELP_RECT;
	m_LockImage.mode = HELP_RECT;

	m_nTopIndex = 0;
	m_nCurMouseIn = -1;
	m_nCurSelect = -1;
	m_nCurDBSelect = -1;
	m_nCurRButton = -1;
    m_nCurLButtonDown = -1;
    m_nCurRButtonDown = -1;
    
	m_nTotalTime = 2000;//��������������
	m_nElapseTime = 0;
	m_eEmptyDrawState = DRAW_STATE_NORMAL;

	m_nDragOldX = 0;
	m_nDragOldY = 0;

    // �����״̬
    m_bMouseDownState = true;
    m_fMouseDownScale = 0.9f;
    m_nMouseDownOffsetX = 0;
    m_nMouseDownOffsetY = 0;
    m_nMouseDownAlpha = 128;

    m_bIsMobileSkillUse = false;
}

ImageGrid::~ImageGrid()
{
    ReleaseControls();
}

bool ImageGrid::AddItemWithImageScale(int index, const char* pImageName, 
									  const wchar_t* pItemName, int ItemNumber, 
									  int lMark, int nImageWidthScale, int nImageHeightScale)
{
	if (!pImageName)
	{
		return false;
	}

	if (0 == pImageName[0])
	{
		return false;
	}

	// ��������ѱ�ʹ�����滻
	long UsedItemIndex =  GetItem(index);

	if (UsedItemIndex != -1)
	{
		ImageItem& item = *m_vItems[UsedItemIndex];

		item.Number = ItemNumber;
		item.Index = index;
		item.lMark = lMark;
		item.ImageName = pImageName;

		if (pItemName)
		{
			item.wcsName = pItemName;
		}

		if (!item.Image.IsNull())
		{
            if (item.IsPng)
            {
                GetPainter()->ReleaseGlyph(item.Image);
            }
            else
            {
                GetPainter()->ReleaseGlyph(item.Image);
            }
		}

        item.rect.left = 0;
        item.rect.top = 0;
		item.Image = CreateGlyph(pImageName, item.IsPng, item.rect.right, 
			item.rect.bottom);

		item.image_scale.x = nImageWidthScale;
		item.image_scale.y = nImageHeightScale;

		return true;
	}
	else
	{
		// �������δ��ʹ��,���½�
		ImageItem * pItem = CORE_NEW(ImageItem);

		pItem->Number = ItemNumber;
		pItem->Index = index;
		pItem->lMark = lMark;
		pItem->ImageName = pImageName;

		pItem->pCoolItem = CORE_NEW(CoolItem);
		pItem->pCoolItem->SetCoolCategory(0);

		if (pItemName)
		{
			pItem->wcsName = pItemName;
		}

        pItem->rect.left = 0;
        pItem->rect.top = 0;
		pItem->Image = CreateGlyph(pImageName, pItem->IsPng, pItem->rect.right, 
			pItem->rect.bottom);
        pItem->cool = -1; // ��ʼ����׼ȷ

		size_t i = 0;

		for (i = 0; i < m_vItems.size(); ++i)
		{
			if (m_vItems[i]->Index > pItem->Index)
			{
				break;
			}
		}

		if (i < m_vItems.size())
		{
			m_vItems.insert(m_vItems.begin() + i , pItem);
		}
		else
		{
			m_vItems.push_back(pItem);
		}

        if (GetCore()->GetEntity(GetVScrollBar()) != NULL)
        {
            UpdateScroll();
        }

		pItem->image_scale.x = nImageWidthScale;
		pItem->image_scale.y = nImageHeightScale;
		return true;
	}

	return false;

}

bool ImageGrid::DelItem(int index)
{
	// ��ȷ���в��ڱ���������µ�ɾ��
	bool bFind = false;

	for (size_t i = 0; i < m_vItems.size(); ++i)
	{
		if (m_vItems[i]->Index == index)
		{
			CORE_DELETE(m_vItems[i]->pCoolItem);
            CORE_DELETE(m_vItems[i]);
			m_vItems.erase(m_vItems.begin() + i);
			bFind = true;
			break;
		}
	}

    if (GetCore()->GetEntity(GetVScrollBar()) != NULL)
    {
        UpdateScroll();
    }

	return bFind;
}

bool ImageGrid::IsEmpty(int index)
{
    for (size_t i = 0; i < m_vItems.size(); ++i)
    {
        if (m_vItems[i]->Index == index)
        {
            return false;
            break;
        }
    }

    return true;
}

void ImageGrid::ReleaseImages()
{
    for (size_t i = 0; i < m_vItems.size(); ++i)
    {
        if (!m_vItems[i]->Image.IsNull())
        {
            if (m_vItems[i]->IsPng)
            {
                GetPainter()->ReleaseGlyph(m_vItems[i]->Image);
            }
            else
            {
                GetPainter()->ReleaseGlyph(m_vItems[i]->Image);
            }
        }
    }

    //std::map<int, CImagePlayer*>::iterator it = m_mapBackImageIndexMap.begin();
	TPodHashMap<int, CImagePlayer*, TPodTraits<int>, 
		TCoreAlloc>::iterator it = m_mapBackImageIndexMap.Begin();
    
	while (it != m_mapBackImageIndexMap.End())
    {
        //CImagePlayer& imageplayer = *(it->second);
        
		//if (imageplayer.IsReady())
        //{
            //imageplayer.Release();
        //}
		CImagePlayer* pImagePlayer = it.GetData();

		pImagePlayer->Release();
		CORE_DELETE(pImagePlayer);
        //CORE_DELETE(it->second);
        it++;
    }

    //m_mapBackImageIndexMap.clear();
	m_mapBackImageIndexMap.Clear();

    //std::map<int, CImagePlayer*>::iterator it_cover = m_mapCoverImageIndexMap.begin();
	TPodHashMap<int, CImagePlayer*, TPodTraits<int>, 
		TCoreAlloc>::iterator it_cover = m_mapCoverImageIndexMap.Begin();

    //while (it_cover != m_mapCoverImageIndexMap.end())
	while (it_cover != m_mapCoverImageIndexMap.End())
    {
        //CImagePlayer& imageplayer = *(it_cover->second);

        //if (imageplayer.IsReady())
        //{
            //imageplayer.Release();
        //}
		CImagePlayer* pImagePlayer = it_cover.GetData();

		pImagePlayer->Release();
		CORE_DELETE(pImagePlayer);
        //CORE_DELETE(it_cover->second);
        //it_cover->second = NULL;
        it_cover++;
    }

    //m_mapCoverImageIndexMap.clear();
	m_mapCoverImageIndexMap.Clear();

    TPodHashMap<int, CImagePlayer*, TPodTraits<int>, 
        TCoreAlloc>::iterator it_cooling = m_mapCoolingImage.Begin();
    while (it_cooling != m_mapCoolingImage.End())
    {
        CImagePlayer* pImagePlayer = it_cooling.GetData();
        pImagePlayer->Release();
        CORE_DELETE(pImagePlayer);
        it_cooling++;
    }
    m_mapCoolingImage.Clear();
}

bool ImageGrid::Clear()
{
    // �ͷ����и����е�ͼƬ
	ReleaseImages();

    // �����Ʒ
    std::vector<ImageItem*>::iterator it = m_vItems.begin();

    while(it != m_vItems.end())
    {
		CORE_DELETE((*it)->pCoolItem);
        CORE_DELETE(*it);
        ++it;
    }

    m_vItems.clear();

    if (GetCore()->GetEntity(GetVScrollBar()) != NULL)
    {
        UpdateScroll();
    }

	return true;
}

bool ImageGrid::SetItemImage(int index, const char* pImageName)
{
	if (!pImageName)
	{
		return false;
	}

	long UsedItemIndex = GetItem(index);

	if (UsedItemIndex == -1)
	{
		return false;
	}

	ImageItem& item = *m_vItems[UsedItemIndex];
	
	if (!item.Image.IsNull())
	{
		// �������ͼƬ������
        if (item.IsPng)
        {
            GetPainter()->ReleaseGlyph(item.Image);
        }
        else
        {
            GetPainter()->ReleaseGlyph(item.Image);
        }
	}

    item.ImageName = pImageName;
    item.rect.left = 0;
    item.rect.top = 0;
	item.Image = CreateGlyph(pImageName, item.IsPng, item.rect.right, 
		item.rect.bottom);

	return true;
}

const char* ImageGrid::GetItemImage(int index)
{
	long UsedItemIndex = GetItem(index);

	if (UsedItemIndex == -1)
	{
		return "";
	}

	ImageItem& item = *m_vItems[UsedItemIndex];

    return item.ImageName.c_str();
}

// ��ͼƬת�ɺڰ���ʾ
bool ImageGrid::ChangeItemImageToBW(int index, bool bChange)
{
    long UsedItemIndex = GetItem(index);

    if (UsedItemIndex == -1)
    {
        return false;
    }

    ImageItem& item = *m_vItems[UsedItemIndex];

    item.IsToBlackAndWhite = bChange;

    return true;
}

bool ImageGrid::SetItemName(int index, const wchar_t* pItemName)
{
	long UsedItemIndex = GetItem(index);

	if (UsedItemIndex == -1)
	{
		return false;
	}

	ImageItem &item = *m_vItems[UsedItemIndex];

	item.wcsName = pItemName;

	return true;
}

const wchar_t* ImageGrid::GetItemName(int index)
{
    long UsedItemIndex = GetItem(index);

    if (UsedItemIndex == -1)
    {
        return L"";
    }

    ImageItem &item = *m_vItems[UsedItemIndex];

    return item.wcsName.c_str();
}

int ImageGrid::FindItem(const wchar_t* pItemName)
{
    for(size_t i=0; i<m_vItems.size(); ++i)
    {
        if (m_vItems[i]->wcsName == pItemName)
        {
            return m_vItems[i]->Index;
        }
    }

    return -1;
}

bool ImageGrid::SetItemNumber(int index, int ItemNumber)
{
	long UsedItemIndex = GetItem(index);

	if (UsedItemIndex == -1)
	{
		return false;
	}

	ImageItem &item = *m_vItems[UsedItemIndex];

	item.Number = ItemNumber;

	return true;
}

int ImageGrid::GetItemNumber(int index)
{
	long UsedItemIndex = GetItem(index);

	if (UsedItemIndex == -1)
	{
		return -1;
	}

	ImageItem &item = *m_vItems[UsedItemIndex];

	return item.Number;
}

bool ImageGrid::SetItemMark(int index, int lMark)
{
	// ��ȡ��m_vItems�е��±�
	long UsedItemIndex = GetItem(index);

	if (UsedItemIndex == -1)
	{
		return false;
	}

	ImageItem &item = *m_vItems[UsedItemIndex];

	item.lMark = lMark;

	return true;
}

int ImageGrid::GetItemMark(int index)
{
	long UsedItemIndex = GetItem(index);

	if (UsedItemIndex == -1)
	{
		return -1;
	}

	ImageItem &item = *m_vItems[UsedItemIndex];

	return (int)item.lMark;
}

bool ImageGrid::SetCoolType(int index, long lCoolType)
{
	long UsedItemIndex = GetItem(index);

	if (UsedItemIndex == -1)
	{
		return false;
	}

	ImageItem &item = *m_vItems[UsedItemIndex];

    item.cool = lCoolType;

	CoolManager* pCoolManager = InnerGetCoolManager();

	if (pCoolManager)
	{
		pCoolManager->AddCool(lCoolType);
    }

	return false;
}

bool ImageGrid::SetCoolTeam(int index, long lCoolTeam)
{
	long UsedItemIndex = GetItem(index);

	if (UsedItemIndex == -1)
	{
		return false;
	}

	ImageItem &item = *m_vItems[UsedItemIndex];

	item.coolteam = lCoolTeam;

	CoolManager* pCoolManager = InnerGetCoolManager();

	if (pCoolManager)
	{
		pCoolManager->AddCool(lCoolTeam);
    }
	return true;
}

// ���Ӹ�����ȴ��Ч��add by zhum,2011.8.10
bool ImageGrid::AddCool(int index,long	lcooltime,float bit)
{
	long UsedItemIndex = GetItem(index);

	if (UsedItemIndex == -1)
	{
		return false;
	}

	ImageItem &item = *m_vItems[UsedItemIndex];

	if(item.pCoolItem!=NULL)
	{
		return item.pCoolItem->StartCool(0,lcooltime,(long)bit);
	}

	return true;
}

// ���Ӹ�����ȴ��Ч��add by zhum,2011.8.10
bool ImageGrid::StopCool(int index)
{
	long UsedItemIndex = GetItem(index);

	if (UsedItemIndex == -1)
	{
		return false;
	}

	ImageItem &item = *m_vItems[UsedItemIndex];

	if(item.pCoolItem!=NULL)
		item.pCoolItem->ClearCooling();

	return true;
}

bool ImageGrid::IsCooling(int index)
{
    long UsedItemIndex = GetItem(index);
    if (UsedItemIndex == -1)
    {
        return false;
    }

    ImageItem &item = *m_vItems[UsedItemIndex];
    if(item.pCoolItem == NULL)
    {
        return false;
    }

    return item.pCoolItem->IsCooling();
}

bool ImageGrid::SetItemAlpha(int itemindex, int alpha)
{
	long UsedItemIndex = GetItem(itemindex);

	if (UsedItemIndex == -1)
	{
		return false;
	}

	ImageItem &item = *m_vItems[UsedItemIndex];

	item.Alpha = alpha;

	return true;
}

int ImageGrid::GetItemAlpha(int itemindex)
{
	long UsedItemIndex = GetItem(itemindex);

	if (UsedItemIndex == -1)
	{
		return false;
	}

	ImageItem &item = *m_vItems[UsedItemIndex];

	return item.Alpha;
}

bool ImageGrid::CoverItem(int index, bool bCover)
{
	if (bCover)
    {
        //m_vCoverMap.Set(index, bCover);
		if(m_vCoverMap.Find(index)==m_vCoverMap.End())
		{
			m_vCoverMap.Add(index, bCover);
		}
		//m_vCoverMap[index] = bCover;
    }
    else
    {
        if (m_vCoverMap.Exists(index))
		{
			m_vCoverMap.Remove(index);
		}
		//std::map<long, bool>::iterator it = m_vCoverMap.find(index);

        //if (it != m_vCoverMap.end())
        //{
        //   m_vCoverMap.erase(it);
        //}
    }

	return true;
}

bool ImageGrid::LockItem(int index, bool bLock)
{
	long UsedItemIndex = GetItem(index);

	if (UsedItemIndex == -1)
	{
		return false;
	}

	ImageItem &item = *m_vItems[UsedItemIndex];

	item.IsLock = bLock;

	return true;
}

// ����Ĭ����ɫ
void ImageGrid::SetSelectColor(const char* szColor)
{
	m_SelectColor = GuiUtil_ColorToInt(szColor);
}

result_string ImageGrid::GetSelectColor()
{
	return GuiUtil_IntToColor(m_SelectColor).c_str();
}

void ImageGrid::SetMouseInColor(const char* szColor)
{
	m_MouseInColor = GuiUtil_ColorToInt(szColor);
}

result_string ImageGrid::GetMouseInColor()
{
	return GuiUtil_IntToColor(m_MouseInColor).c_str();
}

void ImageGrid::SetCoverColor(const char* szColor)
{
	m_CoverColor = GuiUtil_ColorToInt(szColor);
}

result_string ImageGrid::GetCoverColor()
{
    return GuiUtil_IntToColor(m_CoverColor).c_str();
}

result_string ImageGrid::GetLockColor()
{
    return GuiUtil_IntToColor(m_LockColor).c_str();
}

void ImageGrid::SetLockColor(const char* szColor)
{
    m_LockColor = GuiUtil_ColorToInt(szColor);
}

void ImageGrid::SetCoolColor(const char* szColor)
{
    m_CoolColor = GuiUtil_ColorToInt(szColor);
}

result_string ImageGrid::GetCoolColor()
{
	return GuiUtil_IntToColor(m_CoolColor).c_str();
}

// ������ʾ������:0,0,120,40
void ImageGrid::SetViewRect(const char* RectStr)
{
	GuiUtil_StringToRect(RectStr, m_ViewRect);
}

result_string ImageGrid::GetViewRect()
{
	return GuiUtil_RectToString(m_ViewRect).c_str();
}

PERSISTID ImageGrid::CreateGlyph(const char* pName, bool& ispng, 
	int& width, int& height)
{
    Assert(pName);

    ImageAnimationManager* pAniManager = InnerGetAnimation();

    if (pAniManager)
    {
        if (pAniManager->IsAnimation(pName))
        {
            width = m_nGridWidth;
            height = m_nGridHeight;

            return PERSISTID();
        }
    }

    IPainter* pPainter = GetPainter();

    Assert(pPainter);

    PERSISTID id = pPainter->CreateGlyph(pName, false);
    
    // ���Դ���PNG
    if (pPainter->GetGlyphValid(id))
    {
        ispng = true;   //
        width = pPainter->GetGlyphWidth(id);
        height = pPainter->GetGlyphHeight(id);
        return id;
    }

    // ���Դ���TGA
    //id = pPainter->CreateTexture(pName);

    //if (pPainter->GetTextureValid(id))
    //{
    //    ispng = false;   //
    //    width = pPainter->GetTextureWidth(id);
    //    height = pPainter->GetTextureHeight(id);
    //    return id;
    //}

    width = m_nGridWidth;
    height = m_nGridHeight;

    return PERSISTID();

}


ImageAnimationManager* ImageGrid::InnerGetAnimation()
{
	const IVar& var = GetCore()->GetGlobalValue("ImageAnimationManager");
	return (ImageAnimationManager*)GetCore()->GetEntity(var.ObjectVal());
	//return (ImageAnimationManager*)GetCore()->GetSingle(
	//    "ImageAnimationManager");
}

CoolManager* ImageGrid::InnerGetCoolManager()
{
    static CoolManager * s_pCoolManager = NULL;

    if (s_pCoolManager == NULL)
    {
		const IVar& varCoolMrg = GetCore()->GetGlobalValue("CoolManager");		
        s_pCoolManager = (CoolManager*)GetCore()->GetEntity(varCoolMrg.ObjectVal());
    }

	// ��õ���ʵ��
	return s_pCoolManager;
}

void ImageGrid::SetImageModeByString(ImageHelp& image, const char* pName)
{
	if (pName == NULL)
	{
		image.mode = HELP_NONE;
		return;
	}
	
	image.name = pName;

	if (stricmp(pName, "rect") == 0)
	{
		image.mode = HELP_RECT;
        image.width = m_nGridWidth;
        image.height = m_nGridHeight;
		return;
	}

	ImageAnimationManager* pAniManager = InnerGetAnimation();

	if (pAniManager)
	{
		if (pAniManager->IsAnimation(pName))
		{
			image.mode = HELP_ANIMATION;
            point_t pt = pAniManager->GetAnimationSize(pName);
            image.width = pt.x;
            image.height = pt.y;
			return;
		}
	}

	image.image.SetName(pName);
	image.image.Create(GetPainter(), GetGui(), GetAsyncLoad());

	if (image.image.IsReady())
	{
		image.mode = HELP_TEXURE;
        image.width = image.image.GetWidth();
        image.height = image.image.GetHeight();
		return;
	}

	image.image.Release();
	image.mode = HELP_NONE;
}

void ImageGrid::SetDrawMouseIn(const char* pName)
{
	SetImageModeByString(m_MouseInImage, pName);
}

void ImageGrid::SetDrawMouseSelect(const char* pName)
{
	SetImageModeByString(m_SelectImage, pName);
}

void ImageGrid::SetDrawCover(const char* pName)
{
	SetImageModeByString(m_CoverImage, pName);
}

void ImageGrid::SetDrawLock(const char* pName)
{
	SetImageModeByString(m_LockImage, pName);
}

void ImageGrid::SetDrawGridBack(const char* pName)
{
    m_GridBack.image.SetName(pName);
    m_GridBack.image.Create(GetPainter(), GetGui(), GetAsyncLoad());

    //if (m_GridBack.image.IsReady())
    //{
    //    return;
    //}
}

const char* ImageGrid::GetItemBackImage(int index)
{
    CImagePlayer* pImagePlayer;

	if (!m_mapBackImageIndexMap.GetData(index, pImagePlayer))
	{
		return "";
	}

	return pImagePlayer->GetName();
//	std::map<int, CImagePlayer*>::iterator it = 
//		m_mapBackImageIndexMap.find(index);
//
//    if (it != m_mapBackImageIndexMap.end())
//    {
//        CImagePlayer& imageplayer = *(it->second);
//
//        return imageplayer.GetName();
//    }
//
//    return "";
}

bool ImageGrid::SetItemBackImage(int index, const char* pImageName)
{
    //std::map<int, CImagePlayer*>::iterator it = 
	//	m_mapBackImageIndexMap.find(index);
	CImagePlayer* pImagePlayer;

    //if (it != m_mapBackImageIndexMap.end())
	if (m_mapBackImageIndexMap.GetData(index, pImagePlayer))
    {
         //CImagePlayer& imageplayer = *(it->second);

         //if (stricmp(imageplayer.GetName(), pImageName) == 0)
		if (stricmp(pImagePlayer->GetName(), pImageName) == 0)
        {
             // ��ԭ��һ������������
            return true;
        }

         //if (imageplayer.IsReady())
         //{
             //imageplayer.Release();
         //}
		pImagePlayer->Release();

        if ((pImageName == NULL) || (pImageName[0] == 0))
        {
             // ���
			CORE_DELETE(pImagePlayer);
			m_mapBackImageIndexMap.Remove(index);
            //CORE_DELETE(it->second);
            //it->second = NULL;
            //m_mapBackImageIndexMap[index] = NULL;
            return true;
        }

        //imageplayer.SetName(pImageName);
		pImagePlayer->SetName(pImageName);

        //return imageplayer.Create(GetPainter(), GetGui(), GetAsyncLoad());
		return pImagePlayer->Create(GetPainter(), GetGui(), GetAsyncLoad());
    }

    if ((pImageName == NULL) || (pImageName[0] == 0))
    {
        // ���
        return true;
    }

    CImagePlayer* newimageplayer = CORE_NEW(CImagePlayer);
 
    newimageplayer->SetName(pImageName);
    newimageplayer->Create(GetPainter(), GetGui(), GetAsyncLoad());

    //m_mapBackImageIndexMap[index] = newimageplayer;
	m_mapBackImageIndexMap.Add(index, newimageplayer);

    return true;
}

const char* ImageGrid::GetItemCoverImage(int index)
{
    CImagePlayer* pImagePlayer;

	if (!m_mapCoverImageIndexMap.GetData(index, pImagePlayer))
	{
		return "";
	}

	return pImagePlayer->GetName();
//	std::map<int, CImagePlayer*>::iterator it = 
//        m_mapCoverImageIndexMap.find(index);
//
//    if (it != m_mapCoverImageIndexMap.end())
//    {
//        CImagePlayer& imageplayer = *(it->second);
//
//        return imageplayer.GetName();
//    }
//
//    return "";
}

bool ImageGrid::SetItemCoverImage(int index, const char* pImageName)
{
    //std::map<int, CImagePlayer*>::iterator it = 
      //  m_mapCoverImageIndexMap.find(index);
	CImagePlayer* pImagePlayer;

    //if (it != m_mapCoverImageIndexMap.end())
	if (m_mapCoverImageIndexMap.GetData(index, pImagePlayer))
    {
        //CImagePlayer& imageplayer = *(it->second);

        //if (stricmp(imageplayer.GetName(), pImageName) == 0)
		if (stricmp(pImagePlayer->GetName(), pImageName) == 0)
        {
            // ��ԭ��һ������������
            return true;
        }

        //if (imageplayer.IsReady())
        //{
            //imageplayer.Release();
        //}
		pImagePlayer->Release();

        if ((pImageName == NULL) || (pImageName[0] == 0))
        {
            // ���
            CORE_DELETE(pImagePlayer);
			m_mapCoverImageIndexMap.Remove(index);
			//CORE_DELETE(it->second);
            //it->second = NULL;
            //m_mapCoverImageIndexMap[index] = NULL;
            //m_mapCoverImageIndexMap.erase(it);
            return true;
        }

        //imageplayer.SetName(pImageName);
		pImagePlayer->SetName(pImageName);

        //return imageplayer.Create(GetPainter(), GetGui(), GetAsyncLoad());
		return pImagePlayer->Create(GetPainter(), GetGui(), GetAsyncLoad());
    }

    if ((pImageName == NULL) || (pImageName[0] == 0))
    {
        // ���
        return true;
    }

    CImagePlayer* newimageplayer = CORE_NEW(CImagePlayer);

    newimageplayer->SetName(pImageName);
    newimageplayer->Create(GetPainter(), GetGui(), GetAsyncLoad());

    m_mapCoverImageIndexMap.Add(index, newimageplayer);
	//m_mapCoverImageIndexMap[index] = newimageplayer;

    return true;
}

const char* ImageGrid::GetItemCoolingImage(int index)
{
    CImagePlayer* pImagePlayer;
    if (!m_mapCoolingImage.GetData(index, pImagePlayer))
    {
        return "";
    }

    return pImagePlayer->GetName();
}

bool ImageGrid::SetItemCoolingImage(int index, const char* pImageName)
{
    CImagePlayer* pImagePlayer;
    if (m_mapCoolingImage.GetData(index, pImagePlayer))
    {
        if (stricmp(pImagePlayer->GetName(), pImageName) == 0)
        {
            return true;
        }

        pImagePlayer->Release();

        if ((pImageName == NULL) || (pImageName[0] == 0))
        {
            CORE_DELETE(pImagePlayer);
            m_mapCoolingImage.Remove(index);
            return true;
        }

        pImagePlayer->SetName(pImageName);

        return pImagePlayer->Create(GetPainter(), GetGui(), GetAsyncLoad());
    }

    if ((pImageName == NULL) || (pImageName[0] == 0))
    {
        return true;
    }

    CImagePlayer* newimageplayer = CORE_NEW(CImagePlayer);
    newimageplayer->SetName(pImageName);
    newimageplayer->Create(GetPainter(), GetGui(), GetAsyncLoad());
    m_mapCoolingImage.Add(index, newimageplayer);

    return true;
}

void ImageGrid::SetGridsPos(const char* pGridsPos)
{
    if (!pGridsPos || pGridsPos[0] == 0)
    {
        m_szGridsPos = pGridsPos;
        m_vGridsPos.clear();
        return;
    }

    m_szGridsPos = pGridsPos;

    m_vGridsPos.clear();

	CVarList vData;

	StringUtil::ParseToVector(pGridsPos, ';', vData);

	for (size_t i = 0; i < vData.GetCount(); ++i)
	{
		CVarList vSubData;
		
		StringUtil::ParseToVector(vData.StringVal(i), ',', vSubData);

		if (vSubData.GetCount() == 2)
		{
			point_t pt;

			pt.x = atol(vSubData.StringVal(0));
			pt.y = atol(vSubData.StringVal(1));

			m_vGridsPos.push_back(pt);
		}
	}
    /*
	std::vector<std::string> vData;

    StringUtil::ParseToVector(vData, pGridsPos, ";");

    std::vector<std::string> vSubData;

    for (size_t i = 0; i < vData.size(); ++i)
    {
        StringUtil::ParseToVector(vSubData, vData[i].c_str(), ",");

        if (vSubData.size() == 2)
        {
            point_t pt;

            pt.x = atol(vSubData[0].c_str());
            pt.y = atol(vSubData[1].c_str());

            m_vGridsPos.push_back(pt);
        }
    }
	*/
}

void ImageGrid::SetCoolAnimation(const char* pName)
{
	if (pName == NULL)
	{
		return;
	}

	m_szCoolAnimation = pName;
}

void ImageGrid::SetMaxSize(const int value)
{
    m_nMaxSize = value;

    if (GetCore()->GetEntity(GetVScrollBar()) != NULL)
    {
        UpdateScroll();
    }
}

// ʵ��IControl����麯��
bool ImageGrid::Init(const IVarList& args)
{
	if (!IScrollable::Init(args))
	{
		return false;
	}

    SetHasVScroll(true);
	GetCore()->AddExecute(this);
	return true;
}

bool ImageGrid::Shut()
{
    GetCore()->RemoveExecute(this);
    ReleaseControls();
	IScrollable::Shut();
	return true;
}

void ImageGrid::ReleaseControls()
{
    // �ͷ����и����е�ͼƬ
    ReleaseImages();

    // �����Ʒ
    std::vector<ImageItem*>::iterator it = m_vItems.begin();

    while(it != m_vItems.end())
    {
		CORE_DELETE((*it)->pCoolItem);
        CORE_DELETE(*it);
        ++it;
    }

    m_vItems.clear();

    //if (GetPainter())
    //{
        m_SelectImage.image.Release();		// ����Ƴ�
        m_MouseInImage.image.Release();		// �������
        m_CoverImage.image.Release();			// ����
        m_LockImage.image.Release();			// ����
        m_GridBack.image.Release();
    //}
}

void ImageGrid::PrepareDesignInfo(IDesignInfo* di)
{
	IScrollable::PrepareDesignInfo(di);

	CVar varDefault;
	//�����Ƿ��й�����
	di->AddPropInfo("Solid", IDesignInfo::TYPE_BOOL, true, true,
		&(varDefault.SetBool(true),varDefault));
	di->AddPropInfo("ScrollSize", IDesignInfo::TYPE_INT, true, true, 
		&(varDefault.SetInt(17),varDefault));
	di->AddPropInfo("HasVScroll", IDesignInfo::TYPE_BOOL, true, true, 
		&(varDefault.SetBool(true),varDefault));
	di->AddPropInfo("AlwaysVScroll", IDesignInfo::TYPE_BOOL, true, true, 
		&(varDefault.SetBool(false),varDefault));
	di->AddPropInfo("VScrollLeft", IDesignInfo::TYPE_BOOL, true, true, 
		&(varDefault.SetBool(false),varDefault));

	di->AddPropInfo("DrawMouseIn", IDesignInfo::TYPE_STRING, true, false);
	di->AddPropInfo("DrawMouseSelect", IDesignInfo::TYPE_STRING, true, false);
	di->AddPropInfo("DrawCover", IDesignInfo::TYPE_STRING, true, false);
	di->AddPropInfo("DrawLock", IDesignInfo::TYPE_STRING, true, false);
	di->AddPropInfo("DrawGridBack", IDesignInfo::TYPE_IMAGE, true, false);
    di->AddPropInfo("GridBackOffsetX", IDesignInfo::TYPE_INT, true, false);
    di->AddPropInfo("GridBackOffsetY", IDesignInfo::TYPE_INT, true, false);
    di->AddPropInfo("GridCoverOffsetX", IDesignInfo::TYPE_INT, true, false);
    di->AddPropInfo("GridCoverOffsetY", IDesignInfo::TYPE_INT, true, false);
    di->AddPropInfo("SelectColor", IDesignInfo::TYPE_COLOR, true, true);
    di->AddPropInfo("MouseInColor", IDesignInfo::TYPE_COLOR, true, true);
    di->AddPropInfo("CoverColor", IDesignInfo::TYPE_COLOR, true, true);
    di->AddPropInfo("LockColor", IDesignInfo::TYPE_COLOR, true, true);
    di->AddPropInfo("CoolColor", IDesignInfo::TYPE_COLOR, true, true);
	di->AddPropInfo("CoolAnimation", IDesignInfo::TYPE_STRING, true, true);

    di->AddPropInfo("ShowMouseDownState", IDesignInfo::TYPE_BOOL, true, false, &(varDefault.SetBool(true),varDefault));
    di->AddPropInfo("MouseDownScale", IDesignInfo::TYPE_FLOAT, true, false);
    di->AddPropInfo("MouseDownOffsetX", IDesignInfo::TYPE_INT, true, false);
    di->AddPropInfo("MouseDownOffsetY", IDesignInfo::TYPE_INT, true, false);
    di->AddPropInfo("MouseDownAlpha", IDesignInfo::TYPE_INT, true, false);

	di->AddPropInfo("ViewRect", IDesignInfo::TYPE_STRING, true, false);

	di->AddPropInfo("MaxSize", IDesignInfo::TYPE_INT, true, false);
	di->AddPropInfo("RowNum", IDesignInfo::TYPE_INT, true, false);
	di->AddPropInfo("ClomnNum", IDesignInfo::TYPE_INT, true, false);

    di->AddPropInfo("RoundGrid", IDesignInfo::TYPE_BOOL, true, false, &(varDefault.SetBool(false),varDefault));
    di->AddPropInfo("ShowEmpty", IDesignInfo::TYPE_BOOL, true, false, &(varDefault.SetBool(true),varDefault));
    di->AddPropInfo("EmptyShowFade", IDesignInfo::TYPE_BOOL, true, false, &(varDefault.SetBool(false),varDefault));
	di->AddPropInfo("IsEditMode", IDesignInfo::TYPE_BOOL, true, false);
	di->AddPropInfo("Center", IDesignInfo::TYPE_BOOL, true, false, 
		&(varDefault.SetBool(m_bCenter),varDefault));
    di->AddPropInfo("FitGrid", IDesignInfo::TYPE_BOOL, true, false, 
        &(varDefault.SetBool(true),varDefault));
	di->AddPropInfo("GridOffsetX", IDesignInfo::TYPE_INT, true, false);
	di->AddPropInfo("GridOffsetY", IDesignInfo::TYPE_INT, true, false);

    di->AddPropInfo("GridHeight", IDesignInfo::TYPE_INT, true, false, 
		&(varDefault.SetInt(32),varDefault));
    di->AddPropInfo("GridWidth", IDesignInfo::TYPE_INT, true, false, 
		&(varDefault.SetInt(32),varDefault));
    di->AddPropInfo("GridsPos", IDesignInfo::TYPE_STRING, true, false);
    di->AddPropInfo("VScrollIndex", IDesignInfo::TYPE_INT, true, false);
	di->AddSuborInfo("VScrollBar", "ScrollBar");

	di->AddEventInfo("on_mousein_grid");
	di->AddEventInfo("on_mouseout_grid");
	di->AddEventInfo("on_select_changed");
	di->AddEventInfo("on_doubleclick_grid");
	di->AddEventInfo("on_rightclick_grid");
	di->AddEventInfo("on_leftclick_up");
	di->AddEventInfo("on_leftclick_down");
	di->AddEventInfo("on_mouse_move");
	di->AddEventInfo("on_drag_enter");
	di->AddEventInfo("on_drag_move");
	di->AddEventInfo("on_drag_leave");
}

bool ImageGrid::GetLoadFinish()
{
    if (GetLoadFinishValue())
    {
        return true;
    }

	if (!m_SelectImage.image.IsLoadCompelte())
	{
		return false;
	}

	if (!m_MouseInImage.image.IsLoadCompelte())
	{
		return false;
	}

	if (!m_CoverImage.image.IsLoadCompelte())
	{
		return false;
	}

	if (!m_LockImage.image.IsLoadCompelte())
	{
		return false;
	}

	if (!m_GridBack.image.IsLoadCompelte())
	{
		return false;
	}
	
	return IScrollable::GetLoadFinish();
}

bool ImageGrid::DrawGridBack(const int xbase, const int ybase, 
	const int GridWidth, const int GridHeight, const int nGridWidth, 
	const int nGridHeight, const int indexstart, const int indexend)
{
    // ��������б����Ļ��ͻ�����
    if (m_GridBack.image.IsReady())
    {
        long lBackWidth = m_GridBack.image.GetWidth();
        long lBackHeight = m_GridBack.image.GetHeight();

        long x = xbase;
        long y = ybase;

        for (int i = 0; i < m_nRow; ++i)
        {
            for (int j = 0; j < m_nClomn; ++j)
            {
                // ʵ�ʸ�����ţ���0��ʼ,��row * col - 1����
                int grid_index = i * m_nClomn + j;

                if (IsEmpty(grid_index + indexstart))
                {
                    // ��Ҫ��ʾ���������ڵ��뵭������Ҫ��ʾ
                    if(GetShowEmpty())
                    {
                        if (GetEmptyShowFade())
                        {
                            if (m_eEmptyDrawState != DRAW_STATE_NORMAL)
					        {
					            DrawSingleEmptyGridFading(grid_index, indexstart, xbase,
					                ybase, i, j, lBackWidth, lBackHeight);
                            }
                            else if (m_nElapseTime == 0)
                            {
                                // ������
                            }
                            else
                            {
						        point_t point = GetImagePos(grid_index, xbase, ybase, i, j, \
							        GridWidth, GridHeight);

						        DrawImageSingle(grid_index + indexstart, point, \
							        lBackWidth, lBackHeight, 0xffffffff);
                            }
                        }
                        else
                        {
						    point_t point = GetImagePos(grid_index, xbase, ybase, i, j, \
							    GridWidth, GridHeight);

						    DrawImageSingle(grid_index + indexstart, point, \
							    lBackWidth, lBackHeight, 0xffffffff);
                        }
                    }
					else
					{
						// ��ϣ����ʾ�յĸ��ӣ�����ʾ,ֱ�Ӵ�����һ��
						continue;
					}
                }
                else
                {
					point_t point = GetImagePos(grid_index, xbase, ybase, i, j, \
						GridWidth, GridHeight);
	
					DrawImageSingle(grid_index + indexstart, point, \
							lBackWidth, lBackHeight, 0xffffffff);

                }

                x += GridWidth;
            }

            y += GridHeight;
            x = xbase;
        }
    }
    return true;
}

bool ImageGrid::DrawFirstTime()
{
    if (m_IsFirstTime)
    {
        m_IsFirstTime = false;

        int nGridWidth = m_nGridWidth;
        int nGridHeight = m_nGridHeight;

        int GridWidth = (m_ViewRect.right - m_ViewRect.left) / m_nClomn;
        int GridHeight = (m_ViewRect.bottom - m_ViewRect.top) / m_nRow;

        long x = 0;
        long y = 0;

        size_t nSetSize = m_vGridsPos.size();

        point_t point;

        for (long i = 0; i < m_nRow; ++i)
        {
            for (long j = 0; j < m_nClomn; ++j)
            {
                long index = i * m_nClomn + j;

                if (index >= (long)nSetSize)
                {
                    point.x = x;
                    point.y = y;

                    m_vGridsPos.push_back(point);
                }

                x += GridWidth;
            }

            y += GridHeight;
            x = 0;
        }

        return true;
    }
    return false;
}

bool ImageGrid::DrawCover(const int xbase, const int ybase, 
                          const int GridWidth, const int GridHeight, const int nGridWidth, const int nGridHeight,
                          const int indexstart, const int indexend)
{

	ImageAnimationManager* pAniManager = InnerGetAnimation();

	if (NULL == pAniManager)
	{
		return false;
	}

    // ��������б����õĻ�����ƽ��ÿ�
    //if (m_vCoverMap.size() > 0)
	if (m_vCoverMap.GetCount() > 0)
    {
		TPodHashMap<int, bool, TPodTraits<int>, TCoreAlloc>::iterator it1 =
			m_vCoverMap.Begin();
		//std::map<long, bool>::iterator it = m_vCoverMap.begin();

        //for (; it != m_vCoverMap.end(); it++)
		for (; it1 != m_vCoverMap.End(); ++it1)
        {
            //long item_index = it->first;
			long item_index = it1.GetKey();

            if (item_index < (int)indexstart || item_index >= (int)indexend)
            {
                continue;
            }

            if (!m_bShowEmpty && IsEmpty(item_index))
            {
                // ��ϣ����ʾ�յĸ��ӣ�����ʾ,ֱ�Ӵ�����һ��
                continue;
            }

            int Row = (item_index - indexstart) / m_nClomn;
            int Clomn = (item_index - indexstart) % m_nClomn;

            point_t point = GetImagePos(item_index, xbase, ybase, Row, Clomn, 
				GridWidth, GridHeight);

            //std::map<int, CImagePlayer*>::iterator it = 
			//	m_mapCoverImageIndexMap.find(item_index);
			CImagePlayer* pImagePlayer;

            //if (it != m_mapCoverImageIndexMap.end())
			if (m_mapCoverImageIndexMap.GetData(item_index, pImagePlayer))
            {
                //CImagePlayer* pImagePlayer = it->second;

                if (pImagePlayer)
                {
                    if (m_bCenter)
                    {
                        int dx = (int)(nGridWidth - pImagePlayer->GetWidth()) / 2;
                        int dy = (int)(nGridHeight - pImagePlayer->GetHeight()) / 2;

                        point.x += dx;
                        point.y += dy;
                    }
                    else
                    {
                        point.x += m_nGridCoverOffsetX;
                        point.y += m_nGridCoverOffsetY;
                    }

					//-----------------------
					if (pAniManager->IsAnimation(pImagePlayer->GetName()))
					{
						point_t pt = pAniManager->GetAnimationSize(
							pImagePlayer->GetName());
						rect_t rect = { 0, 0, pt.x, pt.y };

						// ����ͼƬ�Ļ��� Ҳû�ø��ӿ�� 
						//if (m_bFitGrid)
						//{
						//	rect.right = nGridWidth;
						//	rect.bottom = nGridWidth;
						//}

						pAniManager->DrawAnimation(pImagePlayer->GetName(), 
							point.x, point.y, rect);
					}
					else
					{
						if (m_bRoundGrid)
						{
							pImagePlayer->DrawRound(CGuiConst::DRAWMODE_TILE,
								point.x, point.y, 
								point.x + pImagePlayer->GetWidth(), 
								point.y + pImagePlayer->GetHeight(), 100);
						}
						else
						{
							pImagePlayer->Draw(CGuiConst::DRAWMODE_TILE,
								point.x, point.y, 
								point.x + pImagePlayer->GetWidth(), 
								point.y + pImagePlayer->GetHeight());
						}
					}
                }
            }
            else
            {
                rect_t targetRect = {point.x, point.y, point.x + nGridWidth, 
					point.y + nGridHeight};

                DrawImageHelp(m_CoverImage, targetRect, targetRect, 
					m_CoverColor);
            }
        }
    }

    return true;
}

bool ImageGrid::DrawEdit(const rect_t ViewRect, const int xbase, const int ybase, 
    const int GridWidth, const int GridHeight, const int nGridWidth, 
	const int nGridHeight)
{
    GetPainter()->DrawRect(ViewRect.left, ViewRect.top, ViewRect.right, 
        ViewRect.bottom, IControl::GetLineColor());

    long x = xbase;
    long y = ybase;

    for (long i = 0; i < m_nRow; ++i)
    {
        for (long j = 0; j < m_nClomn; ++j)
        {
            long grid_index = i * m_nClomn + j;

            if (grid_index < (long)m_vGridsPos.size())
            {
                GetPainter()->DrawRect(
                    xbase + m_vGridsPos[grid_index].x, 
                    ybase + m_vGridsPos[grid_index].y, 
                    xbase + nGridWidth + m_vGridsPos[grid_index].x, 
                    ybase + nGridHeight + m_vGridsPos[grid_index].y, 
                    0xffff0000);
            }
            else
            {
                GetPainter()->DrawRect(x, y, 
                    x + nGridWidth, y + nGridHeight, 0xffff0000);
            }

            x += GridWidth;
        }

        y += GridHeight;
        x = xbase;
    }

    return true;
}

bool ImageGrid::DrawOneGrid(const ImageItem& item, 
                            const int xbase, const int ybase, const int x, const int y,
                            const int nGridWidth, const int nGridHeight)
{
    rect_t targetRect;

    targetRect.left = xbase + x;
    targetRect.top = ybase + y;

    if (m_bFitGrid)
    {
        targetRect.right = targetRect.left + nGridWidth;
        targetRect.bottom = targetRect.top + nGridHeight;
    }
    else if (m_bCenter)
    {
        int nImageWidthScale = (item.rect.right - item.rect.left);
        int nImageHeightScale = (item.rect.bottom - item.rect.top);
        int dx = ((int)nGridWidth - nImageWidthScale) / 2;
        int dy = ((int)nGridHeight - nImageHeightScale) / 2;

        targetRect.left += dx;
        targetRect.top += dy;
        targetRect.right = targetRect.left + nImageWidthScale;
        targetRect.bottom = targetRect.top + nImageHeightScale;
    }
    else
    {
        targetRect.left += m_nGridOffsetX;
        targetRect.top += m_nGridOffsetY;

        int nImageWidthScale = (item.rect.right - item.rect.left);
        int nImageHeightScale = (item.rect.bottom - item.rect.top);
        targetRect.right = targetRect.left + nImageWidthScale;
        targetRect.bottom = targetRect.top + nImageHeightScale;
    }

    // �ƶ��漼������ȴ����
    CoolManager* pCoolManager = InnerGetCoolManager();
    if (m_bIsMobileSkillUse && pCoolManager != NULL)
    {
        // ����m_nGridOffsetX,m_nGridOffsetY����������ȴȦ�Ĵ�С
        /*
        pCoolManager->DrawCooling(item.cool, item.coolteam, GetPainter(), 
            targetRect.left + m_nGridOffsetX, targetRect.top + m_nGridOffsetY, 
            targetRect.right - targetRect.left - 2*m_nGridOffsetX, 
            targetRect.bottom - targetRect.top - 2*m_nGridOffsetY, 
            m_CoolColor, m_szCoolAnimation.c_str(), true);*/

        

        // ��ȴͼ��Ⱦ
        CImagePlayer* pCoolingImage;
        if (m_mapCoolingImage.GetData(item.Index, pCoolingImage))
        {
            if (pCoolingImage)
            {
                pCoolManager->DrawCooling(item.cool, item.coolteam, pCoolingImage, 
                    targetRect.left, targetRect.top, 
                    targetRect.right, targetRect.bottom);
            }
        } 

        // �ڵ���ȴ��Եë�ߵ�����ͼ
        CImagePlayer* pImagePlayer;
        if (m_mapCoverImageIndexMap.GetData(item.Index, pImagePlayer))
        {
            if (pImagePlayer)
            {
                pImagePlayer->Draw(CGuiConst::DRAWMODE_TILE,
                    targetRect.left, targetRect.top, 
                    targetRect.right, targetRect.bottom);
            }
        }
    }

    int old_alpha = 255;
    if (m_bMouseDownState &&
        (GetMouseDownState(item.Index) || 
		 m_nCurLButtonDown == item.Index ||
         m_nCurRButtonDown == item.Index))
    {
        int target_width = (targetRect.right - targetRect.left) * (int)(m_fMouseDownScale * 100) / 100;
        int target_height = (targetRect.bottom - targetRect.top) * (int)(m_fMouseDownScale * 100) / 100;
        if (m_bIsMobileSkillUse)
        {
            int change_width_half = (target_width - (targetRect.right - targetRect.left)) / 2;
            int change_height_half = (target_height - (targetRect.bottom - targetRect.top)) / 2;
            targetRect.left = targetRect.left + m_nMouseDownOffsetX - change_width_half;
            targetRect.top = targetRect.top + m_nMouseDownOffsetY - change_height_half;
            targetRect.right += change_width_half;
            targetRect.bottom += change_height_half;
        }
        else
        {
            targetRect.left += m_nMouseDownOffsetX;
            targetRect.top += m_nMouseDownOffsetY;
            targetRect.right = targetRect.left + target_width;
            targetRect.bottom = targetRect.top + target_height;
        }

        old_alpha = GetPainter()->GetBlendAlpha();
        GetPainter()->SetBlendAlpha(m_nMouseDownAlpha * old_alpha / 255);
    }

    if (!item.Image.IsNull())
    {
        if (item.IsPng)
        {
            if (m_bRoundGrid)
            {
                int m_nSideCut = 1;

                if (item.IsToBlackAndWhite)
                {
                    GetPainter()->DrawBWGlyphRound(
                        targetRect.left + m_nSideCut, 
                        targetRect.top + m_nSideCut, 
                        targetRect.right - m_nSideCut,
                        targetRect.bottom - m_nSideCut, 
                        item.rect.left + m_nSideCut, 
                        item.rect.top + m_nSideCut, 
                        item.rect.right - m_nSideCut, 
                        item.rect.bottom - m_nSideCut, 
                        item.Image, 20);
                }
                else
                {
                    GetPainter()->DrawGlyphRound(
                        targetRect.left + m_nSideCut, 
                        targetRect.top + m_nSideCut, 
                        targetRect.right - m_nSideCut,
                        targetRect.bottom - m_nSideCut, 
                        item.rect.left + m_nSideCut, 
                        item.rect.top + m_nSideCut, 
                        item.rect.right - m_nSideCut, 
                        item.rect.bottom - m_nSideCut, 
                        item.Image, 20);
                }
            }
            else
            {
                if (item.IsToBlackAndWhite)
                {
                    GetPainter()->DrawBWGlyph(targetRect.left, 
                        targetRect.top, targetRect.right, targetRect.bottom, 
                        item.rect.left, item.rect.top, 
                        item.rect.right, item.rect.bottom, 
                        item.Image);
                }
                else
                {
                    // �ƶ��汾������������ȴʱ����ͼ���û�
                    if (m_bIsMobileSkillUse && pCoolManager != NULL 
                        && pCoolManager->IsCooling(item.cool, item.coolteam))
                    {
                        GetPainter()->DrawAddGlyph(targetRect.left,targetRect.top, 
                            targetRect.right, targetRect.bottom, 
                            item.rect.left, item.rect.top, 
                            item.rect.right, item.rect.bottom, 
                            item.Image, 0, 0xff787878);
                    }
                    else
                    {
                        GetPainter()->DrawGlyph(targetRect.left,targetRect.top, 
                            targetRect.right, targetRect.bottom, 
                            item.rect.left, item.rect.top, 
                            item.rect.right, item.rect.bottom, 
                            item.Image);
                    }   
                }
            }
        }
        else
        {
            if (m_bRoundGrid)
            {
                GetPainter()->DrawGlyphRound(targetRect.left ,targetRect.top, 
                    targetRect.right, targetRect.bottom, 
                    item.rect.left, item.rect.top, 
                    item.rect.right, item.rect.bottom, 
                    item.Image, 20);
            }
            else
            {
                GetPainter()->DrawGlyph(targetRect.left ,targetRect.top, 
                    targetRect.right, targetRect.bottom, 
                    item.rect.left, item.rect.top, 
                    item.rect.right, item.rect.bottom, 
                    item.Image);
            }
        }
    }
    else
    {
        ImageAnimationManager* pAniManager = InnerGetAnimation();

        if (pAniManager)
        {
            point_t pt = pAniManager->GetAnimationSize(
                item.ImageName.c_str());
            rect_t rect = { 0, 0, pt.x, pt.y };

            if (m_bFitGrid)
            {
                rect.right = (int)nGridWidth;
                rect.bottom = (int)nGridHeight;
            }

            pAniManager->DrawAnimation(item.ImageName.c_str(), 
                targetRect.left, targetRect.top, rect);
        }
    }

    if (m_nCurLButtonDown == item.Index)
    {
        GetPainter()->SetBlendAlpha(old_alpha);
    }

    rect_t gridRect = { xbase + x, ybase + y, 
        xbase + x + nGridWidth, ybase + y + nGridHeight };

    if (item.IsLock)
    {
        DrawImageHelp(m_LockImage, targetRect, gridRect, m_LockColor);
    }

    if (item.Number > 1)
    {
        wchar_t wcsBuffer[51];

        SafeSwprintf(wcsBuffer, sizeof(wcsBuffer), L"%d", item.Number);

        GetGui()->SetFont(IControl::GetFontName(), GetShadowColor());
        GetPainter()->WriteTextRight(targetRect.left, 
            targetRect.bottom - GetPainter()->GetTextHeight(NULL), 
            targetRect.right, targetRect.bottom, 0xffffffff, wcsBuffer);
    }

    // ������ȴ
    if (pCoolManager != NULL && !m_bIsMobileSkillUse)
    {
        pCoolManager->DrawCooling(item.cool, item.coolteam, GetPainter(), 
            targetRect.left, targetRect.top, 
            targetRect.right - targetRect.left, 
            targetRect.bottom - targetRect.top, m_CoolColor, m_szCoolAnimation.c_str(), m_bRoundGrid);

		ImageAnimationManager * pAniManger = 
			(ImageAnimationManager * )GetCore()->GetEntity(pCoolManager->GetAniMng());

		item.pCoolItem->DrawCooling(GetPainter(), 
			targetRect.left, targetRect.top, 
			targetRect.right - targetRect.left, 
			targetRect.bottom - targetRect.top, m_CoolColor, pAniManger,m_szCoolAnimation.c_str());
    }

    return true;
}


bool ImageGrid::DrawImageHelpByIndex(ImageHelp& imagehelp, int nColor, 
                                            int item_index, int indexstart, 
                                            int indexend, int xbase, int ybase, 
                                            int GridWidth, int GridHeight, 
                                            int nGridWidth, int nGridHeight)
{
    if (item_index > -1 && //�����������ĸ���
        item_index >= indexstart && item_index < indexend) //֮ǰû�л��ƹ�
    {

        if (! m_bShowEmpty && IsEmpty(item_index))
        {
            // ��ϣ����ʾ�յĸ��ӣ�����ʾ,ֱ�Ӵ�����һ��
            return true;
        }

        int Row = (item_index - indexstart) / m_nClomn;
        int Clomn = (item_index - indexstart) % m_nClomn;

        rect_t targetRect;

        int grid_index = item_index - indexstart;

        if (grid_index < (long)m_vGridsPos.size())
        {
            targetRect.left = xbase + m_vGridsPos[grid_index].x;
            targetRect.top = ybase + m_vGridsPos[grid_index].y;
            targetRect.right = (int)(targetRect.left + nGridWidth);
            targetRect.bottom = (int)(targetRect.top + nGridHeight);
        }
        else
        {
            targetRect.left = (int)(Clomn * GridWidth + xbase);
            targetRect.top = (int)(Row * GridHeight + ybase);
            targetRect.right = (int)(targetRect.left + nGridWidth);
            targetRect.bottom = (int)(targetRect.top + nGridHeight);
        }

        // ��߲���Ҫ����fit���������������������ѿ�
        if (m_bCenter)
        {
            int dx = (int)(nGridWidth - imagehelp.width) / 2;
            int dy = (int)(nGridHeight - imagehelp.height) / 2;

            targetRect.left += dx;
            targetRect.right += dx;
            targetRect.top += dy;
            targetRect.bottom += dy;
        }
        else
        {
            targetRect.left += m_nGridOffsetX;
            targetRect.right += m_nGridOffsetX;
            targetRect.top += m_nGridOffsetY;
            targetRect.bottom += m_nGridOffsetY;
        }

        DrawImageHelp(imagehelp, targetRect, targetRect, nColor);
    }

    return true;
}

void ImageGrid::DrawImageHelp(ImageHelp& image, rect_t& targetrect, 
                              rect_t& gridrect, unsigned long dwColor)
{
    if (image.mode == HELP_NONE)
    {
        return;
    }

    if (image.mode == HELP_TEXURE)
    {
        //if (!m_bRoundGrid)
        //{
            if (m_bFitGrid)
            {
                image.image.Draw(CGuiConst::DRAWMODE_FITWINDOW, 
                    targetrect.left, targetrect.top, targetrect.right, 
                    targetrect.bottom);
            }
            else if (m_bCenter)
            {
                image.image.Draw(CGuiConst::DRAWMODE_CENTER, 
                    targetrect.left, targetrect.top, targetrect.right, 
                    targetrect.bottom);
            }
            else
            {
                image.image.Draw(CGuiConst::DRAWMODE_TILE, 
                    targetrect.left, targetrect.top, 
				    targetrect.left + image.image.GetWidth(), 
                    targetrect.top + image.image.GetHeight());
            }
        //}
        /*else
        {
            if (m_bFitGrid)
            {
                image.image.DrawRound(CGuiConst::DRAWMODE_FITWINDOW, 
                    targetrect.left, targetrect.top, targetrect.right, 
                    targetrect.bottom, 100);
            }
            else if (m_bCenter)
            {
                image.image.DrawRound(CGuiConst::DRAWMODE_CENTER, 
                    targetrect.left, targetrect.top, targetrect.right, 
                    targetrect.bottom, 100);
            }
            else
            {
                image.image.DrawRound(CGuiConst::DRAWMODE_TILE, 
                    targetrect.left, targetrect.top, 
				    targetrect.left + image.image.GetWidth(), 
                    targetrect.top + image.image.GetHeight(), 100);
            }
        }*/
    }
    else if (image.mode == HELP_ANIMATION)
    {
        ImageAnimationManager* pAniManager = InnerGetAnimation();

        if (pAniManager)
        {
            point_t pt = pAniManager->GetAnimationSize(image.name.c_str());
            rect_t rect = { 0, 0, pt.x, pt.y };

            if (m_bFitGrid)
            {
                rect.right = m_nGridWidth;
                rect.bottom = m_nGridHeight;
            }

            pAniManager->DrawAnimation(image.name.c_str(), gridrect.left, 
                gridrect.top, rect);
        }
    }
    else if (image.mode == HELP_RECT)
    {
        if (m_bRoundGrid)
        {
            GetPainter()->FillRound(targetrect.left, targetrect.top, 
                targetrect.right, targetrect.bottom, dwColor, 100);
        }
        else
        {
            GetPainter()->FillRect(targetrect.left, targetrect.top, 
                targetrect.right, targetrect.bottom, dwColor);
        }
    }
}

void ImageGrid::Paint(float seconds)
{
    if (!GetVisible())
    {
        return;
    }

	RefreshInnerAbsPos();

    // ��һ�λ���,���г�ʼ��
    if (m_IsFirstTime)
    {
        DrawFirstTime();
    }

	IScrollable::Paint(seconds);

    // ��ʼ��һЩ��������
    // �༭ģʽ�²Ż���ʾ�߿�
    rect_t ViewRect = m_ViewRect;

	ViewRect.left = ViewRect.left + InnerGetAbsLeft();
	ViewRect.right = ViewRect.right + InnerGetAbsLeft();
	ViewRect.top = ViewRect.top + InnerGetAbsTop();
	ViewRect.bottom = ViewRect.bottom + InnerGetAbsTop();

    long ViewWidth = ViewRect.right - ViewRect.left;
    long ViewHeight = ViewRect.bottom - ViewRect.top;

	int nGridWidth = m_nGridWidth;
	int nGridHeight = m_nGridHeight;

    long xbase = ViewRect.left;
    long ybase = ViewRect.top;

    int GridWidth = ViewWidth / m_nClomn;
    int GridHeight = ViewHeight / m_nRow;

    ScrollBar* pScrollbar = (ScrollBar*)GetGui()->GetControl(GetVScrollBar());

    // �������ݸ߶��е�һ����,�����жϻ�������
    long lContentTop = m_nTopIndex;

    if (pScrollbar && pScrollbar->GetVisible())
    {
        lContentTop = pScrollbar->GetCurValue();
    }

    size_t indexstart = lContentTop * m_nClomn;
    size_t indexend = indexstart + m_nRow * m_nClomn;

    if (!m_bRoundGrid)
    {
        // ��������б����Ļ��ͻ�����, Բ���Ȼ���������Ȼ�
        DrawGridBack(xbase, ybase, GridWidth, GridHeight, nGridWidth, nGridHeight, indexstart, indexend);
    }

    // �༭ģʽ�»����߿�
    if (m_bEdit)
    {
        DrawEdit(ViewRect, xbase, ybase, GridWidth, GridHeight, nGridWidth, nGridHeight);
    }
    
	long CurRow = 0;
	long CurClomn = 0;

	long x = 0;
	long y = 0;

	long dx = 0;
	long dy = 0;

	for (size_t i = 0; i < m_vItems.size(); ++i)
	{
		if (m_vItems[i]->Index >= indexstart 
			&& m_vItems[i]->Index < indexend)
		{
            size_t realindex = (size_t)(m_vItems[i]->Index - indexstart);

			CurClomn =  realindex % m_nClomn;

			CurRow = realindex / m_nClomn;

            if (CurRow > m_nRow)
            {
                continue;
            }

            // ͨ�����������Ͻ�
            x = CurClomn * GridWidth;
            y = CurRow * GridHeight;

            // ������Զ���λ��,��ȡ�Զ���λ�����Ͻ�
            if (realindex < m_vGridsPos.size())
            {
                x = m_vGridsPos[realindex].x;
                y = m_vGridsPos[realindex].y;
            }

			int old_alpha = GetPainter()->GetBlendAlpha();
			int cur_alpha = GetPainter()->SetBlendAlpha(old_alpha * m_vItems[i]->Alpha / 255);

            DrawOneGrid(*m_vItems[i], xbase, ybase, x, y, 
				nGridWidth * (m_vItems[i]->image_scale.x), 
				nGridHeight * (m_vItems[i]->image_scale.y));

			GetPainter()->SetBlendAlpha(old_alpha);
        }
	}

    if (m_bRoundGrid)
    {
        // Բ��ʱֻ��ǰ���ں��滭
        DrawGridBack(xbase, ybase, GridWidth, GridHeight, nGridWidth, nGridHeight, indexstart, indexend);
    }

    // �����ڸ�ͼ
    DrawCover(xbase, ybase, GridWidth, GridHeight, nGridWidth, nGridHeight, indexstart, indexend);

    // �������------------------------------------------------------
    DrawImageHelpByIndex(m_MouseInImage, m_MouseInColor, m_nCurMouseIn, 
        indexstart, indexend, xbase, ybase, GridWidth, GridHeight, 
        nGridWidth, nGridHeight);

    // ���ѡ��------------------------------------------------------
    if (IsEmpty(m_nCurSelect))
    {
        // �������ݲ�����
        if (m_bShowEmpty)
        {
            if (m_bShowEmptyFade)
            {
                // ��Ҫ���뵭��
                if (m_nElapseTime != 0)
                {
                    // alhpaΪ0ʱ����ʾ����������ʾ
                    DrawImageHelpByIndex(m_SelectImage, m_SelectColor, m_nCurSelect, 
                        indexstart, indexend, xbase, ybase, GridWidth, GridHeight, 
                        nGridWidth, nGridHeight);
                }
            }
            else
            { 
                // ����Ҫ���뵭������һֱ��ʾ
                DrawImageHelpByIndex(m_SelectImage, m_SelectColor, m_nCurSelect, 
                    indexstart, indexend, xbase, ybase, GridWidth, GridHeight, 
                    nGridWidth, nGridHeight);
            }
        }
    }
    else
    {
        // ���Ӳ�Ϊ����ʾ
        DrawImageHelpByIndex(m_SelectImage, m_SelectColor, m_nCurSelect, 
            indexstart, indexend, xbase, ybase, GridWidth, GridHeight, 
            nGridWidth, nGridHeight);
    }
}

int ImageGrid::GetVerticalTotal() const
{
	size_t size = m_vItems.size();

	if (size == 0 && m_nMaxSize <= 0)
	{
		return 0;
	}

	int lRowNumber = 0;

    if (m_nMaxSize > 0)
    {
        if (m_nMaxSize % m_nClomn != 0)
        {
            lRowNumber = m_nMaxSize / m_nClomn + 1;
        }
        else
        {
            lRowNumber = m_nMaxSize / m_nClomn;
        }
    }
    else
    {
        // ��Ϊ���ɾ�����ǰ���ŵĴӵ͵�������ȡ���һ������
        const ImageItem &FinalItem = *m_vItems[size - 1];

	    if ((FinalItem.Index + 1) % m_nClomn != 0)
	    {
		    lRowNumber = (FinalItem.Index + 1) / m_nClomn + 1;
	    }
	    else
	    {
		    lRowNumber = (FinalItem.Index + 1) / m_nClomn;
	    }
    }

	return lRowNumber;
}

int ImageGrid::GetVerticalCanDisp() const
{
	return m_nRow;
}

void ImageGrid::SetVerticalValue(int value)
{
	if (m_nTopIndex != value && value >= 0 && value <= GetVerticalTotal())
	{
		m_nTopIndex = value;

        if (GetCore()->GetEntity(GetVScrollBar()) != NULL)
        {
            UpdateScroll();
        }
	}
}

int ImageGrid::GetVerticalValue() const
{
	return m_nTopIndex;
}

// �¼�
// ������̧��
int ImageGrid::OnLButtonUp(int x, int y)
{
	GuiUtil_RunCallback(this, "on_leftclick_up", 
		CVarList() << m_nCurMouseIn);

	if (m_nCurMouseIn != -1)
	{
		m_nCurSelect = m_nCurMouseIn;

		GuiUtil_RunCallback(this, "on_select_changed", 
			CVarList() << m_nCurSelect);
	}

    m_nCurLButtonDown = -1;

	return 1;
}

// �϶�����
int ImageGrid::OnDragMove(int x, int y)
{
	if (!GetVisible())
	{
		return 0;
	}

	int drag_x = x - m_nDragOldX;
	int drag_y = y - m_nDragOldY;

	m_nDragOldX = x;
	m_nDragOldY = y;

	return GuiUtil_RunCallback(this, "on_drag_move", 
		CVarList() << m_nCurSelect << drag_x << drag_y << x << y);
}

// �϶�����
int ImageGrid::OnDropIn(int x, int y)
{
	if (m_nCurMouseIn != -1)
	{
		m_nCurSelect = m_nCurMouseIn;

		GuiUtil_RunCallback(this, "on_drop_in", 
			CVarList() << m_nCurSelect);	
	}

	return 1;
}

int ImageGrid::OnDragLeave(int x, int y) 
{ 
	return GuiUtil_RunCallback(this, "on_drag_leave", CVarList());
}

int ImageGrid::OnLButtonDown(int x, int y)
{
	//m_nCurSelect = -1;
	//m_nCurDBSelect = -1;

	m_nCurMouseIn = GetInWhichItem(x, y, m_CurMouseInPos);

	GuiUtil_RunCallback(this, "on_leftclick_down", 
		CVarList() << m_nCurMouseIn);

	if (m_nCurMouseIn != -1)
	{
		m_nCurSelect = m_nCurMouseIn;
        m_nCurLButtonDown = m_nCurMouseIn;
		m_nDragOldX = x;
		m_nDragOldY = y;

		return GuiUtil_RunCallback(this, "on_drag_enter", 
			CVarList() << m_nCurSelect);
	}

	return 1;
}

int ImageGrid::OnLButtonDoubleClick(int x, int y)
{
	if (m_nCurMouseIn != -1)
	{
		m_nCurDBSelect = m_nCurMouseIn;

		GuiUtil_RunCallback(this, "on_doubleclick_grid", 
			CVarList()  << m_nCurDBSelect);
	}

	return 1;
}

int ImageGrid::OnRButtonUp(int x, int y)
{
	if (m_nCurMouseIn != -1)
	{
		m_nCurRButton = m_nCurMouseIn;

		GuiUtil_RunCallback(this, "on_rightclick_grid", 
			CVarList() << m_nCurRButton);
	}

    m_nCurRButtonDown = -1;

	return 1;
}

int ImageGrid::OnRButtonDown(int x, int y)
{
	if (m_nCurMouseIn != -1)
	{
		m_nCurRButtonDown = m_nCurMouseIn;
    }

    return 1;
}

// �������ʱ����ƶ�
int ImageGrid::OnMouseMoveWhenLButtonDown(int x, int y)
{
	return OnMouseMove(x, y);
}

int ImageGrid::GetInWhichItem(int x, int y, point_t& CurMouseInPos) const
{
	// ���������߼��������
	int nCurMouseIn = -1;

	ScrollBar* pScrollbar = (ScrollBar*)GetGui()->GetControl(GetVScrollBar());

	// �������ݸ߶��е�һ����,�����жϻ�������
	long lContentTop = m_nTopIndex;

	if (pScrollbar && pScrollbar->GetVisible())
	{
		lContentTop = pScrollbar->GetCurValue();
	}

	size_t indexstart = lContentTop * m_nClomn;
	size_t indexend = indexstart + m_nRow * m_nClomn;

	point_t pt = { x, y };

	long xbase = InnerGetAbsLeft() + m_ViewRect.left;
	long ybase = InnerGetAbsTop() + m_ViewRect.top;

	int GridWidth = (m_ViewRect.right - m_ViewRect.left) / m_nClomn;
	int GridHeight = (m_ViewRect.bottom - m_ViewRect.top) / m_nRow;

	long CurRow = 0;
	long CurClomn = 0;

	long left = 0;
	long top = 0;

	long dx = 0;
	long dy = 0;

	size_t i = 0;
	size_t grid_index = 0;

	for (i = 0; i < m_vItems.size(); ++i)
	{
		if (m_vItems[i]->Index >= indexstart &&
			m_vItems[i]->Index < indexend && !m_vItems[i]->Image.IsNull())
		{
			grid_index = (size_t)(m_vItems[i]->Index - indexstart);

			CurClomn = grid_index % m_nClomn;

			CurRow = grid_index / m_nClomn;

			left = CurClomn * GridWidth;
			top = CurRow * GridHeight;

			if (grid_index < m_vGridsPos.size())
			{
				left = m_vGridsPos[grid_index].x;
				top = m_vGridsPos[grid_index].y;
			}

			rect_t targetRect = {
				m_vItems[i]->rect.left + xbase + left, 
				m_vItems[i]->rect.top + ybase + top, 
				m_vItems[i]->rect.right + xbase + left, 
				m_vItems[i]->rect.bottom + ybase + top
			};

			targetRect.right = targetRect.left + m_nGridWidth;
			targetRect.bottom = targetRect.top + m_nGridHeight;

			if (PtInRect(&targetRect, pt))
			{
				nCurMouseIn = m_vItems[i]->Index;
				CurMouseInPos.x = targetRect.left;
				CurMouseInPos.y = targetRect.top;
				break;
			}
		}
	}

	if (nCurMouseIn == -1)
	{
		rect_t rectgrid;
		long xp = xbase, yp = ybase;
		size_t Count = m_nRow * m_nClomn;

		for (long i = 0; i < m_nRow; ++i)
		{
			for (long j = 0; j < m_nClomn; ++j)
			{
				// ʵ�ʸ�����ţ���0��ʼ,��row * col - 1����
				grid_index = i * m_nClomn + j;

				if (grid_index < m_vGridsPos.size())
				{
					rectgrid.left = xbase + m_vGridsPos[grid_index].x;
					rectgrid.top = ybase + m_vGridsPos[grid_index].y;
					rectgrid.right = rectgrid.left + m_nGridWidth;
					rectgrid.bottom = rectgrid.top + m_nGridHeight;
				}
				else
				{
					rectgrid.left = xp + dx;
					rectgrid.top = yp + dy;
					rectgrid.right = xp + GridWidth - dx;
					rectgrid.bottom = yp + GridHeight - dy;
				}

				if (PtInRect(&rectgrid, pt))
				{
					nCurMouseIn = grid_index + indexstart;
					CurMouseInPos.x = rectgrid.left;
					CurMouseInPos.y = rectgrid.top;
					break;
				}

				xp += GridWidth;
			}

			yp += GridHeight;
			xp = xbase;
		}
	}
	return nCurMouseIn;
}

// �ж�ָ��λ���Ƿ�͸��
bool ImageGrid::TestTransparent(int x, int y)
{
	// ���IControl��Ϊ��͸����͸��
	if (!IControl::TestTransparent(x, y))
	{
		return false;
	}

	// ���IControl��Ϊ͸���������ڷǿյĸ����ϣ�����Ϊ��͸��
	point_t pos;
	int nCurMouseIn = GetInWhichItem(x, y, pos);
	if (nCurMouseIn > -1 && !IsEmpty(nCurMouseIn))
	{
		return false;
	}

	// ������͸��
	return true;
}

int ImageGrid::OnMouseMove(int x, int y)
{
	int OldIndex = m_nCurMouseIn;
	m_nCurMouseIn = GetInWhichItem(x, y, m_CurMouseInPos);

	// ������������Ƴ�
	if (OldIndex != m_nCurMouseIn)
	{
		// �����ʱ����Ҫ����
		if (OldIndex == -1)
		{
			// ����
			GuiUtil_RunCallback(this, "on_mousein_grid", 
				CVarList() << m_nCurMouseIn);

			OnMouseInGrid(m_CurMouseInPos.x, m_CurMouseInPos.y);
		}
		else 
		{
			// �Ƴ�
			GuiUtil_RunCallback(this, "on_mouseout_grid", 
				CVarList() << OldIndex);

			OnMouseOutGrid();

			if (m_nCurMouseIn != -1)
			{
				// ����
				GuiUtil_RunCallback(this, "on_mousein_grid", 
					CVarList() << m_nCurMouseIn);

				OnMouseInGrid(m_CurMouseInPos.x, m_CurMouseInPos.y);
			}
		}
	}

	return 1;
}

// �����꽹��
int ImageGrid::OnGetCapture(const PERSISTID& lost_id)
{
	bool bOldShowEmpty = GetShowEmpty();
	int res = IControl::OnGetCapture(lost_id);

	if(bOldShowEmpty && GetEmptyShowFade())
	{
		SetEmptyGridState(DRAW_STATE_SHOW);
	}
	
	return res;
}

int ImageGrid::OnLostCapture(const PERSISTID& get_id)
{

    if (m_nCurMouseIn != -1)
    {
        // �Ƴ�
        GuiUtil_RunCallback(this, "on_mouseout_grid", 
			CVarList()  << m_nCurMouseIn);

        OnMouseOutGrid();

        m_nCurMouseIn = -1;
    }

	GuiUtil_RunCallback(this, "on_lost_capture", CVarList() << get_id);

	bool bOldShowEmpty = GetShowEmpty();
	int res = IControl::OnLostCapture(get_id);

	if(bOldShowEmpty && GetEmptyShowFade())
	{
		SetEmptyGridState(DRAW_STATE_HIDE);
	}

    m_nCurLButtonDown = -1;
    m_nCurRButtonDown = -1;

    return res;
}

int ImageGrid::GetMouseInItemLeft()
{
    return m_CurMouseInPos.x;
}

int ImageGrid::GetMouseInItemTop()
{
    return m_CurMouseInPos.y;
}

int ImageGrid::GetItemLeft(int itemindex) const
{
    if (itemindex >= 0 && itemindex < (int)m_vGridsPos.size()) 
    {
        return m_vGridsPos[itemindex].x;
    }

    return -1;
}

int ImageGrid::GetItemTop(int itemindex) const
{
    if (itemindex >= 0 && itemindex < (int)m_vGridsPos.size()) 
    {
        return m_vGridsPos[itemindex].y;
    }

    return -1;
}


bool ImageGrid::SetBindIndex(int itemindex, int bindindex)
{
	//m_vBindIndexMap.Set(itemindex, bindindex);
	if (m_vBindIndexMap.Find(itemindex)==m_vBindIndexMap.End())
	{
		m_vBindIndexMap.Add(itemindex, bindindex );
	}
	//m_vBindIndexMap[itemindex] = bindindex;

    return true;
}

int ImageGrid::GetBindIndex(int itemindex)
{
    int bindindex;

	if (!m_vBindIndexMap.GetData(itemindex, bindindex))
	{
		return -1;
	}

	return bindindex;
//	if (m_vBindIndexMap.find(itemindex) == m_vBindIndexMap.end())
//    {
//        return -1;
//    }
//
//    return m_vBindIndexMap[itemindex];
}

// �ֶ����õ���״̬
bool ImageGrid::SetMouseDownState(int index, bool bDown)
{
	//m_vDownStateMap.Set(index, bDown);
	if (m_vDownStateMap.Find(index)==m_vDownStateMap.End())
	{
		m_vDownStateMap.Add(index, bDown);
	}
	//m_vDownStateMap[index] = bDown;
	//m_nCurLButtonDown = index;
	return true;
}

bool ImageGrid::GetMouseDownState(int index)
{
	// �����ż��
	if (index < 0)
	{
		return false;
	}

	int nMaxIndex = m_nRow * m_nClomn - 1;
	if (nMaxIndex < index)
	{
		return false;
	}

	// �������������ĸ��ӣ��Ƿ���
	if (m_nCurLButtonDown != -1 && m_nCurLButtonDown == index)
	{
		return true;
	}

	// �������Ҽ�����ĸ��ӣ��Ƿ���
	if (m_nCurRButtonDown != -1 && m_nCurRButtonDown == index)
	{
		return true;
	}

	// Map����
	//if (m_vDownStateMap.end() == m_vDownStateMap.find(index))
	bool down;

	if (!m_vDownStateMap.GetData(index, down))
	{
		return false;
	}

	return down;
	//return m_vDownStateMap[index];
}

// �ֶ��������״̬
bool ImageGrid::ClearMouseDowState()
{
	//m_vDownStateMap.clear();
	m_vDownStateMap.Clear();
	m_nCurLButtonDown = -1;
	m_nCurRButtonDown = -1;

	return true;
}

bool ImageGrid::ClearSelectedState()
{
	m_nCurSelect = -1;
	m_nCurLButtonDown = -1;
	m_nCurRButtonDown = -1;
	return true;
}

bool ImageGrid::SetSelectedState(int index)
{
	m_nCurSelect = index;
	m_nCurLButtonDown = -1;
	m_nCurRButtonDown = -1;
	return true;
}

int ImageGrid::GetItem(int index)
{
	for (size_t i = 0; i < m_vItems.size(); ++i)
	{
		if (m_vItems[i]->Index == index)
		{
			return i;
		}
	}

	return -1;
}

point_t ImageGrid::GetImagePos(int index, int xbase, int ybase, 
                             int row, int col, int GridWidth, int GridHeight)
{
	point_t point;

	if (index < (long)m_vGridsPos.size())
	{
		point.x = xbase + m_vGridsPos[index].x;
		point.y = ybase + m_vGridsPos[index].y;

	}
	else
	{
		point.x = (int)(col * GridWidth + xbase);
		point.y = (int)(row * GridHeight + ybase);
	}

	return point;
}

bool ImageGrid::DrawImageSingle(int index, point_t& point, int lBackWidth, 
	int lBackHeight, int blend_color)
{
	//std::map<int, CImagePlayer*>::iterator it = 
	//	m_mapBackImageIndexMap.find(index);
	CImagePlayer* pImagePlayer;

	//if (it != m_mapBackImageIndexMap.end() && it->second != NULL)
	if (m_mapBackImageIndexMap.GetData(index, pImagePlayer)
		&& (pImagePlayer != NULL))
	{
		//CImagePlayer* pImagePlayer = it->second;

		pImagePlayer->Draw(CGuiConst::DRAWMODE_TILE,
			point.x + m_nGridBackOffsetX, 
			point.y + m_nGridBackOffsetY, 
			point.x + m_nGridBackOffsetX + pImagePlayer->GetWidth(), 
			point.y + m_nGridBackOffsetY + pImagePlayer->GetHeight(),
			blend_color);
	}
	else
	{
		m_GridBack.image.Draw(CGuiConst::DRAWMODE_TILE,
			point.x + m_nGridBackOffsetX, 
			point.y + m_nGridBackOffsetY, 
		    point.x + m_nGridBackOffsetX + lBackWidth, 
			point.y + m_nGridBackOffsetY + lBackHeight, 
			blend_color);
	}

	return true;
}

bool ImageGrid::DrawSingleEmptyGridFading(int iGridIndex, int iStartIndex,
		int iXBase, int iYBase, int iRrow, int iCol, int GridWidth, int GridHeight)
								
{
	long lBlend = 0xffffffff;
	eEmptyGridState state = (eEmptyGridState)GetEmptyGridState();

	switch(state)
	{
	case DRAW_STATE_NORMAL:		
		break;

	case DRAW_STATE_SHOW:
	case DRAW_STATE_HIDE:

		lBlend = GetBlend(m_nElapseTime);

		break;

    default:
        break;
	}

	point_t point = GetImagePos(iGridIndex, iXBase, iYBase, iRrow, iCol, 
			GridWidth, GridHeight);

	DrawImageSingle(iGridIndex + iStartIndex, point,
		GridWidth, GridHeight, lBlend);

	return true;
}

//��ȡ��ǰ͸����
int ImageGrid::GetBlend(int fElapseTime)
{
	unsigned int nBlend = fElapseTime * 255 / m_nTotalTime;
	nBlend = nBlend<<24;
	nBlend |= 0x00ffffff;
	return nBlend;
}

void ImageGrid::Execute(float seconds)
{
    // ��Ҫ��ʾ�ո��粻��Ҫ���Ҳ��ڵ��뵭��ʱ,�򲻴���
	int interval = (int)(seconds * 1000.0f);

	switch(m_eEmptyDrawState)
	{
		case DRAW_STATE_NORMAL:
//            return;
			break;
		case DRAW_STATE_SHOW:
			m_nElapseTime += interval;
			break;
		case DRAW_STATE_HIDE:
			m_nElapseTime -= interval;
			break;
	}

	if(m_eEmptyDrawState != DRAW_STATE_NORMAL)
		if(m_nElapseTime < 0 || m_nElapseTime > m_nTotalTime)
		{
			SetEmptyGridState(DRAW_STATE_NORMAL);
		}

	for (size_t i = 0; i < m_vItems.size(); ++i)
	{
		if(m_vItems[i]->pCoolItem!=NULL)
			m_vItems[i]->pCoolItem->UpdateCool(seconds);
	}
}

void ImageGrid::SetEmptyGridState(int state)
{
    if (state < DRAW_STATE_NORMAL || state > DRAW_STATE_HIDE)
    {
        return;
    }

	switch(state)
	{
	case DRAW_STATE_NORMAL:

		if(DRAW_STATE_SHOW == m_eEmptyDrawState)
		{
			m_nElapseTime = m_nTotalTime;
		}
		else if(DRAW_STATE_HIDE == m_eEmptyDrawState)
		{
			m_nElapseTime = 0;
		}

//		GetCore()->RemoveExecute(this);

		break;

	case DRAW_STATE_SHOW:

		if(DRAW_STATE_NORMAL == m_eEmptyDrawState )
		{
//			GetCore()->AddExecute(this);
		}

		break;

	case DRAW_STATE_HIDE:

		if(DRAW_STATE_NORMAL == m_eEmptyDrawState )
		{
//			GetCore()->AddExecute(this);
		}

		break;
	}

	m_eEmptyDrawState = (eEmptyGridState)state;
}
