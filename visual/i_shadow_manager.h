
#ifndef _I_SHADOW_MANAGER_H_
#define _I_SHADOW_MANAGER_H_
#include "i_vis_base.h"

class IShadowManager: public IVisBase
{
public:
	// ������Ӱ�����������Դ
	virtual bool Load() = 0;

	// �Ƿ�ɼ�
	virtual void SetVisible(bool value) = 0;
	virtual bool GetVisible() const = 0;

	// ���Ӱ������
	virtual void SetMaxShadowNum(int value) = 0;
	virtual int GetMaxShadowNum() const = 0;

	// ������Ҷ����Ӱ��
	virtual void SetPlayerShadowID(const PERSISTID& value) = 0;
	virtual PERSISTID GetPlayerShadowID() const = 0;

	// ��ͼ�ߴ�
	virtual void SetTextureSize(int value) = 0;
	virtual int GetTextureSize() const = 0;

	// �����Ƿ�ֻ�ü�����Ӱ
	virtual void SetUseSimpleShadow(bool value) = 0;
	virtual bool GetUseSimpleShadow() const = 0;

	// ����Ӱ�Ӷ���
	virtual PERSISTID Create(const PERSISTID& id) = 0;
	// ɾ��Ӱ�Ӷ���
	virtual bool Delete(const PERSISTID& id) = 0;

	// ����Ӱ���б�
	virtual void GetShadowList(const IVarList& args, IVarList& result) = 0;
};



#endif