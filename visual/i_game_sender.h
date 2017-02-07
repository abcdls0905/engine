//--------------------------------------------------------------------
// �ļ���:		IGameSender.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��2��17��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------
#ifndef _I_GAME_SENDER_H
#define _I_GAME_SENDER_H

#include "i_game_sock.h"
class IGameSender: public IEntity
{
public:
	virtual void SetGameSock(IGameSock* value) = 0;
	
	// �����֤��
	virtual bool GetVerify() = 0;
	// ��¼
	virtual bool Login(const char* account, const char* pswd) = 0;
	// ��¼��������֤��
	virtual bool LoginVerify(const char* account, const char* pswd, 
		const char* verify) = 0;
	// ʹ�õ�¼����¼
	virtual bool LoginByString(const char* account, const char* login_string) = 0;
	// ʹ�����뱣���ܵ�¼
	virtual bool LoginByShield(const char* account, const char* pswd, 
		const char* login_string) = 0;
	// ������ɫ
	virtual void CreateRole(const IVarList& args, IVarList& ret) = 0;
	// ɾ����ɫ
	virtual bool DeleteRole(const wchar_t* name) = 0;
	// �ָ���ɫ
	virtual bool ReviveRole(const wchar_t* name) = 0;
	// ѡ���ɫ����Ϸ
	virtual bool ChooseRole(const wchar_t* name) = 0;
	// ѡ���ɫ��ָ��Ҫ���볡������Ϣ
	virtual bool ChooseRoleAndScene(const wchar_t* name, const char* scene_info) = 0;
	// ���Ϳͻ��˾�����Ϣ
	virtual bool ClientReady() = 0;
	// �����Ϸ�������Ϣ��������ɫʱ��
	virtual bool GetWorldInfo(int type) = 0;
	virtual bool GetWorldInfo2(int type, const wchar_t* info) = 0;
	// ������ͨ����
	virtual bool Speech(const wchar_t* content) = 0;
	// �����Զ�����Ϣ
	virtual void Custom(const IVarList& args, IVarList& ret) = 0;
	// ѡ����Ϸ����
	virtual bool Select(const char* object_ident, int func_id) = 0;
	// �����ƶ���Ŀ���
	virtual void RequestMove(const IVarList& args, IVarList& ret) = 0;
	// ����������Ϣ
	virtual void SendAny(const IVarList& args, IVarList& result) = 0;
	// �����Զ����ƶ���Ϣ
	virtual void MoveCustom(const IVarList& args, IVarList& ret) = 0;
	// ������Ϣ��stub��
	virtual void SendMsgToStub(const IVarList& args, IVarList& ret) = 0;
	// �����Զ�����Ϣ��FsSoloMember
	virtual void SendMsgToMember(const IVarList& args, IVarList& ret) = 0;
	/// \brief ���ط������
	/// \param id ���ط������id
	virtual void SetStubID(const PERSISTID& id) = 0;
	virtual PERSISTID GetStubID() const = 0;
};

#endif //_I_GAME_SENDER_H