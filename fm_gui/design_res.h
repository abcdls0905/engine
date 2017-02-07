//--------------------------------------------------------------------
// �ļ���:		design_res.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��21��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _DESIGN_RES_H
#define _DESIGN_RES_H

#include "../public/macros.h"
#include "../public/i_var_list.h"
#include "../public/core_mem.h"
#include "../utils/string_pod.h"

// ���������Դ

class CDesignRes
{
public:
	enum RESOURCE_TYPES
	{
		//RESTYPE_COLOR,		// ��ɫ
		//RESTYPE_IMAGE,		// ͼԪ
		RESTYPE_FONT,		// ����
		RESTYPE_CURSOR,		// �����״
        RESTYPE_SOUND,      // ������Ч
		RESTYPE_MAX,
	};

private:
	typedef TStringPod<char, int, TStringTraitsCI<char>, 
		TCoreAlloc> collect_container_t;
	typedef TStringPod<char, int, TStringTraitsCI<char>, 
		TCoreAlloc> name_container_t;

public:
	CDesignRes();
	~CDesignRes();

	// ��Դ�����ļ�
	bool FindCollect(const char* file) const;
	bool AddCollect(const char* file);
	bool ClearCollect();

	// �����Դ�ļ�����
	size_t GetResFileCount(size_t type) const;
	// �����Դ�ļ���
	void GetResFileList(size_t type, IVarList& result);
	// ������Դ�ļ�
	bool FindResFile(size_t type, const char* file) const;
	// �����Դ�ļ�
	bool AddResFile(size_t type, const char* file);
	// ɾ����Դ�ļ�����Դ�ļ������ݲ��ᱻ�Ƴ���
	bool RemoveResFile(size_t type, const char* file);
	// �����Դ�ļ�
	bool ClearResFile(size_t type);

	// ��ö�Ӧ������
	size_t GetResType(const char* res_name) const;
	// �����������
	const char* GetResName(size_t type) const;

private:
	CDesignRes(const CDesignRes&);
	CDesignRes& operator=(const CDesignRes&);

private:
	collect_container_t m_Collects;
	name_container_t m_Resources[RESTYPE_MAX];
};

#endif // _DESIGN_RES_H

