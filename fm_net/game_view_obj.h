//--------------------------------------------------------------------
// �ļ���:		game_view_obj.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��2��18��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _GAME_VIEW_OBJ_H
#define _GAME_VIEW_OBJ_H

#include "game_object.h"

// �����Ӵ��еĶ���

class GameViewObj: public GameObject
{
public:
	GameViewObj();
	virtual ~GameViewObj();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();
};

#endif // _GAME_VIEW_OBJ_H

