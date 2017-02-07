#include "object_loader.h"
#include "time_axis.h"
#include "time_axis_macro.h"

#include "utility.h"
#include "helper.h"
#include "rapid_xml_helper.h"
#include "../public/core_log.h"
#include "../public/auto_mem.h"
#include "../public/core_file.h"

using namespace rapidxml;

CObjectLoader* CObjectLoader::NewInstance()
{
	return CORE_NEW(CObjectLoader);
}

CObjectLoader::CObjectLoader()
{
	m_pObject = NULL;
	m_pOwner = NULL;
}

CObjectLoader::~CObjectLoader()
{

}

void CObjectLoader::Release()
{
	if (this->DecRefs() == 0)
	{
		m_pObject = NULL;
		m_pOwner = NULL;
		CORE_DELETE(this);
	}
}

const char* CObjectLoader::GetName()
{
	return m_strFileName.c_str();
}

bool CObjectLoader::Load(bool async)
{
	if(!m_pObject || !m_pOwner)
	{
		return false;
	}

	const char *fname = m_strFileName.c_str();
	FILE* fp = core_file::fopen(fname, "rb");
	if (NULL == fp)
	{
		CORE_TRACE("(TimeAxis::LoadObjectKeyFrame)open file failed");
		CORE_TRACE(fname);
		return false;
	}

	core_file::fseek(fp, 0, SEEK_END);

	size_t file_size = core_file::ftell(fp);

	core_file::fseek(fp, 0, SEEK_SET);

	TAutoMem<char, 256> auto_buf(file_size + 1);

	char* buffer = auto_buf.GetBuffer();

	if (core_file::fread(buffer, sizeof(char), file_size, fp) != file_size)
	{
		core_file::fclose(fp);
		CORE_TRACE("(TimeAxis::LoadObjectKeyFrame)read file error");
		CORE_TRACE(fname);
		return false;
	}

	core_file::fclose(fp);

	buffer[file_size] = 0;

	xml_document<> doc;

	try
	{
		doc.parse<parse_declaration_node>(buffer);
	}
	catch (parse_error& e)
	{
		CORE_TRACE("(TimeAxis::LoadObjectKeyFrame)parse xml error");
		CORE_TRACE(e.what());
		CORE_TRACE(fname);
		return false;
	}

	// 对象节点
	xml_node<>* root = doc.first_node(XML_NODE_OBJECT);
	if (NULL == root)
	{
		CORE_TRACE("(TimeAxis::LoadObjectKeyFrame)no root element");
		CORE_TRACE(fname);
		return false;
	}

	xml_attribute<>* xml_attr = root->first_attribute("version"); //xml_add_attr(&doc, declaration, "version", PROJECT_VERSION);
	if (NULL != xml_attr)
	{
		const char *version = xml_attr->value();
		int ret = strcmp(version, PROJECT_VERSION);
		if (ret < 0)
		{
			CORE_TRACE("(TimeAxis::LoadObjectKeyFrame)");

			char info[256];
			SafeSprintf(info, sizeof(info), "%s file version is %s, that too old!", fname, version);
			CORE_TRACE(info);

			SafeSprintf(info, sizeof(info), "The program version is %s!", PROJECT_VERSION);
			CORE_TRACE(info);
		}
		else if(ret > 0)
		{
			CORE_TRACE("(TimeAxis::LoadObjectKeyFrame)");

			char info[256];
			SafeSprintf(info, sizeof(info), "%s file version is %s, it is the new edition not to be support!", fname, version);
			CORE_TRACE(info);

			SafeSprintf(info, sizeof(info), "The program version is %s!", PROJECT_VERSION);
			CORE_TRACE(info);
			return false;
		}
	}

	// 获取是否为max的轨迹信息
	xml_attr = root->first_attribute(XML_ATTR_CORRECT_Y);
	if (NULL != xml_attr)
	{
		m_pObject->fRAddY = (float)atof(xml_attr->value());;
	}

	// 获取存储的坐标数据的
	xml_attr = root->first_attribute(XML_ATTR_LOCAL_POS);
	if (NULL != xml_attr && strcmp("1", xml_attr->value()) == 0)
	{
		m_pObject->bLocalPosition = true;
		xml_attr = root->first_attribute(XML_ATTR_ORIGIN_X);
		if (NULL != xml_attr )
		{
			m_pObject->fOriginX = (float)atof(xml_attr->value());
		}

		xml_attr = root->first_attribute(XML_ATTR_ORIGIN_Y);
		if (NULL != xml_attr )
		{
			m_pObject->fOriginY = (float)atof(xml_attr->value());
		}

		xml_attr = root->first_attribute(XML_ATTR_ORIGIN_Z);
		if (NULL != xml_attr )
		{
			m_pObject->fOriginZ = (float)atof(xml_attr->value());
		}
	}

	// 获取存储的旋转数据的
	xml_attr = root->first_attribute(XML_ATTR_ROTATION);
	if (NULL != xml_attr && strcmp("1", xml_attr->value()) == 0)
	{
		m_pObject->bRotation = true;
		float f_rx = 0.f,f_ry = 0.f ,f_rz =0.f;
		xml_attr = root->first_attribute(XML_ATTR_ROTATION_X);
		if (NULL != xml_attr )
		{
			f_rx = (float)atof(xml_attr->value());
		}
		xml_attr = root->first_attribute(XML_ATTR_ROTATION_Y);
		if (NULL != xml_attr )
		{
			f_ry = (float)atof(xml_attr->value());
		}
		xml_attr = root->first_attribute(XML_ATTR_ROTATION_Z);
		if (NULL != xml_attr )
		{
			f_rz = (float)atof(xml_attr->value());
		}
		m_pObject->fRotationX = f_rx;
		m_pObject->fRotationY = f_ry;
		m_pObject->fRotationZ = f_rz;
	}

	// 获取对象的附加参数
	xml_attr = root->first_attribute(XML_ATTR_EXT_PROP);
	if (NULL != xml_attr )
	{
		m_pObject->strExtProp = xml_attr->value();

		CVarList param_list;
		fm_movie::ext_split_string(param_list, m_pObject->strExtProp.c_str(), ',');
		if (param_list.GetCount() >= 4)
		{
			m_pObject->initPos.x = (float)atof(param_list.StringVal(1));
			m_pObject->initPos.y = (float)atof(param_list.StringVal(2));
			m_pObject->initPos.z = (float)atof(param_list.StringVal(3));
		}
	}

	// 加载所有控制器
	xml_node<>* xml_controller = root->first_node(XML_NODE_CONTROLLER);
	while (xml_controller)
	{
		// 控制器类型
		xml_attr = xml_controller->first_attribute(XML_ATTR_TYPE_ID);
		if (NULL == xml_attr)
		{
			CORE_TRACE("(TimeAxis::LoadObjectKeyFrame) No Controller type.");
			CORE_TRACE(fname);
			Assert(0);
			return false;
		}
		int controller_type = atoi(xml_attr->value());

		xml_attr = xml_controller->first_attribute(XML_ATTR_USAGE_MODE);
		if (NULL == xml_attr)
		{
			CORE_TRACE("(TimeAxis::LoadObjectKeyFrame) No Controller usage mode.");
			CORE_TRACE(fname);
			Assert(0);
			return false;
		}

		int controller_usage_mode = atoi(xml_attr->value());
		// 是否已经存在该控制器
		KeyFrameController* pController = m_pOwner->GetController(*m_pObject, 
			controller_type);
		if (pController)
		{
			pController->ClearKeyFrames();
		}
		else
		{
			// 添加新控制器
			pController = (KeyFrameController *)CORE_ALLOC(sizeof(KeyFrameController));
			new (pController) KeyFrameController(controller_type, controller_usage_mode, m_pObject);

			m_pObject->controllers.Add(controller_type, pController);
		}

		if (NULL == pController)
		{
			CORE_TRACE("(TimeAxis::LoadObjectKeyFrame) Create controller failed.");
			return false;
		}

		// 加载控制器数据
		if (!pController->LoadController(xml_controller))
		{
			CORE_TRACE("(TimeAxis::LoadObjectKeyFrame) load controller failed.");
			CORE_TRACE(fname);
		}

		xml_controller = xml_controller->next_sibling(XML_NODE_CONTROLLER);
	}

	return true;
}

bool CObjectLoader::Create(bool async)
{
	if (m_pOwner)
	{
		m_pOwner->BuildPathPoint(m_pObject->strName.c_str());
	}
	return true;
}

bool CObjectLoader::EndCreate(int category, bool succeed)
{

	if(!m_pObject || !m_pOwner)
	{
		Release();
		return false;
	}

	m_pObject->pLoader = NULL;

	if (!succeed)
	{
		m_pOwner->DeleteObject(m_pObject->strName.c_str());
		Release();
		return false;
	}
	
	Release();
	return true;
}

void CObjectLoader::SetOwner(TimeAxis* pOwner)
{
	m_pOwner = pOwner;
}

void CObjectLoader::SetFileName(sObjectData* pObject, const char* file_name)
{
	m_pObject = pObject;
	m_strFileName = file_name;
}


