//��Ϸ���ģ��
#pragma once

#include "../public/i_entity.h"

class Game : public IEntity
{
public:
	~Game();
	bool Init(const IVarList& args);
	bool Shut();
	// ������Ϣ����
	bool MsgProc(unsigned int msg, size_t param1, size_t param2, 
		int& result) ;
};