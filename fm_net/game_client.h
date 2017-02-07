//--------------------------------------------------------------------
// �ļ���:		game_client.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��2��17��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _GAME_CLIENT_H
#define _GAME_CLIENT_H

#include "../public/core_mem.h"
#include "../public/fast_str.h"
#include "../public/core_type.h"
#include "../visual/i_game_client.h"
#include "game_object_set.h"

// ��Ϸ�ͻ���

class GameScene;
class GameSceneObj;
class GameView;

class GameClient: public IGameClient
{
public:
	GameClient();
	virtual ~GameClient();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// ������Ҷ����
	void SetPlayerIdent(const char* value);
	virtual const char* GetPlayerIdent() const;

	// ��������
	GameScene* CreateScene(const char* player_ident);
	// ɾ������
	bool DeleteScene();
	// ��õ�ǰ����
	GameScene* GetCurrentScene() const;
	// ��õ�ǰ���Ƕ���
	GameSceneObj* GetCurrentPlayer() const;
	// �����Ӵ�
	GameView* CreateView(const char* view_ident, int capacity);
	// ɾ���Ӵ�
	bool DeleteView(const char* view_ident);
	// ���ָ����ʶ�Ӵ�
	GameView* GetViewByIdent(const char* view_ident) const;

	// �Ƿ���Ҷ���
	virtual bool IsPlayer(const char* ident) const;
	// �����Ҷ���
	virtual PERSISTID GetPlayer() const;
	// ��ó�������
	virtual PERSISTID GetScene() const;
	// ��ó����ڶ���
	virtual PERSISTID GetSceneObj(const char* ident) const;
	// ����Ӵ�����
	virtual PERSISTID GetView(const char* view_ident) const;
	// ����Ӵ��ڶ���
	virtual PERSISTID GetViewObj(const char* view_ident, 
		const char* item_ident) const;
	// ����Ӵ�����
	virtual size_t GetViewCount() const;
	// ����Ӵ��б�
	virtual void GetViewList(const IVarList& args, IVarList& result);

	// �����������
	bool ClearAll();

private:
	core_string m_strPlayerIdent;
	GameScene* m_pScene;
	CGameObjectSet m_Views;
};

#endif // _GAME_CLIENT_H

