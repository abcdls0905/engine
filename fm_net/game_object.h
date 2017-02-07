//--------------------------------------------------------------------
// �ļ���:		game_object.h
// ��  ��:		
// ˵  ��:		
// ��������:	2008��2��17��
// ������:		½����
// ��Ȩ����:	������ţ�������޹�˾
//--------------------------------------------------------------------

#ifndef _GAME_OBJECT_H
#define _GAME_OBJECT_H

#include "../public/macros.h"
#include "../public/core_mem.h"
#include "../public/fast_str.h"
#include "../public/core_type.h"
#include "../public/i_var.h"
#include "../public/i_var_list.h"
#include "../visual/i_game_client.h"

// ��Ϸ�������

class CGamePropSet;
class CGameRecordSet;

class GameObject: public IGameObject
{
public:
	GameObject();
	virtual ~GameObject();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// ������Լ���
	CGamePropSet* GetPropSet() { return m_pPropSet; }
	
	// ��ñ�񼯺�
	CGameRecordSet* GetRecordSet();

	// �������ݶ���
	void SetDataID(const PERSISTID& value);
	virtual PERSISTID GetDataID() const;
	
	// �����ʶ
	void SetIdent(const char* value);
	const char* GetIdentQuick() const { return m_strIdent.c_str(); }
	virtual const char* GetIdent() const;

	// ��ϣֵ
	void SetHash(unsigned int value) { m_nHash = value; }
	unsigned int GetHash() const { return m_nHash; }

	// ��������
	virtual bool FindProp(const char* name);
	// �����������
	virtual int GetPropType(const char* name);
	// ����������б�
	virtual void GetPropList(const IVarList& args, IVarList& result);
	// �������ֵ
	virtual void QueryProp(const IVarList& args, IVarList& result);
	virtual int QueryPropInt(const char* name);
	virtual int64_t QueryPropInt64(const char* name);
	virtual float QueryPropFloat(const char* name);
	virtual double QueryPropDouble(const char* name);
	virtual const char* QueryPropString(const char* name);
	virtual const wchar_t* QueryPropWideStr(const char* name);
	virtual PERSISTID QueryPropObject(const char* name);

	// ���ұ��
	virtual bool FindRecord(const char* name);
	// ��ñ������
	virtual int GetRecordRows(const char* name);
	// ��ñ������
	virtual int GetRecordCols(const char* name);
	// ��ñ��������
	virtual int GetRecordColType(const char* name, size_t index);
	// ��ñ�����б�
	virtual void GetRecordList(const IVarList& args, IVarList& result);
	// ��ñ����ֵ
	virtual void QueryRecord(const IVarList& args, IVarList& result);
	virtual int QueryRecordInt(const char* name, int row, int col);
	virtual int64_t QueryRecordInt64(const char* name, int row, int col);
	virtual float QueryRecordFloat(const char* name, int row, int col);
	virtual double QueryRecordDouble(const char* name, int row, int col);
	virtual const char* QueryRecordString(const char* name, int row, int col);
	virtual const wchar_t* QueryRecordWideStr(const char* name, int row, 
		int col);
	virtual PERSISTID QueryRecordObject(const char* name, int row, int col);
	// ���ұ���
	virtual void FindRecordRow(const IVarList& args, IVarList& result);
	// ��Сд�����в��ұ���
	virtual void FindRecordRowCI(const IVarList& args, IVarList& result);

	// �������ݷ����仯
	bool PropertyChanged(const char* name);
	// ������ݷ����仯
	bool RecordChanged(const char* name, const char* op);
	
private:
	void GetValue(const IVarList& vl, size_t index, IVar& value);
	void AddValue(IVarList& vl, const IVar& value);
	
private:
	IEntity* m_pData;
	core_string m_strIdent;
	unsigned int m_nHash;
	CGamePropSet* m_pPropSet;
	CGameRecordSet* m_pRecordSet;
};

#endif // _GAME_OBJECT_H

