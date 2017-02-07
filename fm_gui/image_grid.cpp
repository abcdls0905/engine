//--------------------------------------------------------------------
// 文件名:		ImageGrid.cpp
// 内  容:		图片网格控件
// 说  明:		
// 创建日期:	2008年8月27日
// 创建人:		杨利平
// 版权所有:	苏州蜗牛电子有限公司
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
/// \brief 图片网格控件
DECLARE_ENTITY(ImageGrid, 0,IScrollable);

/// property: string SelectColor
/// \brief 选中格子的颜色,如果没有DrawMouseSelect才被使用
DECLARE_PROPERTY(result_string, ImageGrid, SelectColor, 
				 GetSelectColor, SetSelectColor);
/// property: string MouseInColor
/// \brief 鼠标移入格子的颜色,如果没有DrawMouseIn才被使用
DECLARE_PROPERTY(result_string, ImageGrid, MouseInColor, 
				 GetMouseInColor, SetMouseInColor);
/// property: string CoverColor
/// \brief 盖住格子的颜色,如果没有DrawCover才被使用
DECLARE_PROPERTY(result_string, ImageGrid, CoverColor, 
				 GetCoverColor, SetCoverColor);
/// property: string LockColor
/// \brief 格子锁定的颜色,如果没有DrawLock才被使用
DECLARE_PROPERTY(result_string, ImageGrid, LockColor, GetLockColor, SetLockColor);
/// property: string CoolColor
/// \brief 冷却的颜色
DECLARE_PROPERTY(result_string, ImageGrid, CoolColor, GetCoolColor, SetCoolColor);
/// property: rect_t ViewRect
/// \brief 实际格子所在的区域,会使用RowNum,ClomnNum将这个区域划成网格
DECLARE_PROPERTY(result_string, ImageGrid, ViewRect, GetViewRect, SetViewRect);

/// property: int MaxSize
/// \brief 容器需要的格子总数，不一定是RowNum * ClomnNum, 需要固定容器大小时使用，不然设成-1
DECLARE_PROPERTY(int, ImageGrid, MaxSize, GetMaxSize, SetMaxSize);

/// property: int RowNum
/// \brief 行数
DECLARE_PROPERTY(int, ImageGrid, RowNum, GetRowNum, SetRowNum);
/// property: int ClomnNum
/// \brief 列数
DECLARE_PROPERTY(int, ImageGrid, ClomnNum, GetClomnNum, SetClomnNum);

/// property: bool RoundGrid
/// \brief 是否显示空的格子，如果为true,则空的格子背景及选中会被绘制
DECLARE_PROPERTY(bool, ImageGrid, RoundGrid, GetRoundGrid, SetRoundGrid);

/// property: bool ShowEmpty
/// \brief 是否显示空的格子，如果为true,则空的格子背景及选中会被绘制
DECLARE_PROPERTY(bool, ImageGrid, ShowEmpty, GetShowEmpty, SetShowEmpty);

/// property: bool EmptyShowFade
/// \brief 空格不显示时是否淡入淡出
DECLARE_PROPERTY(bool, ImageGrid, EmptyShowFade, GetEmptyShowFade, SetEmptyShowFade);

/// property: bool IsEditMode
/// \brief 是否编辑模式,编辑模式下会显示红线网格
DECLARE_PROPERTY(bool, ImageGrid, IsEditMode, GetEditMode, SetEditMode);
/// property: bool Center
/// \brief 实际图片在网格中是否居中,为true时下面GridOffsetX,GridOffsetY将被乎略
DECLARE_PROPERTY(bool, ImageGrid, Center, GetCenter, SetCenter);
// 当且仅当Center为false时有效
/// property: int GridOffsetX
/// \brief 实际图片在网格中相对于格子所上角的X偏称,
DECLARE_PROPERTY(int, ImageGrid, GridOffsetX, GetGridOffsetX, SetGridOffsetX);
/// property: int GridOffsetY
/// \brief 实际图片在网格中相对于格子所上角的Y偏称,
DECLARE_PROPERTY(int, ImageGrid, GridOffsetY, GetGridOffsetY, SetGridOffsetY);
/// property: int GridHeight
/// \brief 空格的高度
DECLARE_PROPERTY(int, ImageGrid, GridHeight, GetGridHeight, SetGridHeight);
/// property: int GridWidth
/// \brief 空格的宽度
DECLARE_PROPERTY(int, ImageGrid, GridWidth, GetGridWidth, SetGridWidth);
/// property: bool FitGrid
/// \brief 按格子大小绘制物品
DECLARE_PROPERTY(bool, ImageGrid, FitGrid, GetFitGrid, SetFitGrid);
/// property: int VScrollIndex
/// \brief 按格子大小绘制物品
DECLARE_PROPERTY(int, ImageGrid, VScrollIndex, GetVerticalValue, SetVerticalValue)
/// property: int GridOffsetY
/// \brief 允许配置每个格子的不同位置,当这个为空时按默认网格排列
///        配置形式:x,y;x,y;x,y;x,y;... 以;号格开每个点,以,格开x坐标和y坐标,这样第一个x,y组合就表示第一个格子相对于控件左上角的位置,
///        用于自定义格子位置,应付一些格子排布不规则的情况
DECLARE_PROPERTY(result_string, ImageGrid, GridsPos, GetGridsPos, SetGridsPos);
/// property: string DrawMouseIn
/// \brief 鼠标移入时的图片有三种情史,下面四个都是一样的配置方法在此一同进行说明:
/// 1.配rect_t,表示移绘制一个半透明区域;          2.配置gui中的一个界面图元,要写出相对路径,
/// 3.配置一个tex中的一个图元,要写出相对路么,   4.配置一个动画图片名,在ImageAnimations.xml中配有的
DECLARE_PROPERTY(result_string, ImageGrid, DrawMouseIn, 
				 GetDrawMouseIn, SetDrawMouseIn);
/// property: string DrawMouseSelect
/// \brief 鼠标选中的显示
DECLARE_PROPERTY(result_string, ImageGrid, DrawMouseSelect, 
				 GetDrawMouseSelect, SetDrawMouseSelect);
/// property: string DrawCover
/// \brief 盖起的显示
DECLARE_PROPERTY(result_string, ImageGrid, DrawCover, GetDrawCover, SetDrawCover);
/// property: string DrawLock
/// \brief 锁定的显示
DECLARE_PROPERTY(result_string, ImageGrid, DrawLock, GetDrawLock, SetDrawLock);
/// property: string DrawGridBack
/// \brief 格子背景图
DECLARE_PROPERTY(result_string, ImageGrid, DrawGridBack, 
				 GetDrawGridBack, SetDrawGridBack);
/// property: string GridBackOffsetX
/// \brief 格子背景图相对于格子左上的X偏移
DECLARE_PROPERTY(int, ImageGrid, GridBackOffsetX, 
				 GetGridBackOffsetX, SetGridBackOffsetX);
/// property: string GridBackOffsetY
/// \brief 格子背景图相对于格子左上的X偏移
DECLARE_PROPERTY(int, ImageGrid, GridBackOffsetY, 
				 GetGridBackOffsetY, SetGridBackOffsetY);

/// property: string GridCoverOffsetX
/// \brief 格子遮盖图相对于格子左上的X偏移
DECLARE_PROPERTY(int, ImageGrid, GridCoverOffsetX, 
                GetGridCoverOffsetX, SetGridCoverOffsetX);
/// property: string GridCoverOffsetY
/// \brief 格子遮盖图相对于格子左上的X偏移
DECLARE_PROPERTY(int, ImageGrid, GridCoverOffsetY, 
                GetGridCoverOffsetY, SetGridCoverOffsetY);

/// property: string GridCoverOffsetY
/// \brief 格子遮盖图相对于格子左上的X偏移
DECLARE_PROPERTY(result_string, ImageGrid, CoolAnimation, 
                GetCoolAnimation, SetCoolAnimation);

// property: bool ShowMouseDownState
/// \brief 鼠标点击状态
DECLARE_PROPERTY(bool, ImageGrid, ShowMouseDownState, 
                GetShowMouseDownState, SetShowMouseDownState);

// property: float MouseDownScale
/// \brief 鼠标点击状态放缩
DECLARE_PROPERTY(float, ImageGrid, MouseDownScale, 
                GetMouseDownScale, SetMouseDownScale);

// property: float MouseDownScale
/// \brief 鼠标点击状态图标偏移X
DECLARE_PROPERTY(int, ImageGrid, MouseDownOffsetX, 
                GetMouseDownOffsetX, SetMouseDownOffsetX);

// property: float MouseDownScale
/// \brief 鼠标点击状态图标偏移Y
DECLARE_PROPERTY(int, ImageGrid, MouseDownOffsetY, 
                GetMouseDownOffsetY, SetMouseDownOffsetY);

// property: int MouseDownAlpha
/// \brief 鼠标点击状态图标透明度
DECLARE_PROPERTY(int, ImageGrid, MouseDownAlpha, 
                GetMouseDownAlpha, SetMouseDownAlpha);

/// \brief 添加文本
/// \method AddItem(int index, const char*pImageName, const wchar_t*pItemName = NULL, 
///                 int ItemNumber = 1, int lMark = -1, int nImageWidthScale = 1, int nImageHeightScale = 1);
/// \param index 格子中的序号
/// \param pImageName 图元路径
/// \param pItemName 物品名
/// \param ItemNumber 物品数量
/// \param lMark 用户自定义数据
/// \param nImageWidthScale 图片的宽度倍数
/// \param nImageHeightScale 图片的高度倍数
/// \return 成功返回true, 失败返回false
//DECLARE_METHOD_7(bool, ImageGrid, AddItemWithImageScale, int, const char*, const wchar_t*, int, int, int, int);
// property: int EmptyGridState
/// \brief 当前空格子的状态
DECLARE_PROPERTY(int, ImageGrid, EmptyGridState, 
                GetEmptyGridState, SetEmptyGridState);

#if 0	//DECLARE_METHOD
/// \brief 添加文本
/// \method AddItem(int index, const char*pImageName, const wchar_t*pItemName = NULL, 
///                 int ItemNumber = 1, int lMark = -1);
/// \param index 格子中的序号
/// \param pImageName 图元路径
/// \param pItemName 物品名
/// \param ItemNumber 物品数量
/// \param lMark 用户自定义数据
/// \return 成功返回true, 失败返回false
DECLARE_METHOD_5(bool, ImageGrid, AddItem, int, const char*, const wchar_t*, int, int);
/// \brief 删除某一格物品
/// \method bool DelItem(int index);
/// \param index 格子序号
/// \return 成功删除返回true
DECLARE_METHOD_1(bool, ImageGrid, DelItem, int);
/// \brief 判断某个格子是否为空
/// \method bool IsEmpty(int index);
/// \param index 格子序号
/// \return 为空返回true,不为空返回false
DECLARE_METHOD_1(bool, ImageGrid, IsEmpty, int);
/// \brief 清理整个容器
/// \method bool Clear();
/// \return true
DECLARE_METHOD_0(bool, ImageGrid, Clear);
/// \brief 设置某个格子的图片
/// \method bool SetItemImage(int index, const char*pImageName)
/// \param index 格子序号
/// \param pImageName 图片名带相对路径
/// \return true
DECLARE_METHOD_2(bool, ImageGrid, SetItemImage, int, const char*);
/// \brief 获取某个格子的图片
/// \method const char*GetItemImage(int index);
/// \param index 格子序号
/// \return 图片名带路径
DECLARE_METHOD_1(std::string, ImageGrid, GetItemImage, int);
/// \brief 将格子中的彩图用黑白绘制
/// \method bool ChangeItemImageToBW(int index, bool bChange);
/// \param index 格子序号
/// \param bChange是否改变
/// \return 成功返回true
DECLARE_METHOD_2(bool, ImageGrid, ChangeItemImageToBW, int, bool);
/// \brief 设置某个格子的名称
/// \method bool SetItemName(int index, const wchar_t*pItemName);
/// \param index 格子序号
/// \param pItemName 格子中物品名
/// \return true
DECLARE_METHOD_2(bool, ImageGrid, SetItemName, int, const wchar_t*);
/// \brief 设置某个格子的名称
/// \method const wchar_t* GetItemName(int index);
/// \param index 格子序号
/// \return 物品名
DECLARE_METHOD_1(std::wstring, ImageGrid, GetItemName, int);

/// \brief 通过名称查找物品,查不到返回-1，查到返回物品序号
/// \method int FindItem(wstring item_name);
/// \param item_name 特品名
/// \return 序号
DECLARE_METHOD_1(int, ImageGrid, FindItem, const wchar_t*);

/// \brief 设置某个格子中的物品数量
/// \method bool SetItemNumber(int index, int ItemNumber);
/// \param index 格子序号
/// \param ItemNumber 物品数量
/// \return true
DECLARE_METHOD_2(bool, ImageGrid, SetItemNumber, int, int);
/// \brief 获取某个格子中的物品数量
/// \method int GetItemNumber(int index);
/// \param index 格子序号
/// \return ItemNumber
DECLARE_METHOD_1(int, ImageGrid, GetItemNumber, int);
/// \brief 设置某个格子的标记(用户自定义)
/// \method bool SetItemMark(int index, int lMark);
/// \param index 格子序号
/// \param lMark 自定义标记
/// \return true
DECLARE_METHOD_2(bool, ImageGrid, SetItemMark, int, int);
/// \brief 设置某个格子的标记(用户自定义)
/// \method bool GetItemMark(int index);
/// \param index 格子序号
/// \return mark
DECLARE_METHOD_1(int, ImageGrid, GetItemMark, int);
/// \brief 设置某个格子的冷却类型
/// \method bool SetCoolType(int index, long lCoolType);
/// \param index 格子序号
/// \param lCoolType 冷却类型
/// \return true
DECLARE_METHOD_2(bool, ImageGrid, SetCoolType, int, long);
/// \brief 设置某个格子的冷却类型
/// \method bool SetCoolTeam(int index, long lCoolTeam);
/// \param index 格子序号
/// \param lCoolTeam 冷却类型组
/// \return true
DECLARE_METHOD_2(bool, ImageGrid, SetCoolTeam, int, long);

/// \brief 开启某个格子的冷却特效
/// \method bool AddCool(int index, long lcooltime,float bit);
/// \param index 格子序号
/// \param lcooltime 冷却时间
/// \param bit 冷却当前比率
/// \return true
DECLARE_METHOD_3(bool, ImageGrid, AddCool, int, long,float);
/// \brief 关闭某个格子的冷却特效
/// \method bool StopCool(int index);
/// \param index 格子序号
/// \return true
DECLARE_METHOD_1(bool, ImageGrid, StopCool, int);

/// \brief 设置某个格子透明度
/// \method bool SetItemAlpha(int index, int alpha)
/// \param index 格子序号
/// \param bCover 透明度alpha
/// \return true
DECLARE_METHOD_2(bool, ImageGrid, SetItemAlpha, int, int);

/// \brief 获取某个格子透明度
/// \method bool GetItemAlpha(int index)
/// \param index 格子序号
/// \return 透明度
DECLARE_METHOD_1(bool, ImageGrid, GetItemAlpha, int);

/// \brief 设置某个格子物品被图片盖住(可表示禁用等)
/// \method bool CoverItem(int index, bool bCover)
/// \param index 格子序号
/// \param bCover 是否盖住
/// \return true
DECLARE_METHOD_2(bool, ImageGrid, CoverItem, int, bool);
/// \brief 设置某个格子被锁定(用到时才设,如背包中物品会允许锁定)
/// \method bool LockItem(int index, bool bLock);
/// \param index 格子序号
/// \param bLock 是否锁定
/// \return true
DECLARE_METHOD_2(bool, ImageGrid, LockItem, int, bool);
/// \brief 设置某个格子的绑定序号
/// \method void SetBindIndex(int itemindex, int bindindex);
/// \param itemindex 格子序号从0开始
/// \param bindindex 绑定index
/// \return 成功返回
DECLARE_METHOD_2(bool, ImageGrid, SetBindIndex, int, int);
/// \brief 获取某个格子的绑定序号
/// \method int GetBindIndex(int itemindex);
/// \param itemindex 格子序号从0开始
/// \param bindindex 绑定index
/// \return 成功绑定序号
DECLARE_METHOD_1(int, ImageGrid, GetBindIndex, int);
/// \brief 设置当前左键点击选中格子序号index
/// \method bool SetSelectItemIndex(int itemindex)
/// \return 格子序号
DECLARE_METHOD_1(bool, ImageGrid, SetSelectItemIndex, int);
/// \brief 获取当前左键点击选中格子序号index
/// \method int GetSelectItemIndex()
/// \return 格子序号
DECLARE_METHOD_0(int, ImageGrid, GetSelectItemIndex);
/// \brief 获取当前鼠标所在的中格子序号index
/// \method int GetMouseInItemIndex()
/// \return 格子序号
DECLARE_METHOD_0(int, ImageGrid, GetMouseInItemIndex);
/// \brief 获取当前被双击的格子序号index
/// \method int GetDBClickItemIndex()
/// \return 格子序号
DECLARE_METHOD_0(int, ImageGrid, GetDBClickItemIndex);
/// \brief 获取当前被右键点击的格子序号index
/// \method int GetRBClickItemIndex()
/// \return 格子序号
DECLARE_METHOD_0(int, ImageGrid, GetRBClickItemIndex);
/// \brief 获取指定格子的x坐标
/// \method int GetItemLeft(int itemindex)
/// \return 指定格子的x坐标
DECLARE_METHOD_1(int, ImageGrid, GetItemLeft, int);
/// \brief 获取指定格子所y坐标
/// \method int GetItemTop(int itemindex)
/// \return 指定格子的y坐标
DECLARE_METHOD_1(int, ImageGrid, GetItemTop, int);
/// \brief 获取当前移入的格子所x坐标
/// \method int GetMouseInItemLeft()
/// \return 当前移入的格子所x坐标
DECLARE_METHOD_0(int, ImageGrid, GetMouseInItemLeft);
/// \brief 获取当前移入的格子所y坐标
/// \method int GetMouseInItemTop()
/// \return 当前移入的格子所y坐标
DECLARE_METHOD_0(int, ImageGrid, GetMouseInItemTop);
/// \brief 获取单格背景
/// \method string GetItemBackImage(int index)
/// \return 单格背景图
DECLARE_METHOD_1(std::string, ImageGrid, GetItemBackImage, int);
/// \brief 设置单格背景
/// \method bool SetItemBackImage(int index, string imagename)
/// \return 单格背景图
DECLARE_METHOD_2(bool, ImageGrid, SetItemBackImage, int, const char*);
/// \brief 获取单格遮盖图
/// \method string GetItemCoverImage(int index)
/// \return 单格遮盖图
DECLARE_METHOD_1(std::string, ImageGrid, GetItemCoverImage, int);
/// \brief 设置单格遮盖图
/// \method bool SetItemCoverImage(int index, string imagename)
/// \return 单格遮盖图
DECLARE_METHOD_2(bool, ImageGrid, SetItemCoverImage, int, const char*);

/// \brief 手动设置点下状态
/// \method bool SetMouseDownState(int index, bool bDown)
/// \return true
DECLARE_METHOD_2(bool, ImageGrid, SetMouseDownState, int, bool);

/// \brief 手动清理点下状态
/// \method bool SetMouseDownState(int index)
/// \return true
DECLARE_METHOD_0(bool, ImageGrid, ClearMouseDowState);

/// \brief 手动清理当前的选中
/// \method bool ClearSelectedState()
/// \return true
DECLARE_METHOD_0(bool, ImageGrid, ClearSelectedState);

/// \brief 手动设置选中状态，不执行回调
/// \method bool SetSelectedState(int index)
/// \return true
DECLARE_METHOD_1(bool, ImageGrid, SetSelectedState, int);
#endif

/// event: int on_mousein_grid(object self, index)
/// \brief 鼠标移入某个格子是触发
/// \param self本控件
/// \param index 移入格子的序号index(注意是实际序号,不是显示序号,因为可能有滚动)
DECLARE_EVENT(on_mousein_grid);
/// event: int on_mouseout_grid(object self, index)
/// \brief 鼠标移出某个格子时触发
/// \param self本控件
/// \param index 移出格子的序号index
DECLARE_EVENT(on_mouseout_grid);
/// event: int on_leftclick_up(object self, index)
/// \brief 鼠标左键弹起某个格子时触发
/// \param self本控件
/// \param index 格子的序号index
DECLARE_EVENT(on_leftclick_up);
/// event: int on_select_changed(object self, index)
/// \brief 鼠标点击某个格子时触发,选择对像改变
/// \param self本控件
/// \param index 格子的序号index
DECLARE_EVENT(on_select_changed);
/// event: int on_doubleclick_grid(object self, index)
/// \brief 鼠标双击某个格子时触发
/// \param self本控件
/// \param index 格子的序号index
DECLARE_EVENT(on_doubleclick_grid);
/// event: int on_rightclick_grid(object self, index)
/// \brief 鼠标右键点击某个格子时触发
/// \param self本控件
/// \param index 格子的序号index
DECLARE_EVENT(on_rightclick_grid);

/// event: int on_leftclick_down(object self, index)
/// \brief 鼠标左键按下某个格子时触发
/// \param self本控件
/// \param index 格子的序号index
DECLARE_EVENT(on_leftclick_down);
/// event: int on_mouse_move(object self, x, y)
/// \brief 鼠标移动时触发
/// \param self本控件
/// \param x, y 鼠标坐标
DECLARE_EVENT(on_mouse_move);
/// event: int on_drag_enter(object self, int index)
/// \brief 开始拖动的回调
/// \param self 本控件
DECLARE_EVENT(on_drag_enter);

/// event: int on_drag_move(object self, int index, int drag_x, int drag_y)
/// \brief 鼠标按住拖动的回调
/// \param self 本控件
/// \param drag_x X方向的拖动距离
/// \param drag_y Y方向的拖动距离
DECLARE_EVENT(on_drag_move);

/// event: int on_drag_leave(object self, int index)
/// \brief 开始拖动的回调
/// \param self 本控件
DECLARE_EVENT(on_drag_leave);

/// event: int on_drag_leave(object self, int index)
/// \brief 开始拖动的回调
/// \param self 本控件
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
    m_nGridBackOffsetX = 0;    // 背景图片相对于格子的X偏移
    m_nGridBackOffsetY = 0;    // 背景图片相对于格子的Y偏移
    m_nGridCoverOffsetX = 0;    // 背景图片相对于格子的X偏移
    m_nGridCoverOffsetY = 0;    // 背景图片相对于格子的Y偏移
	// 选中和移入状态下显示的框或图片
	m_CoolColor = GuiUtil_ColorToInt("100,0,0,0");
	m_SelectColor = GuiUtil_ColorToInt("80,0,255,0");
	m_MouseInColor = GuiUtil_ColorToInt("80,255,255,0");
    m_CoverColor = GuiUtil_ColorToInt("80,255,0,0");
    m_LockColor = GuiUtil_ColorToInt("80,255,255,0");


	m_SelectImage.name = "rect_t";		// 鼠标移出
	m_MouseInImage.name = "rect_t";		// 鼠标移入
	m_CoverImage.name = "rect_t";			// 禁用
	m_LockImage.name = "rect_t";			// 锁定
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
    
	m_nTotalTime = 2000;//渐隐渐显三秒钟
	m_nElapseTime = 0;
	m_eEmptyDrawState = DRAW_STATE_NORMAL;

	m_nDragOldX = 0;
	m_nDragOldY = 0;

    // 鼠标点击状态
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

	// 如果格子已被使用则替换
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
		// 如果格子未被使用,则新建
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
        pItem->cool = -1; // 初始化无准确

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
	// 请确保有不在遍历的情况下调删除
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
    // 释放所有格子中的图片
	ReleaseImages();

    // 清除物品
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
		// 如果已有图片则清理
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

// 将图片转成黑白显示
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
	// 获取在m_vItems中的下标
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

// 增加格子冷却特效，add by zhum,2011.8.10
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

// 增加格子冷却特效，add by zhum,2011.8.10
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

// 设置默认颜色
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

// 设置显示区域如:0,0,120,40
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
    
    // 尝试创建PNG
    if (pPainter->GetGlyphValid(id))
    {
        ispng = true;   //
        width = pPainter->GetGlyphWidth(id);
        height = pPainter->GetGlyphHeight(id);
        return id;
    }

    // 尝试创建TGA
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

	// 获得单件实体
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
             // 和原来一样，不需重设
            return true;
        }

         //if (imageplayer.IsReady())
         //{
             //imageplayer.Release();
         //}
		pImagePlayer->Release();

        if ((pImageName == NULL) || (pImageName[0] == 0))
        {
             // 清除
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
        // 清除
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
            // 和原来一样，不需重设
            return true;
        }

        //if (imageplayer.IsReady())
        //{
            //imageplayer.Release();
        //}
		pImagePlayer->Release();

        if ((pImageName == NULL) || (pImageName[0] == 0))
        {
            // 清除
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
        // 清除
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

// 实现IControl里的虚函数
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
    // 释放所有格子中的图片
    ReleaseImages();

    // 清除物品
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
        m_SelectImage.image.Release();		// 鼠标移出
        m_MouseInImage.image.Release();		// 鼠标移入
        m_CoverImage.image.Release();			// 禁用
        m_LockImage.image.Release();			// 锁定
        m_GridBack.image.Release();
    //}
}

void ImageGrid::PrepareDesignInfo(IDesignInfo* di)
{
	IScrollable::PrepareDesignInfo(di);

	CVar varDefault;
	//设置是否有滚动条
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
    // 如果格子有背景的话就画背景
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
                // 实际格子序号，从0开始,到row * col - 1结束
                int grid_index = i * m_nClomn + j;

                if (IsEmpty(grid_index + indexstart))
                {
                    // 需要显示，或者正在淡入淡出才需要显示
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
                                // 不处理
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
						// 不希望显示空的格子，则不显示,直接处理下一个
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

    // 如果格子有被禁用的话则绘制禁用块
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
                // 不希望显示空的格子，则不显示,直接处理下一个
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

						// 后面图片的绘制 也没用格子宽高 
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

    // 移动版技能栏冷却表现
    CoolManager* pCoolManager = InnerGetCoolManager();
    if (m_bIsMobileSkillUse && pCoolManager != NULL)
    {
        // 这里m_nGridOffsetX,m_nGridOffsetY用来调整冷却圈的大小
        /*
        pCoolManager->DrawCooling(item.cool, item.coolteam, GetPainter(), 
            targetRect.left + m_nGridOffsetX, targetRect.top + m_nGridOffsetY, 
            targetRect.right - targetRect.left - 2*m_nGridOffsetX, 
            targetRect.bottom - targetRect.top - 2*m_nGridOffsetY, 
            m_CoolColor, m_szCoolAnimation.c_str(), true);*/

        

        // 冷却图渲染
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

        // 遮挡冷却边缘毛边的遮罩图
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
                    // 移动版本技能栏，在冷却时技能图标置灰
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

    // 绘制冷却
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
    if (item_index > -1 && //存在移入鼠标的格子
        item_index >= indexstart && item_index < indexend) //之前没有绘制过
    {

        if (! m_bShowEmpty && IsEmpty(item_index))
        {
            // 不希望显示空的格子，则不显示,直接处理下一个
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

        // 这边不需要考虑fit的情况，这个如果放缩会很难看
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

    // 第一次绘制,进行初始化
    if (m_IsFirstTime)
    {
        DrawFirstTime();
    }

	IScrollable::Paint(seconds);

    // 初始化一些参数备用
    // 编辑模式下才会显示线框
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

    // 整个内容高度中的一部分,用于判断绘制内容
    long lContentTop = m_nTopIndex;

    if (pScrollbar && pScrollbar->GetVisible())
    {
        lContentTop = pScrollbar->GetCurValue();
    }

    size_t indexstart = lContentTop * m_nClomn;
    size_t indexend = indexstart + m_nRow * m_nClomn;

    if (!m_bRoundGrid)
    {
        // 如果格子有背景的话就画背景, 圆格不先画，方格才先画
        DrawGridBack(xbase, ybase, GridWidth, GridHeight, nGridWidth, nGridHeight, indexstart, indexend);
    }

    // 编辑模式下绘制线框
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

            // 通常的行列左上角
            x = CurClomn * GridWidth;
            y = CurRow * GridHeight;

            // 如果有自定义位置,则取自定义位置左上角
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
        // 圆格时只画前景在后面画
        DrawGridBack(xbase, ybase, GridWidth, GridHeight, nGridWidth, nGridHeight, indexstart, indexend);
    }

    // 绘制遮盖图
    DrawCover(xbase, ybase, GridWidth, GridHeight, nGridWidth, nGridHeight, indexstart, indexend);

    // 鼠标移入------------------------------------------------------
    DrawImageHelpByIndex(m_MouseInImage, m_MouseInColor, m_nCurMouseIn, 
        indexstart, indexend, xbase, ybase, GridWidth, GridHeight, 
        nGridWidth, nGridHeight);

    // 鼠标选择------------------------------------------------------
    if (IsEmpty(m_nCurSelect))
    {
        // 格子内容不存在
        if (m_bShowEmpty)
        {
            if (m_bShowEmptyFade)
            {
                // 需要淡入淡出
                if (m_nElapseTime != 0)
                {
                    // alhpa为0时不显示，其他都显示
                    DrawImageHelpByIndex(m_SelectImage, m_SelectColor, m_nCurSelect, 
                        indexstart, indexend, xbase, ybase, GridWidth, GridHeight, 
                        nGridWidth, nGridHeight);
                }
            }
            else
            { 
                // 不需要淡入淡出，则一直显示
                DrawImageHelpByIndex(m_SelectImage, m_SelectColor, m_nCurSelect, 
                    indexstart, indexend, xbase, ybase, GridWidth, GridHeight, 
                    nGridWidth, nGridHeight);
            }
        }
    }
    else
    {
        // 格子不为空显示
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
        // 因为添加删除都是按序号的从低到高所以取最后一个即可
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

// 事件
// 鼠标左键抬起
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

// 拖动结束
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

// 拖动放入
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

// 左键按下时鼠标移动
int ImageGrid::OnMouseMoveWhenLButtonDown(int x, int y)
{
	return OnMouseMove(x, y);
}

int ImageGrid::GetInWhichItem(int x, int y, point_t& CurMouseInPos) const
{
	// 坐标所在逻辑格子序号
	int nCurMouseIn = -1;

	ScrollBar* pScrollbar = (ScrollBar*)GetGui()->GetControl(GetVScrollBar());

	// 整个内容高度中的一部分,用于判断绘制内容
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
				// 实际格子序号，从0开始,到row * col - 1结束
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

// 判断指定位置是否透明
bool ImageGrid::TestTransparent(int x, int y)
{
	// 如果IControl认为不透明则不透明
	if (!IControl::TestTransparent(x, y))
	{
		return false;
	}

	// 如果IControl认为透明，但是在非空的格子上，则认为不透明
	point_t pos;
	int nCurMouseIn = GetInWhichItem(x, y, pos);
	if (nCurMouseIn > -1 && !IsEmpty(nCurMouseIn))
	{
		return false;
	}

	// 否则都是透明
	return true;
}

int ImageGrid::OnMouseMove(int x, int y)
{
	int OldIndex = m_nCurMouseIn;
	m_nCurMouseIn = GetInWhichItem(x, y, m_CurMouseInPos);

	// 处理鼠标移入移出
	if (OldIndex != m_nCurMouseIn)
	{
		// 不相等时才需要处理
		if (OldIndex == -1)
		{
			// 移入
			GuiUtil_RunCallback(this, "on_mousein_grid", 
				CVarList() << m_nCurMouseIn);

			OnMouseInGrid(m_CurMouseInPos.x, m_CurMouseInPos.y);
		}
		else 
		{
			// 移出
			GuiUtil_RunCallback(this, "on_mouseout_grid", 
				CVarList() << OldIndex);

			OnMouseOutGrid();

			if (m_nCurMouseIn != -1)
			{
				// 移入
				GuiUtil_RunCallback(this, "on_mousein_grid", 
					CVarList() << m_nCurMouseIn);

				OnMouseInGrid(m_CurMouseInPos.x, m_CurMouseInPos.y);
			}
		}
	}

	return 1;
}

// 获得鼠标焦点
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
        // 移出
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

// 手动设置点下状态
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
	// 索引号检查
	if (index < 0)
	{
		return false;
	}

	int nMaxIndex = m_nRow * m_nClomn - 1;
	if (nMaxIndex < index)
	{
		return false;
	}

	// 检查鼠标左键点击的格子，是否按下
	if (m_nCurLButtonDown != -1 && m_nCurLButtonDown == index)
	{
		return true;
	}

	// 检查鼠标右键点击的格子，是否按下
	if (m_nCurRButtonDown != -1 && m_nCurRButtonDown == index)
	{
		return true;
	}

	// Map查找
	//if (m_vDownStateMap.end() == m_vDownStateMap.find(index))
	bool down;

	if (!m_vDownStateMap.GetData(index, down))
	{
		return false;
	}

	return down;
	//return m_vDownStateMap[index];
}

// 手动清理点下状态
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

//获取当前透明度
int ImageGrid::GetBlend(int fElapseTime)
{
	unsigned int nBlend = fElapseTime * 255 / m_nTotalTime;
	nBlend = nBlend<<24;
	nBlend |= 0x00ffffff;
	return nBlend;
}

void ImageGrid::Execute(float seconds)
{
    // 需要显示空格，如不需要，且不在淡入淡出时,则不处理
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
