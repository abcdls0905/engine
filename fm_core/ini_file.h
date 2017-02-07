//--------------------------------------------------------------------
// �ļ���:		ini_file.h
// ��  ��:		
// ˵  ��:		
// ��������:	2006��1��30��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _INI_FILE_H
#define _INI_FILE_H

#include "../public/macros.h"
#include "../public/i_ini_file.h"
#include "../public/fast_str.h"
#include "../public/core_mem.h"
#include "../utils/array_pod.h"

class TIniFileAlloc
{
public:
	TIniFileAlloc() {}
	~TIniFileAlloc() {}

	// �����ڴ�
	void* Alloc(size_t size) 
	{ 
		return CORE_ALLOC(size); 
	}
	// �ͷ��ڴ�
	void Free(void* ptr, size_t size) 
	{ 
		CORE_FREE(ptr, size); 
	}
	// ����
	void Swap(TIniFileAlloc& src) {}
};

// ��д�����ļ�

class CIniFile: public IIniFile
{
private:
	struct item_t
	{
		size_t nNamePos;
		unsigned int nHash;
		size_t nValuePos;
	};

	struct section_t
	{
		size_t nNamePos;
		unsigned int nHash;
		size_t nItemStart;
		size_t nItemCount;
		section_t* pHashNext;
	};

public:
	static CIniFile* NewInstance(const char* filename, int read_raw_file);

public:
	CIniFile();
	CIniFile(const char* filename, int read_raw_file);
	virtual ~CIniFile();
	
	// �ͷ�
	virtual void Release();

	// �����ļ���
	virtual void SetFileName(const char* filename);
	// ����ļ���
	virtual const char* GetFileName() const;
	// �����ļ�
	virtual bool LoadFromFile();
	// �����ַ���
	virtual bool LoadFromString(const char* str, size_t size);
	// �����ļ�
	virtual bool SaveToFile() const;
	// ������ϣ����
	virtual bool CreateHashIndex();

	// ��Ӷ�
	virtual bool AddSection(const char* section);
	// ���Ҷ�
	virtual bool FindSection(const char* section) const;
	// ���Ҷε�����ֵ
	virtual bool FindSectionIndex(const char* section, size_t& index) const;
	// ����ָ�����еļ�����ֵ
	virtual bool FindSectionItemIndex(size_t sect_index, const char* key, 
		size_t& item_index) const;
	// ɾ����
	virtual bool DeleteSection(const char* section);
	// ��öε�����
	virtual size_t GetSectionCount() const;
	// ���ָ������
	virtual const char* GetSectionByIndex(size_t index) const;
	// ��ö����б�
	virtual size_t GetSectionList(IVarList& result) const;
	// ���ָ�����µļ�����
	virtual size_t GetSectionItemCount(size_t sect_index) const;
	// ���ָ�����µ�ָ������
	virtual const char* GetSectionItemKey(size_t sect_index, 
		size_t item_index) const;
	// ���ָ�����µ�ָ����ֵ
	virtual const char* GetSectionItemValue(size_t sect_index, 
		size_t item_index) const;

	// ���Ҷ��µļ�
	virtual bool FindItem(const char* section, const char* key) const;
	// ɾ����
	virtual bool DeleteItem(const char* section, const char* key);
	// ��ö������м�������
	virtual size_t GetItemCount(const char* section) const;
	// ��ö������м����б�
	virtual size_t GetItemList(const char* section, IVarList& result) const;
	// ��ö�������ָ������ֵ�б�
	virtual size_t GetItemValueList(const char* section, const char* key,
		IVarList& result) const;

	// ��ȡ�����ݣ����������ڵ�ʱ�򷵻�ȱʡֵ(def)
	virtual int ReadInteger(const char* section, const char* key, 
		int def) const;
	virtual char* ReadString(const char* section, const char* key, 
		char* value, size_t maxlen) const;
	virtual const char* ReadString(const char* section, const char* key, 
		const char* def) const;
	virtual float ReadFloat(const char* section, const char* key, 
		float def) const;

	// ��ȡ�����ݣ����������ڵ�ʱ�򷵻�ȱʡֵ(def)
	virtual int ReadInteger(size_t sect_index, const char* key, 
		int def) const;
	virtual char* ReadString(size_t sect_index, const char* key, char* value, 
		size_t maxlen) const;
	virtual const char* ReadString(size_t sect_index, const char* key, 
		const char* def) const;
	virtual float ReadFloat(size_t sect_index, const char* key, 
		float def) const;

	// ��д�����ݣ�����λ������������ӣ�
	virtual bool WriteInteger(const char* section, const char* key, int value);
	virtual bool WriteString(const char* section, const char* key, 
		const char* value);
	virtual bool WriteFloat(const char* section, const char* key, float value);

	// ����п����ظ��ļ����ݣ�����β���������ӣ�
	virtual bool AddInteger(const char* section, const char* key, int value);
	virtual bool AddString(const char* section, const char* key, 
		const char* value);
	virtual bool AddFloat(const char* section, const char* key, float value);

private:
	bool Build(char* buffer, size_t buffer_size);
	size_t AddToBuffer(const char* val);
	bool SetData(const char* section, const char* key, const char* data);
	bool AddData(const char* section, const char* key, const char* data);
	const char* GetData(const char* section, const char* key) const;
	const char* GetData2(size_t sect_index, const char* key) const;
	const CIniFile::section_t* GetSection(const char* name) const;
	const CIniFile::item_t* GetItem(const section_t* pSection, 
		const char* name) const;

private:
	TIniFileAlloc m_Alloc;
	TFastStr<char, 16, TCharTraits<char>, TIniFileAlloc> m_strFileName;
	int m_nReadRawFile;
	char* m_pBuffer;
	size_t m_nLength;
	TArrayPod<section_t, 8, TIniFileAlloc> m_Sections;
	TArrayPod<item_t, 8, TIniFileAlloc> m_Items;
	section_t** m_pBuckets;
	size_t m_nBucketNum;
};

#endif // _INI_FILE_H
