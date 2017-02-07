//--------------------------------------------------------------------
// 文件名:		design_saver.cpp
// 内  容:		
// 说  明:		
// 创建日期:	2007年3月16日
// 创建人:		陆利民
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#include "design_saver.h"
#include "design_res.h"
#include "designer.h"
#include "i_control.h"
#include "../utils/stream_writer.h"
#include "../public/core_log.h"
#include "../public/core_file.h"
#include "../public/var_table.h"
#include "../public/portable.h"
#define RAPIDXML_NO_STREAMS
#include "../extern/rapidxml/rapidxml.hpp"
#include "../extern/rapidxml/rapidxml_print.hpp"

using namespace rapidxml;

// CDesignSaver

CDesignSaver::CDesignSaver()
{
	m_pDesigner = NULL;
}

CDesignSaver::~CDesignSaver()
{
}

void CDesignSaver::SetDesigner(Designer* value)
{
	Assert(value != NULL);

	m_pDesigner = value;
}

ICore* CDesignSaver::GetCore() const
{
	return m_pDesigner->GetCore();
}

static xml_node<>* xml_add_node(xml_document<>* doc, xml_node<>* node, 
	const char* name)
{
	char* node_name = doc->allocate_string(name);
	xml_node<>* new_node = doc->allocate_node(node_element, node_name);

	node->append_node(new_node);

	return new_node;
}

static void xml_add_attr(xml_document<>* doc, xml_node<>* node, 
	const char* name, const char* value)
{
	char* attr_name = doc->allocate_string(name);
	char* attr_value = doc->allocate_string(value);
	xml_attribute<>* new_attr = doc->allocate_attribute(attr_name, attr_value);

	node->append_attribute(new_attr);
}

static bool save_xml_property(xml_document<>* doc, xml_node<>* root, 
	IControl* control, const char* name, bool is_utf8)
{
	Assert(control != NULL);
	Assert(name != NULL);
	Assert(root != NULL);
	
	CVar prop;
	
	if (!g_pCore->GetProperty(control, name, prop))
	{
		CORE_TRACE("(save_xml_property)get failed");
		CORE_TRACE(name);
		return false;
	}
	
	if (prop.IsInteger())
	{
		char buf[32];
		
		SafeSprintf(buf, sizeof(buf), "%d", prop.IntVal());
		xml_add_attr(doc, root, name, buf);
	}
	else if (prop.IsReal())
	{
		char buf[32];
		
		SafeSprintf(buf, sizeof(buf), "%f", prop.DoubleVal());
		xml_add_attr(doc, root, name, buf);
	}
	else if (prop.GetType() == VTYPE_BOOL)
	{
		if (prop.BoolVal())
		{
			xml_add_attr(doc, root, name, "true");
		}
		else
		{
			xml_add_attr(doc, root, name, "false");
		}
	}
	else if (prop.GetType() == VTYPE_STRING)
	{
		xml_add_attr(doc, root, name, prop.StringVal());
	}
	else if (prop.GetType() == VTYPE_WIDESTR)
	{
		if (is_utf8)
		{
			xml_add_attr(doc, root, name, 
				GuiUtil_Utf16ToUtf8(prop.WideStrVal()).c_str());
		}
		else
		{
			xml_add_attr(doc, root, name, 
				GuiUtil_Utf16ToAnsi(prop.WideStrVal()).c_str());
		}
	}
	else
	{
		return false;
	}
	
	return true;
}

static bool save_xml_subor(Designer* pDesigner, xml_document<>* doc, 
	xml_node<>* root, IControl* parent, IControl* control, const char* name, 
	const char* file, bool is_utf8)
{
	Assert(parent != NULL);
	Assert(control != NULL);
	Assert(name != NULL);
	Assert(root != NULL);
	
	// 是否有任何保存的数据
	bool processed = false;
	
	// 附属控件名
	xml_add_attr(doc, root, "name", name);

	IDesignInfo* pDesignInfo = pDesigner->GetDesignInfo(control);
	xml_node<>* element = NULL;
	
	// 保存改变的属性
	CVarList names;
	size_t count = control->GetEntInfo()->GetPropertyAll(names);
	size_t i;
	
	for (i = 0; i < count; i++)
	{
		const IPropInfo* info = 
			control->GetEntInfo()->FindPropertyInfo(names.StringVal(i));
		
		if (!pDesignInfo->FindPropInfo(info->GetName()))
		{
			continue;
		}
		
		// 作为附属控件时是否要保存
		if (!pDesignInfo->GetPropSuborSave(info->GetName()))
		{
			continue;
		}
		
		CVar prop;
		
		g_pCore->GetProperty(control, info->GetName(), prop);

		CVar def_value;

		pDesignInfo->GetPropDefValue(info->GetName(), def_value);
		
		// 测试属性是否仍为缺省值
		if (GuiUtil_PropertyEqual(prop, def_value))
		{
			continue;
		}
		
		if (NULL == element)
		{
			element = xml_add_node(doc, root, "prop");
		}

		if (!save_xml_property(doc, element, control, info->GetName(), 
			is_utf8))
		{
			CORE_TRACE("(save_xml_subor)save property error");
			CORE_TRACE(file);
			return false;
		}
		else
		{
			processed = true;
		}
	}
	
	// 保存附属控件
	CVarList subors;
	size_t subor_num = pDesignInfo->GetSuborList(subors);
	
	for (i = 0; i < subor_num; i++)
	{
		const char* name = subors.StringVal(i);
		IControl* subor = pDesigner->GetSuborControl(control, name);
		
		if (NULL == subor)
		{
			CORE_TRACE("(save_xml_subor)subor not found");
			CORE_TRACE(name);
			continue;
		}
		
		xml_node<>* element = xml_add_node(doc, root, "subor");
		
		if (save_xml_subor(pDesigner, doc, element, control, subor, name, 
			file, is_utf8))
		{
			processed = true;
		}
	}
	
	return processed;
}

static bool save_xml_control(Designer* pDesigner, xml_document<>* doc, 
	xml_node<>* root, IControl* control, const char* file, bool is_utf8)
{
	Assert(control != NULL);
	Assert(root != NULL);
	
	xml_add_attr(doc, root, "name", control->GetName());
	xml_add_attr(doc, root, "entity", control->GetEntInfo()->GetEntityName());

	IVar* pScritName = control->GetCustoms()->GetValue("ScriptName");

	if (NULL == pScritName)
	{
		pScritName = control->GetCustoms()->Add("ScriptName", 
			CVar(VTYPE_STRING, ""));
	}
	
	IVar* pInitFunc = control->GetCustoms()->GetValue("InitFunc");

	if (NULL == pInitFunc)
	{
		pInitFunc = control->GetCustoms()->Add("InitFunc", 
			CVar(VTYPE_STRING, ""));
	}

	// 脚本名
	xml_add_attr(doc, root, "script", pScritName->StringVal());
	// 初始化函数
	xml_add_attr(doc, root, "init", pInitFunc->StringVal());

	IDesignInfo* pDesignInfo = pDesigner->GetDesignInfo(control);
	xml_node<>* element = NULL;
	
	// 保存属性
	CVarList props;
	size_t count = control->GetEntInfo()->GetPropertyAll(props);
	size_t i;
	
	for (i = 0; i < count; i++)
	{
		const IPropInfo* info = 
			control->GetEntInfo()->FindPropertyInfo(props.StringVal(i));

		if (!pDesignInfo->FindPropInfo(info->GetName()))
		{
			continue;
		}
		
		if (!pDesignInfo->GetPropSave(info->GetName()))
		{
			continue;
		}

		CVar prop;

		g_pCore->GetProperty(control, info->GetName(), prop);

		CVar def_value;

		pDesignInfo->GetPropDefValue(info->GetName(), def_value);

		if (GuiUtil_PropertyEqual(prop, def_value))
		{
			continue;
		}

		if (NULL == element)
		{
			element = xml_add_node(doc, root, "prop");
		}
		
		if (!save_xml_property(doc, element, control, info->GetName(), 
			is_utf8))
		{
			CORE_TRACE("(save_xml_control)save property error");
			CORE_TRACE(file);
			return false;
		}
	}

	element = NULL;

//	// 保存回调
//	CVarList keys;
//	size_t key_count = control->GetCallbackList(keys);
//
//	for (size_t i = 0; i < key_count; ++i)
//	{
//		const char* name = keys.StringVal(i);
//		const char* func = control->GetCallbackFunc(name);
//
//		if (StringEmpty(name) || StringEmpty(func))
//		{
//			continue;
//		}
//
//		if (NULL == element)
//		{
//			element = xml_add_node(doc, root, "event");
//		}
//
//		xml_add_attr(doc, element, name, func);
//	}
	
	// 保存附属控件
	CVarList subors;
	size_t subor_num = pDesignInfo->GetSuborList(subors);

	for (i = 0; i < subor_num; i++)
	{
		const char* name = subors.StringVal(i);
		IControl* subor = pDesigner->GetSuborControl(control, name);
		
		if (NULL == subor)
		{
			// 附属控件可能没创建
			continue;
		}
		
		xml_node<>* element = xml_add_node(doc, root, "subor");
		
		save_xml_subor(pDesigner, doc, element, control, subor, name, file, 
			is_utf8);
	}
	
	// 保存子控件
	CVarList children;
	size_t child_count = control->GetChildList(children);
	
	for (i = 0; i < child_count; ++i)
	{
		IControl* pChild = pDesigner->GetControl(children.ObjectVal(i));
		
		// 不能为附属控件
		Assert(NULL == pChild->GetDelegateControl());

		// 不保存选择器
		if (pChild->GetEntInfo()->IsKindOf("Selector"))
		{
			continue;
		}
		
		xml_node<>* element = xml_add_node(doc, root, "control");

		save_xml_control(pDesigner, doc, element, pChild, file, is_utf8);
	}
	
	return true;
}

bool CDesignSaver::SaveForm(IControl* control, const char* path, 
	const char* file, const char* resource)
{
	Assert(control != NULL);
	Assert(path != NULL);
	Assert(file != NULL);
	//Assert(resource != NULL);

	if (!control->GetEntInfo()->IsKindOf("Form"))
	{
		CORE_TRACE("(CDesignSaver::SaveForm)not form");
		CORE_TRACE(file);
		return false;
	}
	
	result_string fname;
	
	if (StringEmpty(path))
	{
		fname = result_string(m_pDesigner->GetDefaultPath()) + file;
	}
	else
	{
		fname = result_string(path) + file;
	}
	
	bool is_utf8 = stricmp(m_pDesigner->GetEncoding(), "utf-8") == 0;

	xml_document<> doc;
	xml_node<>* declaration = doc.allocate_node(node_declaration);
	xml_attribute<>* version = doc.allocate_attribute("version", "1.0");
	xml_attribute<>* encoding = doc.allocate_attribute("encoding", 
		m_pDesigner->GetEncoding());

	declaration->append_attribute(version);
	declaration->append_attribute(encoding);
	doc.append_node(declaration);

	char* root_name = doc.allocate_string("form");
	xml_node<>* root = doc.allocate_node(node_element, root_name);

	doc.append_node(root);

//	// 记录资源集合
//	if (!StringEmpty(resource))
//	{
//		xml_add_attr(&doc, root, "resource", resource);
//	}

	xml_node<>* element = xml_add_node(&doc, root, "control");

	save_xml_control(m_pDesigner, &doc, element, control, file, is_utf8);

	TStreamWriter<char, 256> writer;
	TStreamWriter<char, 256>::iterator it = writer.Begin();

	print(it, doc, 0);

	char* buf = writer.GetData();
	size_t length = writer.GetCount();
	FILE* fp = Port_FileOpen(fname.c_str(), "wb");

	if (NULL == fp)
	{
		CORE_TRACE("(CDesignSaver::SaveForm)open file failed");
		CORE_TRACE(file);
		return false;
	}

	if (core_file::fwrite(buf, sizeof(char), length, fp) != length)
	{
		core_file::fclose(fp);
		CORE_TRACE("(CDesignSaver::SaveForm)write file error");
		CORE_TRACE(file);
		return false;
	}

	core_file::fclose(fp);
	/*
	TiXmlDocument doc;
	
	doc.InsertEndChild(
		TiXmlDeclaration("1.0", m_pDesigner->GetEncoding(), ""));
	
	// 根节点
	TiXmlElement* root = 
		doc.InsertEndChild(TiXmlElement("form"))->ToElement();

	// 记录资源集合
	if (!StringEmpty(resource))
	{
		root->SetAttribute("resource", resource);
	}

	bool is_utf8 = stricmp(m_pDesigner->GetEncoding(), "utf-8") == 0;

	// 保存控件信息
	TiXmlElement element("control");
	
	if (SaveXmlControl(control, file, &element, is_utf8))
	{
		root->InsertEndChild(element);
	}
	
	if (!doc.SaveFile(fname.c_str()))
	{
		CORE_TRACE("(CDesignSaver::SaveForm)save failed");
		CORE_TRACE(file);
		return false;
	}
	*/

	return true;
}

bool CDesignSaver::SaveResource(const char* path, const char* file)
{
	Assert(path != NULL);
	Assert(file != NULL);
	
	result_string fname;
	
	if (StringEmpty(path))
	{
		fname = result_string(m_pDesigner->GetDefaultPath()) + file;
	}
	else
	{
		fname = result_string(path) + file;
	}
	
	xml_document<> doc;
	xml_node<>* declaration = doc.allocate_node(node_declaration);
	xml_attribute<>* version = doc.allocate_attribute("version", "1.0");
	xml_attribute<>* encoding = doc.allocate_attribute("encoding", 
		m_pDesigner->GetEncoding());

	declaration->append_attribute(version);
	declaration->append_attribute(encoding);
	doc.append_node(declaration);

	char* root_name = doc.allocate_string("resource");
	xml_node<>* root = doc.allocate_node(node_element, root_name);

	doc.append_node(root);

	CDesignRes* pDesignRes = m_pDesigner->GetDesignRes();

	for (size_t type = 0; type < CDesignRes::RESTYPE_MAX; ++type)
	{
		result_string res_names = 
			result_string(pDesignRes->GetResName(type)) + "s"; 
		CVarList files;

		pDesignRes->GetResFileList(type, files);

		size_t file_count = files.GetCount();

		for (size_t i = 0; i < file_count; i++)
		{
			xml_node<>* element = xml_add_node(&doc, root, res_names.c_str());		

			xml_add_attr(&doc, element, "name", files.StringVal(i));
		}
	}

	TStreamWriter<char, 256> writer;
	TStreamWriter<char, 256>::iterator it = writer.Begin();

	print(it, doc, 0);

	char* buf = writer.GetData();
	size_t length = writer.GetCount();
	FILE* fp = Port_FileOpen(fname.c_str(), "wb");

	if (NULL == fp)
	{
		CORE_TRACE("(CDesignSaver::SaveResource)open file failed");
		CORE_TRACE(file);
		return false;
	}

	if (core_file::fwrite(buf, sizeof(char), length, fp) != length)
	{
		core_file::fclose(fp);
		CORE_TRACE("(CDesignSaver::SaveResource)write file error");
		CORE_TRACE(file);
		return false;
	}

	core_file::fclose(fp);
	/*
	TiXmlDocument doc;
	
	doc.InsertEndChild(
		TiXmlDeclaration("1.0", m_pDesigner->GetEncoding(), ""));
	
	// 根节点
	TiXmlElement* root = 
		doc.InsertEndChild(TiXmlElement("resource"))->ToElement();
	CDesignRes* pDesignRes = m_pDesigner->GetDesignRes();
	
	for (size_t type = 0; type < CDesignRes::RESTYPE_MAX; ++type)
	{
		result_string res_names = 
			result_string(pDesignRes->GetResName(type)) + "s"; 
		CVarList files;

		pDesignRes->GetResFileList(type, files);

		size_t file_count = files.GetCount();
		
		for (size_t i = 0; i < file_count; i++)
		{
			TiXmlElement* element = root->InsertEndChild(
				TiXmlElement(res_names.c_str()))->ToElement();		
			
			element->SetAttribute("name", files.StringVal(i));
		}
	}
	
	if (!doc.SaveFile(fname.c_str()))
	{
		CORE_TRACE("(CDesignSaver::SaveResource)save failed");
		CORE_TRACE(file);
		return false;
	}
	*/
	return true;
}

