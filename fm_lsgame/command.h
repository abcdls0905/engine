//--------------------------------------------------------------------
// �ļ���:       CommandManager.h
// ��  ��:        
// ˵  ��:          ʹ��commandͨ�ţ���ģ����ʣ��������
// ��������:    2014��1��15��17:50:53
// ������:        chenhh
// ��Ȩ����:    ������ţ�������޹�˾
//--------------------------------------------------------------------
#ifndef __COMMAND_H__
#define __COMMAND_H__

#include "../public/i_var_list.h"
#include "../visual/i_event_manager.h"

#include <map>
#include <string>
#include <vector>

//
class CommandManager
{
private:
    typedef std::vector<EventDelegate> SLOT_DELEGATES;
    typedef std::map<uint64_t, SLOT_DELEGATES> MAP_FORM_DELEGATES;
    typedef std::map<int, MAP_FORM_DELEGATES> MAP_EVENT;
    MAP_EVENT m_senders;

public:
    void ConnectCommand(int ievent_id, const PERSISTID& FormID, EventDelegate recv_callback);
    int InvokeCommand(int ievent_id, const IVarList& arg);
    void DisConnectCommand(const PERSISTID& sender);

    //
    static CommandManager& Instance()
    {
        static CommandManager s_inst;
        return s_inst;
    }
};
#endif