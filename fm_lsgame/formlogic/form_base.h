

#pragma once
#include "../public/persistid.h"
#include "../public/i_entity.h"
#include "../public/core_log.h"
#include "../util/macro_util.h"
#include "../define/logdefine.h"

#include <string>
#include <map>
#include <vector>
//
class Form;
class IVarList;
class IControl;

//
class FormBase : public IEntity
{
public:
	FormBase(/*const char*szPathFile, bool isCenterAlign = true*/);
	virtual ~FormBase();

	virtual bool	Init(const IVarList& args){return true;}
	virtual bool	Shut();

	void			Load(bool bStatic=false, bool bAsyncLoad = false, bool bCenter = false);
	void			Close();

	void			Show();
	void			ShowModal();
	void			Hide();

	bool			IsShow();

	void			LoadAndHide(bool bStatic=false, bool bShowModal=false, bool bAsyncLoad = false, bool bCenter = false);

protected:
	//FormBase();
	FormBase(const FormBase&);
	FormBase& operator = (const FormBase&);

protected:
	virtual void	OnCreate(){};
	virtual void	OnClosed(){};
	virtual void	OnShow(bool bShow=true);
	virtual void	OnHide();
	virtual void	Maximize(); 
	Form*			GetMainForm();
	IControl*		GetControl(const char* szPathName);

private:
	//
	bool	Create(bool bStatic = false, bool bAsyncLoad = false, bool bCenter = false);

protected:
	PERSISTID	m_formID;
	std::string	m_strFormFile;
	//bool		m_bCenterAlign;
};