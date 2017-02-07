//--------------------------------------------------------------------
// �ļ���:		i_design_info.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��15��	
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _I_DESIGN_INFO_H
#define _I_DESIGN_INFO_H

#include "../public/macros.h"
#include "../public/i_var.h"
#include "../public/i_var_list.h"

// �����Ϣ�ӿ�

class IDesignInfo
{
public:
	// �ɱ༭��������
	enum PROPINFO_TYPES
	{
		TYPE_UNKNOWN,
		TYPE_BOOL,
		TYPE_INT,
		TYPE_INT64,
		TYPE_FLOAT,
		TYPE_DOUBLE,
		TYPE_STRING,
		TYPE_WIDESTR,
		TYPE_FONT,		// ������Դ
		TYPE_COLOR,		// ��ɫ��Դ
		TYPE_IMAGE,		// ͼԪ	
		TYPE_CURSOR,	// ������
        TYPE_SOUND,     // ������Ч
		TYPE_FILE,		// �ļ�
	};

public:
	virtual ~IDesignInfo() = 0;

	// ����������Ϣ
	virtual bool FindPropInfo(const char* name) const = 0;
	// ���������Ϣ
	virtual bool AddPropInfo(const char* name, int type, 
		bool save, bool child_save, const IVar* def_value = NULL) = 0;
	// ɾ��������Ϣ
	virtual bool RemovePropInfo(const char* name) = 0;
	// �����������
	virtual size_t GetPropCount() const = 0;
	// ������������б�
	virtual size_t GetPropList(IVarList& result) const = 0;
	// ���������
	virtual const char* GetPropName(size_t index) const = 0;
	// �����������
	virtual int GetPropType(const char* name) const = 0;
	// ��������Ƿ񱣴�
	virtual bool GetPropSave(const char* name) const = 0;
	// �����Ϊ�����ؼ�ʱ�����Ƿ񱣴�
	virtual bool GetPropSuborSave(const char* name) const = 0; 
	// ������Ե�ȱʡֵ�������ǰֵ��ȱʡֵ��ͬ����Ҫ���棩
	virtual bool GetPropDefValue(const char* name, 
		IVar& def_value) const = 0;
	// ����������͵��ַ�����ʾ
	virtual const char* GetPropTypeString(int type) const = 0;

	// ���Ҹ����ؼ���Ϣ
	virtual bool FindSuborInfo(const char* name) const = 0;
	// ��Ӹ����ؼ���Ϣ
	virtual bool AddSuborInfo(const char* name, const char* type) = 0;
	// ɾ�������ؼ���Ϣ
	virtual bool RemoveSuborInfo(const char* name) = 0;
	// ��ø����ؼ�����
	virtual size_t GetSuborCount() const = 0;
	// ��ø����ؼ������б�
	virtual size_t GetSuborList(IVarList& result) const = 0;
	// ��ø����ؼ�ʵ������
	virtual const char* GetSuborType(const char* name) const = 0;

	// �����¼���Ϣ
	virtual bool FindEventInfo(const char* name) const = 0;
	// ����¼���Ϣ
	virtual bool AddEventInfo(const char* name) = 0;
	// ɾ���¼���Ϣ
	virtual bool RemoveEventInfo(const char* name) = 0;
	// ����¼�����
	virtual size_t GetEventCount() const = 0;
	// ����¼������б�
	virtual size_t GetEventList(IVarList& result) const = 0;
};

inline IDesignInfo::~IDesignInfo() {}

#endif // _I_DESIGN_INFO_H
