//--------------------------------------------------------------------
// 文件名:		lib_main.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2013年2月17日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "../public/module.h"

IIntCreator& Render_IntCreator();
IIntCreator& Painter_IntCreator();
IEntCreator& IVisBase_EntCreator();
IEntCreator& DeviceCaps_EntCreator();

#define REG_ENTITY(p) IEntCreator& p##_EntCreator();LinkEntCreator(p##_EntCreator());

void lib_main_render()
{
	LinkIntCreator(Render_IntCreator());
	LinkIntCreator(Painter_IntCreator());
	LinkEntCreator(IVisBase_EntCreator());
	LinkEntCreator(DeviceCaps_EntCreator());
	REG_ENTITY(TextureCache);
}
