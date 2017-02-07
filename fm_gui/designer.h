//--------------------------------------------------------------------
// 文件名:		designer.h
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _DESIGNER_H
#define _DESIGNER_H

#include "../public/module.h"
#include "../public/core_type.h"
#include "../utils/string_pod.h"

// 界面设计器

class Gui;
class IPainter;
class IControl;
class IDesignInfo;
class CDesignLoader;
class CDesignSaver;
class CDesignRes;

class Designer: public IEntity
{
public:
	Designer();
	virtual ~Designer();

	virtual bool Init(const IVarList& args);
	virtual bool Shut();

	void SetGui(Gui* pGui);
	Gui* GetGui() const;

	void SetPainter(IPainter* pPainter);
	IPainter* GetPainter() const { return m_pPainter; }

	// 获得缺省资源路径
	const char* GetDefaultPath() const;

	// 可编辑模式
	void SetDesignMode(bool value);
	bool GetDesignMode() const;
	
	// 当前编辑窗口（Form）
	void SetDesignFormID(const PERSISTID& value);
	PERSISTID GetDesignFormID() const;

	// 资源文件编码方式
	void SetEncoding(const char* value);
	const char* GetEncoding() const;
	
	// 缺省的资源集合文件名
	void SetDefResourceName(const char* value);
	const char* GetDefResourceName() const;
	// 获得资源文件数量
	int GetResFileCount(const char* res_name) const;
	// 获得资源文件名
	void GetResFileList(const IVarList& args, IVarList& result) const;
	// 查找资源文件
	bool FindResFile(const char* res_name, const char* file) const;
	// 添加资源文件
	bool AddResFile(const char* res_name, const char* file);
	// 删除资源文件（资源文件的内容不会被移除）
	bool RemoveResFile(const char* res_name, const char* file);
	// 清除资源文件
	bool ClearResFile(const char* res_name);

	// 分解颜色元素
	int GetColorAlpha(const char* color) const;
	int GetColorRed(const char* color) const;
	int GetColorGreen(const char* color) const;
	int GetColorBlue(const char* color) const;

	// 分解图元参数
	//void GetImageInfo(const IVarList& args, IVarList& result) const;

	// 获得贴图的尺寸
	int GetTexWidth(const char* tex) const;
	int GetTexHeight(const char* tex) const;
	
	// 加载主窗口配置文件
	bool LoadDesktop(const char* path, const char* file);
	// 异步加载主窗口配置文件
	bool LoadDesktopAsync(const char* path, const char* file);
	// 加载窗口配置文件
	PERSISTID LoadForm(const char* path, const char* file, bool bStatic=false);
	// 加载窗口配置文件
	PERSISTID LoadFormAsync(const char* path, const char* file, bool bStatic=false);
	// 加载资源集合文件
	bool LoadResource(const char* path, const char* file);

	// 保存窗口到配置文件
	bool SaveForm(const char* path, const char* file, const char* resource);
	// 保存资源集合文件
	bool SaveResource(const char* path, const char* file);

	// 设计器中创建控件
	PERSISTID Create(const char* name);
	// 设计器中复制控件
	PERSISTID Clone(const PERSISTID& id);
	// 设计器中删除控件
	bool Delete(const PERSISTID& id);

	// 获得控件的可编辑属性数量
	int GetPropCount(const PERSISTID& id) const;
	// 获得可编辑属性名称
	void GetPropList(const IVarList& args, IVarList& result) const;
	// 获得可编辑属性类型
	result_string GetPropType(const PERSISTID& id, const char* name) const;
	// 获得可编辑属性当前值
	result_string GetPropValue(const PERSISTID& id, const char* name) const;
	// 设置可编辑属性当前值
	bool SetPropValue(const PERSISTID& id, 
		const char* name, const char* value);
	// 获得可编辑属性是否保存
	bool GetPropSave(const PERSISTID& id, const char* name) const;
	// 获得可编辑属性在附属控件中是否保存
	bool GetPropSuborSave(const PERSISTID& id, const char* name) const;

	// 获得附属控件数量
	int GetSuborCount(const PERSISTID& id) const;
	// 获得附属控件名字
	void GetSuborList(const IVarList& args, IVarList& result) const;
	// 获得附属控件类型
	result_string GetSuborType(const PERSISTID& id, const char* name) const;
	// 获得附属控件对象
	PERSISTID GetSubor(const PERSISTID& id, const char* name) const;

	// 获得事件数量
	int GetEventCount(const PERSISTID& id) const;
	// 获得事件名称
	void GetEventList(const IVarList& args, IVarList& result) const;

	// 获得容器中的被编辑控件数量
	int GetControlCount(const PERSISTID& id);
	// 获得容器中的被编辑控件列表
	void GetControlList(const IVarList& args, IVarList& result) const;
	
//	// 查找事件的回调函数
//	bool FindCallback(const PERSISTID& id, const char* name) const;
//	// 添加回调函数
//	bool AddCallback(const PERSISTID& id, 
//		const char* name, const char* func);
//	// 删除回调函数
//	bool RemoveCallback(const PERSISTID& id, const char* name);
//	// 清除所有回调函数
//	bool ClearCallback(const PERSISTID& id);
//	// 获得回调函数数量
//	int GetCallbackCount(const PERSISTID& id) const;
//	// 获得回调事件名
//	void GetCallbackList(const IVarList& args, IVarList& result) const;
//	// 获得回调函数名
//	result_string GetCallbackFunc(const PERSISTID& id, const char* name) const;
	
	// 子控件名字集合
	bool FindMember(const char* name) const;
	bool AddMember(const char* name);
	bool RemoveMember(const char* name);
	bool ClearMember();
	int GetMemberCount() const;
	void GetMemberList(const IVarList& args, IVarList& result);

	// 资源文件
	CDesignRes* GetDesignRes() const;
	// 获得主窗口
	IControl* GetDesktopForm() const;
	// 获得ID对应控件
	IControl* GetControl(const PERSISTID& id) const;
	// 获得被编辑窗口
	IControl* GetDesignForm() const;
	// 获得设计信息
	IDesignInfo* GetDesignInfo(IControl* control) const;
	// 创建控件
	IControl* CreateControl(const char* name);
	// 复制控件
	IControl* CloneControl(IControl* proto);
	// 获得附属控件
	IControl* GetSuborControl(IControl* control, const char* name) const;
	
	// 设置控件内部属性
	bool SetControlProperty(IControl* control, const char* name, 
		const char* value, bool is_utf8);

private:
	Gui* m_pGui;
	IPainter* m_pPainter;
	CDesignLoader* m_pLoader;
	CDesignSaver* m_pSaver;
	CDesignRes* m_pDesignRes;
	bool m_bDesignMode;
	PERSISTID m_DesignFormID;
	core_string m_strEncoding;
	core_string m_strDefResourceName;
	TStringPod<char, int, TStringTraits<char>, TCoreAlloc> m_Members;
};

#endif // _DESIGNER_H

