//--------------------------------------------------------------------
// 文件名:		ImageGrid.h
// 内  容:		图片网格控件
// 说  明:		
// 创建日期:	2008年8月27日
// 创建人:		杨利平
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef ImageGrid_h_2008_8_27
#define ImageGrid_h_2008_8_27

#include "../public/core_mem.h"
#include "../utils/pod_hash_map.h"
#include "i_scrollable.h"
#include "cool_manager.h"
#include <vector>
#include <string>
//#include <map>
//#include <windows.h>

// 附助的的图元结构,有可能是静态位图,也有可能是图片动画,也有可能仅仅是个矩形，也可以不画
enum HelpMode
{
	HELP_NONE = 0,
	HELP_RECT = 1,
	HELP_TEXURE = 2,
	HELP_ANIMATION = 3,
};

// 空格子渐隐渐显代码
enum eEmptyGridState
{
	DRAW_STATE_NORMAL,		//正常状态
	DRAW_STATE_SHOW,		//渐显状态
	DRAW_STATE_HIDE,		//渐隐状态
};

struct ImageHelp
{
    ImageHelp():mode(HELP_RECT),
        width(32),height(32),name("rect_t"){}
	HelpMode mode;
    int width;
    int height;
	std::string name;
	CImagePlayer image;
};

class ImageAnimationManager;

class ImageGrid: public IScrollable
{
public:
	ImageGrid();
	virtual ~ImageGrid();

    // 向容器中添加一个物品,如果原来已存在则被替换
    // index 在格子中的序号从0开始
    // pImageName 图片文件名
	// pItemText 文本
	// ItemNumber 整形数量
	// lMark 用户自定义标记
	virtual bool AddItem(int index, const char* pImageName, 
		const wchar_t* pItemName = NULL, int ItemNumber = 1, int lMark = -1)
	{
		return AddItemWithImageScale(index, pImageName, pItemName, ItemNumber, lMark);
	}

    // 向容器中添加一个物品,如果原来已存在则被替换
    // index 在格子中的序号从0开始
    // pImageName 图片文件名
	// pItemText 文本
	// ItemNumber 整形数量
	// lMark 用户自定义标记
	// nImageWidthScale 图片的宽度倍数
	// nImageHeightScale 图片的高度倍数
	virtual bool AddItemWithImageScale(int index, const char* pImageName, 
		const wchar_t* pItemName = NULL, int ItemNumber = 1, int lMark = -1,
		int nImageWidthScale = 1, int nImageHeightScale = 1);

	// 清理整个容器
	virtual bool Clear();
	// 删除一格物品
	virtual bool DelItem(int index);

    // 通过名称查找物品,查不到返回-1，查到返回物品序号
    int FindItem(const wchar_t* pItemName);

    // 判断一格是否为空
    bool IsEmpty(int index);

	bool SetItemImage(int index, const char* pImageName);
	const char* GetItemImage(int index);

    // 将图片转成黑白显示
    bool ChangeItemImageToBW(int index, bool bChange);

	bool SetItemName(int index, const wchar_t* pItemName);
    const wchar_t* GetItemName(int index);

	bool SetItemNumber(int index, int ItemNumber);
	int GetItemNumber(int index);

	bool SetItemMark(int index, int lMark);
	int GetItemMark(int index);

    bool SetItemAlpha(int itemindex, int alpha);
    int GetItemAlpha(int itemindex);

	bool SetCoolType(int index, long lCoolType);
	bool SetCoolTeam(int index, long lCoolTeam);
	bool CoverItem(int index, bool bCover);
	bool LockItem(int index, bool bLock);

	// 增加格子冷却特效，add by zhum,2011.8.10
	bool AddCool(int index,long	lcooltime,float bit);
	bool StopCool(int index);
    bool IsCooling(int index);

    bool SetItemTip(int index, const char* pTipIDName);

    bool SetSelectItemIndex(long index) { m_nCurSelect = index; return true; };
	int GetSelectItemIndex() {return m_nCurSelect;};
	int GetMouseInItemIndex() {return m_nCurMouseIn;}
	int GetDBClickItemIndex() {return m_nCurDBSelect;}
	int GetRBClickItemIndex() {return m_nCurRButton;}

	// 设置默认颜色
	void SetSelectColor(const char* szColor);
	result_string GetSelectColor();

	void SetMouseInColor(const char* szColor);
	result_string GetMouseInColor();

    void SetCoverColor(const char* szColor);
    result_string GetCoverColor();

    void SetLockColor(const char* szColor);
    result_string GetLockColor();

	void SetCoolColor(const char* szColor);
	result_string GetCoolColor();

	// 设置显示区域如:0,0,120,40
	void SetViewRect(const char* RectStr);
	result_string GetViewRect();

    int GetMaxSize() {return m_nMaxSize;}
    void SetMaxSize(const int value);

	int GetRowNum() {return m_nRow;}
	void SetRowNum(const int lRowNum) { if (lRowNum > 0) m_nRow = lRowNum; }

	int GetClomnNum() {return m_nClomn;}
	void SetClomnNum(const int lClomnNum) { if (lClomnNum > 0) m_nClomn = lClomnNum; }

    bool GetRoundGrid() { return m_bRoundGrid; }
    void SetRoundGrid(bool value) { m_bRoundGrid = value; }

    bool GetShowEmpty() { return m_bShowEmpty; }
    void SetShowEmpty(bool value) { m_bShowEmpty = value; }

    bool GetEmptyShowFade() { return m_bShowEmptyFade; }
    void SetEmptyShowFade(bool value) { m_bShowEmptyFade = value; }

	bool GetEditMode() { return m_bEdit; }
	void SetEditMode(bool IsEdit) { m_bEdit = IsEdit; }

	bool GetCenter() { return m_bCenter; }
	void SetCenter(bool IsCenter) { m_bCenter = IsCenter; }

    int GetGridOffsetX() { return m_nGridOffsetX; }
    void SetGridOffsetX(int OffsetX) { m_nGridOffsetX = OffsetX; }

    int GetGridOffsetY() { return m_nGridOffsetY; }
    void SetGridOffsetY(int OffsetY) { m_nGridOffsetY = OffsetY; }

    bool GetFitGrid() { return m_bFitGrid; }
    void SetFitGrid(bool IsFit) { m_bFitGrid = IsFit; }

    int GetGridHeight() { return m_nGridHeight; }
    void SetGridHeight(int height) { m_nGridHeight = height; }

    int GetGridWidth() { return m_nGridWidth; }
    void SetGridWidth(int width) { m_nGridWidth = width; }

	const char* GetDrawMouseIn(){ return m_MouseInImage.name.c_str();}
	void SetDrawMouseIn(const char* pName);

	const char* GetDrawMouseSelect() { return m_SelectImage.name.c_str(); }
	void SetDrawMouseSelect(const char*pName);

	const char* GetDrawCover() { return m_CoverImage.name.c_str();}
	void SetDrawCover(const char* pName);

	const char* GetDrawLock() { return m_LockImage.name.c_str();}
	void SetDrawLock(const char* pName);

    const char* GetDrawGridBack() { return m_GridBack.image.GetName();}
    void SetDrawGridBack(const char* pName);

    const char* GetItemBackImage(int index);
    bool SetItemBackImage(int index, const char* pImageName);

    const char* GetItemCoverImage(int index);
    bool SetItemCoverImage(int index, const char* pImageName);

    const char* GetItemCoolingImage(int index);
    bool SetItemCoolingImage(int index, const char* pImageName);

    int GetGridBackOffsetX() { return m_nGridBackOffsetX;}
    void SetGridBackOffsetX(int nOffsetX) { m_nGridBackOffsetX = nOffsetX; }

    int GetGridBackOffsetY() { return m_nGridBackOffsetY;}
    void SetGridBackOffsetY(int nOffsetY) { m_nGridBackOffsetY = nOffsetY; }

    int GetGridCoverOffsetX() { return m_nGridCoverOffsetX;}
    void SetGridCoverOffsetX(int nOffsetX) { m_nGridCoverOffsetX = nOffsetX; }

    int GetGridCoverOffsetY() { return m_nGridCoverOffsetY;}
    void SetGridCoverOffsetY(int nOffsetY) { m_nGridCoverOffsetY = nOffsetY; }

    const char* GetGridsPos() { return m_szGridsPos.c_str(); }
    void SetGridsPos(const char* pGridsPos);

    const char* GetCoolAnimation() { return m_szCoolAnimation.c_str();}
    void SetCoolAnimation(const char* pName);

    virtual void SetVerticalValue(int value);
    virtual int GetVerticalValue() const;

    int GetMouseInItemLeft();
    int GetMouseInItemTop();

    int GetItemLeft(int itemindex) const;
    int GetItemTop(int itemindex) const;

    bool SetBindIndex(int itemindex, int bindindex);
    int GetBindIndex(int itemindex);

    // 鼠标点击状态
    bool GetShowMouseDownState() {return m_bMouseDownState;}
    void SetShowMouseDownState(bool value){m_bMouseDownState = value;}

    // 鼠标点击状态放缩
    float GetMouseDownScale() {return m_fMouseDownScale;}
    void SetMouseDownScale(float value){m_fMouseDownScale = value;}

    // 鼠标点击状态图标偏移X
    int GetMouseDownOffsetX() {return m_nMouseDownOffsetX;}
    void SetMouseDownOffsetX(int value){m_nMouseDownOffsetX = value;}

    // 鼠标点击状态图标偏移Y
    int GetMouseDownOffsetY() {return m_nMouseDownOffsetY;}
    void SetMouseDownOffsetY(int value){m_nMouseDownOffsetY = value;}

    // 鼠标点击状态图标透明度
    int GetMouseDownAlpha() {return m_nMouseDownAlpha;}
    void SetMouseDownAlpha(int value){m_nMouseDownAlpha = value;}

    // 手动设置点下状态
    bool SetMouseDownState(int index, bool bDown);
	
	// 获取点下状态
	bool GetMouseDownState(int index);

    // 手动清理点下状态
    bool ClearMouseDowState();

	// 手动清理当前的选中
	bool ClearSelectedState();
	// 手动设置选中状态
	bool SetSelectedState(int index);

    // 获取当前空格子的状态
    int GetEmptyGridState() { return m_eEmptyDrawState; };
    void SetEmptyGridState(int value);

    // 是否是移动版技能栏使用
    void SetIsMobileSkillUse(bool isMobileSkillUse) {m_bIsMobileSkillUse = isMobileSkillUse;}
    bool GetIsMobileSkillUse() {return m_bIsMobileSkillUse;}

protected:
	// 实现IControl里的虚函数
	virtual bool Init(const IVarList& args);
	virtual bool Shut();
    virtual void Execute(float seconds);

	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	// 是否加载完成
	virtual bool GetLoadFinish();

	//  滚动条相关
	virtual int GetVerticalTotal() const;
	virtual int GetVerticalCanDisp() const;

	// 事件
	// 鼠标左键抬起
	virtual int OnLButtonUp(int x, int y);
	virtual int OnLButtonDown(int x, int y);
	virtual int OnLButtonDoubleClick(int x, int y);
	virtual int OnMouseMove(int x, int y);
	// 获得鼠标焦点
	virtual int OnGetCapture(const PERSISTID& lost_id);
    // 失去鼠标焦点
    virtual int OnLostCapture(const PERSISTID& get_id);
	// 鼠标右键抬起
	virtual int OnRButtonUp(int x, int y);
    // 鼠标右键按下
    virtual int OnRButtonDown(int x, int y);

    virtual void OnMouseInGrid(int x, int y){};
    virtual void OnMouseOutGrid(){};

	// 拖拽
	virtual int OnDragMove(int x, int y);
	virtual int OnDragLeave(int x, int y);
	// 拖动放入
	virtual int OnDropIn(int x, int y);

	// 左键按下时鼠标移动
	virtual int OnMouseMoveWhenLButtonDown(int x, int y);

	// 判断指定位置是否透明
	virtual bool TestTransparent(int x, int y);

protected:
	// 如果格子物品存在返回在vItems中的序号,否则返回-1
	int GetItem(int index);

	// 判断指定坐标落在哪个item内
	int GetInWhichItem(int x, int y, point_t& CurMouseInPos) const;

    ImageAnimationManager* InnerGetAnimation();
    CoolManager* InnerGetCoolManager();

	// targetrect为格子中图片的绝对矩形，targetrect为格子的绝对矩形
	void DrawImageHelp(ImageHelp& image, rect_t& targetrect, rect_t& gridrect, 
		unsigned long dwColor);

	//
	void SetImageModeByString(ImageHelp& image, const char* pName);

    PERSISTID CreateGlyph(const char* pName, bool& ispng, int& width, int& height);

    // 清理控件，配置图等
    void ReleaseControls();
    // 释放所有格子中的图片
    void ReleaseImages();

    //
    bool DrawFirstTime();
    //
    bool DrawGridBack(const int xbase, const int ybase, 
        const int GridWidth, const int GridHeight, const int nGridWidth, const int nGridHeight,
        const int indexstart, const int indexend);

    //
    bool DrawCover(const int xbase, const int ybase, 
        const int GridWidth, const int GridHeight, const int nGridWidth, const int nGridHeight,
        const int indexstart, const int indexend);

    //
    bool DrawEdit(const rect_t ViewRect, 
        const int xbase, const int ybase, 
        const int GridWidth, const int GridHeight, 
        const int nGridWidth, const int nGridHeight);

    //
    // 格子中的物品
    struct ImageItem
    {
        ImageItem(): IsCover(false), IsLock(false), Number(0), 
            IsPng(false), IsToBlackAndWhite(false), Alpha(255),
			cool(0),
			coolteam(0)
        {		
			image_scale.x = 1;
			image_scale.y = 1;
        }

        long lMark;
        std::wstring wcsName;		// 名称
        PERSISTID Image;
        rect_t rect;
        std::string ImageName;
        unsigned long Index;		// 在格子中的序号
        unsigned long Number;		// 数量
        long cool;					// 冷却
		long coolteam;				// 冷却大分类
		unsigned int Alpha;			// 透明度
        bool IsCover;
        bool IsLock;
        bool IsPng;					// 是否是PNG图,是为TRUE,FALSE表示是TGA
        bool IsToBlackAndWhite;     // 是否转成黑白

		CoolItem*	pCoolItem;		// 给每个格子增加冷却特效，add by zhum,2011.8.10
		point_t image_scale;			// 图片的放大倍数 add by liujie, 2011.11.17
    };
    //
    bool DrawOneGrid(const ImageItem& item, 
        const int xbase, const int ybase, const int x, const int y, 
        const int nGridWidth, const int nGridHeight);

    // 
    bool DrawImageHelpByIndex(ImageHelp& imagehelp, int nColor, int index, 
        int indexstart, int indexend, int xbase, int ybase, int GridWidth, 
        int GridHeight, int nGridWidth, int nGridHeight);

    // 绘制单格
	bool DrawImageSingle(int index, point_t &point,
		int lBackWidth, int lBackHeight, int blend_color= 0xffffffff);

    // 绘制单个空格的淡入淡出
    bool DrawSingleEmptyGridFading(int iGridIndex, int iStartIndex,
		    int iXBase, int iYBase, int iRrow, int iCol, int iWidth, int iHeight);

    // 获取图片位置
    point_t GetImagePos(int, int, int, int, int, int, int);

    // 根据当前时间，获取透明度
    int GetBlend(int nElapseTime);
protected:
	// 编辑配置的属性
	bool m_bEdit;				// 是否在编辑模式
	bool m_bCenter;				// 格子是否居中对齐
    bool m_bFitGrid;            // 填满GRID,不管大小自动调整
    bool m_bRoundGrid;          // 是否显示圆形格子
    bool m_bShowEmpty;

    int m_nGridEmptyMode;       // 是否显示空的格子，如果为true,则空的格子背景及选中会被绘制
	long m_nRow;				// 行数
	long m_nClomn;				// 列数
    long m_nMaxSize;            // 所能容纳的最大的格子数，由次确定滚动条的滚动值
	rect_t m_ViewRect;			// 显示区域
    long m_nGridOffsetX;        // 格子相对于自身所在线框格中的偏移
    long m_nGridOffsetY;        // 格子相对于自身所在线框格中的偏移
    long m_nGridHeight;         // 空格的高度
    long m_nGridWidth;          // 空格的宽度
    long m_nGridBackOffsetX;    // 背景图片相对于格子的X偏移
    long m_nGridBackOffsetY;    // 背景图片相对于格子的Y偏移
    long m_nGridCoverOffsetX;    // 背景图片相对于格子的X偏移
    long m_nGridCoverOffsetY;    // 背景图片相对于格子的Y偏移

    // 自定义每个格子的位置
    // 所有格子的配置字符串x,y;x,y;x,y;x,y型式来配置每个格子的位置,通常这种配置下没有滚动条且不受m_ViewRect限制
    // x,y为相对于控件左上角的位置
    std::string m_szGridsPos; 
    std::vector<point_t> m_vGridsPos;

	unsigned int m_CoolColor;			// 准确颜色
	// 选中和移入状态下显示的框或图片
	unsigned int m_SelectColor;
	unsigned int m_MouseInColor;
	unsigned int m_CoverColor;
	unsigned int m_LockColor;

	ImageHelp m_SelectImage;		// 鼠标移出
	ImageHelp m_MouseInImage;		// 鼠标移入
	ImageHelp m_CoverImage;			// 禁用
	ImageHelp m_LockImage;			// 锁定
	ImageHelp m_GridBack;           // 格子背景
	std::string m_szCoolAnimation;

    std::vector<ImageItem*> m_vItems;

	int m_nTopIndex;                // 逻辑格子序号在实际中处在0位置
	int m_nCurMouseIn;              // 鼠标所有的逻辑格子序号
	int m_nCurSelect;               // 选中的逻辑格子序号
	int m_nCurDBSelect;             // 双击选中的逻辑格子序号
	int m_nCurRButton;              // 右键最近点击的逻辑格子序号
    int m_nCurLButtonDown;
    int m_nCurRButtonDown;

	int m_nDragOldX;
	int m_nDragOldY;

    point_t m_CurMouseInPos;
    
	bool m_IsFirstTime;

    // 盖住禁用
    //std::map<long, bool> m_vCoverMap;
	TPodHashMap<int, bool, TPodTraits<int>, TCoreAlloc> m_vCoverMap;
    // 绑定序号，用于每个格子对应不同的ViewItem,index
    // 从格子序号到绑定序号的对应
    //std::map<long, long> m_vBindIndexMap;
	TPodHashMap<int, int, TPodTraits<int>, TCoreAlloc> m_vBindIndexMap;
    // 从格子序号到背景图的对应
    //std::map<int, CImagePlayer*> m_mapBackImageIndexMap;
	TPodHashMap<int, CImagePlayer*, TPodTraits<int>, 
		TCoreAlloc> m_mapBackImageIndexMap;
    // 从格子序号到遮盖图的对应
    //std::map<int, CImagePlayer*> m_mapCoverImageIndexMap;
	TPodHashMap<int, CImagePlayer*, TPodTraits<int>, 
		TCoreAlloc> m_mapCoverImageIndexMap;

    // 冷却图
    TPodHashMap<int, CImagePlayer*, TPodTraits<int>, TCoreAlloc> m_mapCoolingImage;

    // 空格淡入淡出控制参数
	eEmptyGridState m_eEmptyDrawState;
	int m_nTotalTime;
	int m_nElapseTime;
    bool m_bShowEmptyFade;

    // 鼠标点击状态
	//std::map<int, bool> m_vDownStateMap;	// 格子的按下状态
	TPodHashMap<int, bool, TPodTraits<int>, TCoreAlloc> m_vDownStateMap; // 格子的按下状态
    bool m_bMouseDownState;		// 鼠标左/右建按下状态处理
    float m_fMouseDownScale;
    int m_nMouseDownOffsetX;
    int m_nMouseDownOffsetY;
    int m_nMouseDownAlpha;

    bool m_bIsMobileSkillUse;   // 是否是移动版本技能栏使用
};

#endif //ImageGrid_h_2008_8_27
