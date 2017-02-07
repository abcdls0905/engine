//--------------------------------------------------------------------
// �ļ���:		i_control.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��9��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _I_CONTROL_H
#define _I_CONTROL_H

#include "../public/module.h"
#include "../public/core_mem.h"
#include "../public/core_type.h"
#include "../public/var.h"
#include "../public/var_list.h"
#include "../utils/array_pod.h"
#include "../utils/string_alias.h"
#include "../visual/i_painter.h"
#include "gui_const.h"
#include "gui_utils.h"
#include "color_data.h"
#include "image_player.h"
#include "i_design_info.h"
#include "gui.h"

#define DECLARE_EVENT(p)


// ����ؼ�����

class IGuiHook;

class IControl: public IEntity
{
public:
	// ��Դ����ģʽ
	enum LOAD_MODE_ENUM 
	{
		LOADMODE_DEFAULT,	// �븸�ؼ���ͬ��ͬ������
		LOADMODE_ASYNC,		// �첽����
		LOADMODE_SYNC,		// ͬ������
	};

//private:
//	typedef TStringAlias<char, char, TStringTraits<char>,
//		TStringTraits<char>, TCoreAlloc> callback_container_t;

public:
	IControl();
	virtual ~IControl() = 0;

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
		
	// ���ƿؼ�
	virtual void Paint(float seconds);
	
	// �����������Ϣ
	virtual void PrepareDesignInfo(IDesignInfo* di);

	// �Ƿ�������
	virtual bool GetLoadFinish();
    
	bool GetLoadFinishValue() { return m_bLoadFinish; }
	
	// ����
	virtual void SetText(const wchar_t* val);
	virtual const wchar_t* GetText() const;

    // ��ȡ��ʾ�ı�
	const wchar_t* GetShowText() const { return m_wsShowText.c_str(); }

    // ��ȡ���λ��
    virtual point_t GetCaretPos();

	// ����������
	virtual void SetCaretName(const char* name) {}
	virtual const char* GetCaretName() const { return ""; }
	
    // ����Դ����
    void SetDataSource(const char* source);
    const char* GetDataSource() const;

	// ��С�ߴ�
	virtual int GetMinWidth() const { return 4; }
	virtual int GetMinHeight() const { return 4; }
	
	// �Ƿ���Ҫ����TAB��
	virtual bool GetAllowTab() const { return false; }
	// �Ƿ���Ҫ����س���
	virtual bool GetAllowReturn() const { return false; }

	// �Ƿ�����
	virtual bool IsContainer() const { return false; }
	// �ÿؼ�������
	virtual IControl* FindChild(const char* name) const { return NULL; }
	// �ÿؼ����ӱ��ڵ㿪ʼ���еݹ���ң��������ڵ�
	virtual IControl* QueryControl(const char* name);
	// ����ӿؼ�
	virtual bool AddChild(IControl* child) { return false; }
	// ��ָ���ؼ�֮�����
	virtual bool InsertAfterChild(IControl* child, 
		IControl* after) { return false; }
	// ��ָ���ؼ�֮ǰ����
	virtual bool InsertBeforeChild(IControl* child, 
		IControl* before) { return false; }
	// �Ƴ��ӿؼ�
	virtual bool RemoveChild(IControl* child) { return false; }
    virtual bool RemoveChildNoEvent(IControl* child) { return false;}
	// ����ӿؼ�
	virtual void ClearChild() {}
	// ɾ�������ӿؼ�
	virtual void DeleteAllChild() {}
	// ���ӿؼ��ŵ����ZOrder��
	virtual bool SendToBack(IControl* child) { return false; }
	// ���ӿؼ��ŵ���ǰ��ZOrder��
	virtual bool BringToFront(IControl* child) { return false; }
	// �Ƿ��ӿؼ�
	virtual bool IsChildControl(IControl* child) const { return false; }
	// ����ӿؼ�����
	virtual size_t GetChildCount() const { return 0; }
	// ���ָ��λ�õ��ӿؼ�
	virtual IControl* GetChildByIndex(size_t index) const { return NULL; }
	// ����ӿؼ�ID�б�
	virtual size_t GetChildList(IVarList& result) const { return 0; }
	// ��Ҫ��ʾ�����ϲ�Ŀؼ�
	virtual bool FindTopLayer(const PERSISTID& id) { return false; }
	virtual bool AddTopLayer(const PERSISTID& id) { return false; }
	virtual bool RemoveTopLayer(const PERSISTID& id) { return false; }
    // ɾ���������ϲ�Ŀؼ�
    virtual void DeleteAllTopLayer() {}
	// �ص�
	// ������
	virtual int OnKeyDown(unsigned int vk);
	// ��̧��
	virtual int OnKeyUp(unsigned int vk);
	// ����������
	virtual int OnLButtonDown(int x, int y);
	// ������̧��
	virtual int OnLButtonUp(int x, int y);
	// ������˫��
	virtual int OnLButtonDoubleClick(int x, int y);
	// ��������������
	virtual int OnLButtonHover(int x, int y);
	// ����Ҽ�����
	virtual int OnRButtonDown(int x, int y);
	// ����Ҽ�̧��
	virtual int OnRButtonUp(int x, int y);
	// ����Ҽ�˫��
	virtual int OnRButtonDoubleClick(int x, int y);
	// ����Ҽ���������
	virtual int OnRButtonHover(int x, int y);
	// ������
	virtual int OnMouseWheel(int x, int y, int delta);
	// ����ƶ�
	virtual int OnMouseMove(int x, int y);
	// �϶�
	virtual int OnDragMove(int x, int y);
	// �϶�����
	virtual int OnDropIn(int x, int y);
	// �����϶�
	virtual int OnDragLeave(int x, int y);
	// �����ַ�
	virtual int OnChar(unsigned int ch);
	// ������뽹��
	virtual int OnGetFocus(const PERSISTID& lost_id);
	// ʧȥ���뽹��
	virtual int OnLostFocus(const PERSISTID& get_id);
	// ����ӿؼ�
	virtual int OnAdd(const PERSISTID& child_id);
	// �Ƴ��ӿؼ�
	virtual int OnRemove(const PERSISTID& child_id);
	// ���븸�ؼ�
	virtual int OnEnter(const PERSISTID& parent_id);
	// �뿪���ؼ�
	virtual int OnLeave(const PERSISTID& parent_id);
	// �����꽹��
	virtual int OnGetCapture(const PERSISTID& lost_id);
	// ʧȥ��꽹��
	virtual int OnLostCapture(const PERSISTID& get_id);
	// �����ؼ��¼�
	virtual int OnSuborEvent(IControl* subor, 
		int event, const IVarList& args);
	// ���Ͻ�����ı�
	virtual int OnLeftChanged(int old);
	virtual int OnTopChanged(int old);
	// �ߴ�ı�
	virtual int OnWidthChanged(int old);
	virtual int OnHeightChanged(int old);
	// ���ø�����ʾ
	virtual int OnSetHint(IControl* hint, int x, int y);
	// ���������Ĳ˵�
	virtual int OnSetContext(IControl* context);
	// ���������״
	virtual int OnSetCursor(int x, int y);
	// �������λ���ĸ��ؼ���
	virtual IControl* OnGetInWhat(int x, int y);
    // �ж�ָ��λ���Ƿ�͸��
    virtual bool TestTransparent(int x, int y);

	// �ؼ���
	void SetName(const char* val);
	const char* GetName() const { return m_strName.c_str(); }
	unsigned int GetHash() const { return m_nHash; }
	
	// ����ڸ��������Ե�ľ���
	void SetLeft(int val);
	int GetLeft() const { return m_nLeft; }

	// ����ڸ������ϱ�Ե�ľ���
	void SetTop(int val);
	int GetTop() const { return m_nTop; }
	
	// �ؼ����
	int GetWidth() const { return m_nWidth; }
	void SetWidth(int val);

	// �ؼ��߶�
	int GetHeight() const { return m_nHeight; }
	void SetHeight(int val);

	// ����ê�����ʵ�ʾ�������λ��
	void SetAbsLeft(int val);
	int GetAbsLeft() const;
	void SetAbsTop(int val);
	int GetAbsTop() const;

	// ǰ��ɫ,����ɫ
	void SetForeColor(unsigned int val);
	unsigned int GetForeColor() const { return m_ForeColor.GetValue(); }

	// ����ɫ
	void SetBackColor(unsigned int val);
	unsigned int GetBackColor() const { return m_BackColor.GetValue(); }

	// ����ɫ
	void SetLineColor(unsigned int val);
	unsigned int GetLineColor() const { return m_LineColor.GetValue(); }

	// ��ɫ
	void SetBlendColor(unsigned int val);
	unsigned int GetBlendColor() const { return m_BlendColor.GetValue(); }

	// ������Ӱɫ/���ɫ
	void SetShadowColor(unsigned int val);
	unsigned int GetShadowColor() const { return m_ShadowColor.GetValue(); }

	// ����/��ֹ
	void SetEnabled(bool val) { m_bEnabled = val; }
	bool GetEnabled() const { return m_bEnabled; }

	// �Ƿ���ʾ
	virtual void SetVisible(bool val) { m_bVisible = val; }
	bool GetVisible() const { return m_bVisible; }

	// �Ƿ���Խ�������
	void SetCanFocus(bool val) { m_bCanFocus = val; }
	bool GetCanFocus() const { return m_bCanFocus; }

	//�Ƿ�֧�����뷨����
	virtual bool GetCanIMEFocus() const { return false; }
	
	// �Ƿ�ɱ�ѡ��
	void SetCanSelect(bool val) { m_bCanSelect = val; }
	bool GetCanSelect() const { return m_bCanSelect; }
	
	// ��ǰ�Ƿ�ӵ�����뽹��
	void SetFocused(bool val) { m_bFocused = val; }
	bool GetFocused() const { return m_bFocused; } 

	// �Ƿ��Ѳ������
	void SetCapture(bool val) { m_bCapture = val; }
	bool GetCapture() const { return m_bCapture; }

	// ������
	virtual void SetFontName(const char* val);
	const char* GetFontName() const { return m_strFontName.c_str(); }

	// �����
	void SetCursorName(const char* val);
	const char* GetCursorName() const { return m_strCursorName.c_str(); }
	
	// ������ʾ����
	void SetHintText(const wchar_t* value);
	const wchar_t* GetHintText() const { return m_wsHintText.c_str(); }
	 // ��ȡת�����,��ΪHintText��������Ǹ�IDNAME
    const wchar_t* GetRealHintText() const { return m_wsRealHintText.c_str(); }
	
	// ������ʾ����
	void SetHintType(const char* value);
	const char* GetHintType() const { return m_strHintType.c_str(); }

	// �����Ĳ˵�
	void SetContext(const char* value);
	const char* GetContext() const { return m_strContext.c_str(); }

	// �������ݶ���
	void SetDataID(const PERSISTID& value);
	PERSISTID GetDataID() const;

	// �Ƿ��첽����
	void SetAsyncLoad(bool value);
	bool GetAsyncLoad() const;

	// TAB���ƶ���˳��
	void SetTabIndex(int value) { m_nTabIndex = value; }
	int GetTabIndex() const { return m_nTabIndex; }
	
	// �Ƿ����TAB�����뽹���ƶ����˿ؼ�
	void SetTabStop(bool value) { m_bTabStop = value; }
	bool GetTabStop() const { return m_bTabStop; }

	// �Ƿ������״̬
	void SetDesignMode(bool value) { m_bDesignMode = value; }
	bool GetDesignMode() const { return m_bDesignMode; }

	// �Ƿ��б߿�
	void SetNoFrame(bool value) { m_bNoFrame = value; }
	bool GetNoFrame() const { return m_bNoFrame; }
	
	// �Ƿ��Զ�������С
	void SetAutoSize(bool value);
	bool GetAutoSize() const;
	
	// ����ͼƬ��Ⱦģʽ
	void SetDrawModeString(const char* value);
	const char* GetDrawModeString() const;
	int GetDrawMode() const;
	
	// ����ͼƬ
	void SetBackImage(const char* value);
	const char* GetBackImage() const { return m_BackImage.GetName(); }

	// ǰ��ɫ�ַ���
	void SetForeColorString(const char* val);
	result_string GetForeColorString() const;

	// ����ɫ�ַ���
	void SetBackColorString(const char* val);
	result_string GetBackColorString() const;

	// ����ɫ�ַ���
	void SetBlendColorString(const char* val);
	result_string GetBlendColorString() const;

	// ������Ӱɫ/���ɫ
	void SetShadowColorString(const char* val);
	result_string GetShadowColorString() const;

    // �����ؼ���͸����
    void SetAlpha(int val);
    int GetAlpha() const { return m_nAlpha; }

	// �߾�ɫ�ַ���
	void SetLineColorString(const char* val);
	result_string GetLineColorString() const;

	// ��ֱê����ʽ
	void SetVAnchorString(const char* val);
	const char* GetVAnchorString() const;

	// ˮƽê����ʽ
	void SetHAnchorString(const char* val);
	const char* GetHAnchorString() const;

    // ͸������,����ĳЩ�ؼ�͸������Ҳ��Ӧ��Ϣ
    void SetTestTrans(bool value) {	m_bTestTrans = value; }
    bool GetTestTrans() const {	return m_bTestTrans; }

	// ����
	void SetGui(Gui* value) { m_pGui = value; }
	Gui* GetGui() const { return m_pGui; }
	
	// ��������
	void SetParentControl(IControl* value) { m_pParent = value; }
	IControl* GetParentControl() const { return m_pParent;	}
	
	// ������Ϣί������
	void SetDelegateControl(IControl* value) { m_pDelegate = value; }
	IControl* GetDelegateControl() const {	return m_pDelegate;	}

    // ������������
    void SetInSound(const char* value);
    const char* GetInSound() const;

    // ����Ƴ�������
    void SetOutSound(const char* value);
    const char* GetOutSound() const;

    // �����������
    void SetClickSound(const char* value);
    const char* GetClickSound() const;

	// �Ƿ�ָ�����������
	bool IsAncestorOf(IControl* pControl);
	
	// ���Ƿ��ڿؼ���Χ��
    bool InRectangle(int x, int y) const;
	
	// ��õ����ĸ��ؼ���
	IControl* GetInWhat(int x, int y);
	
	// ��ö���������FORM��
	IControl* GetRootControl();
	// ������������FORM��
	PERSISTID GetRootID();

    // ��ȡ�˿ؼ���ֱ�Ӹ����洰��Form
    PERSISTID GetParentFormID();

	// ������������
	PERSISTID GetParentID() const
	{
		if (NULL == m_pParent)
		{
			return PERSISTID();
		}

		return m_pParent->GetID();
	}

	// ��ö��������ؼ�
	IControl* GetMasterControl();
	// ��ö�����������
	PERSISTID GetMasterID();
	// �����������
	PERSISTID GetDelegateID();
	// ���ݶ���Ż�ÿؼ�
	IControl* GetControl(const PERSISTID& id) const;
	
	// ������ͼ�Ƿ���Ч
	bool HasBackImage() { return m_BackImage.IsReady(); }
	// �ͷű�����ͼ
	void RelaseBackImage();

	// ���������ؼ�
	IControl* CreateSuborControl(IControl* parent, const char* name);
    // ɾ�������ؼ�
    bool RemoveSuborControl(IControl* pControl);

    // ��ȡm_nInnerAbsLeft,m_nInnerAbsTop
    void SetRefreshInnerAbsPos(bool value){ m_bRefreshInnerAbsPos = value; }
    bool GetRefreshInnerAbsPos(){ return m_bRefreshInnerAbsPos; }

	// ��¼�ص���Ϣ
	//bool AddCallback(const char* event, const char* func);
	//bool RemoveCallback(const char* event);
	//bool ClearCallback();
	//bool FindCallback(const char* event) const;
	//const char* GetCallbackFunc(const char* event) const;
	//size_t GetCallbackCount() const;
	//size_t GetCallbackList(IVarList& result) const;

    //�Ƿ���ö�enter����Ӧ
    virtual void SetDisableEnter(bool value){ m_bIsDisableEnter = value; }
    virtual bool GetDisableEnter()const { return m_bIsDisableEnter; }

	// ���ý���ص�
	void SetGuiHook(IGuiHook* value);
	IGuiHook* GetGuiHook() const { return m_pGuiHook; }

	// �Ƿ����Ϣ͸��
	void SetTransparent(bool value);
	bool GetTransparent() const;

protected:
	int InnerGetAbsLeft() const { return m_nInnerAbsLeft; }
	int InnerGetAbsTop() const { return m_nInnerAbsTop; }

	int GetDrawModeByString(const char* value, 
		int nDefauleMode = CGuiConst::DRAWMODE_FITWINDOW) const;
	const char* GetStringByDrawMode(int nDrawMode)const;

	// ��û�����
	IPainter* GetPainter() const { return m_pPainter; }
	
	// ���ó�ʼ�ߴ�
	void SetDesignSize(int width, int height)
	{
        m_nWidth = width;
        m_nHeight = height;
	}

	void InnerSetLeft(int value);
	void InnerSetTop(int value);
	void InnerSetWidth(int value);
	void InnerSetHeight(int value);

	void InnerSetVAnchor(int value);
	void InnerSetHAnchor(int value);

	CImagePlayer& InnerGetBackImage() { return m_BackImage; }

	// ��������ͼ
	bool DrawBackImage(float seconds, int x1, int y1, int x2, int y2);

    virtual bool RefreshInnerAbsPos();

	// �Ƿ����Ϣ͸����ȱʡֵ
	void SetTransparentDefault(bool value) { m_bTransparentDefault = value; SetTransparent(value); }
	bool GetTransparentDefault() const { return m_bTransparentDefault; }

private:
    bool m_bRefreshInnerAbsPos;

protected:
    int m_nInnerAbsLeft;
    int m_nInnerAbsTop;

private:
	Gui* m_pGui;
	IPainter* m_pPainter;
	IGuiHook* m_pGuiHook;
	IControl* m_pParent;
	IControl* m_pDelegate;
	IEntity* m_pData;
	CColorData m_ForeColor;
	CColorData m_BackColor;
	CColorData m_LineColor;
	CColorData m_BlendColor;
	CColorData m_ShadowColor;
	core_string m_strName;
	unsigned int m_nHash;
	int m_nTop;
	int m_nLeft;
	int m_nWidth;
	int m_nHeight;
    int m_nAlpha;
	int m_nLoadMode;
	bool m_bDesignMode;
	bool m_bEnabled;
	bool m_bVisible;
	bool m_bCanFocus;
	bool m_bCanSelect;
	bool m_bFocused;
	bool m_bCapture;
    bool m_bLoadFinish;
    //�Ƿ���ö�enter����Ӧ
    bool m_bIsDisableEnter;
	int m_nVAnchor;
	int m_nHAnchor;
	core_wstring m_wsText;
    core_wstring m_wsShowText;
	core_string m_strFontName;
	core_string m_strCursorName;
	core_wstring m_wsHintText;
    core_wstring m_wsRealHintText;
	core_string m_strHintType;
    core_string m_strContext;
    core_string m_strDataSource;
    core_string m_strInSound;
    core_string m_strOutSound;
    core_string m_strClickSound;
	int m_nTabIndex;
	bool m_bTabStop;
	bool m_bNoFrame;
	bool m_bAutoSize;
    bool m_bTestTrans;
	int m_nDrawMode;
	CImagePlayer m_BackImage;
	TArrayPod<IControl*, 1, TCoreAlloc> m_Subors;
	//callback_container_t* m_pCallbacks;
	// �Ƿ�͸����ȱʡֵ
	bool m_bTransparentDefault;

	// �Ƿ�͸��
	bool m_bTransparent;
};

#endif // _I_CONTROL_H

