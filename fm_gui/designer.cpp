//--------------------------------------------------------------------
// 文件名:		designer.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "designer.h"
#include "design_info.h"
#include "design_loader.h"
#include "design_saver.h"
#include "design_res.h"
#include "form.h"
#include "i_control.h"
#include "../public/var_list.h"
#include "../public/core_mem.h"
#include "../public/core_log.h"
#include "../public/core_file.h"
#include "../visual/i_painter.h"

/// \file designer.cpp
/// \brief 资源设计器

/// entity: Designer
/// \brief 资源设计器实体
DECLARE_ENTITY(Designer, 0, IEntity);

/// readonly: string DefaultPath
/// \brief 缺省资源路径
DECLARE_PROPERTY_GET(const char*, Designer, DefaultPath, GetDefaultPath);
/// readonly: bool DesignMode
/// \brief 是否在设计状态
DECLARE_PROPERTY(bool, Designer, DesignMode, GetDesignMode, SetDesignMode);

/// property: object DesignForm
/// \brief 当前被设计表单
DECLARE_PROPERTY(PERSISTID, Designer, DesignForm, GetDesignFormID, 
	SetDesignFormID);
/// property: string Encoding
/// \brief XML资源文件编码
DECLARE_PROPERTY(const char*, Designer, Encoding, GetEncoding, SetEncoding);
/// method: string GetDefResourceName()
/// \brief 缺省的资源集合文件名
DECLARE_PROPERTY(const char*, Designer, DefResourceName, GetDefResourceName, 
	SetDefResourceName);

// Designer

Designer::Designer()
{
	m_pGui = NULL;
	m_pPainter = NULL;
	m_bDesignMode = false;
	//m_strEncoding = "gb2312";
	m_strEncoding = "utf-8";
	m_strDefResourceName = "skin\\resource.xml";
	m_pLoader = CORE_NEW(CDesignLoader);
	m_pLoader->SetDesigner(this);
	m_pSaver = CORE_NEW(CDesignSaver);
	m_pSaver->SetDesigner(this);
	m_pDesignRes = CORE_NEW(CDesignRes);
}

Designer::~Designer()
{
	CORE_DELETE(m_pLoader);
	CORE_DELETE(m_pSaver);
	CORE_DELETE(m_pDesignRes);
    m_pDesignRes = NULL;
    m_pSaver = NULL;
    m_pLoader = NULL;
}

bool Designer::Init(const IVarList& args)
{
	return true;
}

bool Designer::Shut()
{
	return true;
}

void Designer::SetGui(Gui* pGui)
{
	Assert(pGui != NULL);

	m_pGui = pGui;
}

Gui* Designer::GetGui() const
{
	return m_pGui;
}

void Designer::SetPainter(IPainter* pPainter)
{
	Assert(pPainter != NULL);
	
	m_pPainter = pPainter;
}

CDesignRes* Designer::GetDesignRes() const
{
	return m_pDesignRes;
}

IControl* Designer::GetDesktopForm() const
{
	if (NULL == m_pGui->GetDesktopForm())
	{
		m_pGui->CreateDesktopForm();
	}
	
	return m_pGui->GetDesktopForm();
}

const char* Designer::GetDefaultPath() const
{
	return m_pPainter->GetDefaultPath();
}

void Designer::SetDesignMode(bool value)
{
	m_bDesignMode = value;
}

bool Designer::GetDesignMode() const
{
	return m_bDesignMode;
}

void Designer::SetDesignFormID(const PERSISTID& value)
{
	IEntity* pEntity = GetCore()->GetEntity(value);

	if (NULL == pEntity)
	{
		return;
	}

	if (!pEntity->GetEntInfo()->IsKindOf("Form"))
	{
		return;
	}

	m_DesignFormID = pEntity->GetID();
}

PERSISTID Designer::GetDesignFormID() const
{
	return m_DesignFormID;
}

void Designer::SetEncoding(const char* value)
{
	Assert(value != NULL);

	m_strEncoding = value;
}

const char* Designer::GetEncoding() const
{
	return m_strEncoding.c_str();
}

void Designer::SetDefResourceName(const char* value)
{
	Assert(value != NULL);

	m_strDefResourceName = value;
}

const char* Designer::GetDefResourceName() const
{
	return m_strDefResourceName.c_str();
}

int Designer::GetResFileCount(const char* res_name) const
{
	Assert(res_name != NULL);

	size_t type = m_pDesignRes->GetResType(res_name);

	if (type == CDesignRes::RESTYPE_MAX)
	{
		return 0;
	}

	return (int)m_pDesignRes->GetResFileCount(type);
}

void Designer::GetResFileList(const IVarList& args, IVarList& result) const
{
	size_t type = m_pDesignRes->GetResType(args.StringVal(0));
	
	if (type == CDesignRes::RESTYPE_MAX)
	{
		return;
	}

	m_pDesignRes->GetResFileList(type, result);
}

bool Designer::FindResFile(const char* res_name, const char* file) const
{
	Assert(res_name != NULL);
	Assert(file != NULL);
	
	size_t type = m_pDesignRes->GetResType(res_name);
	
	if (type == CDesignRes::RESTYPE_MAX)
	{
		return false;
	}
	
	return m_pDesignRes->FindResFile(type, file);
}

bool Designer::AddResFile(const char* res_name, const char* file)
{
	Assert(res_name != NULL);
	Assert(file != NULL);

	size_t type = m_pDesignRes->GetResType(res_name);
	
	if (type == CDesignRes::RESTYPE_MAX)
	{
		return false;
	}
	
	if (!m_pLoader->LoadXmlResFile((int)type, GetDefaultPath(), file))
	{
		return false;
	}

	return m_pDesignRes->AddResFile(type, file);
}

bool Designer::RemoveResFile(const char* res_name, const char* file)
{
	Assert(res_name != NULL);
	Assert(file != NULL);

	size_t type = m_pDesignRes->GetResType(res_name);
	
	if (type == CDesignRes::RESTYPE_MAX)
	{
		return false;
	}
	
	return m_pDesignRes->RemoveResFile(type, file);
}

bool Designer::ClearResFile(const char* res_name)
{
	Assert(res_name != NULL);
	
	if (StringEmpty(res_name))
	{
		for (size_t i = 0; i < CDesignRes::RESTYPE_MAX; i++)
		{
			m_pDesignRes->ClearResFile(i);
		}

		return true;
	}
	
	size_t type = m_pDesignRes->GetResType(res_name);
	
	if (type == CDesignRes::RESTYPE_MAX)
	{
		return false;
	}
	
	return m_pDesignRes->ClearResFile(type);
}

int Designer::GetColorAlpha(const char* color) const
{
	Assert(color != NULL);

	unsigned int value = GuiUtil_ColorToInt(color);

	return ((value >> 24) & 0xFF);
}

int Designer::GetColorRed(const char* color) const
{
	Assert(color != NULL);
	
	unsigned int value = GuiUtil_ColorToInt(color);
	
	return ((value >> 16) & 0xFF);
}

int Designer::GetColorGreen(const char* color) const
{
	Assert(color != NULL);
	
	unsigned int value = GuiUtil_ColorToInt(color);
	
	return ((value >> 8) & 0xFF);
}

int Designer::GetColorBlue(const char* color) const
{
	Assert(color != NULL);
	
	unsigned int value = GuiUtil_ColorToInt(color);
	
	return ((value >> 0) & 0xFF);
}

int Designer::GetTexWidth(const char* tex) const
{
	Assert(tex != NULL);

	PERSISTID id = m_pPainter->FindGlyph(tex);

	if (id.IsNull())
	{
		return 0;
	}

	return m_pPainter->GetGlyphWidth(id);
}

int Designer::GetTexHeight(const char* tex) const
{
	Assert(tex != NULL);
	
	PERSISTID id = m_pPainter->FindGlyph(tex);
	
	if (id.IsNull())
	{
		return 0;
	}
	
	return m_pPainter->GetGlyphHeight(id);
}

bool Designer::SetControlProperty(IControl* control, const char* name, 
	const char* value, bool is_utf8)
{
	Assert(control != NULL);
	Assert(name != NULL);
	Assert(value != NULL);
	
	CVar prop;
	
	if (!GetCore()->GetProperty(control, name, prop))
	{
		return false;
	}
	
	if (prop.IsInteger())
	{
		prop.SetInt(atoi(value));
	}
	else if (prop.IsReal())
	{
		prop.SetFloat(float(atof(value)));
	}
	else if (prop.GetType() == VTYPE_BOOL)
	{
		if (stricmp(value, "true") == 0)
		{
			prop.SetBool(true);
		}
		else
		{
			prop.SetBool(false);
		}
	}
	else if (prop.GetType() == VTYPE_STRING)
	{
		prop.SetString(value);
	}
	else if (prop.GetType() == VTYPE_WIDESTR)
	{
		if (is_utf8)
		{
			prop.SetWideStr(GuiUtil_Utf8ToUtf16(value).c_str());
		}
		else
		{
			prop.SetWideStr(GuiUtil_AnsiToUtf16(value).c_str());
		}
	}
	else
	{
		return false;
	}
	
	return GetCore()->SetProperty(control, name, prop);
}

bool Designer::LoadDesktop(const char* path, const char* file)
{
	return m_pLoader->LoadDesktop(path, file, false);
}

bool Designer::LoadDesktopAsync(const char* path, const char* file)
{
	return m_pLoader->LoadDesktop(path, file, true);
}

PERSISTID Designer::LoadForm(const char* path, const char* file, bool bStatic)
{
	return m_pLoader->LoadForm(path, file, false, bStatic);
}

PERSISTID Designer::LoadFormAsync(const char* path, const char* file, bool bStatic)
{
	return m_pLoader->LoadForm(path, file, true, bStatic);
}

bool Designer::LoadResource(const char* path, const char* file)
{
	return m_pLoader->LoadResource(path, file);
}

bool Designer::SaveForm(const char* path, const char* file, 
	const char* resource)
{
	return m_pSaver->SaveForm(GetDesignForm(), path, file, resource);
}

bool Designer::SaveResource(const char* path, const char* file)
{
	return m_pSaver->SaveResource(path, file);
}

IDesignInfo* Designer::GetDesignInfo(IControl* control) const
{
	return m_pGui->GetDesignInfo(control);
}

IControl* Designer::GetControl(const PERSISTID& id) const
{
	return m_pGui->GetControl(id);
}

IControl* Designer::GetDesignForm() const
{
	return GetControl(m_DesignFormID);
}

IControl* Designer::CreateControl(const char* name)
{
	Assert(name != NULL);
	
	IControl* pControl = m_pGui->CreateControl(name);
	
	if (NULL == pControl)
	{
		return NULL;
	}
	
	if (m_bDesignMode)
	{
		pControl->SetDesignMode(true);
	}
	
	return pControl;
}

// 复制控件
static bool copy_control(Designer* designer, IControl* src, IControl* dst)
{
	ICore* pCore = designer->GetCore();
	
	// 复制所有内部属性
	// 获得属性列表
	CVarList prop_list;

	src->GetEntInfo()->GetPropertyAll(prop_list);

	CVar prop;

	for (size_t i = 0; i < prop_list.GetCount(); ++i)
	{
		const char* name = prop_list.StringVal(i);

		// 复制所有属性
		if (pCore->GetProperty(src, name, prop))
		{
			pCore->SetProperty(dst, name, prop);
		}
	}

	// 复制附属控件
	CVarList subor_list;

	designer->GetGui()->GetDesignInfo(src)->GetSuborList(subor_list);

	for (size_t i = 0; i < subor_list.GetCount(); ++i)
	{
		const char* subor_name = subor_list.StringVal(i);

		IControl* src_subor = designer->GetSuborControl(src, subor_name);

		if (NULL == src_subor)
		{
			continue;
		}

		IControl* dst_subor = designer->GetSuborControl(dst, subor_name);

		if (NULL == dst_subor)
		{
			continue;
		}

		copy_control(designer, src_subor, dst_subor);
	}

	return true;
}

IControl* Designer::CloneControl(IControl* proto)
{
	Assert(proto != NULL);

	IControl* pControl = m_pGui->CreateControl(
		proto->GetEntInfo()->GetEntityName());

	if (NULL == pControl)
	{
		return NULL;
	}

	copy_control(this, proto, pControl);

	if (m_bDesignMode)
	{
		pControl->SetDesignMode(true);
	}

	return pControl;
}

PERSISTID Designer::Create(const char* name)
{
	Assert(name != NULL);

	IControl* pControl = CreateControl(name);
	
	if (NULL == pControl)
	{
		return PERSISTID();
	}

	return pControl->GetID();
}

PERSISTID Designer::Clone(const PERSISTID& id)
{
	IControl* pProto = GetControl(id);

	if (NULL == pProto)
	{
		return PERSISTID();
	}

	IControl* pControl = CloneControl(pProto);

	if (NULL == pControl)
	{
		return PERSISTID();
	}

	return pControl->GetID();
}

bool Designer::Delete(const PERSISTID& id)
{
	IControl* pControl = GetControl(id);

	if (NULL == pControl)
	{
		return false;
	}
	
	IControl* parent = pControl->GetParentControl();
	
	if (parent != NULL)
	{
		parent->RemoveChild(pControl);
	}
	
	pControl->Release();
	
	return true;
}

int Designer::GetPropCount(const PERSISTID& id) const
{
	IControl* pControl = GetControl(id);

	if (NULL == pControl)
	{
		return 0;
	}
	
	return (int)m_pGui->GetDesignInfo(pControl)->GetPropCount();
}

void Designer::GetPropList(const IVarList& args, IVarList& result) const
{
	IControl* pControl = GetControl(args.ObjectVal(0));
	
	if (NULL == pControl)
	{
		return;
	}

	GetDesignInfo(pControl)->GetPropList(result);
}

result_string Designer::GetPropType(const PERSISTID& id, 
	const char* name) const
{
	Assert(name != NULL);
	
	IControl* pControl = GetControl(id);
	
	if (NULL == pControl)
	{
		return "";
	}
	
	int type = m_pGui->GetDesignInfo(pControl)->GetPropType(name);
	
	return m_pGui->GetDesignInfo(pControl)->GetPropTypeString(type);
}

result_string Designer::GetPropValue(const PERSISTID& id, 
	const char* name) const
{
	Assert(name != NULL);
	
	IControl* pControl = GetControl(id);
	
	if (NULL == pControl)
	{
		return "";
	}
	
	CVar prop;

	if (!GetCore()->GetProperty(pControl, name, prop))
	{
		return "";
	}
	
	if (prop.GetType() == VTYPE_BOOL)
	{
		if (prop.BoolVal())
		{
			return "true";
		}
		else
		{
			return "false";
		}
	}

	if (prop.IsInteger())
	{
		char buf[32];

		SafeSprintf(buf, sizeof(buf), "%d", prop.IntVal());

		return result_string(buf);
	}

	if (prop.IsReal())
	{
		char buf[128];

		SafeSprintf(buf, sizeof(buf), "%.05f", prop.FloatVal());

		return result_string(buf);
	}
	
	if (prop.GetType() == VTYPE_STRING)
	{
		return result_string(prop.StringVal());
	}
	
	if (prop.GetType() == VTYPE_WIDESTR)
	{
		return GuiUtil_Utf16ToAnsi(prop.WideStrVal()).c_str();
	}
	
	return "";
}

bool Designer::SetPropValue(const PERSISTID& id, const char* name, 
	const char* value)
{
	Assert(name != NULL);
	Assert(value != NULL);
	
	IControl* pControl = GetControl(id);
	
	if (NULL == pControl)
	{
		return false;
	}
	
	if (pControl->GetEntInfo()->FindPropertyInfo(name) == NULL)
	{
		CORE_TRACE("(Designer::SetPropValue)no this property");
		CORE_TRACE(name);
		return false;
	}

	return SetControlProperty(pControl, name, value, false);
}

bool Designer::GetPropSave(const PERSISTID& id, const char* name) const
{
	Assert(name != NULL);
	
	IControl* pControl = GetControl(id);
	
	if (NULL == pControl)
	{
		return false;
	}
	
	return m_pGui->GetDesignInfo(pControl)->GetPropSave(name);
}

bool Designer::GetPropSuborSave(const PERSISTID& id, 
	const char* name) const
{
	Assert(name != NULL);
	
	IControl* pControl = GetControl(id);
	
	if (NULL == pControl)
	{
		return false;
	}
	
	return m_pGui->GetDesignInfo(pControl)->GetPropSuborSave(name);
}

int Designer::GetSuborCount(const PERSISTID& id) const
{
	IControl* pControl = GetControl(id);
	
	if (NULL == pControl)
	{
		return 0;
	}
	
	return (int)m_pGui->GetDesignInfo(pControl)->GetSuborCount();
}

void Designer::GetSuborList(const IVarList& args, IVarList& result) const
{
	IControl* pControl = GetControl(args.ObjectVal(0));
	
	if (NULL == pControl)
	{
		return;
	}

	GetDesignInfo(pControl)->GetSuborList(result);
}

result_string Designer::GetSuborType(const PERSISTID& id, 
	const char* name) const
{
	Assert(name != NULL);
	
	IControl* pControl = GetControl(id);
	
	if (NULL == pControl)
	{
		return "";
	}
	
	return m_pGui->GetDesignInfo(pControl)->GetSuborType(name);
}

PERSISTID Designer::GetSubor(const PERSISTID& id, const char* name) const
{
	Assert(name != NULL);
	
	IControl* pControl = GetControl(id);
	
	if (NULL == pControl)
	{
		return PERSISTID();
	}
		
	IControl* pSubor = GetSuborControl(pControl, name);

	if (NULL == pSubor)
	{
		return PERSISTID();
	}

	return pSubor->GetID();
}

// 根据名字查找到附属控件
IControl* Designer::GetSuborControl(IControl* control, 
	const char* name) const
{
	Assert(control != NULL);
	Assert(name != NULL);
	
	// 附属控件必须包含在内部属性中
	CVar prop;
	
	if (!GetCore()->GetProperty(control, name, prop))
	{
		return NULL;
	}
	
	if (prop.GetType() != VTYPE_OBJECT)
	{
		return NULL;
	}
	
	IControl* child = GetControl(prop.ObjectVal());
	
	if (NULL == child)
	{
		return NULL;
	}

	if (child->GetParentControl() != control)
	{
		// 不是子对象
		return NULL;
	}
	
	return child;
}

int Designer::GetEventCount(const PERSISTID& id) const
{
	IControl* pControl = GetControl(id);
	
	if (NULL == pControl)
	{
		return 0;
	}
	
	return (int)m_pGui->GetDesignInfo(pControl)->GetEventCount();
}

void Designer::GetEventList(const IVarList& args, IVarList& result) const
{
	IControl* pControl = GetControl(args.ObjectVal(0));
	
	if (NULL == pControl)
	{
		return;
	}
	
	GetDesignInfo(pControl)->GetEventList(result);
}

int Designer::GetControlCount(const PERSISTID& id)
{
	IControl* pControl = GetControl(id);
	
	if (NULL == pControl)
	{
		return 0;
	}
	
	return (int)pControl->GetChildCount();
}

void Designer::GetControlList(const IVarList& args, IVarList& result) const
{
	result.Clear();
	
	IControl* pControl = GetControl(args.ObjectVal(0));
	
	if (NULL == pControl)
	{
		return;
	}

	size_t child_count = pControl->GetChildCount();

	for (size_t i = 0; i < child_count; ++i)
	{
		result.AddObject(pControl->GetChildByIndex(i)->GetID());
	}
}

/*
bool Designer::FindCallback(const PERSISTID& id, const char* name) const
{
	Assert(name != NULL);

	IControl* pControl = GetControl(id);

	if (NULL == pControl)
	{
		return false;
	}

	const char* func = pControl->GetCallbackFunc(name);

	return strlen(func) > 0;
}

bool Designer::AddCallback(const PERSISTID& id, const char* name, 
	const char* func)
{
	Assert(name != NULL);
	Assert(func != NULL);
	
	IControl* pControl = GetControl(id);
	
	if (NULL == pControl)
	{
		return false;
	}

	return pControl->AddCallback(name, func);
}

bool Designer::RemoveCallback(const PERSISTID& id, const char* name)
{
	Assert(name != NULL);
	
	IControl* pControl = GetControl(id);
	
	if (NULL == pControl)
	{
		return false;
	}
	
	return pControl->RemoveCallback(name);
}

bool Designer::ClearCallback(const PERSISTID& id)
{
	IControl* pControl = GetControl(id);
	
	if (NULL == pControl)
	{
		return false;
	}

	return pControl->ClearCallback();
}

int Designer::GetCallbackCount(const PERSISTID& id) const
{
	IControl* pControl = GetControl(id);
	
	if (NULL == pControl)
	{
		return 0;
	}
	
	return (int)pControl->GetCallbackCount();
}

void Designer::GetCallbackList(const IVarList& args, IVarList& result) const
{
	IControl* pControl = GetControl(args.ObjectVal(0));
	
	if (NULL == pControl)
	{
		return;
	}
	
	pControl->GetCallbackList(result);
}

result_string Designer::GetCallbackFunc(const PERSISTID& id, 
	const char* name) const
{
	IControl* pControl = GetControl(id);
	
	if (NULL == pControl)
	{
		return "";
	}
	
	return pControl->GetCallbackFunc(name);
}
*/

bool Designer::FindMember(const char* name) const
{
	return m_Members.Exists(name);
}

bool Designer::AddMember(const char* name)
{
	return m_Members.Add(name, 0);
}

bool Designer::RemoveMember(const char* name)
{
	return m_Members.Remove(name);
}

bool Designer::ClearMember()
{
	m_Members.Clear();
	
	return true;
}

int Designer::GetMemberCount() const
{
	return (int)m_Members.GetCount();
}

void Designer::GetMemberList(const IVarList& args, IVarList& result)
{
	result.Clear();
	
	TStringPod<char, int, TStringTraits<char>, TCoreAlloc>::iterator it = 
		m_Members.Begin();
	
	for (; it != m_Members.End(); ++it)
	{
		result.AddString(it.GetKey());
	}
}
