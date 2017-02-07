//--------------------------------------------------------------------
// 文件名:       CommandManager.cpp
// 内  容:        
// 说  明:          使用command通信，跨模块访问，减少耦合
// 创建日期:    2014年1月15日17:50:53
// 创建人:        chenhh
// 版权所有:    苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "command.h"

//
void CommandManager::ConnectCommand(int ievent_id, const PERSISTID& FormID,EventDelegate recv_callback)
{
        MAP_FORM_DELEGATES& e =  m_senders[ievent_id];
        e[FormID.nData64].push_back(recv_callback);
        return;
}

//
int CommandManager::InvokeCommand(int ievent_id, const IVarList& arg)
{
    if(m_senders.empty())
    {
        return 0;
    }

    MAP_EVENT::iterator itMap =  m_senders.find(ievent_id);
    if(itMap == m_senders.end())
    {
        return 0;
    }

    MAP_FORM_DELEGATES& form_delegates = itMap->second;
    MAP_FORM_DELEGATES::iterator iter = form_delegates.begin();
    for (; iter != form_delegates.end(); ++iter)
    {
        SLOT_DELEGATES& delegates = iter->second;
        for(size_t i = 0; i < delegates.size(); i++)
        {
            delegates[i]("", arg);
        }
    }
    return 1;
}

void CommandManager::DisConnectCommand( const PERSISTID& sender )
{
    if(m_senders.empty())
    {
        return;
    }

    MAP_EVENT::iterator iterMap = m_senders.begin();
    for (; iterMap != m_senders.end(); ++iterMap)
    {
        MAP_FORM_DELEGATES& form_delegates = iterMap->second;
        MAP_FORM_DELEGATES::iterator iter_form_delegates = form_delegates.find(sender.nData64);
        if (iter_form_delegates != form_delegates.end())
        {
            form_delegates.erase(iter_form_delegates);
        }
    }
}
