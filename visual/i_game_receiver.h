#ifndef _I_GAME_RECEIVER_H
#define _I_GAME_RECEIVER_H

#include "../public/module.h"

class IGameReceiver: public IEntity
{
public:
	// ��Ϸ�ͻ��˶���
	virtual void SetClientID(const PERSISTID& id) = 0;
	virtual PERSISTID GetClientID() const = 0;

	// ��Ϸ��Ϣ�������
	virtual void SetGameMsgHanderID(const PERSISTID& id) = 0;
	virtual PERSISTID GetGameMsgHandlerID() const = 0;

	// �Զ�����Ϣ����������
	virtual void SetCustomHandlerID(const PERSISTID& id) = 0;
	virtual PERSISTID GetCustomHandlerID() const = 0;
	
	// �Զ�����Ϣ������������
	virtual void SetMaxCustomArguments(int value) = 0;
	virtual int GetMaxCustomArguments() const = 0;

	// ��ý�ɫ����
	virtual int GetRoleCount() const = 0;
	// ���ÿ����ɫ����Ϣ��������
	virtual int GetRoleInfoCount() const = 0;
	// ��ý�ɫλ��
	virtual int GetRoleIndex(int index) const = 0;
	// ��ý�ɫ��־
	virtual int GetRoleSysFlags(int index) const = 0;
	// ��ý�ɫ����
	virtual const wchar_t* GetRoleName(int index) const = 0;
	// ��ý�ɫ����
	virtual const wchar_t* GetRolePara(int index) const = 0;
	// ��ý�ɫ�Ƿ�ɾ��
	virtual int GetRoleDeleted(int index) const = 0;
	// ��ý�ɫɾ��ʱ��
	virtual double GetRoleDeleteTime(int index) const = 0;
	// ������еĽ�ɫ��Ϣ�����ܰ���������Ϣ��
	virtual void GetRoleInfo(const IVarList& args, IVarList& ret) const = 0;
	// ��ò˵��������
	virtual int GetMenuCount() const = 0;
	// ��ò˵��������
	virtual int GetMenuType(int index) const = 0;
	// ��ò˵����ѡ����
	virtual int GetMenuMark(int index) const = 0;
	// ��ò˵��������
	virtual const wchar_t* GetMenuContent(int index) const = 0;

	// �������������Ϣ
	virtual bool ClearAll() = 0;

	// ������Ϣ
	virtual bool ProcessMessage(int src,const void* pdata, size_t len) = 0;

	// ������Ϣͳ������
	virtual bool DumpMsgStat(const char* file_name) = 0;

	// �����������
	virtual int	GetPropertyCount() const = 0;
	// ���������
	virtual const char* GetPropertyName(size_t index) const = 0;
	// �����������
	virtual int GetPropertyType(size_t index) const = 0;
	// ��ñ������
	virtual int GetRecordCount() const = 0;
	// ��ñ����
	virtual const char* GetRecordName(size_t index) const = 0;
	// ��ñ������
	virtual int GetRecordCols(size_t index) const = 0;

	/// \brief ���ط������
	/// \param id ���ط������id
	virtual void SetStubID(const PERSISTID& id) = 0;
	virtual PERSISTID GetStubID() const = 0;

	//////////////////////////////////////////////////////////////////////////
	//tyz  2014/2/28
	// ��ñ������
	virtual int GetRecordColsType(size_t index,size_t col) const = 0;

};

#endif