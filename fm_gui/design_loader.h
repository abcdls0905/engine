//--------------------------------------------------------------------
// �ļ���:		design_loader.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��16��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _DESIGN_LOADER_H
#define _DESIGN_LOADER_H

#include "../public/module.h"
#include "../utils/string_pod.h"
#include <map>
#include <vector>

// ������Ƽ���
class Designer;

class CDesignLoader
{
public:
	CDesignLoader();
	~CDesignLoader();

	void SetDesigner(Designer* value);
	// ���������������ļ�
	bool LoadDesktop(const char* path, const char* file, bool async_load);
	// ���ش��������ļ�
	PERSISTID LoadForm(const char* path, const char* file, bool async_load, bool bStatic=false);
	// ����ֻ������Դ�������ļ�
	bool LoadResource(const char* path, const char* file);
	// ����ָ�����͵���Դ�����ļ�
	bool LoadXmlResFile(int type, const char* path, const char* file);

private:
	CDesignLoader(const CDesignLoader&);
	CDesignLoader& operator=(const CDesignLoader&);

	bool LoadXmlRes(int type, const char* path, const char* file);
	

private:
	Designer* m_pDesigner;

	//
	TStringPod<char, size_t> m_mapIniFile;
	std::vector<char*> m_vecFileInf;
};

#endif // _DESIGN_LOADER_H

