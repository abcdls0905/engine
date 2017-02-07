//--------------------------------------------------------------------
// �ļ���:		designer.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��16��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _DESIGNER_H
#define _DESIGNER_H

#include "../public/module.h"
#include "../public/core_type.h"
#include "../utils/string_pod.h"

// ���������

class Gui;
class IPainter;
class IControl;
class IDesignInfo;
class CDesignLoader;
class CDesignSaver;
class CDesignRes;

class Designer: public IEntity
{
public:
	Designer();
	virtual ~Designer();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	void SetGui(Gui* pGui);
	Gui* GetGui() const;

	void SetPainter(IPainter* pPainter);
	IPainter* GetPainter() const { return m_pPainter; }

	// ���ȱʡ��Դ·��
	const char* GetDefaultPath() const;

	// �ɱ༭ģʽ
	void SetDesignMode(bool value);
	bool GetDesignMode() const;
	
	// ��ǰ�༭���ڣ�Form��
	void SetDesignFormID(const PERSISTID& value);
	PERSISTID GetDesignFormID() const;

	// ��Դ�ļ����뷽ʽ
	void SetEncoding(const char* value);
	const char* GetEncoding() const;
	
	// ȱʡ����Դ�����ļ���
	void SetDefResourceName(const char* value);
	const char* GetDefResourceName() const;
	// �����Դ�ļ�����
	int GetResFileCount(const char* res_name) const;
	// �����Դ�ļ���
	void GetResFileList(const IVarList& args, IVarList& result) const;
	// ������Դ�ļ�
	bool FindResFile(const char* res_name, const char* file) const;
	// �����Դ�ļ�
	bool AddResFile(const char* res_name, const char* file);
	// ɾ����Դ�ļ�����Դ�ļ������ݲ��ᱻ�Ƴ���
	bool RemoveResFile(const char* res_name, const char* file);
	// �����Դ�ļ�
	bool ClearResFile(const char* res_name);

	// �ֽ���ɫԪ��
	int GetColorAlpha(const char* color) const;
	int GetColorRed(const char* color) const;
	int GetColorGreen(const char* color) const;
	int GetColorBlue(const char* color) const;

	// �ֽ�ͼԪ����
	//void GetImageInfo(const IVarList& args, IVarList& result) const;

	// �����ͼ�ĳߴ�
	int GetTexWidth(const char* tex) const;
	int GetTexHeight(const char* tex) const;
	
	// ���������������ļ�
	bool LoadDesktop(const char* path, const char* file);
	// �첽���������������ļ�
	bool LoadDesktopAsync(const char* path, const char* file);
	// ���ش��������ļ�
	PERSISTID LoadForm(const char* path, const char* file, bool bStatic=false);
	// ���ش��������ļ�
	PERSISTID LoadFormAsync(const char* path, const char* file, bool bStatic=false);
	// ������Դ�����ļ�
	bool LoadResource(const char* path, const char* file);

	// ���洰�ڵ������ļ�
	bool SaveForm(const char* path, const char* file, const char* resource);
	// ������Դ�����ļ�
	bool SaveResource(const char* path, const char* file);

	// ������д����ؼ�
	PERSISTID Create(const char* name);
	// ������и��ƿؼ�
	PERSISTID Clone(const PERSISTID& id);
	// �������ɾ���ؼ�
	bool Delete(const PERSISTID& id);

	// ��ÿؼ��Ŀɱ༭��������
	int GetPropCount(const PERSISTID& id) const;
	// ��ÿɱ༭��������
	void GetPropList(const IVarList& args, IVarList& result) const;
	// ��ÿɱ༭��������
	result_string GetPropType(const PERSISTID& id, const char* name) const;
	// ��ÿɱ༭���Ե�ǰֵ
	result_string GetPropValue(const PERSISTID& id, const char* name) const;
	// ���ÿɱ༭���Ե�ǰֵ
	bool SetPropValue(const PERSISTID& id, 
		const char* name, const char* value);
	// ��ÿɱ༭�����Ƿ񱣴�
	bool GetPropSave(const PERSISTID& id, const char* name) const;
	// ��ÿɱ༭�����ڸ����ؼ����Ƿ񱣴�
	bool GetPropSuborSave(const PERSISTID& id, const char* name) const;

	// ��ø����ؼ�����
	int GetSuborCount(const PERSISTID& id) const;
	// ��ø����ؼ�����
	void GetSuborList(const IVarList& args, IVarList& result) const;
	// ��ø����ؼ�����
	result_string GetSuborType(const PERSISTID& id, const char* name) const;
	// ��ø����ؼ�����
	PERSISTID GetSubor(const PERSISTID& id, const char* name) const;

	// ����¼�����
	int GetEventCount(const PERSISTID& id) const;
	// ����¼�����
	void GetEventList(const IVarList& args, IVarList& result) const;

	// ��������еı��༭�ؼ�����
	int GetControlCount(const PERSISTID& id);
	// ��������еı��༭�ؼ��б�
	void GetControlList(const IVarList& args, IVarList& result) const;
	
//	// �����¼��Ļص�����
//	bool FindCallback(const PERSISTID& id, const char* name) const;
//	// ��ӻص�����
//	bool AddCallback(const PERSISTID& id, 
//		const char* name, const char* func);
//	// ɾ���ص�����
//	bool RemoveCallback(const PERSISTID& id, const char* name);
//	// ������лص�����
//	bool ClearCallback(const PERSISTID& id);
//	// ��ûص���������
//	int GetCallbackCount(const PERSISTID& id) const;
//	// ��ûص��¼���
//	void GetCallbackList(const IVarList& args, IVarList& result) const;
//	// ��ûص�������
//	result_string GetCallbackFunc(const PERSISTID& id, const char* name) const;
	
	// �ӿؼ����ּ���
	bool FindMember(const char* name) const;
	bool AddMember(const char* name);
	bool RemoveMember(const char* name);
	bool ClearMember();
	int GetMemberCount() const;
	void GetMemberList(const IVarList& args, IVarList& result);

	// ��Դ�ļ�
	CDesignRes* GetDesignRes() const;
	// ���������
	IControl* GetDesktopForm() const;
	// ���ID��Ӧ�ؼ�
	IControl* GetControl(const PERSISTID& id) const;
	// ��ñ��༭����
	IControl* GetDesignForm() const;
	// ��������Ϣ
	IDesignInfo* GetDesignInfo(IControl* control) const;
	// �����ؼ�
	IControl* CreateControl(const char* name);
	// ���ƿؼ�
	IControl* CloneControl(IControl* proto);
	// ��ø����ؼ�
	IControl* GetSuborControl(IControl* control, const char* name) const;
	
	// ���ÿؼ��ڲ�����
	bool SetControlProperty(IControl* control, const char* name, 
		const char* value, bool is_utf8);

private:
	Gui* m_pGui;
	IPainter* m_pPainter;
	CDesignLoader* m_pLoader;
	CDesignSaver* m_pSaver;
	CDesignRes* m_pDesignRes;
	bool m_bDesignMode;
	PERSISTID m_DesignFormID;
	core_string m_strEncoding;
	core_string m_strDefResourceName;
	TStringPod<char, int, TStringTraits<char>, TCoreAlloc> m_Members;
};

#endif // _DESIGNER_H

