//--------------------------------------------------------------------
// 文件名:       CommandManager.h
// 内  容:        
// 说  明:          使用command通信，跨模块访问，减少耦合
// 创建日期:    2014年1月15日17:50:53
// 创建人:        chenhh
// 版权所有:    苏州蜗牛电子有限公司
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