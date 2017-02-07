//--------------------------------------------------------------------
// 文件名:		gui.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "gui.h"
#include "gui_input.h"
#include "i_control.h"
#include "label.h"
#include "form.h"
#include "font.h"
#include "caret.h"
#include "cursor.h"
#include "designer.h"
#include "design_info.h"
#include "../visual/i_gui_hook.h"
#include "../public/auto_mem.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/inlines.h"
#include "../public/converts.h"
#include "../public/portable.h"
#include "text.h"


#include "../public/module.h"




//
#define REG_ENTITY(p) IEntCreator& p##_EntCreator();LinkEntCreator(p##_EntCreator());

//
void lib_main_gui()
{
	REG_ENTITY(Text);

	REG_ENTITY(IContainer);
	REG_ENTITY(IControl);
	REG_ENTITY(IScrollable);

	REG_ENTITY(Gui);
	REG_ENTITY(Adjuster);
	REG_ENTITY(Balloon);
	REG_ENTITY(BalloonSet);
	REG_ENTITY(Button);
	REG_ENTITY(Caret);
	REG_ENTITY(CheckButton);
	REG_ENTITY(ComboBox);
	REG_ENTITY(ControlBox);
	REG_ENTITY(Cursor);
	REG_ENTITY(Designer);
	REG_ENTITY(Edit);
	REG_ENTITY(FloatEdit);
	REG_ENTITY(Font);
	REG_ENTITY(Form);
	REG_ENTITY(Grid);
	REG_ENTITY(GroupBox);
	REG_ENTITY(GroupScrollableBox);

	REG_ENTITY(ImageList);
	REG_ENTITY(Label);
	REG_ENTITY(ListBox);
	REG_ENTITY(MainMenu);
	REG_ENTITY(Menu);
	REG_ENTITY(MenuItem);
	REG_ENTITY(Picture);
	REG_ENTITY(ProgressBar);
	REG_ENTITY(RadioButton);
	REG_ENTITY(RoundBox);
	REG_ENTITY(ScrollBar);
	REG_ENTITY(Selector);
	REG_ENTITY(Splitter);
	REG_ENTITY(TextBox);
	REG_ENTITY(TrackBar);
	REG_ENTITY(RichEdit);
	REG_ENTITY(RichInputBox);

	REG_ENTITY(TrackRect);
	REG_ENTITY(TreeNode);
	REG_ENTITY(TreeView);
	REG_ENTITY(MultiTextBox);
	REG_ENTITY(ImageAnimationManager);
	REG_ENTITY(HyperLinkStyleManager);
	REG_ENTITY(CoolManager);
	REG_ENTITY(ImageGrid);
	REG_ENTITY(SceneBox);
}

