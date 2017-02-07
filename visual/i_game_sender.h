//--------------------------------------------------------------------
// 文件名:		IGameSender.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年2月17日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------
#ifndef _I_GAME_SENDER_H
#define _I_GAME_SENDER_H

#include "i_game_sock.h"
class IGameSender: public IEntity
{
public:
	virtual void SetGameSock(IGameSock* value) = 0;
	
	// 获得验证码
	virtual bool GetVerify() = 0;
	// 登录
	virtual bool Login(const char* account, const char* pswd) = 0;
	// 登录并发送验证码
	virtual bool LoginVerify(const char* account, const char* pswd, 
		const char* verify) = 0;
	// 使用登录串登录
	virtual bool LoginByString(const char* account, const char* login_string) = 0;
	// 使用密码保护盾登录
	virtual bool LoginByShield(const char* account, const char* pswd, 
		const char* login_string) = 0;
	// 创建角色
	virtual void CreateRole(const IVarList& args, IVarList& ret) = 0;
	// 删除角色
	virtual bool DeleteRole(const wchar_t* name) = 0;
	// 恢复角色
	virtual bool ReviveRole(const wchar_t* name) = 0;
	// 选择角色进游戏
	virtual bool ChooseRole(const wchar_t* name) = 0;
	// 选择角色并指定要进入场景的信息
	virtual bool ChooseRoleAndScene(const wchar_t* name, const char* scene_info) = 0;
	// 发送客户端就绪消息
	virtual bool ClientReady() = 0;
	// 获得游戏世界的信息（创建角色时）
	virtual bool GetWorldInfo(int type) = 0;
	virtual bool GetWorldInfo2(int type, const wchar_t* info) = 0;
	// 发送普通聊天
	virtual bool Speech(const wchar_t* content) = 0;
	// 发送自定义消息
	virtual void Custom(const IVarList& args, IVarList& ret) = 0;
	// 选择游戏对象
	virtual bool Select(const char* object_ident, int func_id) = 0;
	// 请求移动到目标点
	virtual void RequestMove(const IVarList& args, IVarList& ret) = 0;
	// 发送任意消息
	virtual void SendAny(const IVarList& args, IVarList& result) = 0;
	// 发送自定义移动消息
	virtual void MoveCustom(const IVarList& args, IVarList& ret) = 0;
	// 发送消息到stub服
	virtual void SendMsgToStub(const IVarList& args, IVarList& ret) = 0;
	// 发送自定义消息到FsSoloMember
	virtual void SendMsgToMember(const IVarList& args, IVarList& ret) = 0;
	/// \brief 本地服务对象
	/// \param id 本地服务对象id
	virtual void SetStubID(const PERSISTID& id) = 0;
	virtual PERSISTID GetStubID() const = 0;
};

#endif //_I_GAME_SENDER_H