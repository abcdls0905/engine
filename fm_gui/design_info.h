//--------------------------------------------------------------------
// �ļ���:		design_info.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��15��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _DESIGN_INFO_H
#define _DESIGN_INFO_H

#include "../public/macros.h"
#include "../public/core_type.h"
#include "../utils/array_pod.h"
#include "i_design_info.h"

// �������Ϣ

class CDesignInfo: public IDesignInfo
{
private:
	struct prop_info_t
	{
		core_string strName;
		unsigned int nHash;
		int nType;
		bool bSave;
		bool bSuborSave;
		CVar DefValue;
	};

	struct subor_info_t
	{
		core_string strName;
		unsigned int nHash;
		core_string strType;
	};

	struct event_info_t
	{
		core_string strName;
		unsigned int nHash;
	};

public:
	CDesignInfo();
	virtual ~CDesignInfo();

	// ����������Ϣ
	virtual bool FindPropInfo(const char* name) const;
	// ���������Ϣ
	virtual bool AddPropInfo(const char* name, int type, 
		bool save, bool child_save, const IVar* def_value = NULL);
	// ɾ��������Ϣ
	virtual bool RemovePropInfo(const char* name);
	// �����������
	virtual size_t GetPropCount() const;
	// ������������б�
	virtual size_t GetPropList(IVarList& result) const;
	// ���������
	virtual const char* GetPropName(size_t index) const;
	// �����������
	virtual int GetPropType(const char* name) const;
	// ��������Ƿ񱣴�
	virtual bool GetPropSave(const char* name) const;
	// �����Ϊ�����ؼ�ʱ�����Ƿ񱣴�
	virtual bool GetPropSuborSave(const char* name) const; 
	// ������Ե�ȱʡֵ�������ǰֵ��ȱʡֵ��ͬ����Ҫ���棩
	virtual bool GetPropDefValue(const char* name, 
		IVar& def_value) const;
	// ����������͵��ַ�����ʾ
	virtual const char* GetPropTypeString(int type) const;
	
	// ���Ҹ����ؼ���Ϣ
	virtual bool FindSuborInfo(const char* name) const;
	// ��Ӹ����ؼ���Ϣ
	virtual bool AddSuborInfo(const char* name, const char* type);
	// ɾ�������ؼ���Ϣ
	virtual bool RemoveSuborInfo(const char* name);
	// ��ø����ؼ�����
	virtual size_t GetSuborCount() const;
	// ��ø����ؼ������б�
	virtual size_t GetSuborList(IVarList& result) const;
	// ��ø����ؼ�ʵ������
	virtual const char* GetSuborType(const char* name) const;
	
	// �����¼���Ϣ
	virtual bool FindEventInfo(const char* name) const;
	// ����¼���Ϣ
	virtual bool AddEventInfo(const char* name);
	// ɾ���¼���Ϣ
	virtual bool RemoveEventInfo(const char* name);
	// ����¼�����
	virtual size_t GetEventCount() const;
	// ����¼������б�
	virtual size_t GetEventList(IVarList& result) const;

private:
	CDesignInfo(const CDesignInfo&);
	CDesignInfo& operator=(const CDesignInfo&);

	bool FindPropInfoIndex(const char* name, size_t& index) const;
	bool FindSuborInfoIndex(const char* name, size_t& index) const;
	bool FindEventInfoIndex(const char* name, size_t& index) const;
	
private:
	TArrayPod<prop_info_t*, 1, TCoreAlloc> m_PropInfo;
	TArrayPod<subor_info_t*, 1, TCoreAlloc> m_SuborInfo;
	TArrayPod<event_info_t*, 1, TCoreAlloc> m_EventInfo;
};

#endif // _DESIGNINFO_H

