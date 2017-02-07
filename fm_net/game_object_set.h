//--------------------------------------------------------------------
// �ļ���:		game_object_set.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��7��22��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _GAME_OBJECT_SET_H
#define _GAME_OBJECT_SET_H

#include "../public/core_mem.h"
#include "../utils/array_pod.h"
#include "game_object.h"

// ��Ϸ��������

class GameObject;

class CGameObjectSet
{
public:
	CGameObjectSet();
	~CGameObjectSet();

	// ����
	bool Find(const char* ident) const;
	// ���
	bool Add(const char* ident, GameObject* object);
	// ɾ��
	bool Remove(const char* ident);
	// �ı��ʶ��
	bool Change(const char* old_ident, const char* new_ident);
	// ���
	void Clear();
	// ��ȡ����
	GameObject* GetObjectByIdent(const char* ident) const;
	// ��ȡ�����
	PERSISTID GetObjectID(const char* ident) const;
	// �������
	size_t GetObjectCount() const;
    // ��ȡ����ͨ�����
    PERSISTID GetObjectIDByIndex(const size_t index) const;
	// ��ö����б�
	void GetObjectList(IVarList& result);

private:
	CGameObjectSet(const CGameObjectSet&);
	CGameObjectSet& operator=(const CGameObjectSet&);

	bool FindIndex(const char* ident, size_t& index) const;
	
private:
	TArrayPod<GameObject*, 1, TCoreAlloc> m_Objects;
};

#endif // _GAME_OBJECT_SET_H

