//--------------------------------------------------------------------
// �ļ���:		ActionEditor.h
// ��  ��:		
// ˵  ��:		
// ��������:	2012��10��16��
// ������:		����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------
#ifndef __ACTION_EDITOR_H__
#define __ACTION_EDITOR_H__

#include "i_frame_receiver_user.h"
#include "../public/module.h"

// �����༭��

class ActionEditor: public IFrameReceiverUser
{
public:
	ActionEditor() {}
	virtual ~ActionEditor() {}

private:
	virtual void ProcessFrameData(char* pFrameBuffer) {}
};

#endif // __ACTION_EDITOR_H__
