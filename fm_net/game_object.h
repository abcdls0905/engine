//--------------------------------------------------------------------
// 文件名:		game_object.h
// 内  容:		
// 说  明:		
// 创建日期:	2008年2月17日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
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

// 游戏对象基类

class CGamePropSet;
class CGameRecordSet;

class GameObject: public IGameObject
{
public:
	GameObject();
	virtual ~GameObject();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	// 获得属性集合
	CGamePropSet* GetPropSet() { return m_pPropSet; }
	
	// 获得表格集合
	CGameRecordSet* GetRecordSet();

	// 关联数据对象
	void SetDataID(const PERSISTID& value);
	virtual PERSISTID GetDataID() const;
	
	// 对象标识
	void SetIdent(const char* value);
	const char* GetIdentQuick() const { return m_strIdent.c_str(); }
	virtual const char* GetIdent() const;

	// 哈希值
	void SetHash(unsigned int value) { m_nHash = value; }
	unsigned int GetHash() const { return m_nHash; }

	// 查找属性
	virtual bool FindProp(const char* name);
	// 获得属性类型
	virtual int GetPropType(const char* name);
	// 获得属性名列表
	virtual void GetPropList(const IVarList& args, IVarList& result);
	// 获得属性值
	virtual void QueryProp(const IVarList& args, IVarList& result);
	virtual int QueryPropInt(const char* name);
	virtual int64_t QueryPropInt64(const char* name);
	virtual float QueryPropFloat(const char* name);
	virtual double QueryPropDouble(const char* name);
	virtual const char* QueryPropString(const char* name);
	virtual const wchar_t* QueryPropWideStr(const char* name);
	virtual PERSISTID QueryPropObject(const char* name);

	// 查找表格
	virtual bool FindRecord(const char* name);
	// 获得表格行数
	virtual int GetRecordRows(const char* name);
	// 获得表格列数
	virtual int GetRecordCols(const char* name);
	// 获得表格列类型
	virtual int GetRecordColType(const char* name, size_t index);
	// 获得表格名列表
	virtual void GetRecordList(const IVarList& args, IVarList& result);
	// 获得表格数值
	virtual void QueryRecord(const IVarList& args, IVarList& result);
	virtual int QueryRecordInt(const char* name, int row, int col);
	virtual int64_t QueryRecordInt64(const char* name, int row, int col);
	virtual float QueryRecordFloat(const char* name, int row, int col);
	virtual double QueryRecordDouble(const char* name, int row, int col);
	virtual const char* QueryRecordString(const char* name, int row, int col);
	virtual const wchar_t* QueryRecordWideStr(const char* name, int row, 
		int col);
	virtual PERSISTID QueryRecordObject(const char* name, int row, int col);
	// 查找表行
	virtual void FindRecordRow(const IVarList& args, IVarList& result);
	// 大小写不敏感查找表行
	virtual void FindRecordRowCI(const IVarList& args, IVarList& result);

	// 属性数据发生变化
	bool PropertyChanged(const char* name);
	// 表格数据发生变化
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

