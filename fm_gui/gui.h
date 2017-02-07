//--------------------------------------------------------------------
// �ļ���:		gui.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��16��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _GUI_H
#define _GUI_H

#include "../public/module.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"
#include "../utils/string_pod.h"
#include "../utils/string_alias.h"
#include "../utils/pod_hash_map.h"
#include "name_list.h"
#include "gui_utils.h"
#include "../visual/i_gui_hook.h"
#include "../visual/i_event_manager.h"
// ������

#include "../utils/lock_util.h"
#include "../utils/thread.h"

class IControl;
class IGuiHook;
class IPainter;
class IDesignInfo;
class Font;
class Caret;
class Cursor;
class Designer;
class ImageAnimationManager;
class AnimationManger;

class Gui: public IEntity
{
private:
	struct sound_data_t
	{
		core_string strSoundFile;
	};

	typedef TStringPod<char, sound_data_t*, TStringTraits<char>, 
		TCoreAlloc> sound_container_t;
	typedef TStringPod<char, IDesignInfo*, TStringTraits<char>, 
		TCoreAlloc> des_info_container_t;

	int RunCallback(const char* event, const IVarList& args)
	{
		return event_send(GetID(), event, args);
	}

	static void WorkerProc(void* pParameter);
public:
	Gui();
	virtual ~Gui();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
	virtual bool MsgProc(unsigned int msg, size_t param1, size_t param2, 
		int& result);

	void Update(float seconds);
	void Realize();

	// �Ƿ���ʾ�ؼ�
	void SetVisible(bool value);
	bool GetVisible() const;

	// ȱʡ��Դ·��
	void SetDefaultPath(const char* value);
	const char* GetDefaultPath() const;

	// ��ð󶨴��ڵľ��
	//void* GetWinHandle() const;

	// ��ý�����Ⱦ������
	const char* GetPainterName() const;

	// ����ID��ÿؼ�
	IControl* GetControl(const PERSISTID& id) const;

	// ��ӱ����ؼ�
	bool AddBackControl(const PERSISTID& id);
	// �Ƴ������ؼ�
	bool RemoveBackControl(const PERSISTID& id);

	// ���ģʽ����
	bool AddModalForm(IControl* pControl);
	// �Ƴ�ģʽ����
	bool RemoveModalForm(IControl* pControl);
	// ��õ�ǰģʽ����
	IControl* GetModalForm() const;
	// ��õ�ǰģʽ���ڶ���
	PERSISTID GetModalFormID() const;

	// ��������ڶ���
	PERSISTID GetDesktopID() const;
	// ��������ڿؼ�
	IControl* GetDesktopForm() const;
	// ����������
	bool CreateDesktopForm();
	// ����ѡ����
	PERSISTID CreateSelector(const PERSISTID& parent_id);
	// ����ͼԪ�б�
	PERSISTID CreateImageList();

	// �����ؼ�
	IControl* CreateControl(const char* name);
	// ɾ���ؼ�
	bool DeleteControl(IControl* pControl);

	// ���ؼ��Ƿ�������ʾ
	bool IsControlVisible(IControl* pControl);

	// ����
	PERSISTID Create(const char* name);
	// ɾ��
	bool Delete(const PERSISTID& id);

	// �������뽹��
	void SetFocusControl(IControl* val);
	IControl* GetFocusControl() const;

	// ���ü������뽹��
	void SetFocused(const PERSISTID& id);
	// ��ü������뽹��
	PERSISTID GetFocused() const;

	// ��ȡ����Form��ʵ��
	void GetAllFormList(const IVarList& args, IVarList& result);

	// ��ô��ڼ�����
	PERSISTID GetLoader() const;
	// ��ô��������
	PERSISTID GetDesigner();
	// ��õ�ǰ֡��
	float GetFPS() const;

	// ��õ�ǰ֡��
	float GetAverageFPS() const;

	// ������������
	double GetPerDrawCallNum() const;

	// �������������
	double GetPerDrawTriangleNum() const;

	// �����ڿ��
	int GetWidth() const;
	// �����ڸ߶�
	int GetHeight() const;
	// ��ȡ�豸���
	int GetDeviceWidth() const;
	// ��ȡ�豸�߶�
	int GetDeviceHeight() const;

	// ���ƴ��ڲ���������������Ϣ
	//bool GenMouseHoverMessage(int delay_ms);

	// ������Դ
	bool FindSound(const char* name) const;
	bool AddSound(const char* name, const char* sound_file);
	bool GetSound(const char* name, sound_data_t& data) const;
	void ClearSound();
	bool PlayingSound(const char* name);
	const char* GetSoundFile(const char* name) const;

	// ����
	void ClearFont();
	PERSISTID NewFont();
	bool FindFont(const char* name) const;
	bool AddDefaultFont();
	bool AddFont(const PERSISTID& id);
	bool AddReferFont(const char* name, const char* refer_name);
	PERSISTID GetFont(const char* name) const;
	bool SetFont(const char* name, unsigned int shadow_color = 0);
	// ���
	void ClearCursor();
	PERSISTID NewCursor();
	bool FindCursor(const char* name) const;
	bool AddCursor(const PERSISTID& id);
	PERSISTID GetCursor(const char* name) const;
	// �������λ��
	bool SetCursorPos(int x, int y);
	// ���õ�ǰ���
	bool SetCurrentCursor(const char* name);

	// ������
	void ClearCaret();
	PERSISTID NewCaret();
	bool FindCaret(const char* name) const;
	bool AddCaret(const PERSISTID& id);
	PERSISTID GetCaret(const char* name) const;

	// ������ʾ
	void ClearHint();
	PERSISTID NewHint(const char* control_name);
	bool FindHint(const char* name) const;
	bool ShowHint(const PERSISTID& id) const;
	bool AddHint(const PERSISTID& id);
	PERSISTID GetHint(const char* name) const;

	// �����Ĳ˵�
	void ClearContext();
	PERSISTID NewContext(const char* control_name);
	bool FindContext(const char* name) const;
	bool AddContext(const PERSISTID& id);
	PERSISTID GetContext(const char* name) const;

	// ��ʾ����ʽ�˵�
	bool TrackPopupMenu(const PERSISTID& id, int x, int y);

	// ��������Ϣ
	IDesignInfo* GetDesignInfo(IControl* pControl);

	// ��õ�ǰ�������Ŀؼ�
	PERSISTID GetCaptureID() const;

	// ά����ǰ������Ŀؼ�������֧�ֶ�㴥��
	PERSISTID GetClickedID(size_t nTouchId) const;
	void AddClickedID(size_t nTouchId, const PERSISTID& clickedId);
	void RemoveClickedID(size_t nTouchId);

	//PERSISTID GetCoolManagerID() const;

	PERSISTID GetTextManagerID() const;

	// �Ƿ���ʾ֡��
	void SetShowFPS(bool value);
	bool GetShowFPS() const;

	// �Ƿ�����������
	void SetScaleEnable(bool value);
	bool GetScaleEnable() const;

	// �������ű���
	void SetScaleRatio(float value);
	float GetScaleRatio() const;

	// ���ܼ����
	void SetLogPerf(bool bValue);
	bool GetLogPerf() const;

	// ͨ���ı�������ת������
	bool ConvertText(const wchar_t* src, result_wstring& dst);

	// �Ƿ���ʾ��Ⱦ����Ϣ
	void SetShowRenderInfo(bool value);
	bool GetShowRenderInfo() const;

	// ���ֹͣʱ��ʾ������ʾ����ʱʱ��
	void SetHintDelay(float value);
	float GetHintDelay() const;

	// �Ƿ���������ʻ��д���
	void SetWordChangeLine(bool value);
	bool GetWordChangeLine() const;

	// ȱʡ�����ʽ
	void SetCursorName(const char* value);
	const char* GetCursorName() const;

	// ��������ƶ�
	void SetAbandomMouseMove(bool value);
	bool GetAbandomMouseMove() const;

	// ��������Խ���λ��
	bool GetCursorCoord(point_t& pos);
	// ���������3D�豸λ��
	bool GetCursor3DCoord(point_t & pos);
	// ��ȡ��ǰ���ͻ���λ��
	void GetCursorClientPos(const IVarList& args, IVarList& result);
	// ��ȡ��ǰ������ʱD3D�豸��ߵ�λ��
	void GetCursorPosition(const IVarList& args, IVarList& result);
	// ��ȡ��ǰ������ʱD3D�豸��ߵ�λ��
	void GetCursor3DPosition(const IVarList& args, IVarList& result);
	//// ���������ӿؼ���λ��
	//void GetCursorPositionInChildControl(const IVarList& args, 
	//	IVarList& result);
	//bool GetCursorCoordInChildControl(point_t& pos);
	// �������������ͼ
	bool DumpAllFontTex();
	// ������꽹��
	bool ClearCapture();

	// �����ӵ���ʾ��Ϣ
	//bool ClearRenderInfo();
	// �����ʾ��Ϣ
	//void AddRenderInfo(const IVarList& args, IVarList& result);
	// �����Ⱦ�����ͳ����Ϣ
	//void GetPrimitiveInfo(const IVarList& args, IVarList& result);

	unsigned int GetCodePage()
	{
		return m_CurCP;
	}

	// �Ƿ����ռ�ð�������
	bool GuiHasKeyMsg();

	// ���ý���ص�
	void SetGuiHook(IGuiHook* value);


	// ��ȡ����������
	PERSISTID GetAnimationManagerIDNew() const;

	//��ȡ����������
	ImageAnimationManager* GetAnimationManager() const;


	// Ƕ��3D�������� add by zhangbj 
	bool FindScene(const PERSISTID& id);
	bool AddScene(const PERSISTID& id);
	bool RemoveScene(const PERSISTID& id);
	// end

	//����ָ���ؼ�����Ϣ
	bool MsgLButtonDownToControl(IControl* pControl, size_t param1, size_t param2, int& result);
	bool MsgMouseMoveToControl(IControl* pControl, size_t param1, size_t param2, int& result);
	bool MsgLButtonUpToControl(IControl* pControl, size_t param1, size_t param2, int& result);
	bool MsgKeyDownToControl(IControl* pControl, size_t param1, size_t param2, int& result);


private:
	bool FindFontIndex(const char* name, size_t& index) const;
	bool FindCaretIndex(const char* name, size_t& index) const;
	bool FindCursorIndex(const char* name, size_t& index) const;
	bool FindHintIndex(const char* name, size_t& index) const;
	bool FindContextIndex(const char* name, size_t& index) const;

	// ��ƿؼ�
	bool DesignControl(IControl* control, int x, int y);

	int OnDesignEntry(IControl* pControl);
	int OnDesignClick(IControl* pControl, int x, int y);
	int OnShowHint(int x, int y);
	int OnShowContext(IControl* pControl, int x, int y);

	// ������߶�
	int GetCursorHeight() const;
	// ��ʾ�ؼ�������ʾ
	bool ShowControlHint(IControl* pControl, int x, int y);
	// ��λ������ʾ״̬
	bool ResetHintStatus(int x, int y);

	// ��ʾ�ؼ������Ĳ˵�
	bool ShowControlContext(IControl* pControl, int x, int y);
	// ��λ�����Ĳ˵�״̬
	bool ResetContextStatus(int x, int y);
	// ��λ���˵�״̬
	bool ResetMainMenuStatus(IControl* capture);

	// ��ʾ����ʽ�˵�
	bool ShowPopupMenu(IControl* pMenu, int x, int y);
	// ���ص���ʽ�˵�
	bool HidePopupMenu();

	// �ı����뽹��ؼ�
	bool ChangeFocus(IControl* get_control, IControl* lost_control);
	// �ı䲶�����ؼ�
	bool ChangeCapture(IControl* get_control, IControl* lost_control);

	Caret* InnerGetCaret() const;
	Cursor* InnerGetCursor() const;
	//GameHand* InnerGetGameHand() const;

	//
	bool InnerCreateCursor(const char* name, const char* image);

	//
	AnimationManger* InnerGetAnimationManger() const;

	// ������Unicode�ַ����͵�����ؼ�
	bool SendCharToControl(wchar_t ch);

	//����GUIϵͳ����Ϣ
	bool MsgMouseMove(void* hwnd, size_t param1, size_t param2, int& result);

	bool MsgLButtonDown(void* hwnd, size_t param1, size_t param2, int& result);

	bool MsgLButtonUp(void* hwnd, size_t param1, size_t param2, int& result);

	bool MsgKeyDown(void* hwnd, size_t param1, size_t param2, int& result);

	bool MsgKeyUp(void* hwnd, size_t param1, size_t param2, int& result);

	bool MsgChar(void* hwnd, size_t param1, size_t param2, int& result);


	//   // ��ȡ��ǰ���뷨IME����
	//   bool LoadCurIMESetting();
	//	// ���úͽ���IME
	//	void EnableIME(HWND hwnd);
	//	void DisableIME(HWND hwnd);
	//	// ��ȡ��ǰ����ؼ��Ƿ���IME����ؼ�
	//	IControl * GetIMEInputControl(IControl* pControl);
	//	// ������Ϣ����
	//	bool MsgMouseMove(void* hwnd, size_t param1, size_t param2, int& result);
	//	bool MsgSetCursor(void* hwnd, size_t param1, size_t param2, int& result);
	//	bool MsgLButtonDown(void* hwnd, size_t param1, size_t param2, int& result);
	//	bool MsgLButtonUp(void* hwnd, size_t param1, size_t param2, int& result);
	//	bool MsgLButtonDblclk(void* hwnd, size_t param1, size_t param2, 
	//		int& result);
	//	bool MsgRButtonDown(void* hwnd, size_t param1, size_t param2, int& result);
	//	bool MsgRButtonUp(void* hwnd, size_t param1, size_t param2, int& result);
	//	bool MsgRButtonDblclk(void* hwnd, size_t param1, size_t param2, 
	//		int& result);
	//	bool MsgMouseWheel(void* hwnd, size_t param1, size_t param2, int& result);
	//	bool MsgMouseHover(void* hwnd, size_t param1, size_t param2, int& result);
	//	bool MsgSysKeyDown(void* hwnd, size_t param1, size_t param2, int& result);
	//    bool MsgInputLanguage(void* hwnd, size_t param1, size_t param2, 
	//        int& result);
	//    bool MsgIMECompositon(void* hwnd, size_t param1, size_t param2, 
	//       int& result);
	//	bool MsgKillFocus(void* hwnd, size_t param1, size_t param2, int& result);
	//	bool MsgSize(void* hwnd, size_t param1, size_t param2, int& result);
	//	bool MsgSizing(void* hwnd, size_t param1, size_t param2, int& result);
	//	bool MsgMove(void* hwnd, size_t param1, size_t param2, int& result);
	//	bool MsgMoving(void* hwnd, size_t param1, size_t param2, int& result);
	//	bool MsgClose(void* hwnd, size_t param1, size_t param2, int& result);
	//	bool MsgActivate(void* hwnd, size_t param1, size_t param2, int& result);

private:

	bool EnableLogPerf();
	void DiscardLogPerf();

	int m_nMouseClientX;
	int m_nMouseClientY;

	core_string m_strPainterName;
	core_string m_strCursorName;

	IPainter* m_pPainter;
	IGuiHook* m_pGuiHook;
	IControl* m_pDesktop;
	Designer* m_pLoader;
	Designer* m_pDesigner;

	bool m_bFocusCanInput;
	//bool m_bLastInputOpenState;
	PERSISTID m_FocusID;            // ���뽹��
	PERSISTID m_CaptureID;          // ��꽹��
	TPodHashMap<int, PERSISTID, TPodTraits<int>, TCoreAlloc> m_ClickedCotrols; // ����������ѡ�ж�������֧�ֶ�㴥��

	unsigned int m_nTickCount;
	int m_nFrameCount;
	float m_fTimeCount;
	float m_fFPS;
	int m_nCountFPS;
	float m_fAverageFPS;
	double m_dPerDrawCallNum;
	double m_dPerDrawTriangleNum;

	double m_dEffectModelDrawCallMaxNumPreFrame;
	double m_dEffectParticleDrawCallMaxNumPreFrame;
	double m_dGuiDrawCallMaxNumPreFrame;
	double m_dMaxValTimeCount;

	bool m_bVisible;
	bool m_bShowFPS;
	bool m_bShowRenderInfo;
	bool m_bWordChangeLine;
	bool m_bAbandomMouseMove;
	//CNameList m_RenderInfo;

	bool m_bLogPerf;

	float m_fHintDelay;
	float m_fHintCount;

	//HKL m_hkl;                  // ��ǰ���̲���
	//DWORD m_ImeProp;            // ��ǰ���뷨����
	//CHARSETINFO m_csInfo;       // ��ǰ�ַ�����Ϣ
	unsigned int m_CurCP;       // ��ǰ����ҳ

	PERSISTID m_HintID;
	PERSISTID m_ContextID;
	PERSISTID m_CursorID;
	PERSISTID m_CaretID;
	PERSISTID m_PopupMenuID;
	PERSISTID m_TextManager;
	PERSISTID m_AnimMngID;
	//PERSISTID m_CoolManagerID;

	sound_container_t m_Sounds;
	des_info_container_t m_DesInfo;

	TStringPod<char, int, TStringTraits<char>, TCoreAlloc> m_Colors;
	TStringPod<char, size_t, TStringTraits<char>, TCoreAlloc> m_FontIndex;

	TStringAlias<char, char, TStringTraits<char>, TStringTraits<char>, TCoreAlloc> m_MapReferFont;

	TArrayPod<Font*, 1, TCoreAlloc> m_Fonts;
	TArrayPod<PERSISTID, 1, TCoreAlloc> m_BackControls;
	TArrayPod<PERSISTID, 1, TCoreAlloc> m_ModalFormStack;
	TArrayPod<Cursor*, 1, TCoreAlloc> m_Cursors;
	TArrayPod<Caret*, 1, TCoreAlloc> m_Carets;
	TArrayPod<IControl*, 1, TCoreAlloc> m_Hints;
	TArrayPod<IControl*, 1, TCoreAlloc> m_Contexts;

	TArrayPod<PERSISTID, 1, TCoreAlloc> m_Scenes; // add by zhangbj

	CThread* m_pThread;
	CThreadWaiter* m_pThreadWaiter;
	static FILE* s_fp_GuiLogPerf;
	PERSISTID m_ClickedID;
};

#endif // _GUI_H
