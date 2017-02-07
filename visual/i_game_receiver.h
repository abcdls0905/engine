#ifndef _I_GAME_RECEIVER_H
#define _I_GAME_RECEIVER_H

#include "../public/module.h"

class IGameReceiver: public IEntity
{
public:
	// 游戏客户端对象
	virtual void SetClientID(const PERSISTID& id) = 0;
	virtual PERSISTID GetClientID() const = 0;

	// 游戏消息处理对象
	virtual void SetGameMsgHanderID(const PERSISTID& id) = 0;
	virtual PERSISTID GetGameMsgHandlerID() const = 0;

	// 自定义消息处理器对象
	virtual void SetCustomHandlerID(const PERSISTID& id) = 0;
	virtual PERSISTID GetCustomHandlerID() const = 0;
	
	// 自定义消息的最大参数数量
	virtual void SetMaxCustomArguments(int value) = 0;
	virtual int GetMaxCustomArguments() const = 0;

	// 获得角色数量
	virtual int GetRoleCount() const = 0;
	// 获得每个角色的信息参数数量
	virtual int GetRoleInfoCount() const = 0;
	// 获得角色位置
	virtual int GetRoleIndex(int index) const = 0;
	// 获得角色标志
	virtual int GetRoleSysFlags(int index) const = 0;
	// 获得角色名称
	virtual const wchar_t* GetRoleName(int index) const = 0;
	// 获得角色参数
	virtual const wchar_t* GetRolePara(int index) const = 0;
	// 获得角色是否被删除
	virtual int GetRoleDeleted(int index) const = 0;
	// 获得角色删除时间
	virtual double GetRoleDeleteTime(int index) const = 0;
	// 获得所有的角色信息（可能包含附加信息）
	virtual void GetRoleInfo(const IVarList& args, IVarList& ret) const = 0;
	// 获得菜单项的数量
	virtual int GetMenuCount() const = 0;
	// 获得菜单项的类型
	virtual int GetMenuType(int index) const = 0;
	// 获得菜单项的选择标记
	virtual int GetMenuMark(int index) const = 0;
	// 获得菜单项的内容
	virtual const wchar_t* GetMenuContent(int index) const = 0;

	// 清除所有数据信息
	virtual bool ClearAll() = 0;

	// 处理消息
	virtual bool ProcessMessage(int src,const void* pdata, size_t len) = 0;

	// 导出消息统计数据
	virtual bool DumpMsgStat(const char* file_name) = 0;

	// 获得属性数量
	virtual int	GetPropertyCount() const = 0;
	// 获得属性名
	virtual const char* GetPropertyName(size_t index) const = 0;
	// 获得属性类型
	virtual int GetPropertyType(size_t index) const = 0;
	// 获得表格数量
	virtual int GetRecordCount() const = 0;
	// 获得表格名
	virtual const char* GetRecordName(size_t index) const = 0;
	// 获得表格列数
	virtual int GetRecordCols(size_t index) const = 0;

	/// \brief 本地服务对象
	/// \param id 本地服务对象id
	virtual void SetStubID(const PERSISTID& id) = 0;
	virtual PERSISTID GetStubID() const = 0;

	//////////////////////////////////////////////////////////////////////////
	//tyz  2014/2/28
	// 获得表格列数
	virtual int GetRecordColsType(size_t index,size_t col) const = 0;

};

#endif