//--------------------------------------------------------------------
// �ļ���:		design_saver.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��3��16��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _DESIGN_SAVER_H
#define _DESIGN_SAVER_H

#include "../public/module.h"

// ������Ʊ���

class IControl;
class Designer;
class TiXmlDocument;
class TiXmlElement;

class CDesignSaver
{
public:
	CDesignSaver();
	~CDesignSaver();

	void SetDesigner(Designer* value);
	
	// ���洰�ڵ�XML�����ļ�
	bool SaveForm(IControl* control, const char* path, const char* file, 
		const char* resource);
	// ������Դ�����ļ�
	bool SaveResource(const char* path, const char* file);

private:
	CDesignSaver(const CDesignSaver&);
	CDesignSaver& operator=(const CDesignSaver&);
	
	ICore* GetCore() const;
	
	// ���渽���ؼ�
	bool SaveXmlSubor(IControl* parent, IControl* control, 
		const char* name, const char* file, TiXmlElement* root, bool is_utf8);
	// ����ؼ�
	bool SaveXmlControl(IControl* control, const char* file, 
		TiXmlElement* root, bool is_utf8);
	// ����ؼ�����
	bool SaveXmlProperty(IControl* control, const char* name, 
		TiXmlElement* root, bool is_utf8);

private:
	Designer* m_pDesigner;
};

#endif // _DESIGN_SAVER_H

