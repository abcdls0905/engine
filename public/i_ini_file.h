//--------------------------------------------------------------------
// �ļ���:		i_ini_file.h
// ��  ��:		
// ˵  ��:		
// ��������:	2006��1��30��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _PUBLIC_I_INI_FILE_H
#define _PUBLIC_I_INI_FILE_H

#include "macros.h"
#include "i_var_list.h"

// ��д�����ļ��ӿ�

class IIniFile
{
public:
	enum { READ_RAW_FILE = 1 };

public:
	virtual ~IIniFile() = 0;
	
	// �ͷ�
	virtual void Release() = 0;

	// �����ļ���
	virtual void SetFileName(const char* filename) = 0;
	// ����ļ���
	virtual const char* GetFileName() const = 0;
	// �����ļ�
	virtual bool LoadFromFile() = 0;
	// �����ַ���
	virtual bool LoadFromString(const char* str, size_t size) = 0;
	// �����ļ�
	virtual bool SaveToFile() const = 0;
	// ������ϣ����
	virtual bool CreateHashIndex() = 0;

	// ��Ӷ�
	virtual bool AddSection(const char* section) = 0;
	// ���Ҷ�
	virtual bool FindSection(const char* section) const = 0;
	// ���Ҷε�����ֵ
	virtual bool FindSectionIndex(const char* section, 
		size_t& index) const = 0;
	// ����ָ�����еļ�����ֵ
	virtual bool FindSectionItemIndex(size_t sect_index, const char* key, 
		size_t& item_index) const = 0;
	// ɾ����
	virtual bool DeleteSection(const char* section) = 0;
	// ��öε�����
	virtual size_t GetSectionCount() const = 0;
	// ���ָ������
	virtual const char* GetSectionByIndex(size_t index) const = 0;
	// ��ö����б�
	virtual size_t GetSectionList(IVarList& result) const = 0;
	// ���ָ�����µļ�����
	virtual size_t GetSectionItemCount(size_t sect_index) const = 0;
	// ���ָ�����µ�ָ������
	virtual const char* GetSectionItemKey(size_t sect_index, 
		size_t item_index) const = 0;
	// ���ָ�����µ�ָ����ֵ
	virtual const char* GetSectionItemValue(size_t sect_index, 
		size_t item_index) const = 0;

	// ���Ҷ��µļ�
	virtual bool FindItem(const char* section, const char* key) const = 0;
	// ɾ����
	virtual bool DeleteItem(const char* section, const char* key) = 0;
	// ��ö������м�������
	virtual size_t GetItemCount(const char* section) const = 0;
	// ��ö������м����б�
	virtual size_t GetItemList(const char* section, 
		IVarList& result) const = 0;
	// ��ö�������ָ������ֵ�б�
	virtual size_t GetItemValueList(const char* section, const char* key,
		IVarList& result) const = 0;

	// ��ȡ�����ݣ����������ڵ�ʱ�򷵻�ȱʡֵ(def)
	virtual int ReadInteger(const char* section, const char* key, 
		int def) const = 0;
	virtual char* ReadString(const char* section, const char* key, char* value, 
		size_t maxlen) const = 0;
	virtual const char* ReadString(const char* section, const char* key, 
		const char* def) const = 0;
	virtual float ReadFloat(const char* section, const char* key, 
		float def) const = 0;

	// ��ȡ�����ݣ����������ڵ�ʱ�򷵻�ȱʡֵ(def)
	virtual int ReadInteger(size_t sect_index, const char* key, 
		int def) const = 0;
	virtual char* ReadString(size_t sect_index, const char* key, char* value, 
		size_t maxlen) const = 0;
	virtual const char* ReadString(size_t sect_index, const char* key, 
		const char* def) const = 0;
	virtual float ReadFloat(size_t sect_index, const char* key, 
		float def) const = 0;
	
	// ��д�����ݣ�����λ������������ӣ�
	virtual bool WriteInteger(const char* section, const char* key, 
		int value) = 0;
	virtual bool WriteString(const char* section, const char* key, 
		const char* value) = 0;
	virtual bool WriteFloat(const char* section, const char* key, 
		float value) = 0;

	// ����п����ظ��ļ����ݣ�����β���������ӣ�
	virtual bool AddInteger(const char* section, const char* key, 
		int value) = 0;
	virtual bool AddString(const char* section, const char* key, 
		const char* value) = 0;
	virtual bool AddFloat(const char* section, const char* key, 
		float value) = 0;
};

inline IIniFile::~IIniFile() {}

#endif // _PUBLIC_I_INI_FILE_H
