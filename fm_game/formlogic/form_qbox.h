#include <vector>
#include "../fm_gui/designer.h"
#include "../fm_gui/form.h"
#include "../fm_gui/label.h"
#include "global.h"
#include "utility.h"

class form_qbox
{
	struct Item
	{
		PERSISTID form;
		PERSISTID btn_ok;
		PERSISTID btn_cancel;
	};

	void on_btn_sure_click(const char* event, const IVarList& arg)
	{
		
	}
	std::vector<Item> m_forms;
public:
	form_qbox()
	{
	
	}

	void Show(const wchar_t* text, EventDelegate* callback = NULL)
	{
		
	}
};