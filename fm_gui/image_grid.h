//--------------------------------------------------------------------
// �ļ���:		ImageGrid.h
// ��  ��:		ͼƬ����ؼ�
// ˵  ��:		
// ��������:	2008��8��27��
// ������:		����ƽ
// ��Ȩ����:	������ţ�������޹�˾
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

// �����ĵ�ͼԪ�ṹ,�п����Ǿ�̬λͼ,Ҳ�п�����ͼƬ����,Ҳ�п��ܽ����Ǹ����Σ�Ҳ���Բ���
enum HelpMode
{
	HELP_NONE = 0,
	HELP_RECT = 1,
	HELP_TEXURE = 2,
	HELP_ANIMATION = 3,
};

// �ո��ӽ������Դ���
enum eEmptyGridState
{
	DRAW_STATE_NORMAL,		//����״̬
	DRAW_STATE_SHOW,		//����״̬
	DRAW_STATE_HIDE,		//����״̬
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

    // �����������һ����Ʒ,���ԭ���Ѵ������滻
    // index �ڸ����е���Ŵ�0��ʼ
    // pImageName ͼƬ�ļ���
	// pItemText �ı�
	// ItemNumber ��������
	// lMark �û��Զ�����
	virtual bool AddItem(int index, const char* pImageName, 
		const wchar_t* pItemName = NULL, int ItemNumber = 1, int lMark = -1)
	{
		return AddItemWithImageScale(index, pImageName, pItemName, ItemNumber, lMark);
	}

    // �����������һ����Ʒ,���ԭ���Ѵ������滻
    // index �ڸ����е���Ŵ�0��ʼ
    // pImageName ͼƬ�ļ���
	// pItemText �ı�
	// ItemNumber ��������
	// lMark �û��Զ�����
	// nImageWidthScale ͼƬ�Ŀ�ȱ���
	// nImageHeightScale ͼƬ�ĸ߶ȱ���
	virtual bool AddItemWithImageScale(int index, const char* pImageName, 
		const wchar_t* pItemName = NULL, int ItemNumber = 1, int lMark = -1,
		int nImageWidthScale = 1, int nImageHeightScale = 1);

	// ������������
	virtual bool Clear();
	// ɾ��һ����Ʒ
	virtual bool DelItem(int index);

    // ͨ�����Ʋ�����Ʒ,�鲻������-1���鵽������Ʒ���
    int FindItem(const wchar_t* pItemName);

    // �ж�һ���Ƿ�Ϊ��
    bool IsEmpty(int index);

	bool SetItemImage(int index, const char* pImageName);
	const char* GetItemImage(int index);

    // ��ͼƬת�ɺڰ���ʾ
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

	// ���Ӹ�����ȴ��Ч��add by zhum,2011.8.10
	bool AddCool(int index,long	lcooltime,float bit);
	bool StopCool(int index);
    bool IsCooling(int index);

    bool SetItemTip(int index, const char* pTipIDName);

    bool SetSelectItemIndex(long index) { m_nCurSelect = index; return true; };
	int GetSelectItemIndex() {return m_nCurSelect;};
	int GetMouseInItemIndex() {return m_nCurMouseIn;}
	int GetDBClickItemIndex() {return m_nCurDBSelect;}
	int GetRBClickItemIndex() {return m_nCurRButton;}

	// ����Ĭ����ɫ
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

	// ������ʾ������:0,0,120,40
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

    // �����״̬
    bool GetShowMouseDownState() {return m_bMouseDownState;}
    void SetShowMouseDownState(bool value){m_bMouseDownState = value;}

    // �����״̬����
    float GetMouseDownScale() {return m_fMouseDownScale;}
    void SetMouseDownScale(float value){m_fMouseDownScale = value;}

    // �����״̬ͼ��ƫ��X
    int GetMouseDownOffsetX() {return m_nMouseDownOffsetX;}
    void SetMouseDownOffsetX(int value){m_nMouseDownOffsetX = value;}

    // �����״̬ͼ��ƫ��Y
    int GetMouseDownOffsetY() {return m_nMouseDownOffsetY;}
    void SetMouseDownOffsetY(int value){m_nMouseDownOffsetY = value;}

    // �����״̬ͼ��͸����
    int GetMouseDownAlpha() {return m_nMouseDownAlpha;}
    void SetMouseDownAlpha(int value){m_nMouseDownAlpha = value;}

    // �ֶ����õ���״̬
    bool SetMouseDownState(int index, bool bDown);
	
	// ��ȡ����״̬
	bool GetMouseDownState(int index);

    // �ֶ��������״̬
    bool ClearMouseDowState();

	// �ֶ�����ǰ��ѡ��
	bool ClearSelectedState();
	// �ֶ�����ѡ��״̬
	bool SetSelectedState(int index);

    // ��ȡ��ǰ�ո��ӵ�״̬
    int GetEmptyGridState() { return m_eEmptyDrawState; };
    void SetEmptyGridState(int value);

    // �Ƿ����ƶ��漼����ʹ��
    void SetIsMobileSkillUse(bool isMobileSkillUse) {m_bIsMobileSkillUse = isMobileSkillUse;}
    bool GetIsMobileSkillUse() {return m_bIsMobileSkillUse;}

protected:
	// ʵ��IControl����麯��
	virtual bool Init(const IVarList& args);
	virtual bool Shut();
    virtual void Execute(float seconds);

	virtual void PrepareDesignInfo(IDesignInfo* di);
	virtual void Paint(float seconds);

	// �Ƿ�������
	virtual bool GetLoadFinish();

	//  ���������
	virtual int GetVerticalTotal() const;
	virtual int GetVerticalCanDisp() const;

	// �¼�
	// ������̧��
	virtual int OnLButtonUp(int x, int y);
	virtual int OnLButtonDown(int x, int y);
	virtual int OnLButtonDoubleClick(int x, int y);
	virtual int OnMouseMove(int x, int y);
	// �����꽹��
	virtual int OnGetCapture(const PERSISTID& lost_id);
    // ʧȥ��꽹��
    virtual int OnLostCapture(const PERSISTID& get_id);
	// ����Ҽ�̧��
	virtual int OnRButtonUp(int x, int y);
    // ����Ҽ�����
    virtual int OnRButtonDown(int x, int y);

    virtual void OnMouseInGrid(int x, int y){};
    virtual void OnMouseOutGrid(){};

	// ��ק
	virtual int OnDragMove(int x, int y);
	virtual int OnDragLeave(int x, int y);
	// �϶�����
	virtual int OnDropIn(int x, int y);

	// �������ʱ����ƶ�
	virtual int OnMouseMoveWhenLButtonDown(int x, int y);

	// �ж�ָ��λ���Ƿ�͸��
	virtual bool TestTransparent(int x, int y);

protected:
	// ���������Ʒ���ڷ�����vItems�е����,���򷵻�-1
	int GetItem(int index);

	// �ж�ָ�����������ĸ�item��
	int GetInWhichItem(int x, int y, point_t& CurMouseInPos) const;

    ImageAnimationManager* InnerGetAnimation();
    CoolManager* InnerGetCoolManager();

	// targetrectΪ������ͼƬ�ľ��Ծ��Σ�targetrectΪ���ӵľ��Ծ���
	void DrawImageHelp(ImageHelp& image, rect_t& targetrect, rect_t& gridrect, 
		unsigned long dwColor);

	//
	void SetImageModeByString(ImageHelp& image, const char* pName);

    PERSISTID CreateGlyph(const char* pName, bool& ispng, int& width, int& height);

    // ����ؼ�������ͼ��
    void ReleaseControls();
    // �ͷ����и����е�ͼƬ
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
    // �����е���Ʒ
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
        std::wstring wcsName;		// ����
        PERSISTID Image;
        rect_t rect;
        std::string ImageName;
        unsigned long Index;		// �ڸ����е����
        unsigned long Number;		// ����
        long cool;					// ��ȴ
		long coolteam;				// ��ȴ�����
		unsigned int Alpha;			// ͸����
        bool IsCover;
        bool IsLock;
        bool IsPng;					// �Ƿ���PNGͼ,��ΪTRUE,FALSE��ʾ��TGA
        bool IsToBlackAndWhite;     // �Ƿ�ת�ɺڰ�

		CoolItem*	pCoolItem;		// ��ÿ������������ȴ��Ч��add by zhum,2011.8.10
		point_t image_scale;			// ͼƬ�ķŴ��� add by liujie, 2011.11.17
    };
    //
    bool DrawOneGrid(const ImageItem& item, 
        const int xbase, const int ybase, const int x, const int y, 
        const int nGridWidth, const int nGridHeight);

    // 
    bool DrawImageHelpByIndex(ImageHelp& imagehelp, int nColor, int index, 
        int indexstart, int indexend, int xbase, int ybase, int GridWidth, 
        int GridHeight, int nGridWidth, int nGridHeight);

    // ���Ƶ���
	bool DrawImageSingle(int index, point_t &point,
		int lBackWidth, int lBackHeight, int blend_color= 0xffffffff);

    // ���Ƶ����ո�ĵ��뵭��
    bool DrawSingleEmptyGridFading(int iGridIndex, int iStartIndex,
		    int iXBase, int iYBase, int iRrow, int iCol, int iWidth, int iHeight);

    // ��ȡͼƬλ��
    point_t GetImagePos(int, int, int, int, int, int, int);

    // ���ݵ�ǰʱ�䣬��ȡ͸����
    int GetBlend(int nElapseTime);
protected:
	// �༭���õ�����
	bool m_bEdit;				// �Ƿ��ڱ༭ģʽ
	bool m_bCenter;				// �����Ƿ���ж���
    bool m_bFitGrid;            // ����GRID,���ܴ�С�Զ�����
    bool m_bRoundGrid;          // �Ƿ���ʾԲ�θ���
    bool m_bShowEmpty;

    int m_nGridEmptyMode;       // �Ƿ���ʾ�յĸ��ӣ����Ϊtrue,��յĸ��ӱ�����ѡ�лᱻ����
	long m_nRow;				// ����
	long m_nClomn;				// ����
    long m_nMaxSize;            // �������ɵ����ĸ��������ɴ�ȷ���������Ĺ���ֵ
	rect_t m_ViewRect;			// ��ʾ����
    long m_nGridOffsetX;        // ������������������߿���е�ƫ��
    long m_nGridOffsetY;        // ������������������߿���е�ƫ��
    long m_nGridHeight;         // �ո�ĸ߶�
    long m_nGridWidth;          // �ո�Ŀ��
    long m_nGridBackOffsetX;    // ����ͼƬ����ڸ��ӵ�Xƫ��
    long m_nGridBackOffsetY;    // ����ͼƬ����ڸ��ӵ�Yƫ��
    long m_nGridCoverOffsetX;    // ����ͼƬ����ڸ��ӵ�Xƫ��
    long m_nGridCoverOffsetY;    // ����ͼƬ����ڸ��ӵ�Yƫ��

    // �Զ���ÿ�����ӵ�λ��
    // ���и��ӵ������ַ���x,y;x,y;x,y;x,y��ʽ������ÿ�����ӵ�λ��,ͨ������������û�й������Ҳ���m_ViewRect����
    // x,yΪ����ڿؼ����Ͻǵ�λ��
    std::string m_szGridsPos; 
    std::vector<point_t> m_vGridsPos;

	unsigned int m_CoolColor;			// ׼ȷ��ɫ
	// ѡ�к�����״̬����ʾ�Ŀ��ͼƬ
	unsigned int m_SelectColor;
	unsigned int m_MouseInColor;
	unsigned int m_CoverColor;
	unsigned int m_LockColor;

	ImageHelp m_SelectImage;		// ����Ƴ�
	ImageHelp m_MouseInImage;		// �������
	ImageHelp m_CoverImage;			// ����
	ImageHelp m_LockImage;			// ����
	ImageHelp m_GridBack;           // ���ӱ���
	std::string m_szCoolAnimation;

    std::vector<ImageItem*> m_vItems;

	int m_nTopIndex;                // �߼����������ʵ���д���0λ��
	int m_nCurMouseIn;              // ������е��߼��������
	int m_nCurSelect;               // ѡ�е��߼��������
	int m_nCurDBSelect;             // ˫��ѡ�е��߼��������
	int m_nCurRButton;              // �Ҽ����������߼��������
    int m_nCurLButtonDown;
    int m_nCurRButtonDown;

	int m_nDragOldX;
	int m_nDragOldY;

    point_t m_CurMouseInPos;
    
	bool m_IsFirstTime;

    // ��ס����
    //std::map<long, bool> m_vCoverMap;
	TPodHashMap<int, bool, TPodTraits<int>, TCoreAlloc> m_vCoverMap;
    // ����ţ�����ÿ�����Ӷ�Ӧ��ͬ��ViewItem,index
    // �Ӹ�����ŵ�����ŵĶ�Ӧ
    //std::map<long, long> m_vBindIndexMap;
	TPodHashMap<int, int, TPodTraits<int>, TCoreAlloc> m_vBindIndexMap;
    // �Ӹ�����ŵ�����ͼ�Ķ�Ӧ
    //std::map<int, CImagePlayer*> m_mapBackImageIndexMap;
	TPodHashMap<int, CImagePlayer*, TPodTraits<int>, 
		TCoreAlloc> m_mapBackImageIndexMap;
    // �Ӹ�����ŵ��ڸ�ͼ�Ķ�Ӧ
    //std::map<int, CImagePlayer*> m_mapCoverImageIndexMap;
	TPodHashMap<int, CImagePlayer*, TPodTraits<int>, 
		TCoreAlloc> m_mapCoverImageIndexMap;

    // ��ȴͼ
    TPodHashMap<int, CImagePlayer*, TPodTraits<int>, TCoreAlloc> m_mapCoolingImage;

    // �ո��뵭�����Ʋ���
	eEmptyGridState m_eEmptyDrawState;
	int m_nTotalTime;
	int m_nElapseTime;
    bool m_bShowEmptyFade;

    // �����״̬
	//std::map<int, bool> m_vDownStateMap;	// ���ӵİ���״̬
	TPodHashMap<int, bool, TPodTraits<int>, TCoreAlloc> m_vDownStateMap; // ���ӵİ���״̬
    bool m_bMouseDownState;		// �����/�ҽ�����״̬����
    float m_fMouseDownScale;
    int m_nMouseDownOffsetX;
    int m_nMouseDownOffsetY;
    int m_nMouseDownAlpha;

    bool m_bIsMobileSkillUse;   // �Ƿ����ƶ��汾������ʹ��
};

#endif //ImageGrid_h_2008_8_27
