//--------------------------------------------------------------------
// �ļ���:		game_view.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��2��17��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _GAME_VIEW_H
#define _GAME_VIEW_H

#include "game_object.h"
#include "game_object_set.h"

// ��Ϸ�����Ӵ�

class GameViewObj;

class GameView: public GameObject
{
public:
	GameView();
	virtual ~GameView();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// �Ӵ���������
	void SetCapacity(int value);
	int GetCapacity() const;

	// ����Ӵ������
	GameViewObj* AddViewObj(const char* item_ident);
	// ɾ���Ӵ������
	bool RemoveViewObj(const char* item_ident);
	// �ƶ��Ӵ������
	bool ChangeViewObj(const char* old_item_ident, const char* new_item_ident);
	// ����Ӵ������
	GameViewObj* GetViewObjByIdent(const char* item_ident) const;

	// ����Ӵ�����������
	virtual PERSISTID GetViewObj(const char* item_ident) const;
	// ����Ӵ����������
	virtual size_t GetViewObjCount() const;
	// ����Ӵ�����������б�
	virtual void GetViewObjList(const IVarList& args, IVarList& result);
	
private:
	int m_nCapacity;
	CGameObjectSet m_Objects;
};

#endif // _GAME_VIEW_H

