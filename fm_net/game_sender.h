//--------------------------------------------------------------------
// �ļ���:		game_sender.h
// ��  ��:		
// ˵  ��:		
// ��������:	2007��5��24��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _GAME_SENDER_H
#define _GAME_SENDER_H

#include "../public/module.h"
#include "../utils/store_archive.h"
#include "../visual/i_game_sender.h"
// ��Ϸ���ݷ���

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
	
	// �����֤��
	bool GetVerify();
	// ��¼
	bool Login(const char* account, const char* pswd);
	// ��¼��������֤��
	bool LoginVerify(const char* account, const char* pswd, 
		const char* verify);
	// ʹ�õ�¼����¼
	bool LoginByString(const char* account, const char* login_string);
	// ʹ�����뱣���ܵ�¼
	bool LoginByShield(const char* account, const char* pswd, 
		const char* login_string);
	// ������ɫ
	void CreateRole(const IVarList& args, IVarList& ret);
	// ɾ����ɫ
	bool DeleteRole(const wchar_t* name);
	// �ָ���ɫ
	bool ReviveRole(const wchar_t* name);
	// ѡ���ɫ����Ϸ
	bool ChooseRole(const wchar_t* name);
	// ѡ���ɫ��ָ��Ҫ���볡������Ϣ
	bool ChooseRoleAndScene(const wchar_t* name, const char* scene_info);
	// ���Ϳͻ��˾�����Ϣ
	bool ClientReady();
	// �����Ϸ�������Ϣ��������ɫʱ��
	bool GetWorldInfo(int type);
	bool GetWorldInfo2(int type, const wchar_t* info);
	// ������ͨ����
	bool Speech(const wchar_t* content);
	// �����Զ�����Ϣ
	void Custom(const IVarList& args, IVarList& ret);
	// ѡ����Ϸ����
	bool Select(const char* object_ident, int func_id);
	// �����ƶ���Ŀ���
	void RequestMove(const IVarList& args, IVarList& ret);
	// ����������Ϣ
	void SendAny(const IVarList& args, IVarList& result);
	// �����Զ����ƶ���Ϣ
	virtual void MoveCustom(const IVarList& args, IVarList& ret);
	// ������Ϣ��stub��
	virtual void SendMsgToStub(const IVarList& args, IVarList& ret);
	// �����Զ�����Ϣ��FsSoloMember
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

