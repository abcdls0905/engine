//--------------------------------------------------------------------
// 文件名:		game_sender.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年5月24日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _GAME_SENDER_H
#define _GAME_SENDER_H

#include "../public/module.h"
#include "../utils/store_archive.h"
#include "../visual/i_game_sender.h"
// 游戏数据发送

class GameSock;
class ISoloStub;

class GameSender: public IGameSender
{
public:
	GameSender();
	virtual ~GameSender();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	void SetGameSock(IGameSock* value);
	void SetStubID(const PERSISTID& id);
	PERSISTID GetStubID() const;
	
	// 获得验证码
	bool GetVerify();
	// 登录
	bool Login(const char* account, const char* pswd);
	// 登录并发送验证码
	bool LoginVerify(const char* account, const char* pswd, 
		const char* verify);
	// 使用登录串登录
	bool LoginByString(const char* account, const char* login_string);
	// 使用密码保护盾登录
	bool LoginByShield(const char* account, const char* pswd, 
		const char* login_string);
	// 创建角色
	void CreateRole(const IVarList& args, IVarList& ret);
	// 删除角色
	bool DeleteRole(const wchar_t* name);
	// 恢复角色
	bool ReviveRole(const wchar_t* name);
	// 选择角色进游戏
	bool ChooseRole(const wchar_t* name);
	// 选择角色并指定要进入场景的信息
	bool ChooseRoleAndScene(const wchar_t* name, const char* scene_info);
	// 发送客户端就绪消息
	bool ClientReady();
	// 获得游戏世界的信息（创建角色时）
	bool GetWorldInfo(int type);
	bool GetWorldInfo2(int type, const wchar_t* info);
	// 发送普通聊天
	bool Speech(const wchar_t* content);
	// 发送自定义消息
	void Custom(const IVarList& args, IVarList& ret);
	// 选择游戏对象
	bool Select(const char* object_ident, int func_id);
	// 请求移动到目标点
	void RequestMove(const IVarList& args, IVarList& ret);
	// 发送任意消息
	void SendAny(const IVarList& args, IVarList& result);
	// 发送自定义移动消息
	virtual void MoveCustom(const IVarList& args, IVarList& ret);
	// 发送消息到stub服
	virtual void SendMsgToStub(const IVarList& args, IVarList& ret);
	// 发送自定义消息到FsSoloMember
	void SendMsgToMember(const IVarList& args, IVarList& ret);


private:
	bool AddMsgVarList(CStoreArchive& ar, const IVarList& args, size_t beg, 
		size_t end);
	bool InnerSendAny(const IVarList& msg);

private:
	GameSock* m_pGameSock;
	ISoloStub* m_pSoloStub;
};

#endif // _GAME_SENDER_H

