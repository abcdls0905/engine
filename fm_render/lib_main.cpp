//--------------------------------------------------------------------
// �ļ���:		lib_main.cpp
// ��  ��:		
// ˵  ��:		
// ��������:	2013��2��17��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
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
