//--------------------------------------------------------------------
// �ļ���:		game_scene.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��2��17��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _GAME_SCENE_H
#define _GAME_SCENE_H

#include "game_object.h"
#include "game_object_set.h"

// ��Ϸ����

class GameSceneObj;

class GameScene: public GameObject
{
public:
	GameScene();
	virtual ~GameScene();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// ��ӳ����ж���
	GameSceneObj* AddSceneObj(const char* object_ident);
	// ɾ�������ж���
	bool RemoveSceneObj(const char* object_ident);
	// ��ó����ж���
	GameSceneObj* GetSceneObjByIdent(const char* object_ident) const;

	// ��ó����ж����
	virtual PERSISTID GetSceneObj(const char* object_ident) const;
	// ��ó����ж�������
	virtual size_t GetSceneObjCount() const;
    // ��ó����ж���ͨ�����
    virtual PERSISTID GetSceneObjByIndex(size_t index) const;
	// ��ó����ж����б�
	virtual void GetSceneObjList(const IVarList& args, IVarList& result);

private:
	CGameObjectSet m_Objects;
};

#endif // _GAME_SCENE_H

