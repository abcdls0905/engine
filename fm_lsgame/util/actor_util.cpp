//--------------------------------------------------------------------
// 文件名:		actor_util.h
// 内  容:		模型的工具接口
// 说  明:		
// 创建日期:	2014年1月20日
// 创建人:		张志明
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------


#include "../../visual/i_actor.h"
#include "../helper.h"
#include "../global.h"
#include "actor_util.h"
#include "../custom_prop_define.h"


// 获得角色主体
//IVisBase* get_actor_role(IVisBase* pRole)
//{
//    if(pRole == NULL)
//    {
//        return NULL;
//    }
//    if (!pRole->GetEntInfo()->IsKindOf("Actor"))
//    {
//        return NULL;
//    }
//
//    PERSISTID pActorRoleID = ((IActor*)pRole)->GetLinkObject("actor_role");
//    IVisBase* pActorRole =  (IVisBase*)g_core->GetEntity(pActorRoleID);
//
//    if (pActorRole == NULL)
//    {
//        return pRole;
//    }
//
//    return pActorRole;
//}

//// 不显示影子
//void hide_player_shadow(IVisBase* pRole)
//{
//    if(pRole == NULL)
//    {
//        return;
//    }
//
//    IEntity* pShadow = CHelper::QueryCustomEntity(pRole, "shadow");
//    if (pShadow)
//    {
//        CHelper::SetPropertyBool(pShadow, "Visible", false);
//    }
//}
//
//// 显示影子
//void show_player_shadow(IVisBase* pRole)
//{
//    if(pRole == NULL)
//    {
//        return ;
//    }
//
//    IEntity* pShadow = CHelper::QueryCustomEntity(pRole, "shadow");
//    if (pShadow)
//    {
//        CHelper::SetPropertyBool(pShadow, "Visible", true);
//    }
//}

bool is_actor_createfinish(IVisBase* pRole)
{
    if (!pRole)
    {
        return false;
    }

    if (CHelper::FindCustom(pRole, CUSTOM_PROP_OBJECT_CREATE_FINISH) 
        && CHelper::QueryCustomBool(pRole, CUSTOM_PROP_OBJECT_CREATE_FINISH))
    {
        return true;
    }

    return false;
}

class ActorSet
{
public:
    inline void Set(const PERSISTID& key, const PERSISTID& value)
    {
        m_index[key.nData64] = value.nData64;
    }

    inline void Remove(const PERSISTID& key)
    {
        std::map<int64_t, int64_t>::iterator it = m_index.find(key.nData64);
        if(it == m_index.end())
            return;
        m_index.erase(it);
    }

    inline PERSISTID Find(const PERSISTID& key)
    {
        std::map<int64_t, int64_t>::iterator it = m_index.find(key.nData64);
        if(it == m_index.end())
            return PERSISTID(0, 0);
        PERSISTID pid;
        pid.nData64 = it->second;
        return pid;
    }

    void SetRole(const PERSISTID& key, const PERSISTID& value)
    {
        m_rolekey = key;
        m_rolevalue = value;
    }

    void Clear()
    {
        for(std::map<int64_t, int64_t>::iterator it = m_index.begin(); it!= m_index.end(); it++)
        {
            PERSISTID pid;
            pid.nData64  = it->second;
            g_core->DeleteEntity(pid);
        }
        m_index.clear();
        SetRole(PERSISTID(), PERSISTID());
    }

    const PERSISTID& GetRole()const
    {
        return m_rolevalue;
    }

private:
    std::map<int64_t, int64_t> m_index;
    PERSISTID m_rolekey;
    PERSISTID m_rolevalue;
};

ActorSet g_actorset;

IActor* actor_find(const PERSISTID& id)
{
    return (IActor*)g_core->GetEntity(g_actorset.Find(id));
}

void actor_set(const PERSISTID& key, const PERSISTID& actor_id)
{
    g_actorset.Set(key, actor_id);
}

void actor_setrole(const PERSISTID& key, const PERSISTID& actor_id)
{
    g_actorset.SetRole(key, actor_id);
}

void actor_remove(const PERSISTID& id)
{
    g_actorset.Remove(id);
}

void actor_clear()
{
    g_actorset.Clear();
}

IActor* actor_getrole()
{
    return (IActor*)g_core->GetEntity(g_actorset.GetRole());
}
