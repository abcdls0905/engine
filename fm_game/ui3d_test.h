#pragma once

#include "../public/i_entity.h"

class UI3DTest : public IEntity
{
public:
	// ��ʼ�����ɴ�����������ʵ�壩
	virtual bool Init(const IVarList& args);

	// �رգ���ɾ���������õĸ���ʵ�壬����Դ�ͷ�Ӧ�����������н��У�
	virtual bool Shut();

	// ������Ϣ����
	virtual bool MsgProc(unsigned int msg, size_t param1, size_t param2, 
		int& result);

	virtual void Execute(float seconds) ;

};